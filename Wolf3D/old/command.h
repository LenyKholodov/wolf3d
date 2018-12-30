#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <misc.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <iomanip.h>
#include <malloc.h>

enum PARM_TYPE {PT_STRING,PT_ZSTRING,
     	        PT_WORD,
     	        PT_INT,PT_FLOAT};

typedef struct
{
  int length;
  char* data;
} SubStrData;

typedef struct
{
  int ptype;
  union
  {
    SubStrData s;
    char*      zs;
    SubStrData w;
    long       i;
    double     f;
  } data;
} parmUnit;

class ParmStack
{
protected:
  int mSize,maxSize;
  parmUnit* mParams;  
public:
  void close(void) 
      { if(mParams!=NULL) { ::free(mParams); mParams=NULL; } mSize=maxSize=0; }
  ParmStack(void) 	  { mParams=NULL; close(); }
  ParmStack(int n) 	  { mParams=NULL; maxsize(n); }
  ~ParmStack(void)        { close(); }
  int maxsize(void) const { return maxSize; }
  int maxsize(int n) 
      { close(); 
        if((mParams=(parmUnit*)malloc(n*sizeof(parmUnit)))!=NULL) maxSize=n;
	return maxSize; }
  void clear(void) 	  { mSize=0; }
  parmUnit& operator[](int i) { return mParams[i]; }
  int size(void) const { return mSize; }
  parmUnit& top(void)  	      { return mParams[mSize-1]; }
  BOOL append(void)            
       { if(mSize<maxSize) { mSize++; return TRUE;} return FALSE; }
};

class VarPool;

typedef void (*tCmdFn)(VarPool&,ParmStack&,ostream&);

#define NAME_SIZE 32
#define VALUE_SIZE 128
#define NOTHING	   -1

enum VarType { VT_FREE,
               VT_INT,VT_FLOAT,VT_STRING,
	       VT_PINT,VT_PFLOAT,
	       VT_FUNC,
	       VT_MACRO };

typedef struct
{
  uint left,owner,right;
  VarType type;
  long crc;
  char name[NAME_SIZE];
  union
  {
    long   i;		
    double f;
    char   s[VALUE_SIZE];
    long   *pi;
    double *pf;
    char   m[VALUE_SIZE];
    tCmdFn func;
  } value;
} VarUnit;

typedef struct
{
  int var:1;
  int macro:1;
  int func:1;
} ViewAttr;     

class VarPool
{
protected:
  uint mCount,		// размер буфера
      mFirstVar,	// индекс первого блока в дереве
      mVarCount,	// кол-во переменных
      mFirstFree;	// индекс первого свободного блока
  uint AllocUnit(void); 
  BOOL FreeUnit(uint i);
  void restoreFreeChunks(void);
  void listChain(uint i,ostream& os,ViewAttr attr);
  void listChain2(uint i,ostream& os);
  long crc(const char*) const;
  uint insert(const char* name,long crcn,uint owner);
  static int sort_function(const void*,const void*);
  uint makethree(uint start,uint end,uint owner);
  VarUnit* mPool;	// указатель на буфер
  BOOL find(const char* name,long& crcn,uint& i);
public:
  VarUnit* find(const char* name)
  	   { long c; uint i; return find(name,c,i)?mPool+i:NULL; }
  long unitsize(void) const { return sizeof(VarUnit); }
  uint varcount(void) const { return mVarCount; }
  VarPool(void) 	    
  	{ mPool=NULL; mVarCount=mCount=0; mFirstVar=mFirstFree=NOTHING; }
  VarPool(int n)  	    
        { mPool=NULL;mVarCount=mCount=0;mFirstVar=mFirstFree=NOTHING;open(n);}
  void clear(void);
  BOOL open(uint count)	    { clear(); return resize(count); }
  BOOL resize(uint count);
  VarPool(uint count);
  void close(void);
  void free(void) 	    { close(); }
  ~VarPool(void)	    { close(); }
  void pack(void);
  void dump(ostream& os);
  void list(ostream& os,ViewAttr attr);
  void list(ostream& os) { ViewAttr a; a.macro=a.func=0; a.var=1; list(os,a); }
  BOOL set(const char* name,const char* value);
  BOOL set(const char* name,long value);
  BOOL set(const char* name,int value)	 { return set(name,long(value)); }
  BOOL set(const char* name,double value);
  BOOL set(const char* name,float value) { return set(name,double(value)); }
  BOOL setf(const char* name,tCmdFn fn);
  BOOL setm(const char* name,const char* macro);
  BOOL link(const char* name,long*);
  BOOL link(const char* name,double*);
  BOOL exist(const char* name) { long c; uint i; return find(name,c,i); }
  BOOL kill(const char*);
  BOOL gets(const char*,char*);
  BOOL getm(const char*,char*);
  BOOL geti(const char*,long*);
  BOOL getf(const char*,double*);
  friend ostream& operator<<(ostream& os,VarPool& vp) 
  	 { vp.listChain2(vp.mFirstVar,os); return os; }
};


#endif