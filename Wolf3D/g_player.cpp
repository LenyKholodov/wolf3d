#include "g_player.h"
#include "g_system.h"
#include "g_triger.h"
#include "g_weap.h"
#include "g_anim.h"

#include "fin.h"
#include "defs.h"

#include <tables.h>
#include <string.h>

#include <debug\debug.h>

make<Player,GAME_PLAYER> makePlayer;

void   Player::Load (BinFile* file)
{
  GameObject::Load (file);

  PlayerTag     tag;

  file->read (&tag,sizeof(tag));

  playerRes     = tag.res;

  mX            = tag.exempl.x;
  mY            = tag.exempl.y;
  mAngle        = tag.exempl.angle;

  mBlock        = tag.mBlock;
  mOldBlock     = tag.mOldBlock;

  mOldId        = tag.mOldId;

  mStatePos     = tag.mStatePos;
  mState        = tag.mState;
  mDeath        = tag.mDeath;

  mHealth.Load  (file);
  mProtect.Load (file);

                //Очищаем таблицу активных состояний

  memset (mActives,0,PLAYER_MAX_ACTIVES*sizeof(uchar));

                //устанавливаем часы

  mLast = clock();
}

void   Player::Save (BinFile* file)
{
  GameObject::Save (file);

  PlayerTag     tag;

  tag.res           = playerRes;

  tag.exempl.x      = mX;
  tag.exempl.y      = mY;
  tag.exempl.angle  = mAngle;

  tag.mBlock        = mBlock;
  tag.mOldBlock     = mOldBlock;

  tag.mOldId        = mOldId;

  tag.mStatePos     = mStatePos;
  tag.mState        = mState;
  tag.mDeath        = mDeath;

  file->write (&tag,sizeof(tag));

  mHealth.Save  (file);
  mProtect.Save (file);
}

BOOL   Player::Exec (GameMessage* mes)
{
  PlayerMessage*     message = (PlayerMessage*)mes;

  switch (message->id)
  {
    case RESET_ACTIVE:
    {
      memset (mActives,0,PLAYER_MAX_ACTIVES*sizeof(uchar));
      return TRUE;
    }
    case ADD_MOVE_FORWARD: mActives[FORWARD]++;      return TRUE;
    case SUB_MOVE_FORWARD: mActives[FORWARD]--;      return TRUE;
    case ADD_MOVE_BACK:    mActives[BACK]++;         return TRUE;
    case SUB_MOVE_BACK:    mActives[BACK]--;         return TRUE;
    case ADD_TURN_LEFT:    mActives[TURN_LEFT]++;    return TRUE;
    case SUB_TURN_LEFT:    mActives[TURN_LEFT]--;    return TRUE;
    case ADD_TURN_RIGHT:   mActives[TURN_RIGHT]++;   return TRUE;
    case SUB_TURN_RIGHT:   mActives[TURN_RIGHT]--;   return TRUE;
    case ADD_STRAFE_LEFT:  mActives[STRAFE_LEFT]++;  return TRUE;
    case SUB_STRAFE_LEFT:  mActives[STRAFE_LEFT]--;  return TRUE;
    case ADD_STRAFE_RIGHT: mActives[STRAFE_RIGHT]++; return TRUE;
    case SUB_STRAFE_RIGHT: mActives[STRAFE_RIGHT]--; return TRUE;
    case ALREADY_DELETE:   return TRUE;
    case ALREADY_CREATE:   return TRUE;
    case PLAYER_MOVE:
    {
      PlayerMessage* message = (PlayerMessage*)mes;

      TurnRight (message->angle);
      GoForward (message->dist);

      return TRUE;
    }

    case TELEPORT_TO:
    {
      TeleportToMessage* message = (TeleportToMessage*)mes;

      SetPos (int2fixed(message->x),int2fixed(message->y),GetAngle());

      return TRUE;
    }
    case CHANGE_LEVEL:
    {
      return TRUE;
    }

    case ECHO_ON:          owner->PutMessage (mes,callBack,sizeof(EchoOnMessage)); return TRUE;
    case I_SEE_YOU:        owner->PutMessage (mes,callBack,sizeof(PlayerISeeYou)); return TRUE;
    case HEY:
    {
       PlaySoundMessage message (playerRes.sound[PlayerRes::HEY],
                                 fixed2int(GetX()),
                                 fixed2int(GetY()));

       owner->PutMessage (&message,sizeof(message),id());

       return TRUE;
    }

    case PRESS:         Press ();    return TRUE;
    case GET_PLAYER_INFO:
    {
         GetPlayerInfoMessage*  callInfo = (GetPlayerInfoMessage*)mes;
         PlayerInfoMessage      info (mX,mY,mAngle,
                                      mHealth.count(),mProtect.count(),
                                      playerRes.mapSprite);

         owner->PutMessage (&info,callInfo->destId,sizeof(info),id());

         return TRUE;
    }
    case LUCKY_SHOT:
    {
         PlaySoundMessage message (playerRes.sound[PlayerRes::LUCK],
                                   fixed2int(GetX()),
                                   fixed2int(GetY()));

         owner->PutMessage (&message,sizeof(message));
         owner->PutMessage (mes,callBack,sizeof(PlayerLuckyShot));

         return TRUE;
    }
    case PLAYER_DEATH:
    {
         if (mDeath)
            return TRUE;

         mDeath    = TRUE;      //Умер!!!
         mState    = PlayerRes::DEATH;
         mStatePos = 0;

         return TRUE;
    }
    case CROSS_SHOT:
    {
         if (mDeath)
            return TRUE;

         PlayerCrossShot*       message = (PlayerCrossShot*)mes;

                //Уменьшаем здоровбе, потрошим броню
         if (mProtect.count()>0)
         {
            mProtect.SubStore  (message->subHealth>>1);
            mHealth.SubStore   (message->subHealth>>1);
         }
         else
            mHealth.SubStore  (message->subHealth);

                //Будим того, кто так нами управляет
         PlayerSleepOff  sleepOff (message->player);

         owner->PutMessage (&sleepOff,callBack,sizeof (sleepOff));

                //А что если игрок уже мёртв?
         if (mHealth.count() <= 0)
         {
                  //Поздравляем победителя
            PlayerLuckyShot   mes1 (id());

            owner->PutMessage (&mes1,message->player,sizeof(mes1));

              //Поздравляем того, кто нами управлял
            PlayerDeath       mes2 (message->player);

            owner->PutMessage (&mes2,callBack,sizeof(mes2));

              //Умер!!!
            mDeath    = TRUE;
            mState    = PlayerRes::DEATH;
            mStatePos = 0;

            return TRUE;
         }
         else           //Просто попали
         {
            mState    = PlayerRes::CROSS_SHOT;
            mStatePos = 0;
         }

                //Ну и конечно покричим с тоски
         PlaySoundMessage play (playerRes.sound[PlayerRes::CROSS_SHOT],
                                fixed2int(GetX()),
                                fixed2int(GetY()));

         owner->PutMessage (&play,sizeof(play));

         return TRUE;
    }
    case TICK:
    {
      tick();

      return TRUE;
    }
    default:                                     return FALSE;
  }
}

void   Player::dump (ostream& os) const
{
  os<<"Player "<<id()
    <<" X = "<<fixed2float(mX)
    <<" Y = "<<fixed2float(mY)
    <<" Angle = "<<angle2int(mAngle)
    <<" Health "<<mHealth.count()
    <<" Protect "<<mProtect.count()
    <<endl;
}

BOOL   Player::GoForward   (Fixed dist)
{
  Fixed     x = mX + mul(dist,cos(mAngle)),
            y = mY + mul(dist,sin(mAngle));
            y = mY + mul(dist,sin(mAngle));

  return SetPos (x,y,mAngle);
}

BOOL   Player::GoBack      (Fixed dist)
{
  return GoForward (-dist);
}

void   Player::TurnLeft    (Angle  rot)
{
  mAngle                  -= rot;

  mBlock.id.sprite.alpha  = (mAngle>>13);

  owner->SetBlock (Index(fixed2int(mY),fixed2int(mX)),mBlock);
}

void   Player::TurnRight   (Angle rot)
{
  TurnLeft (-rot);
}

BOOL   Player::StrafeLeft  (Fixed dist)
{
  mAngle -= ANGLE_90;
  BOOL state = GoForward (dist);
  mAngle += ANGLE_90;

  return state;
}

BOOL   Player::StrafeRight (Fixed dist)
{
  return StrafeLeft (-dist);
}

void    Player::Press ()
{
  Fixed x = mX + cos(mAngle),
        y = mY + sin(mAngle);

  TrigerPressMessage press (AngleToSide (mAngle-ANGLE_180) );

  owner->SendMessage (Index(fixed2int(y),fixed2int(x)),&press,sizeof(press));
}

BOOL   Player::SetPos      (Fixed x,Fixed y,Angle angle)
{
  if (x<0 || y<0 || x>int2fixed(MAX_X_TILE) || y>int2fixed(MAX_Y_TILE))
     return FALSE;

  int       index    = Index(fixed2int(y),fixed2int(x));
  int       oldIndex = Index(fixed2int(mY),fixed2int(mX));

  if (oldIndex == index)
  {
    mX                       = x;
    mY                       = y;
    mAngle                   = angle;

    mBlock.id.sprite.xfrac   = frac(x)>>12;
    mBlock.id.sprite.yfrac   = frac(y)>>12;

    owner->SetBlock (index,mBlock);
    owner->SetObject (index,id());

    return TRUE;
  }
  else if (owner->IsCross(index))
  {
    mX                       = x;
    mY                       = y;
    mAngle                   = angle;

    mBlock.id.sprite.xfrac   = frac(x)>>12;
    mBlock.id.sprite.yfrac   = frac(y)>>12;

    ID      _oldId           = owner->GetObject (index);
    Block   _oldBlock        = *owner->GetBlock (index);

    owner->SetBlock (index,mBlock);
    owner->SetObject (index,id());

    owner->SetBlock (oldIndex,mOldBlock);
    owner->SetObject (oldIndex,mOldId);

    mOldId    =  _oldId;
    mOldBlock = _oldBlock;

        //Если под нами пуля - не выкрутишься
    if (mOldBlock.type == Map::SPRITE &&
        owner->GetObject(mOldId)->type() == SHOT)
    {
      mOldBlock.type  = Map::FREE;
      mOldBlock.cross = 1;
    }

    return TRUE;
  }

  return FALSE;
}

void            Player::tick        ()
{
  if      (mActives [FORWARD] > mActives[BACK]) GoForward  (playerRes.speed);
  else if (mActives [FORWARD] < mActives[BACK]) GoForward  (-playerRes.speed);

  if      (mActives [TURN_LEFT] > mActives[TURN_RIGHT]) TurnLeft   (PLAYER_TURN);
  else if (mActives [TURN_LEFT] < mActives[TURN_RIGHT]) TurnLeft   (-PLAYER_TURN);

  if      (mActives [STRAFE_LEFT] > mActives[STRAFE_RIGHT]) StrafeLeft  (playerRes.speed);
  else if (mActives [STRAFE_LEFT] < mActives[STRAFE_RIGHT]) StrafeLeft  (-playerRes.speed);

                //управление консолью игрока
  for (int i=0;i<PLAYER_MAX_ACTIVES;i++)
      if (mActives [i] < 0 )
         mActives [i] = 0;

  if (mDeath && mState == -1)
  {
    delete this;

    return;
  }

  if (mState == ANIM_DEFAULT_FACE) mState = PlayerRes::NORM;

                //анимация

  if (clock() - mLast > PLAYER_CHANGE_FACE_TIME)
  {
    int index               = Index(fixed2int(mY),fixed2int(mX));

    mBlock.id.sprite.face   = NextFace (playerRes.sprite[mState],mStatePos,mState);

    owner->SetBlock (index,mBlock);

    mLast = clock();
  }
}

Player::Player  (ArgPtr ptr)
       :mHealth  (((CreatePlayer*)ptr)->param().health,100),
        mProtect (((CreatePlayer*)ptr)->param().protect,100),
        mStatePos (0), mState (-1), mDeath (FALSE)
{
  CreatePlayer* arg        = (CreatePlayer*)ptr;

  playerRes                = arg->resourceClass;

  mX                       = arg->param().x;
  mY                       = arg->param().y;
  mAngle                   = arg->param().angle;

  mBlock.type              = Map::MONSTR;
  mBlock.id.sprite.z       = SPRITE_BOTTOM;
  mBlock.id.sprite.id      = arg->resourceClass.mapSprite;
  mBlock.id.sprite.alpha   = mAngle>>13;
  mBlock.id.sprite.already = 0;
  mBlock.id.sprite.face    = 0;
  mBlock.id.sprite.xfrac   = frac(mX)>>12;
  mBlock.id.sprite.yfrac   = frac(mY)>>12;
  mBlock.cross             = 0;

  int index = Index(fixed2int(mY),fixed2int(mX));

  mOldBlock = * owner->GetBlock       (index);
  mOldId    = owner->GetObject  (index);

  owner->SetBlock        (index,mBlock);
  owner->SetObject       (index,id());

  owner->AddActive (id(),PR_PLAYER);

  memset (mActives,0,PLAYER_MAX_ACTIVES*sizeof(uchar));

                //Устанавливаем счётчик
  mLast = clock();
}

Player::Player  (BinFile* file)
{
  Load (file);
}

Player::~Player ()
{
  owner->DelActive (id());

  int       index = Index(fixed2int(mY),fixed2int(mX));

        //установить текстуры умершего
  Block              block = mBlock;

  block.type           = Map::SPRITE;
  block.cross          = 1;

  for (int i=0;i<ANIM_NUM && playerRes.sprite[PlayerRes::DEATH][i]!=NOSPRITE;i++);

  i>0 ? block.id.sprite.face = playerRes.sprite[PlayerRes::DEATH][i-1] :
        block.id.sprite.face = PLAYER_DIE;

  owner->SetBlock   (index,block);
  owner->SetObject  (index,mOldId);

        //звук смерти

  PlaySoundMessage play (playerRes.sound[PlayerRes::DEATH],
                         fixed2int(GetX()),
                         fixed2int(GetY()));

  owner->PutMessage (&play,sizeof(play));
}
