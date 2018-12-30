#include <cbinfile.h>

tAllocFnPtr cacheBinFile::m_alloc_fn=malloc;
tFreeFnPtr  cacheBinFile::m_free_fn=free;


cacheBinFile::~cacheBinFile(void)
{
	close();
}

void cacheBinFile::invalidatebuf(void)
{
	if(dirty)
  	{
    		if(fileseekpos!=filebufpos)
      			f->seek(filebufpos);
    		bufread=f->write(buffer,bufread);
    		fileseekpos=filebufpos+bufread;
    		dirty=FALSE;
  	}
  	filebufpos+=bufread;
  	bufread=0;
  	bufpos=0;
}

BOOL cacheBinFile::open(BinFile* fil, int len)
{
	close();
	if(!fil->getmode())
    	  	return FALSE;
  	f=fil;
  	int fmode=f->getmode();
  	if(fmode&can_seek==0 || fmode==can_seek)
	{
		SetErrorInfo(BFE_SEEK);		
    		return FALSE;
    	}
  	if((buffer=(char*)__alloc_fn(len))==NULL)
	{
		SetErrorInfo(BFE_NOMEM);
		return FALSE;
	}
  	filelen=f->length();
  	buflen=len;
  	bufpos=bufread=filepos=0;
  	dirty=FALSE;
  	filebufpos=fileseekpos=f->tell();
  	mode=fmode;
  	return TRUE;
}

void cacheBinFile::close(void)
{
	if(mode)
  	{
    		invalidatebuf();
    		__free_fn(buffer);
  	}
  	BinFile::close();
}

long cacheBinFile::read(void *buf, long len)
{
	if(!canread())
    		return BinFile::read(buf,len);
  	int len0=bufread-bufpos;
  	if(len<len0)
    		len0=len;
  	len-=len0;
  	memcpy(buf,buffer+bufpos,len0);
  	bufpos+=len0;
  	filepos=filebufpos+bufpos;
  	if(!len)
    		return len0;
	if(len<(buflen-bufread))
  	{
    		if(fileseekpos!=(filebufpos+bufread))
      			f->seek(filebufpos+bufread);
    		int l=f->read(buffer,buflen-bufread);
    		if(l<len)
      		  	len=l;
    		memcpy((char*)buf+len0,buffer+bufread,len);
    		bufpos+=len;
    		bufread+=l;
  	}
  	else
  	{
    		invalidatebuf();
    		if(len>=buflen)
    		{
      			if(fileseekpos!=filebufpos)
        		f->seek(filebufpos);
      			len=f->read((char*)buf+len0,len);
      			filebufpos+=len;
    		}
    		else
    		{
      			if(fileseekpos!=filebufpos)
        			f->seek(filebufpos);
      			bufread=f->read(buffer,buflen);
      			if(bufread<len)
        			len=bufread;
      			memcpy((char*)buf+len0,buffer,len);
      			bufpos=len;
    		}
  	}
  	fileseekpos=filebufpos+bufread;
  	filepos=filebufpos+bufpos;
  	return len0+len;
}

long cacheBinFile::write(const void *buf, long len)
{
  	if(!canwrite())
    		return BinFile::write(buf,len);
  	if(!canresize())
    		if(len>(filelen-filepos))
      			len=filelen-filepos;
  	int len0=buflen-bufpos;
  	if(len<len0)
    		len0=len;
  	if(len0)
    		dirty=TRUE;
  	len-=len0;
  	memcpy(buffer+bufpos,buf,len0);
  	bufpos+=len0;
  	if(bufpos>bufread)
    		bufread=bufpos;
  	filepos=filebufpos+bufpos;
  	if(filepos>filelen)
    		filelen=filepos;
  	if(!len)
    	   	return len0;
  	invalidatebuf();
	if(len>=buflen)
  	{
    		if(fileseekpos!=filebufpos)
      			f->seek(filebufpos);
    		len=f->write((const char*)buf+len0,len);
    		filebufpos+=len;
    		fileseekpos=filebufpos;
  	}
  	else
  	{
    		memcpy(buffer,(const char*)buf+len0,len);
    		bufread=len;
    		bufpos=len;
  	}
  	filepos=filebufpos+bufpos;
  	if(filepos>filelen)
    		filelen=filepos;
  	return len0+len;
}

long cacheBinFile::seek(long pos)
{
  	if(!canseek())
	{
		SetErrorInfo(BFE_SEEK);
		return filepos;
	}
  	if(pos<0)
    		pos=0;
  	if(pos>filelen)
    		pos=filelen;
  	if((pos>=filebufpos) && (pos<=(filebufpos+bufread)))
    		bufpos=pos-filebufpos;
  	else
  	{
    		invalidatebuf();
    		filebufpos=pos;
  	}
  	filepos=pos;
  	return filepos;
}

long cacheBinFile::resize(long pos)
{
	if(!canresize())
    		return BinFile::resize(pos);
  	invalidatebuf();
  	filelen=f->resize(pos);
  	filepos=filebufpos=f->tell();
  	return filelen;
}
