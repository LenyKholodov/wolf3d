#ifndef __BINFILE_H__
#define __BINFILE_H__

#include <error.h>
#include <misc.h>

#ifdef getc
  #undef getc
#endif
#ifdef putc
  #undef putc
#endif

const int BINFILE_ERROR_ID=ERROR_ID+0x0200;

enum { BFE_NO_ERROR=0,
       BFE_READ,
       BFE_WRITE,
       BFE_SEEK,
       BFE_RESIZE,
       BFE_STD_OPEN,
       BFE_STD_CLOSE,
       BFE_STD_READ,
       BFE_STD_WRITE,
       BFE_STD_SEEK,
       BFE_STD_RESIZE,
       BFE_NOMEM
     };

class BinFileError: public ErrorPro
{
public:
	int  ErrXCode;
	char filename[_MAX_PATH];
	BinFileError(void) { IdCode=BINFILE_ERROR_ID; *filename=0; }
	virtual void dump(ostream&) const;
};

extern BinFileError bfError;

class BinFile
{
protected:
	int mode;
	long filepos,
	     filelen;
	void SetErrorInfo(int code) const { CurrentError=&bfError; bfError.ErrXCode=errno; bfError.ErrCode=code; }
public:
  enum { 
       	 can_read  =0x01, 
	 can_write =0x02, 
	 can_seek  =0x04, 
	 can_chsize=0x08,
	 can_resize=0x08,
	 can_rw    =0x03
	};
  enum { OPEN_RO=0, 
       	 OPEN_RW, 
	 OPEN_CR,
	 OPEN_WPAK // only DirPak
	};

       	BinFile(void)	    	   { BinFile::close(); }
       	virtual ~BinFile(void);

       	virtual void close(void);

// abstract functions
       	virtual long read(void *buf, long len);
	virtual long write(const void *buf, long len);
  	virtual BOOL reset(void);
  	virtual long seek(long pos);
  	virtual long resize(long pos);

// basic types io
   	virtual int getc(void);
  	virtual int gets(void);
  	virtual long getl(void);
  	virtual BinFile &putc(char c);
  	virtual BinFile &puts(int16 s);
  	virtual BinFile &putl(long l);

   	BinFile& operator[](long pos)    { seek(pos); return *this; }

  	int      getmode(void)  const 	 { return mode; }
	BOOL	 bad(void)	const	 { return mode==0; }
       	long     size(void)     const  	 { return filelen; }
	long 	 length(void)	const	 { return filelen; }
       	long     size(long pos)	  	 { return resize(pos); }
	long 	 seekcur(long pos)  	 { return seek(filepos+pos); } 
  	long 	 seekend(long pos)	 { return seek(filelen+pos); }
	long 	 tell(void)     const  	 { return filepos;           }		
	long 	 getpos(void)	const  	 { return filepos;           }
	BOOL 	 eof(void)	const    { return filepos==filelen;  }
	BOOL	 canread(void)	const	 { return mode&can_read;     }
	BOOL	 canwrite(void)	const	 { return mode&can_write;    }
	BOOL	 canseek(void)	const	 { return mode&can_seek;     }
	BOOL	 canresize(void)const	 { return mode&can_resize;   }
};

#endif
