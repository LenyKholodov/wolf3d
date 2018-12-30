#ifndef __GAME_SUBJECT__
#define __GAME_SUBJECT__

#include "g_object.h"

/////////////////////////////////////////////////////////////////////////////
//////Предмет (аптеска, оружие и т.д.)
////////////////////////////////////////////////////////////////////////////
class   Subject: public GameObject
{
  public:
                        Subject  (ArgPtr);
                        Subject  (BinFile*);
          virtual       ~Subject ();

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*)     ;
    virtual     void            Save (BinFile*)     ;
    virtual     BOOL            Exec (GameMessage*) ;

    virtual     GameObjects     type () const  { return SUBJECT; }
    virtual     void            dump (ostream&) const;

/////////////////////////////////////////////////////////////////////////////
/////Предмет можно взять, положить и использовать
/////////////////////////////////////////////////////////////////////////////
    virtual     BOOL            Include (ID player);
    virtual     void            Exclude ();

    virtual     void            Use     ()      = 0;

  protected:
                ID              playerOwner;    //игрок влаелец
};

#endif