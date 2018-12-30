#ifndef __TYPE_GAME_ID__
#define __TYPE_GAME_ID__

#include <types.h>
#include <iostream.h>

/////////////////////////////////////////////////////////////////////////////
/////���ᠭ�� ��뫪� �� ��ꥪ�
/////////////////////////////////////////////////////////////////////////////
#pragma pack (1)
struct  ID
{
  int16          type;   //��� ��ꥪ�
  int16          id;     //��� ������ � ⠡��� ��ꥪ⮢

                 ID ()  {} //��������! ���樠������ �� ������ �ந��������!!
                 ID (int t,int x): type(t), id(x) {}
                 ID (const ID& x): id(x.id), type(x.type) {}

        operator =  (const ID& x) { id = x.id; type = x.type; }
        operator == (const ID& x) { return type == x.type && id == x.id; }
        operator != (const ID& x) { return type != x.type || id != x.id; }

        operator int () { return type != -1 && id != -1; } //??

 friend ostream& operator<<  (ostream&,const ID&);
};
#pragma pack (0)

static ostream& operator << (ostream& os,const ID& id)
{
  os<<"("<<id.type<<","<<id.id<<")";
  return os;
}

#endif