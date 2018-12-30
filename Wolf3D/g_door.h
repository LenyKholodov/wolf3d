#ifndef __GAME_DOOR__
#define __GAME_DOOR__

#include "g_triger.h"

class   Door: public Triger
{
  public:
                                Door(ArgPtr);
                                Door(BinFile*);
              virtual           ~Door ();

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     BOOL            Exec (GameMessage*);
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     GameObjects     type () const  { return DOOR; }
    virtual     void            dump (ostream&) const;

////////////////////////////////////////////////////////////////////////////
//////Собственно управление дверью
////////////////////////////////////////////////////////////////////////////
    virtual     void    On   ();
    virtual     void    Off  ();

  protected:
             DoorRes    doorRes;

             int        mDoorState;      //насколько открыта
             int        mDoorStep;
             int        mDoorWait;
};

#endif