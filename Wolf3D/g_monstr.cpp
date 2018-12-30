#include "g_monstr.h"
#include "t_wplr.h"
#include "g_system.h"
#include "g_iq.h"

#include "cross.h"

#include <tables.h>
#include <stdlib.h>

int     HARD_LEVEL = 0;

make<Monstr,GAME_MONSTR>        makeMonstr;

AtackTarget                     atackTarget;
FarFollowTarget                 farFollowTarget;
NearFollowTarget                nearFollowTarget;
ViewOut                         viewOut;
RandomMove                      randomMove;

static GameIQ*                  Monstr::table [IQ_MAX_PROBS] =
{
        &atackTarget,
        &farFollowTarget,
        &nearFollowTarget,
        &viewOut,
        &randomMove
};

////////////////////////implementation of class Men//////////////////////////

void            Men::Load (BinFile* file)
{
  GameObject::Load (file);

  file->read (&mPlayer,sizeof(mPlayer));
  file->read (&callBack,sizeof(callBack));
}

void            Men::Save (BinFile* file)
{
  GameObject::Save (file);

  file->write (&mPlayer,sizeof(mPlayer));
  file->write (&callBack,sizeof(callBack));
}

BOOL            Men::Exec (GameMessage* mes)
{
  switch (mes->id)
  {
    case TICK:
    {
      if (player())     //???
         tick ();

      return TRUE;
    }
    case SLEEP_OFF: sleep (TRUE,((PlayerSleepOff*)mes)->player); return TRUE;
    case PLAYER_DEATH:
    {
      owner->DelActive (id());

      mPlayer = ID(-1,-1);

      return TRUE;
    }
    case ALREADY_DELETE:
    {
      AlreadyDeleteMessage * message = (AlreadyDeleteMessage *)mes;

      dout<<"Monstr: Delete monstr "<<id()<<" player "<<player()<<endl;

      if (message->obj == player())
      {
         mPlayer = ID(-1,-1);
         delete this;
      }

      dout<<"Monstr: Delete OK"<<endl;

      return TRUE;
    }
    case ALREADY_CREATE:     return TRUE;
    case CHANGE_LEVEL:       return TRUE;
    default:    return FALSE;
  }
}

void            Men::dump (ostream& os) const
{
  os<<"Men: player "<<mPlayer<<endl;
}

void            Men::sleep   (BOOL state,ID )
{
  if (state)
     owner->AddActive (id(),PR_MONSTR);
  else
     owner->DelActive (id());
}

Men::Men  (ArgPtr ptr)
{
  if (ptr == NULL)
     return;

        //создаём заводную обезьянку

  CreateWeaponPlayer* arg = (CreateWeaponPlayer*)ptr;

  mPlayer = owner->CreateObject (WEAPON_PLAYER,arg,id());
}

Men::Men  (BinFile* file)
{
  Load (file);
}

Men::~Men ()
{
  if (player() != ID(-1,-1))
  {
    PlayerDeath message (player());

    owner->PutMessage (&message,player(),sizeof(PlayerDeath));

    owner->DelActive (id());
  }
}

//////////////////////implementation of class Monstr/////////////////////////

void            Monstr::Load (BinFile* file)
{
  Men::Load (file);

  MonstrTag     tag;

  file->read (&tag,sizeof(tag));

  param      = tag.param;
  iqRes      = tag.iqRes;
  target     = tag.target;
  speed      = tag.speed;
  skin       = tag.skin;

  curMethod[CURRENT_METHOD] = tag.curMethod [CURRENT_METHOD];
  curMethod[PREV_METHOD]    = tag.curMethod [PREV_METHOD];

  lastTime = clock();
}

void            Monstr::Save (BinFile* file)
{
  Men::Save (file);

  MonstrTag     tag;

  tag.curMethod[CURRENT_METHOD] = curMethod [CURRENT_METHOD];
  tag.curMethod[PREV_METHOD]    = curMethod [PREV_METHOD];

  tag.param      = param;
  tag.iqRes      = iqRes;
  tag.target     = target;
  tag.speed      = speed;
  tag.skin       = skin;

  file->write (&tag,sizeof(tag));
}

BOOL            Monstr::Exec (GameMessage* mes)
{
  switch (mes->id)
  {
    case NOT_PROCESS:
    {
      GameNotProcess* proc = (GameNotProcess*)mes;

      if (proc->obj == target.id)
         sleep (FALSE,ID(-1,-1));
      else if (proc->obj == player())
      {
         mPlayer = ID(-1,-1);
         owner->DelActive (id());
         delete this;
      }

      return TRUE;
    }
    case TICK:
    {
      if (target.id != ID(-1,-1))
      {
        GetPlayerInfoMessage  getinfo (id());

        owner->PutMessage (&getinfo,target.id,sizeof(getinfo),id());
      }

      return Men::Exec (mes);
    }
    case PLAYER_INFO:
    {
      PlayerInfoMessage*  info = (PlayerInfoMessage*)mes;

      target.x     = info->x;
      target.y     = info->y;
      target.skin  = info->skin;

      return TRUE;
    }
    case LUCKY_SHOT:
    {
      PlayerLuckyShot* message = (PlayerLuckyShot*)mes;

      if (message->dest == target.id)
         sleep (FALSE,ID(-1,-1));

      return TRUE;
    }
    case I_SEE_YOU:
    {
      if (target.id != ID(-1,-1) || ((PlayerISeeYou*)mes)->player==target.id)
         return TRUE;

      if (IsFriend (((PlayerISeeYou*)mes)->player))
         return TRUE;

      alarm (((PlayerISeeYou*)mes)->player);
      sleep (TRUE,((PlayerISeeYou*)mes)->player);

      return TRUE;
    }
    case ECHO_ON:
    {
      if (target.id != ID(-1,-1) || ((EchoOnMessage*)mes)->object==target.id)
         return TRUE;

      if (IsFriend (((EchoOnMessage*)mes)->object))
         return TRUE;

      alarm (((EchoOnMessage*)mes)->object);
      sleep (TRUE,((EchoOnMessage*)mes)->object);

      return TRUE;
    }
    case ALARM:
    {
      PlayerAlarmMessage* message = (PlayerAlarmMessage*)mes;

      if (target.id != ID(-1,-1) || message->target==target.id ||
          message->skin != skin)
         return TRUE;

      sleep (TRUE,message->target);
      
      return TRUE;
    }
    case SLEEP_OFF:
    {
      PlayerSleepOff*    message  = (PlayerSleepOff*)mes;

      if (target.id == ID(-1,-1) || message->player == target.id)
         return TRUE;

      if (IsFriend (message->player))
         return TRUE;

      alarm (message->player);
      sleep (TRUE,message->player);

      return TRUE;
    }
    case PLAYER_DEATH:
    {
      PlayerDeath* message = (PlayerDeath*)mes;

      sleep (FALSE,ID(-1,-1));

      return Men::Exec(mes);
    }

    default: return Men::Exec (mes);
  }
}

void            Monstr::dump (ostream& os) const
{
  os<<"Monstr & his target : x "<<target.x<<" y "<<target.y<<" IQ "
     <<target.IQ<<endl;
}

BOOL            Monstr::IsFriend (ID id)
{
  Player* plr = (Player*)owner->GetObject(id);

  if (plr->type () == ERROR_OBJECT || plr->Skin () == skin)
     return TRUE;
  else
     return FALSE;
}

void            Monstr::alarm   (ID who)
{
        //ALARM!!!
  Ring*              ring     = owner->GetRing (type());
  PlayerAlarmMessage alarm(who,skin);

  owner->PutMessage (&alarm,ring->id(),sizeof(alarm));
}

void            Monstr::sleep (BOOL state,ID trg)
{
          //Если пора баиньки - то подчиняемся приказу

  if (trg == ID(-1,-1))
  {
     target.id = ID(-1,-1);
     target.x  = -1;
     target.y  = -1;

     Men::sleep (state,trg);
     return;
  }

  if (IsFriend (trg))
     return;

          //Проверям - а может быть - цель наш коллега

  Player* plr = (Player*)owner->GetObject(trg);

  if (plr->type () == ERROR_OBJECT || plr->Skin () == skin)
     return ;

  if (target.id == ID(-1,-1) && trg != ID(-1,-1))
  {
        //Say @HEY@ to papa

    PlayerHeyMessage message;

    owner->PutMessage (&message,player(),sizeof(message));
  }

        //Устанавливаем новую цель

  if (target.id == trg)
  {
     if (target.IQ++ > MAX_TARGET_IQ)
       target.IQ=MAX_TARGET_IQ;
  }
  else
  {
          //Устанавливаем цель

    target.IQ  = 0;

    target.id  = trg;
    target.x   = -1;
    target.y   = -1;

    Men::sleep (state,trg);

    lastTime = clock();
  }
}

void    Monstr::think         ()
{
  Flag  flag ("MONSTR_THINK_IN");

//  if (curMethod [CURRENT_METHOD] != curMethod [PREV_METHOD])
//  {
//     curMethod [PREV_METHOD] = curMethod [CURRENT_METHOD];
//
//     return;
//  }

  if (owner->GetObject(player())->type() != WEAPON_PLAYER)
  {
    curMethod [PREV_METHOD]    = IQClass::VIEW_OUT;
    curMethod [CURRENT_METHOD] = IQClass::VIEW_OUT;
    return;
  }

  WeaponPlayer* monstrPtr = (WeaponPlayer*)owner->GetObject (player());

                //Вычисление параметров

  param.monstr    = monstrPtr;
  param.target    = target.id;
  param.owner     = owner;
  param.step      = speed;
  param.angle     = FindAngle (monstrPtr->GetX(),monstrPtr->GetY(),
                               target.x,target.y);
  param.dist      = abs (mul (monstrPtr->GetX()-target.x,
                              icos (param.angle) ));
  param.tuda_suda = target.IQ;

                //Анализ

  IQMark        maxMark    = 0,
                mark;
  int           findMethod = IQClass::ATACK;

  for (int i=0;i<IQ_MAX_PROBS;i++)
      if ( table [i])
      {
         mark = table[i]->analyse (param) * iqRes.prob [i];

         if (mark > maxMark)
         {
           maxMark    = mark;
           findMethod = i;
         }
      }

  curMethod [PREV_METHOD]    = curMethod [CURRENT_METHOD];
  curMethod [CURRENT_METHOD] = (IQClass::Actives)findMethod;
}

void            Monstr::tick  ()
{
  if (player()!=ID(-1,-1) && clock()-lastTime > MONSTR_CHANGE_TIME &&
      target.x!=-1 && target.y!=-1)
  {
    think ();   //TRUE or FALSE

    lastTime = clock();

    if (table [curMethod[CURRENT_METHOD]] == NULL)
    {
      dout<<"Monstr: Wrong IQ system work!"<<endl;
      return;
    }
                  //Передача управления самому эффективному методу

    table[curMethod[CURRENT_METHOD]]->process (param);
  }
}

Monstr::Monstr  (ArgPtr ptr)
       : Men (ptr)
{
  target.x  = -1;
  target.y  = -1;
  target.id = ID(-1,-1);
  target.IQ = 0;
  skin      = -1;

  if (ptr == NULL)
  {
     memset (&iqRes,-1,sizeof(unsigned)*IQ_MAX_PROBS);
     return;
  }

  CreatePlayer*  arg = (CreatePlayer*)ptr;

  speed = arg->resourceClass.speed;
  skin  = arg->resourceClass.mapSprite;

  iqRes.prob [IQClass::ATACK]        = arg->resourceClass.iq[IQClass::ATACK];
  iqRes.prob [IQClass::FAR_FOLLOW]   = arg->resourceClass.iq[IQClass::FAR_FOLLOW];
  iqRes.prob [IQClass::NEAR_FOLLOW]  = arg->resourceClass.iq[IQClass::NEAR_FOLLOW];
  iqRes.prob [IQClass::VIEW_OUT]     = arg->resourceClass.iq[IQClass::VIEW_OUT];
  iqRes.prob [IQClass::RANDOM_MOVE]  = arg->resourceClass.iq[IQClass::RANDOM_MOVE];

  curMethod [CURRENT_METHOD] = IQClass::ATACK;
  curMethod [PREV_METHOD]    = IQClass::ATACK;
}

Monstr::Monstr  (BinFile* file)
{
  Load (file);
}

Monstr::~Monstr ()
{
}