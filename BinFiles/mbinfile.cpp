#include <mbinfile.h>
#include <misc.h>

tAllocFnPtr memBinFile::m_alloc_fn=malloc;
tFreeFnPtr  memBinFile::m_free_fn=free;

memBinFile::~memBinFile(void)	
{ 
  close();     
}

BOOL memBinFile::open(long size,BOOL crsize)
{
  close();
  int fmode=can_read|can_write|can_seek|(crsize?can_resize:0);
  if(size && (buffer=(char*)__alloc_fn(filelen=size))==NULL)
  {
    SetErrorInfo(BFE_NOMEM);
    return FALSE;
  }
  owner=TRUE;
  mode=fmode;
  return TRUE;
}

BOOL memBinFile::open(void* ptr,long size,BOOL ro)
{
  close();
  owner=FALSE;
  filelen=size;
  buffer=(char*)ptr;
  mode=(ro?can_read:can_rw)|can_seek;
  return TRUE;
}
  
void memBinFile::close(void)
{
  if(owner)
    __free_fn(buffer);
  buffer=NULL;
  BinFile::close();
}
        
long memBinFile::read(void *buf, long len)
{
  if(!canread())
    return BinFile::read(buf,len);
  if((filepos+len)>filelen && (len=filelen-filepos)<=0)
    return 0;
  memcpy(buf,buffer+filepos,len);
  filepos+=len;
  return len;
}

long memBinFile::write(const void *buf, long len)
{
  if(!canwrite())
    return BinFile::write(buf,len);
  if(len>(filelen-filepos))
  {
    if(canresize())
      resize(filepos+len);
    if(len>(filelen-filepos) && (len=filelen-filepos)<=0)
      return 0;
  }
  memcpy(buffer+filepos,buf,len);
  filepos+=len;
  return len;
}

long memBinFile::resize(long len)
{
  if(!canresize())
    return len;
  if(len<=0)
  {
    __free_fn(buffer);
    buffer=NULL;
    return filelen=filepos=0;
  }
  if(buffer==NULL)
  {
    open(len);
    return filelen;		
  }
  void *ptr=__realloc_fn(buffer,len);
  if(ptr==NULL)
  {       	
    SetErrorInfo(BFE_NOMEM);		
    return filelen;
  }
  buffer=(char*)ptr;
  if(filepos>(filelen=len))
    filepos=len;	
  return filelen;
}

int memBinFile::getc(void)
{
  if(filepos<filelen)
    return buffer[filepos++];
  return EOF;
}

int memBinFile::gets(void)
{
	if(filepos>=filelen-1)
		return getc();
	int16 data=*((int16*)(buffer+filepos));
	filepos+=2;
	return data;
}

long memBinFile::getl(void)
{
	if(filepos==filelen)
		return 0;
	if(filepos>=filelen-2)
		return gets();
	int16 data=*((long*)(buffer+filepos));
	filepos+=2;
	return data;
}

BinFile& memBinFile::putc(char c)
{
	if(eof())
		resize(filelen+1);
	if(filepos<filelen)
		buffer[filepos++]=c;
	return *this;
}

BinFile& memBinFile::puts(int16 s)
{
	return putc(lobyte(s)).putc(hibyte(s));
}

BinFile& memBinFile::putl(long l)
{
	return puts(loword(l)).puts(hiword(l));
}

long memBinFile::AssignFrom(BinFile& f)
{
	close();
	if(open(f.size()))
		f[0].read(buffer,filelen);
	return filelen;
}

long memBinFile::AssignTo(BinFile& f)
{
	if(bad())
		return 0;
	f.size(filelen);
	return f[0].write(buffer,filelen);
}
