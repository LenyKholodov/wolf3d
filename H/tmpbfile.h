#ifndef __TMP_BINFILE_H__
#define __TMP_BINFILE_H__

#include <sbinfile.h>

class tmpBinFile : public stdBinFile
{
public:
	tmpBinFile(void);
	virtual ~tmpBinFile(void) 	{ close(); }

	BOOL open(void)		{ return open(tempname(NULL)); }
	BOOL open(const char *name) { return stdBinFile::open(name,BinFile::OPEN_CR); }

	virtual void close(void);
};

#endif
