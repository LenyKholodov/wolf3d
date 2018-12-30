#include <abinfile.h>

tAllocFnPtr aBinFile::m_alloc_fn=malloc;
tFreeFnPtr  aBinFile::m_free_fn=free;

aBinFile::~aBinFile(void)
{
	close();
}

BOOL aBinFile::open(BinFile* fil, long ofs, long len,int fm)
{
   debug("aBinFile::open-starting("<<fil<<",offs="<<ofs<<",len="<<len<<",fm="<<fm<<")\n");
   if(fil==NULL || fil->bad())
		return FALSE;
  	close();
  	int fmode=fil->getmode();
  	if(!fmode||!(fmode&can_seek))
    	   	return FALSE;
  	filelen=len;
  	long l=(f=fil)->length();
	fofs=(ofs>l)?l:ofs;
  	if((fofs+filelen)>l)
    		filelen=l-fofs;
  	mode=fmode&fm;
  	return TRUE;
}

long aBinFile::read(void *buf, long len)
{	
  	if(!canread())
		return BinFile::read(buf,len);
  	if((filepos+len)>filelen)
    		len=filelen-filepos;
  	f->seek(fofs+filepos);
  	len=f->read(buf, len);
  	filepos+=len;
  	return len;
}

long aBinFile::write(const void *buf, long len)
{
	if(!canwrite())
		return BinFile::write(buf,len);
  	if((filepos+len)>filelen && (!canresize()))
    		len=filelen-filepos;
  	f->seek(fofs+filepos);
  	len=f->write(buf,len);
  	if((filepos+=len)>filelen)
    		filelen=filepos;
  	return len;
}

long aBinFile::resize(long pos)
{
  	if(!canresize())
    		return BinFile::resize(pos);
	if(f->size()<pos+fofs)
		pos=f->resize(pos+fofs)-fofs;
	return filelen=pos;
}

void aBinFile::close(void)
{
	f=NULL;
	BinFile::close();
}
