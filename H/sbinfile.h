#ifndef __STD_BINFILE_H__
#define __STD_BINFILE_H__

#include <binfile.h>

class stdBinFile : public BinFile
{
protected:
	int handle;
	char fname[_MAX_PATH];
	void SetErrorInfo(int code) const { BinFile::SetErrorInfo(code); strcpy(bfError.filename,fname); bfError.ErrXCode=errno; }
public:
	stdBinFile(void):BinFile()			  {}
	stdBinFile(const char *name, int type):BinFile()  { open(name,type); }
	virtual ~stdBinFile(void);
	BOOL open(const char *name, int type);
        virtual void close(void);
	virtual long read(void *buf, long len);
	virtual long write(const void *buf, long len);
	virtual long seek(long pos);
	virtual long resize(long pos);
};

#endif