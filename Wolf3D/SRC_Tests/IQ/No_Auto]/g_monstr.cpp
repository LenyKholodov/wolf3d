#include "g_monstr.h"
#include "t_wplr.h"
#include "g_system.h"
#include "g_iq.h"

#include "cross.h"

#include <tables.h>
#include <stdlib.h>

int     HARD_LEVEL = 0;

make<Monstr,GAME_MONSTR>        makeMonstr;

TurnToTarget                    turnToTarget;
AtackTarget                     atackTarget;
FolowTarget                     folowTarget;
SearchForTarget                 searchForTarget;
RandomIQ                        randomIQ;

static GameIQ*                  Monstr::table [IQ_MAX_PROBS] =
{
        &atackTarget,
        &folowTarget,
        &turnToTarget,
        &searchForTarget,
        &randomIQ
};

////////////////////////implementation of class Men//////////////////////////

void            Men::Load (BinFile* file)
{
  file->read (&mPlayer,sizeof(mPlayer));
  file->read (&callBack,sizeof(callBack));
}

void            Men::Save (BinFile* file)
{
  file->write (&mPlayer,sizeof(mPlayer));
  file->write (&callBack,sizeof(callBack));
}

BOOL            Men::Exec (GameMessage* mes)
{
  switch (mes->id)
  {
    case TICK:
    {
      tick ();

      return TRUE;
    }
    case SLEEP_OFF: sleep (TRUE,((PlayerSleepOff*)mes)->player); return TRUE;
    case PLAYER_DEATH:
    {
      delete this;
      return TRUE;
    }
    case ALREADY_DELETE:     return TRUE;
    case ALREADY_CREATE:     return TRUE;
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
     owner->AddActive (id());
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
  owner->DelActive (id());

  owner->DeleteObject (mPlayer);
}

//////////////////////implementation of class Monstr/////////////////////////

void            Monstr::Load (BinFile* file)
{
  Men::Load (file);

  MonstrTag     tag;

  file->read (&tag,sizeof(tag));

}

void            Monstr::Save (BinFile* file)
{
  Men::Save (file);

  MonstrTag     tag;

  file->write (&tag,sizeof(tag));
}

BOOL            Monstr::Exec (GameMessage* mes)
{
  switch (mes->id)
  {
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

      return TRUE;
    }
    case NOT_PROCESS:
    {
      GameNotProcess* proc = (GameNotProcess*)mes;

      if (proc->obj == target.id && proc->mesId == GET_PLAYER_INFO)
         sleep (FALSE,ID(-1,-1));

      return TRUE;
    }
    case LUCKY_SHOT:
    {
      sleep (FALSE,ID(-1,-1));
      return TRUE;
    }

    default: return Men::Exec (mes);
  }
}

void            Monstr::dump (ostream& os) const
{
  os<<"Monstr & his target : x "<<target.x<<" y "<<target.y<<" IQ "
     <<target.IQ<<endl;
}

void            Monstr::sleep (BOOL state,ID trg)
{
  if (target.id == trg)
     if (target.IQ++ > MAX_TARGET_IQ) target.IQ=MAX_TARGET_IQ;
  else
    target.IQ = 0;

  target.id = trg;
  target.x  = -1;
  target.y  = -1;

  Men::sleep (state,trg);

  lastTime = clock();
}

void    Monstr::think         ()
{
  Player*       targetPtr = (Player*)owner->GetObject (target.id);
  WeaponPlayer* monstrPtr = (WeaponPlayer*)owner->GetObject (player());

                //Вычисление параметров

  param.monstr    = monstrPtr;
  param.target    = targetPtr;
  param.owner     = owner;
  param.angle     = FindAngle (monstrPtr->GetX(),monstrPtr->GetY(),
                               targetPtr->GetX(),targetPtr->GetY());
  param.dist      = abs (mul (monstrPtr->GetX()-targetPtr->GetX(),
                              icos (param.angle) ));
  param.tuda_suda = target.IQ;


                //Анализ

  IQMark        maxMark = 0,
                mark;
  int           in = -1;

  for (int i=0;i<IQ_MAX_PROBS;i++)
      if ( table [i])
      {
         mark = table[i]->analyse (param) * iqRes.prob [i];

         if (mark > maxMark)
         {
           maxMark    = mark;
           curMethod  = table[i];
           in         = i;
         }
      }

  dout<<"Find method "<<in<<endl;
}

void            Monstr::tick  ()
{
  if (clock()-lastTime > MONSTR_CHANGE_TIME && target.x!=-1 && target.y!=-1)
  {
    think ();

    lastTime = clock();

    Player*       targetPtr = (Player*)owner->GetObject (target.id);
    WeaponPlayer* monstrPtr = (WeaponPlayer*)owner->GetObject (player());

    if (curMethod == NULL)
    {
      dout<<"Monstr: Wrong IQ system work!"<<endl;
      return;
    }
                  //Передача управления самому эффективному методу

    curMethod->process (param);
  }
}

Monstr::Monstr  (ArgPtr ptr)
       : Men (ptr),
         curMethod (NULL)
{
  target.x  = -1;
  target.y  = -1;
  target.id = ID(-1,-1);
  target.IQ = 0;

  iqRes.prob [IQClass::TURN_ANGLE] = 1;
  iqRes.prob [IQClass::ATACK]      = 1;
  iqRes.prob [IQClass::FIND]       = 1;
  iqRes.prob [IQClass::GO]         = 1;
  iqRes.prob [IQClass::RANDOM]     = 1;
}

Monstr::Monstr  (BinFile* file)
{
  Load (file);
}

Monstr::~Monstr ()
{
}