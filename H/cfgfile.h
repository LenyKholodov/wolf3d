#ifndef  __CONFIG_FILE__
#define  __CONFIG_FILE__

#include <misc.h>
#include <cbinfile.h>

enum{
  CFG_LONG=1,
  CFG_STRING,
  CFG_FLOAT,
  CFG_ANY
};

enum{
  MD_FORWARD=1,
  MD_BACK
};

typedef struct
{
 long CRC;
 char type;
 long idOffs,varOffs;
 long idLen,varLen;
}tVar;

class Config
{
  protected:
    char *bottom,*curBottom;
    tVar *top,*curTop;
    long  count;

    tVar*  BinFind(long CRC);
    tVar*  Find(const char* id);
    void   Shift(tVar* middle,int mode);
    char*  Insert(const char* id,int type,long len);
    tVar*  Insert(const long CRC);
    BOOL   LoadVar(char* string);
  public:
    Config():bottom(NULL),count(0) { New(300000);}
    Config(int size):bottom(NULL),count(0) { New(size); }
    virtual ~Config() { Remove(); }

    BOOL  SetVar(const char* id,void* var,int len);//any value
    BOOL  SetVar(const char* id,char* var);//string
    BOOL  SetVar(const char* id,long var);//long integer
    BOOL  SetVar(const char* id,float var);//float value

    void  Debug();
    long  GetCount() {return count;}

    BOOL  FindVar(const char* id);
    void* GetVar(const char* id,int& type);//for all values

    char* GetString(const char* id);
    long  GetLong(const char* id);
    float GetFloat(const char* id);

    void  DelVar(const char* id);
    void  New(const long size);
    void  Remove();

    BOOL  Save(BinFile* file);
    void  Save(const char* path);
    BOOL  Load(BinFile* file);
    BOOL  Load(const char* path);
};

#endif