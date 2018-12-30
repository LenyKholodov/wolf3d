#ifndef __RING__
#define __RING__

#include "g_ttable.h"

struct  RingNode
{
  ID            id;     //Для кольца
  int16         next,
                prev;

  int16         nextFree;       //Для системы
};

        //Продумать удаление

/////////////////////////////////////////////////////////////////////////////
/////Закольцованный итератор
/////////////////////////////////////////////////////////////////////////////
struct  RingNode;
class   Ring: public GameObject
{
  public:
                                Ring (int max = MAX_RING_SIZE) ;
                                Ring (BinFile*);
                  virtual       ~Ring ();

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return RING; }
    virtual     void            dump (ostream&) const;

////////////////////////////////////////////////////////////////////////////
//////Работа с итератором
////////////////////////////////////////////////////////////////////////////
                BOOL            More ()   const;
                BOOL            IsEmpty() const { return mFirst == -1; }
                int             GetPos()  const { return mCurrent; }
                void            Reset();       //На начало списка
                ID              Next ();       //in active iterator
                ID              Prev ();       //in active iterator
                ID              Current () const;

    virtual     int             Add         (ID);
    virtual     void            Del         ();
    virtual     void            Del         (ID);
    virtual     void            ClearAll    ();

                int             count       () const { return mCount; }
                BOOL            IsBelong    (ID) const ;

  protected:
             int                mCount;
             int                mFirst;
             int                mFirstFree;
             int                mCurrent;
             RingNode*          mPool   ;
};

#endif