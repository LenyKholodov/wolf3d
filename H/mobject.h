#ifndef __WDT_OBJECT_H__
#define __WDT_OBJECT_H__

#include <iostream.h>

class WDT_Object
{
protected:
	int IdCode;
public:
	virtual void dump(ostream&) const=NULL;
	BOOL Is(int Code) const { return IdCode==Code; }
	friend ostream& operator<<(ostream&,const WDT_Object&);
};

inline ostream& operator<<(ostream& os,const WDT_Object& obj)
{
	obj.dump(os);
	return os;
}

#endif