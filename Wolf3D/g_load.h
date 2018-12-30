#ifndef __GAME_LOADER__
#define __GAME_LOADER__

#include "g_types.h"
#include "g_object.h"

/////////////////////////////////////////////////////////////////////////////
/////Тип make-function
/////////////////////////////////////////////////////////////////////////////

        //Как создавать объекты с параметрами пример двери

typedef GameObject* (*tLoadObject)(BinFile*,ID,GameSystem*);
typedef GameObject* (*tMakeObject)(ArgPtr,ID,GameSystem*);

/////////////////////////////////////////////////////////////////////////////
/////Массив make-functions для создания объектов требующих загрузки
/////////////////////////////////////////////////////////////////////////////

extern tMakeObject     makeFn [MAX_TYPES];
extern tLoadObject     loadFn [MAX_TYPES];

/////////////////////////////////////////////////////////////////////////////
/////Шаблон для регистрации make-function
/////////////////////////////////////////////////////////////////////////////

template <class ClassName,GameObjects ClassType>
class make
{
  public:
                                make  () {
                     makeFn [ClassType] = &makeObject;
                     loadFn [ClassType] = &loadObject;
                                          }

    static      GameObject*     loadObject (BinFile* file,
                                            ID id,
                                            GameSystem* owner = gameSystem)
          {  return new (id,ID(-1,-1),owner) ClassName (file);    }

    static      GameObject*     makeObject (ArgPtr ptr,
                                            ID callback = ID(-1,-1),
                                            GameSystem* owner = gameSystem)
          { return new (ClassType,callback,owner) ClassName (ptr); }

};

/////////////////////////////////////////////////////////////////////////////
////Функция регистрации нового игрового класса
/////////////////////////////////////////////////////////////////////////////

BOOL   RegisterGameClass (GameObjects,tMakeObject);


#endif