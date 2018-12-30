#include "g_iq.h"
#include "g_random.h"

#include "g_system.h"
#include "g_monstr.h"

#include "cross.h"

#include <debug.h>
#include <tables.h>

//////////////////Реализация конечных автоматов//////////////////////////////

        //Поворот к цели

IQMark  TurnToTarget::analyse (const IQParam& param)
{
  if (param.dist > MAX_ATACK_DIST)
     return 0;

  Angle phi = ( param.angle-param.monstr->GetAngle() ) & 0x7FFF;

  if (phi <= PLAYER_TURN)
     return 0;

  return 75;
}

void    TurnToTarget::process (const IQParam& param)
{
  (param.angle-param.monstr->GetAngle())>>15 ?
              param.monstr->TurnRight (-PLAYER_TURN) :
              param.monstr->TurnLeft  (-PLAYER_TURN) ;
}

        //Атака

IQMark  AtackTarget::analyse (const IQParam& param)
{
  if (param.dist > MAX_ATACK_DIST)
     return 0;

  CrossIndex    index;
  IQMark        mark;

  Angle angle = param.angle - param.monstr->GetAngle ();

  if (param.dist > MAX_ATACK_DIST)
     return 0;
  if ((angle & 0x7fff) > PLAYER_TURN) //!!!!!!!!
     return 0;
  else
    param.monstr->TurnRight (angle);    //tuda_suda

  FindCross (param.monstr->GetX(),
             param.monstr->GetY(),
             param.monstr->GetAngle(),
             param.dist+FX_ONE,
             param.owner->GetMap(),
             index);

  if (index.crossIndex != -1 &&
      param.owner->GetBlock(index.crossIndex)->type==Map::MONSTR)
        return  100;
  else
        return  0;
}

void    AtackTarget::process (const IQParam& param)
{
  param.monstr->Fire ();
}

        //Преследование цели

IQMark  FolowTarget::analyse (const IQParam& param)
{
  Angle phi = param.angle - param.monstr->GetAngle();

  if ((phi & 0x7fff) > PLAYER_TURN)
     return 0;

        //Добавить анализ на дверь
        //И шаг в сторону ???

  CrossIndex index;

  FindCross (param.monstr->GetX(),
             param.monstr->GetY(),
             param.monstr->GetAngle(),
             param.dist,
             param.owner->GetMap(),
             index);

  if (index.crossIndex == -1 ||
      param.owner->GetBlock(index.crossIndex)->type == Map::MONSTR)
     return 90 + fixed2int (param.dist) ;
  else
     return 0;
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
             PLAYER_STEP,param.owner->GetMap(),index);

  if (index.crossIndex == -1)
     mark = param.tuda_suda >> 2;
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
  return 1;
//  if (param.dist > MAX_ATACK_DIST)
//     return 5;
//  else
//     return 15 - fixed2int (param.dist); //!!
}

void    RandomIQ::process (const IQParam& param)
{
  param.monstr -> TurnRight (Random (PLAYER_TURN<<1)-PLAYER_TURN);
  param.monstr -> GoForward (FX_ONE);
}

