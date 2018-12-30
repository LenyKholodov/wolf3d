#include "g_iq.h"
#include "g_random.h"

#include "cross.h"

#include <debug.h>
#include <tables.h>

//////////////////Реализация конечных автоматов//////////////////////////////

        //Поворот к цели
        
IQMark  TurnToTarget::analyse (const IQParam& param)
{
  if (param.angle-monstr->GetAngle() < PLAYER_TURN)
     return 0;

  int        mark   = 100 - fixed2int(param.dist);

  return mark<0 ? 0 : mark;
}

void    TurnToTarget::process (const IQParam& param)
{
  param.angle-param.monstr->GetAngle() < ANGFLE_180 ?
              monstr->TurnRight (PLAYER_TURN) :
              monstr->TurnLeft  (PLAYER_TURN) ;
}

        //Атака

IQMark  AtackTarget::analyse (const IQParam& param)
{
  CrossIndex    index;
  IQMark        mark
  Angle angle = param.angle - monstr->GetAngle ();

  if (param.dist > MAX_ATACK_DIST)
     return 0;
  if (angle > PLAYER_TURN || ANGLE_360-angle<ANGLE_360-PLAYER_TURN)
     return 0;
  else
    monkey->TurnRight (angle + (param.tuda_suda << 3 ) );

  FindCross (monstr->GetX(),monstr->GetY(),monstr->GetAngle(),
             param.dist+FX_ONE,param.owner->GetMap(),index);

  if (index.crossIndex != -1 && 
      param.owner->GetBlock(index.crossIndex)->type==Map::MONSTR)
        mark   = 100 - fixed2int(param.dist);

  return mark<0 ? 0 : mark;        
}

void    AtackTarget::process (const IQParam& param)
{
  param.monstr->Fire ();
}

        //Преследование цели

IQMark  FolowTarget::analyse (const IQParam& param)
{
  CrossIndex index;
  int        mark ;

  FindCross (param.monstr->GetX(),param.monstr->GetY(),param.monstr->GetAngle(),
             FX_ONE,param.owner->GetMap(),index);

  if (index.crossIndex == -1)
     mark = 100 - fixed2int (param.dist);
  else
     return 0;

  return mark < 0 ? 0 : mark;
}

void    FolowTarget::process (const IQParam& param)
{
  param.monstr->GoForward (PLAYER_STEP);
}

        //Убегание

IQMark  RunAway::analyse (const IQParam & param)
{
  CrossIndex index;
  int        mark ;

  FindCross (param.monstr->GetX(),param.monstr->GetY(),
             ANGLE_360-param.monstr->GetAngle(),
             FX_ONE,param.owner->GetMap(),index);

  if (index.crossIndex == -1)
     mark = tuda_suda >> 2;
  else
     return 30;

  return mark < 0 ? 0 : mark;
}

void    RunAway::process (const IQParam& param)
{
  param.monstr->GoBack (PLAYER_STEP);
}

        //Поиск
        
IQMark  SearchForTarget::analyse (const IQParam& param)
{
  CrossIndex index;

  FindCross (param.monstr->GetX(),
             param.monstr->GetY(),
             param.monstr->GetAngle(),
             param.dist,
             param.owner->GetMap(),
             index);

  if (index.crossIndex == -1)
     return 0;
  else
     return 90;
}

void    SearchForTarget::process (const IQParam& param)
{
  CrossIndex index;

  FindCross (param.monstr->GetX(),
             param.monstr->GetY(),
             param.monstr->GetAngle()-ANGLE_90,
             FX_ONE,param.owner->GetMap(),index);

  if (index.crossIndex != -1)
  {
    Block* block = param.owner->GetBlock (index.crossIndex);

    if (block->type == Map::DOOR)
       if (block->id.door.state == 0)
       {
         param.monstr->TurnLeft (ANGLE_90);
         param.monstr->Press();
       }
       else
         return;

    if (!param.owner->GetBlock (index.crossIndex)->cross)
    {
      FindCross (param.monstr->GetX(),
                 param.monstr->GetY(),
                 param.monstr->GetAngle(),
                 FX_ONE,param.owner->GetMap(),index);

      if (index.crossIndex == -1)
        param.monstr -> GoForward (FX_ONE);
      else
        param.monstr -> TurnRight (ANGLE_90);
    }
  }
  else
  {
    param.monstr->TurnLeft  (ANGLE_90);
    param.monstr->GoForward (FX_ONE);
  }
}

        //Случайное

IQMark  RandomIQ::analyse (const IQParam& param)
{
  if (param.dist > ATACK_DIST)
     return 15;
  else
     return 32 - fixed2int (param.dist);
}

void    RandomIQ::process (const IQParam& param)
{
  param.monstr -> TurnRight (Random (PLAYER_TURN<<1)-PLAYER_TURN);
  param.monstr -> GoForward (FX_ONE);
}


//////////////////implementation of class GameIQSystem///////////////////////

void    GameIQSystem::think         (WeaponPlayer* monkey,
                                     Player* target,
                                     int targetIQ
                                    )
{
  if (target == NULL || monkey == NULL)
  {
    dout<<"Game IQ System: Wrong pointer"<<endl;
    return;
  }

                //Анализ
                
  IQMark        maxMark = 0,
                mark;
  GameIQ*       method  = NULL;
                
  for (int i=0;i<IQ_MAX_PROBS;i++)
      if ( table [i])
      {
         mark = table[i]->analyse (monkey,target) * iqRes.prob [i];

         if (mark > maxMark)
         {
           maxMark = mark;
           method  = table[i];
         }
      }

  if (method == NULL)
  {
    dout<<"Game IQ System: Wrong IQ system work!"<<endl;
    return;
  }

                //Передача управления самому эффективному методу

  method->process (monkey,target,targetIQ);
}

void    GameIQSystem::Load    (BinFile* file)
{
}

void    GameIQSystem::Save    (BinFile* file)
{
}

GameIQSystem::GameIQSystem  (const IQClass& x)
             : iqRes (x)
             {       }

GameIQSystem::~GameIQSystem ()
             {       }
