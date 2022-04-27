#ifndef __CBINFILE_H__
#define __CBINFILE_H__

#include "binfile.h"

class cacheBinFile: public BinFile
{
protected:
	BinFile *f;
	char *buffer;
	int buflen, bufpos, bufread;
  	long filebufpos, fileseekpos;
  	BOOL dirty;
  	void invalidatebuf(void);
public:
  	cacheBinFile(void)			{}
	cacheBinFile(BinFile* fil, int len)	{ open(fil,len); }	
	cacheBinFile(BinFile& fil, int len)	{ open(fil,len); }
	virtual ~cacheBinFile(void);
	BOOL open(BinFile* fil, int len);
	BOOL open(BinFile& fil, int len)	{ return open(&fil,len); }
	virtual void close(void);
	virtual long read(void *buf, long len);
	virtual long write(const void *buf, long len);
	virtual long seek(long pos);
	virtual long resize(long pos);
};

#endif
