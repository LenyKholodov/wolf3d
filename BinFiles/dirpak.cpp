#include <dirpak.h>
#include <malloc.h>

tAllocFnPtr DirPak::m_alloc_fn=malloc;
tFreeFnPtr  DirPak::m_free_fn=free;

int32 DirPak::CRCRegion(int32 offs,int32 len)
{
  debug("DirPak: CRCRegion("<<offs<<","<<len<<")"<<endl);
  ulong crc=0xFFFFFFFF;
  mDirFile->seek(offs);
  const int BlockSize=4000;
  uchar* buffer=(uchar*)__alloc_fn(BlockSize);
  if(buffer!=NULL)
  {
    for(;len>BlockSize;len-=BlockSize)
    {
      mDirFile->read(buffer,BlockSize);
      crc=CRC32(buffer,BlockSize,crc);
    }
    mDirFile->read(buffer,len);
    crc=CRC32(buffer,len,crc);
    __free_fn(buffer);
  }
  else
    for(;len-->0;crc=CRC32(mDirFile->getc(),crc));
  return crc;
}

int CompareFileRec(const void* a,const void* b)
{
  uint32 an=((const tFileListEntry*)a)->ncrc,
  	 bn=((const tFileListEntry*)b)->ncrc;
  return an>bn?1:(an<bn?-1:0);
}

BOOL DirPak::_openDP(void)
{
  // Дано: файл уже открыт
  // работаем в 2 прохода:
  // 1й - ищем описание каталога и считаем кол-во файлов
  // 2й - загружаем описание файлов и сортируем их
  // не очень красиво, ну и х. с ним

  debug("DirPak::_openDP - starting"<<endl);
  tLumpTag tag;
  if(!mDirFile->reset())
  {
    debug("DirPak::_openDP can't going to begin of file\n");
    return FALSE;
  }
  // попытка № 1: ищем тэг индекса
  mDirFile->seekend(-sizeof(tLumpTag)-4);
  if(mDirFile->read(&tag,sizeof(tag))==sizeof(tag))
    if(tag.tag==DirPakBackIndexTag) // всё ок, индекс есть!
    {
      mDirFile->seek(mDirFile->getl()); // прыгаем по указанному адресу
      mDirFile->read(&tag,sizeof(tag));
      if(tag.tag==DirPakIndexTag) // в натуре индексный таг
      {
        mCount=tag.size/sizeof(tFileListEntry);
        debug("DirPak::_openDP index found("<<mCount<<")"<<endl);
        if((mFiles=(tFileListEntry*)__alloc_fn(tag.size))==NULL)
  	{
    	  closeDP();
    	  debug("DirPak::_openDP: no memory for filelist("<<mCount<<")\n");
    	  return FALSE;
	}
	// read pak info
	mDirFile->read(mFiles,tag.size);
	for(int i=0;i<mCount;i++)
	  if(mFiles[i].type==DirPakInfoTag)
	    if(mFiles[i].size<sizeof(tDirPakInfoLump))
      	    { 
              debug("DirPak::_openDP - warning: DirPakInfoLump corrupted\n");
      	    }	
      	    else
	    {	      
              if((mPakInfo=(tDirPakInfoLump*)__alloc_fn(mFiles[i].size))==NULL)
	      { 
                debug("DirPak::_openDP - warning: no memory for DirPakInfoLump");
	      }	
	      else
	      { 
	      	mDirFile->seek(mFiles[i].offset);
                mDirFile->read(mPakInfo,mFiles[i].size);
		break;
              }  		
	    }  
	return TRUE;
      }
    }
  mDirFile->reset();
  for(mCount=0;!mDirFile->eof();)
  {
    if(mDirFile->read(&tag,sizeof(tag))!=sizeof(tag))
    {
      debug("DirPak::_openDP - warning: LumpTag corrupted\n");
      break;
    }
    if(tag.tag==DirPakInfoTag && mPakInfo==NULL)
    {
      if(tag.size<sizeof(tDirPakInfoLump))
      { 
        debug("DirPak::_openDP - warning: DirPakInfoLump corrupted\n");
	mDirFile->seekcur(tag.size);
      }	
      else
        if((mPakInfo=(tDirPakInfoLump*)__alloc_fn(tag.size))==NULL)
	{ 
          debug("DirPak::_openDP - warning: no memory for DirPakInfoLump");
	  mDirFile->seekcur(tag.size);
	}	
	else
          mDirFile->read(mPakInfo,tag.size);
      continue;
    }     		
    if(tag.tag==PakFileTag || tag.tag==FileTag)
    {
      mCount++; 
      mDirFile->seekcur(tag.size);
      continue;
    }
    debug("DirPak::_openDP - warning: i don't know this tag="<<tag.tag<<endl);
    mDirFile->seekcur(tag.size);
  }
  debug("mCount="<<mCount<<"\n DirInfoPtr="<<mPakInfo<<endl);
  mDirFile->reset();  
// 2й - загружаем описание файлов
  if((mFiles=(tFileListEntry*)__alloc_fn(sizeof(tFileListEntry)*mCount))==NULL)
  {
    closeDP();
    debug("DirPak::_openDP: no memory for filelist("<<mCount<<")\n");
    return FALSE;
  }
  tFileInfoTag fileinfo;
  for(int i=0;i<mCount;)
  {
    if(mDirFile->read(&tag,sizeof(tag))!=sizeof(tag))
    {
      debug("DirPak::_openDP - error: LumpTag corrupted at pos "<<mDirFile->tell()<<"\n");
      closeDP();
      return FALSE;
    }
    if(tag.tag==PakFileTag || tag.tag==FileTag)
    {
      if(mDirFile->read(&fileinfo,sizeof(tFileInfoTag))!=sizeof(tFileInfoTag))
      {
        debug("DirPak::_openDP - error: LumpTag corrupted\n");
        closeDP();
        return FALSE;
      }
      debug("DirPak::_openDP - read info #"<<i<<" ="<<fileinfo.name<<endl);
      strncpy(mFiles[i].name,fileinfo.name,_MAX_PATH);
      mFiles[i].ncrc  =CRC32((uchar*)mFiles[i].name,min(strlen(fileinfo.name),_MAX_PATH));
      mFiles[i].type  =tag.tag;
      mFiles[i].ver   =fileinfo.ver;
      mFiles[i].crc   =fileinfo.crc;
      mFiles[i].offset=mDirFile->getpos();
      mFiles[i].size  =tag.size-sizeof(tFileInfoTag);
//      mFiles[i].handle=NULL;
      mDirFile->seekcur(tag.size-sizeof(tFileInfoTag));
      i++;
    }
    else // skip lump
      mDirFile->seekcur(tag.size);
  }
//... и сортируем их
  qsort(mFiles,mCount,sizeof(tFileListEntry),CompareFileRec);
  return TRUE;	   	
}

BOOL DirPak::openDP(const char* fname,long cSize,BOOL ro)
{
  closeDP();
  if((mSbf=new stdBinFile(fname,(mReadOnly=ro)?BinFile::OPEN_RO:
  	      			BinFile::OPEN_RW))==NULL || mSbf->bad())
  {
    delete mSbf;
    mSbf=NULL;
    return FALSE;
  }

  mDirFile=mSbf;
  if(cSize>0)
  {
    BinFile* cache=new cacheBinFile(mSbf,cSize);
    if(cache!=NULL && !cache->bad())
      mDirFile=cache;
  }
  return _openDP();
}

BOOL DirPak::openDP(BinFile* bf)
{
  closeDP();
  if(!bf->canseek() || !bf->canread())
  {
    debug("DirPak::openDP: no requered file access\n");
    return FALSE;
  }
  mReadOnly=!bf->canwrite();
  if(!mReadOnly && !bf->canresize())
  {
    return FALSE;
  }
  mDirFile=bf;
  return _openDP();
}

void DirPak::closeDP(void)
{
  __free_fn(mPakInfo);
  mPakInfo=NULL;
  if(!openedDP())
    return;
//  close(); 
  if(mDirFile!=mSbf)
    delete mDirFile;
  delete mSbf;
  mSbf=NULL;
  mDirFile=NULL;
  __free_fn(mFiles);
  mFiles=NULL;
  mCount=0;
}

BinFile* DirPak::open(int index,int mode)
{
  if(mReadOnly)
    mode=BinFile::OPEN_RO;
  if(index<0 || index>=mCount)// || opened(index)!=NULL)
    return NULL;
  if(mFiles[index].type==PakFileTag)
    mode=BinFile::OPEN_RO;
  BinFile* bf=NULL;  
  switch(mode)
  {
    case BinFile::OPEN_RW:
	if((bf=new aBinFile(mDirFile,mFiles[index].offset,
		   mFiles[index].size))==NULL || bf->bad())
	{
	  delete bf;
	  bf=NULL;
	}
	break;
    case BinFile::OPEN_RO:	
        bf=mFiles[index].type==PakFileTag?
		new unpakBinLump(mDirFile,mFiles[index].offset,mFiles[index].size,-1):
	   mFiles[index].type==FileTag?new aBinFile(mDirFile,mFiles[index].offset,mFiles[index].size,BinFile::can_read|BinFile::can_seek):NULL;
	if(bf==NULL || bf->bad())
	{
	  delete bf;
	  bf=NULL;
	}
	break;
  }
//  return mFiles[index].handle=bf;
  return bf;
}

int DirPak::FindEntry(const char* fname) const
{
  if(!openedDP())
    return -1;
  uint32 key=CRC32(fname,min(strlen(fname),_MAX_PATH));
  const tFileListEntry *base=mFiles,
  		       *probe;
  for(int count=mCount;count>0;)
  {
    int i=count>>1;
    if(key==(probe=base+i)->ncrc)
    {     
      if(!strncmp(probe->name,fname,_MAX_PATH))
        return probe-mFiles;
      i=probe-mFiles;
      // find in left half
      for(int j=i-1;j>=0 && mFiles[j].ncrc==key;j--)
        if(!strncmp(mFiles[j].name,fname,_MAX_PATH))
          return j;
      // find in right half
      for(j=i+1;j<mCount && mFiles[j].ncrc==key;j++)
        if(!strncmp(mFiles[j].name,fname,_MAX_PATH))
          return j;
      break;
    }
    if(key<probe->ncrc)
      count=i;
    else
    {
      base=probe+1;
      count-=i+1;
    }
  }
  return -1;
}

BOOL DirPak::checkFile(int index)
{
  debug("DirPak: check file#"<<index<<endl);
  if(index>=mCount || index<0 || !openedDP())
    return FALSE;
  return mFiles[index].crc==CRCRegion(mFiles[index].offset,mFiles[index].size);
}

BOOL DirPak::check(void)
{
  debug("DirPak: check"<<endl);
  if(!openedDP())
    return FALSE;
  for(int i=0;i<mCount;i++)
    if(!checkFile(i))
      return FALSE;
  return TRUE;
}

BOOL DirPak::indexw(void) // запись индекса
{
  if(!(openedDP() && mDirFile->canwrite() && mDirFile->canresize()))
    return FALSE;
// сканирование пакета
  mDirFile->reset(); 
  tLumpTag tag;
  for(int count=0;!mDirFile->eof();)
  {
    if(mDirFile->read(&tag,sizeof(tag))!=sizeof(tag))
    {
      debug("DirPak::indexw - warning: LumpTag corrupted\n");
      break;
    }
    if(tag.tag==DirPakBackIndexTag)
      continue;
    if(tag.tag==DirPakIndexTag)
    {
      mDirFile->seekcur(tag.size);
      continue;
    }
    if(tag.tag==PakFileTag || tag.tag==FileTag || tag.tag==DirPakInfoTag)
    {
      count++;
      mDirFile->seekcur(tag.size);
      continue;
    }
    debug("DirPak::indexw - warning: i don't know this tag="<<tag.tag<<endl);
    mDirFile->seekcur(tag.size);
  }
  debug("count="<<count<<endl);
  mDirFile->reset();
// 2й - загружаем описание файлов
  tFileListEntry* files; 
  if((files=new tFileListEntry[count])==NULL)
  {
    debug("DirPak::indexw no memory for filelist("<<count<<")\n");
    return FALSE;
  }  
  for(int i=0;i<count;)
  {
    if(mDirFile->read(&tag,sizeof(tag))!=sizeof(tag))
    {
      debug("DirPak::indexw - error: LumpTag corrupted at pos "<<mDirFile->tell()<<"\n");
      debug("DirPak::indexw - error at index"<<i<<"\n");
      delete files;
      return FALSE;
    }
    if(tag.tag==PakFileTag || tag.tag==FileTag)
    {
      tFileInfoTag fileinfo;
      if(mDirFile->read(&fileinfo,sizeof(tFileInfoTag))!=sizeof(tFileInfoTag))
      {
        debug("DirPak::indexw - error: LumpTag corrupted\n");
      	delete files;
        return FALSE;
      }
      debug("DirPak::indexw - read info #"<<i<<" ="<<fileinfo.name<<endl);
      strncpy(files[i].name,fileinfo.name,_MAX_PATH);
      files[i].ncrc  =CRC32((uchar*)files[i].name,min(strlen(fileinfo.name),_MAX_PATH));
      files[i].type  =tag.tag;
      files[i].ver   =fileinfo.ver;
      files[i].crc   =fileinfo.crc;
      files[i].offset=mDirFile->getpos();
      files[i].size  =tag.size-sizeof(tFileInfoTag);
//      mFiles[i].handle=NULL;
      mDirFile->seekcur(tag.size-sizeof(tFileInfoTag));
      i++;
    }
    else 
    {
      if(tag.tag==DirPakInfoTag)
      {
        strncpy(files[i].name,"",_MAX_PATH);
      	files[i].ncrc  =CRC32((uchar*)files[i].name,1);
      	files[i].type  =tag.tag;
      	files[i].ver   =0;
      	files[i].crc   =0;
      	files[i].offset=mDirFile->getpos();
      	files[i].size  =tag.size;
//      mFiles[i].handle=NULL;
      	i++;
      }
      // skip lump
      mDirFile->seekcur(tag.size);
    }
  }
//... и сортируем их
  qsort(files,count,sizeof(tFileListEntry),CompareFileRec);
// выгрузка блока файлов
  tag.tag=DirPakIndexTag;
  tag.size=sizeof(tFileListEntry)*count;
  int pos=mDirFile->size();
  mDirFile->seek(pos);
  mDirFile->write(&tag,sizeof(tag));
  mDirFile->write(files,sizeof(tFileListEntry)*count);
  delete files;
// выгрузка маркера индекса
  tag.tag=DirPakBackIndexTag;
  tag.size=4; 
  mDirFile->write(&tag,sizeof(tag));
  mDirFile->putl(pos);
  return TRUE;	   	
}