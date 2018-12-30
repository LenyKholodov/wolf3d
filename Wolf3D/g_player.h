#ifndef __GAME_PLAYER__
#define __GAME_PLAYER__

#include "g_types.h"
#include "g_object.h"
#include "g_box.h"
#include "g_load.h"

#include "map.h"

#include <fixed.h>
#include <time.h>

/////////////////////////////////////////////////////////////////////////////
//////Интерфейс игрока (бот, плейер, лаймер)
/////////////////////////////////////////////////////////////////////////////
class   Player: public GameObject
{
  public:
                Player  (ArgPtr);
                Player  (BinFile*);
     virtual    ~Player ();

                enum Actives {
                     FORWARD = 1,
                     BACK,
                     TURN_LEFT,
                     TURN_RIGHT,
                     STRAFE_LEFT,
                     STRAFE_RIGHT,
                     FIRE
                };

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return GAME_PLAYER; }
    virtual     void            dump (ostream&) const;

////////////////////////////////////////////////////////////////////////////
//////Интерфейс работы (дублируется системой сообщений)
////////////////////////////////////////////////////////////////////////////
                BOOL            GoForward   (Fixed dist = FX_ONE);
                BOOL            GoBack      (Fixed dist = FX_ONE);
                void            TurnRight   (Angle = ANGLE_1);
                void            TurnLeft    (Angle = ANGLE_1);
                BOOL            StrafeRight (Fixed = FX_ONE);
                BOOL            StrafeLeft  (Fixed = FX_ONE);
                void            Press       ();
                BOOL            SetPos      (Fixed,Fixed,Angle = 0);
                Fixed           GetX        () const { return mX;     }
                Fixed           GetY        () const { return mY;     }
                Angle           GetAngle    () const { return mAngle; }

                int             Skin        () const { return playerRes.mapSprite; }

   virtual      void            tick        ();

  protected:
             PlayerRes    playerRes;
             int          mStatePos;
             int          mState;
             BOOL         mDeath;

             Fixed        mX, mY;
             Angle        mAngle;

             GameBox      mHealth,
                          mProtect;

             clock_t      mLast;

             signed char  mActives [PLAYER_MAX_ACTIVES]; //возможные действия

             Block        mBlock,
                          mOldBlock;

             ID           mOldId;
};


#endif