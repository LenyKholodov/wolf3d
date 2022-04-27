#ifndef __A_BIN_FILE__
#define __A_BIN_FILE__

#include <binfile.h>

class aBinFile : public BinFile
{
protected:
	BinFile *f;
	long fofs;
public:
	aBinFile(void)		{ f=NULL; }
	aBinFile(BinFile* fil, long ofs, long len,int fmode=can_read|can_write|can_seek) { open(fil,ofs,len,fmode); }
	virtual ~aBinFile(void);

	BOOL open(BinFile* fil, long ofs, long len,int fm=can_read|can_write|can_seek);

	virtual long read(void *buf, long len);
	virtual long write(const void *buf, long len);
       	virtual void close(void);
	virtual long resize(long pos);
};


#endif