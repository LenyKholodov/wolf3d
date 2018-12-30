#include <binfile.h>

BinFileError bfError;

tAllocFnPtr BinFile::m_alloc_fn=malloc;
tFreeFnPtr  BinFile::m_free_fn=free;

void BinFileError::dump(ostream& os) const
{
	os<<"BinFile error: ";
	switch(ErrCode)
	{
		case BFE_NO_ERROR:    os<<"no errors!"<<endl; break;
		case BFE_READ:        os<<"read unavailable."<<endl; break;
		case BFE_WRITE:       os<<"write unavailable."<<endl; break;
		case BFE_SEEK: 	      os<<"seek unavailable."<<endl; break;
		case BFE_RESIZE:      os<<"resize unavailable."<<endl; break;		
       		case BFE_STD_OPEN:    os<<"Can't open file '"<<filename<<"'. "
			     		<<strerror(ErrXCode)<<endl; break;
       		case BFE_STD_CLOSE:   os<<"Can't close file '"<<filename<<"'. "
			     		<<strerror(ErrXCode)<<endl; break;
       		case BFE_STD_READ:os<<"Can't read file '"<<filename<<"'. "
			     		<<strerror(ErrXCode)<<endl; break;
       		case BFE_STD_WRITE:os<<"Can't write file '"<<filename<<"'. "
			     		<<strerror(ErrXCode)<<endl; break;
		case BFE_STD_SEEK: os<<"Can't seek file '"<<filename<<"'. "
			     		<<strerror(ErrXCode)<<endl; break;
       		case BFE_STD_RESIZE:os<<"Can't chsize file '"<<filename<<"'. "
			     		<<strerror(ErrXCode)<<endl; break;
		case BFE_NOMEM:	    os<<"no memory."<<endl; break;
	}
}

BinFile::~BinFile(void)
{
	close();
}

void BinFile::close(void)
{
	filepos=filelen=mode=0;
}

long BinFile::read(void*,long)
{
	SetErrorInfo(BFE_READ);
	return 0;
}

long BinFile::write(const void *, long)
{
	SetErrorInfo(BFE_WRITE);
	return 0;
}

BOOL BinFile::reset(void)
{
	return seek(0)==0;
}

long BinFile::seek(long pos)
{
	if(canseek())
  	{
		if(pos<0)
    	   		pos=0;
  		if(pos>filelen)
    			pos=filelen;
		filepos=pos;
	}
	else
		SetErrorInfo(BFE_SEEK);
	return filepos;
}

long BinFile::resize(long)
{
	SetErrorInfo(BFE_RESIZE);
	return filelen;
}

int BinFile::getc(void)
{
	char c;
	return read(&c,sizeof(c))?c:EOF;
}

int BinFile::gets(void)
{
	int16 s=0;
	return read(&s,sizeof(s))?s:EOF;
}

long BinFile::getl(void)
{
	long l=0;
	return read(&l, sizeof(l))?l:EOF;
}

BinFile &BinFile::putc(char c)
{
	write(&c, 1);
	return *this;
}

BinFile &BinFile::puts(int16 s)
{
	write(&s, 2);
	return *this;
}

BinFile &BinFile::putl(long l)
{
	write(&l, 4);
	return *this;
}