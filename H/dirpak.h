#ifndef __DIR_PAK_H__
#define __DIR_PAK_H__

#include <sbinfile.h>
#include <cbinfile.h>
#include <abinfile.h>
#include <pakbfile.h>

const int DIRPAK_ERROR_ID=ERROR_ID+0x300;

class DirPakError:public ErrorPro
{
public:
	DirPakError(void)	{ IdCode=DIRPAK_ERROR_ID; }
	virtual void dump(ostream&) const;
};

#pragma pack(1)

enum { PD_NORMAL=0,PD_PACK };

typedef struct
{
	char name[_MAX_PATH];
	long offs,
	     size,
	     rsize,
	     crc,
	     type;
} packDirEntry;

typedef struct
{
	ulong  signature;
	uint16 ver;
	ulong  diroffs;
	uint16 filecount;
	ulong  dircrc;
	ulong  crc;
} packDirHeader;

#pragma pack()

class DirPak
{
protected:
	BOOL dirty,read_only;

	stdBinFile    sbf;
	cacheBinFile  cache;
	pakBinFile   pak;
	aBinFile      sector;

	packDirEntry* dir;
	BinFile* dirFile;
	BinFile** files;
	int Count;
	ulong CRCRegion(long offs,long len);
	BOOL  _openDP(void);
public:
	DirPak(void)			{ dirty=FALSE; dirFile=NULL; files=NULL; dir=NULL; Count=0; }
	~DirPak(void)		    	{ closeDP(); }
	
	BOOL openedDP(void)   const { return dirFile!=NULL; }
	BinFile* opened(int i)
		 {  return (i>=0 && i<Count && files!=NULL)?files[i]:NULL; }

	BOOL openDP(const char* fname,long cSize=32768,BOOL ro=FALSE);
	BOOL openDP(BinFile* bf);
	void closeDP(void);

	BinFile* open(const char* fname,int mode=BinFile::OPEN_RO);
	BinFile* open(int index,int mode=BinFile::OPEN_RO);
	void close(BinFile* f);
	void close(int);
	void close(void);

	long FileCount(void)  const { return Count; }
	packDirEntry* GetDir(void) const { return dir; }
	BOOL FileExist(const char* fname) const { return FindEntry(fname)>=0; }
	int FindEntry(const char* fname) const;
	BOOL checkFile(int index);
	BOOL checkFile(const char* fname) { return checkFile(FindEntry(fname)); }
	BOOL check(void);
};

#endif