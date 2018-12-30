#ifndef __TYPE_GAME_ID__
#define __TYPE_GAME_ID__

#include <types.h>
#include <iostream.h>

/////////////////////////////////////////////////////////////////////////////
/////Описание ссылки на объект
/////////////////////////////////////////////////////////////////////////////
#pragma pack (1)
struct  ID
{
  int16          type;   //Тип объекта
  int16          id;     //Его индекс в таблице объектов

                 ID ()  {} //Внимание! Инициализация не должна производится!!
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