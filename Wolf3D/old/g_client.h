#ifndef __GAME_CLIENT__
#define __GAME_CLIENT__

#include "g_system.h"

/////////////////////////////////////////////////////////////////////////////
//////Клиент - интерфейс на главной машине
/////////////////////////////////////////////////////////////////////////////
class    GameClient: public GameObject
{
  public:
                                GameClient  (ArgPtr = NULL);
                                GameClient  (BinFile*);
            virtual             ~GameClient ();

            ID                  GetServer () const { owner->GetDispatch(); }

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return GAME_CLIENT; }
    virtual     void            dump (ostream&) const;
};

#endif