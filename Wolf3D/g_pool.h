#ifndef __GAME_POOL__
#define __GAME_POOL__

#include "g_object.h"

/////////////////////////////////////////////////////////////////////////////
//////Пул всех игровых объектов
/////////////////////////////////////////////////////////////////////////////
struct  PoolNode;
class   ErrorObject;
class   GamePool
{
  public:
                  GamePool  (int cl_num,int cl_size = GAME_CLASTER_SIZE);
         virtual  ~GamePool ();

/////////////////////////////////////////////////////////////////////////////
//////Создание объектов (и их удаление)
/////////////////////////////////////////////////////////////////////////////
    PoolIndex     CreateObject ();
    void          DeleteObject (PoolIndex);
    void          ClearAll     ();

/////////////////////////////////////////////////////////////////////////////
//////Служебные
/////////////////////////////////////////////////////////////////////////////
    BOOL          Belong         (void*) const;
    int           GetClasterSize ()      const  { return mClaster; }
    int           GetPoolSize    ()      const  { return mPoolSize; }
    int           GetClasterNum  ()      const  { return mClasterNum; }

/////////////////////////////////////////////////////////////////////////////
//////Доступ к объектам
/////////////////////////////////////////////////////////////////////////////
    GameObject*   GetObject    (PoolIndex);

  protected:
          ErrorObject*         mError;
          uchar*               mPool;     //Пул объектов
          PoolNode*            mPoolPtr;  //Указатели на объекты
          PoolIndex            mFirst;    //Первый объект в списке
          int                  mClaster,
                               mPoolSize,
                               mClasterNum;
};

//////////////////////////Пул игровых обектов////////////////////////////////

//extern  GamePool*       gamePool;

#endif