#include <sbinfile.h>

#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>

tAllocFnPtr stdBinFile::m_alloc_fn=malloc;
tFreeFnPtr  stdBinFile::m_free_fn=free;

stdBinFile::~stdBinFile(void)
{
  close();
}

void stdBinFile::close(void)
{
  if(mode && ::close(handle)==-1)
      SetErrorInfo(BFE_STD_CLOSE);
  *fname=0;
  BinFile::close();
}

BOOL stdBinFile::open(const char *name, int type)
{
  close();
  int fmode;
  strcpy(fname,name);
  switch(type)
  {
    case OPEN_RO:
    	          fmode=can_read|can_seek;
		  handle=::open(fname, O_RDONLY|O_BINARY);
		  break;
    case OPEN_RW:
    case OPEN_CR:
	 	// check path
		  {
		    char* ptr=strrchr(fname,'\\');
		    if(ptr!=NULL)
		    {
		      *ptr=0;
		      if(!MakeDir(fname))
		      {
			*ptr='\\';
			SetErrorInfo(BFE_STD_OPEN);
			return FALSE;
		      }
		      *ptr='\\';
		  }
		}
		fmode=can_read|can_seek|can_write|can_chsize;
		handle=::open(name,O_RDWR|O_CREAT|O_BINARY,S_IWRITE|S_IREAD);
		if(type==OPEN_CR)
		  ::chsize(handle,0);
		break;
    default:	handle=-1;
  }
  if(handle<0)
  {
    SetErrorInfo(BFE_STD_OPEN);
    return FALSE;
  }
  filepos=0;
  filelen=::filelength(handle);
  mode=fmode;
  return TRUE;
}

long stdBinFile::read(void *buf, long len)
{
	if(!canread())
    		return BinFile::read(buf,len);
  	if((len=::read(handle,buf,len))==-1)
	{
		SetErrorInfo(BFE_STD_READ);
		return -1;
	}
  	filepos+=len;
  	return len;
}

long stdBinFile::write(const void *buf, long len)
{
	if(!canread())
    		return BinFile::write(buf,len);
  	if(!canresize())
    		if(len>(filelen-filepos))
      			len=filelen-filepos;
	if((len=::write(handle, buf, len))==-1)
	{
		SetErrorInfo(BFE_STD_WRITE);
		return -1;
	}
  	if((filepos+=len)>filelen)
    		filelen=filepos;
  	return len;
}

long stdBinFile::seek(long pos)
{
  	if(pos==filepos)
    		return filepos;
    	BinFile::seek(pos);
	if(::lseek(handle,filepos,SEEK_SET)==-1)
	{
		SetErrorInfo(BFE_STD_SEEK);
		return -1;
	}
  	return filepos;
}

long stdBinFile::resize(long pos)
{
  	if(!canresize())
    		return BinFile::resize(pos);
  	if(::chsize(handle, pos)==-1)
	{
		SetErrorInfo(BFE_STD_RESIZE);
		return -1;
	}
  	filepos=::tell(handle);
  	return filelen=::filelength(handle);
}