#ifndef __GAME_USER_PLAYER__
#define __GAME_USER_PLAYER__

#include "g_player.h"
#include "g_box.h"

/////////////////////////////////////////////////////////////////////////////
//Пользователь - игрок
/////////////////////////////////////////////////////////////////////////////
class UserPlayer: public Player
{
  public:
                                UserPlayer(ArgPtr = NULL);
                                UserPlayer(BinFile*);
                  virtual       ~UserPlayer();

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return USER_PLAYER; }
    virtual     void            dump (ostream&) const;

    virtual     void            tick        ();

////////////////////////////////////////////////////////////////////////////
//////Функции относящиеся к данному классу
////////////////////////////////////////////////////////////////////////////
                int             IsPresent    (int) const;

                void            LoadWeapon   (int cache,int id,int box = -1,int cnt = -1);
                void            SelectWeapon (int);
                int             NextWeapon   ();
                int             PrevWeapon   ();

                void            Fire         ();

                int             GetFrags     () const { return mFrags; }
                void            SetFrags     (int x) { mFrags = x; }

  protected:
                ID              CreateBox    (int,int,int);
                void            RefreshFace  ();

  protected:
                ID              mWeapons [USER_WEAPONS];
                GameBox*        mBoxes   [USER_WEAPONS];

                int             mCurWeapon;

    static      tPoint          mMoveTable [MAX_WEAPON_MOVE];
                int             mMovePos;
                int             mMoveLast;
                int             mFrags;
};

#endif
