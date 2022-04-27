#ifndef __WDT_ERROR__
#define __WDT_ERROR__
#include <types.h>
#include <mobject.h>

CLASSDEF(ErrorPro);

extern ErrorPro* CurrentError;

const int ERROR_ID=0x1000;

class ErrorPro: virtual public WDT_Object
{
public:
	int ErrCode;
	ErrorPro(int code=0):ErrCode(code) { IdCode=ERROR_ID; }
	virtual void dump(ostream&) const;
	void message(ostream& os) const	{ dump(os); }
	int get(void) const { return ErrCode; }
	friend ostream& operator<<(ostream&,const ErrorPro*);
};

inline ostream& operator<<(ostream& os,const ErrorPro* err)
{
	if(err!=NULL)
		err->dump(os);
	else
		os<<"All right";
	return os;
}

const char* DosErrStr(int DosErrCode);

#endif
