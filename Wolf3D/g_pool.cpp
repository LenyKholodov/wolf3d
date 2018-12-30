#include "g_pool.h"
#include "g_object.h"

#include <debug\debug.h>

/////////////////////Для устранения ошибок - отладка/////////////////////////

class   ErrorObject: public GameObject
{
  public:
                                ErrorObject () { SetAttr (OBJ_UNLOAD_PROTECT); }
               virtual          ~ErrorObject() {}

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*)        { dump (dout); }
    virtual     void            Save (BinFile*)        { dump (dout); }
    virtual     BOOL            Exec (GameMessage*)    { dump (dout); return FALSE; }

    virtual     GameObjects     type () const  { return ERROR_OBJECT; }
    virtual     void            dump (ostream& os) const
    {
      #ifdef __NO_OBJECT_MESSAGE__
        os<<"Game pool! Error in logic address"<<endl;
      #endif
    }

                void*   operator new (size_t,void* x) { return x; }
};

/////////////////////implementation of class GamePool////////////////////////

struct  PoolNode
{
  PoolIndex     data;
  PoolIndex     next;
};

PoolIndex       GamePool::CreateObject ()
{
  if (mFirst == -1)
#ifndef NDEBUG
  {
     dout<<"Game pool: Pool is full!"<<endl;
#endif
     return -1;
#ifndef NDEBUG
  }
#endif

  PoolNode  _free = mPoolPtr [mFirst];

  mFirst = mPoolPtr[mFirst].next;

  return _free.data;
}

void            GamePool::DeleteObject (PoolIndex obj)
{
  mPoolPtr [obj].next = mFirst;
  mFirst = obj;
}

void            GamePool::ClearAll     ()
{
  mFirst = 0;

  for (int i=0;i<mClasterNum;i++)
  {
    mPoolPtr [i].data = i;
    mPoolPtr [i].next = i+1;
  }

  mPoolPtr [i-1].next = -1;
  mPoolPtr [i-1].data = -1;

        //Для отладки
  mError = new (GetObject(CreateObject())) ErrorObject;
}

BOOL            GamePool::Belong (void* x) const
{
  return mPool<=x && x<=mPool+mPoolSize*mClaster;
}

GameObject*     GamePool::GetObject    (PoolIndex i)
{
#ifndef NDEBUG
  if (i < 0 || i >= mClasterNum)    return mError;
  else
#endif
  return (GameObject*)(mPool + mPoolPtr [i].data * mClaster);
}

GamePool::GamePool (int cl_num,int cl_size)
        : mFirst (0), mPool (new uchar [cl_size*cl_num]),
          mPoolPtr (new PoolNode [cl_size]),
          mClaster(cl_size), mPoolSize (cl_num),
          mClasterNum (cl_num)
{
  if (mPool == NULL || mPoolPtr == NULL)
  {
     dout<<"Game pool: No free memory"<<endl;
     return ;
  }

  ClearAll();
}

GamePool::~GamePool ()
{
  if (mPool)     delete mPool;
  if (mPoolPtr)  delete mPoolPtr;
}