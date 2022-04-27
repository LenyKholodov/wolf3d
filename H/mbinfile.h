#ifndef __MEM_BINFILE_H__
#define __MEM_BINFILE_H__

#include <binfile.h>

class memBinFile : public BinFile
{
protected:
        BOOL  owner;
        char* buffer;
public:
        memBinFile(void)                { buffer=NULL; }
        virtual ~memBinFile(void);
        BOOL open(long size,BOOL crsize=TRUE);
        BOOL open(void* ptr,long size,BOOL ro=FALSE);

        void* GetBuffer(void)            { return buffer; }

        virtual void close(void);

        virtual long read(void *buf, long len);
        virtual long write(const void *buf, long len);
          virtual long resize(long);

// basic types io
           virtual int getc(void);
        virtual int gets(void);
          virtual long getl(void);
          virtual BinFile &putc(char c);
          virtual BinFile &puts(int16 s);
          virtual BinFile &putl(long l);
// Extended
           long        AssignFrom(BinFile* f)        { return AssignFrom(*f); }
           long        AssignFrom(BinFile&);
           long        AssignTo(BinFile* f)        { return AssignTo(*f); }
           long        AssignTo(BinFile&);
};

#endif