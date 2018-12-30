#ifndef __ACTIVE_LIST__
#define __ACTIVE_LIST__

#include "g_ring.h"
#include "g_load.h"

typedef unsigned char priority_t;

/////////////////////////////////////////////////////////////////////////////
/////Список объектов, которые требуют периодической передачи управления
/////////////////////////////////////////////////////////////////////////////
class   ActiveList: public Ring
{
  public:
                        ActiveList (int max = MAX_RING_SIZE);
                        ActiveList (ArgPtr = NULL);
                        ActiveList (BinFile*);
    virtual             ~ActiveList ();

    virtual     BOOL    Exec (GameMessage*);

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     GameObjects     type () const  { return ACTIVE_LIST; }

    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);

////////////////////////////////////////////////////////////////////////////
//////Передача управления активным объектам
////////////////////////////////////////////////////////////////////////////
                void            NextTick        ();   //активный итератор
                void            TickAll         ();

    virtual     int             Add             (ID,priority_t = 0);


  protected:
             uchar*             mPriority ;
             int                mPos;
};

#endif