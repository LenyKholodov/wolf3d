#include "g_iq.h"
#include "g_random.h"

#include "g_system.h"
#include "g_monstr.h"

#include "cross.h"

#include <debug.h>
#include <tables.h>

//////////////////Реализация конечных автоматов//////////////////////////////

        //Атака

IQClass::Actives  AtackTarget::analyse (const IQParam& param)
{
  CrossIndex    index;

  Angle angle = param.angle - param.monstr->GetAngle ();

  if (param.dist > MAX_ATACK_DIST)
     return IQClass::GO;
  if ((angle & 0x7fff) > PLAYER_TURN)
     return IQClass::TURN_ANGLE;

  FindCross (param.monstr->GetX(),
             param.monstr->GetY(),
             param.monstr->GetAngle(),
             param.dist+FX_ONE,
             param.owner->GetMap(),
             index);

  if (index.crossIndex != -1 &&
      param.owner->GetBlock(index.crossIndex)->type==Map::MONSTR) //check for ID
        return  IQClass::ATACK;
  else
        return  IQClass::GO;
}

void    AtackTarget::process (const IQParam& param)
{
//  param.monstr->TurnRight (param.angle - param.monstr->GetAngle ());  //tuda_suda
  param.monstr->Fire ();
}

        //Поворот к цели

IQClass::Actives  TurnToTarget::analyse (const IQParam& param)
{
  Angle phi = ( param.angle-param.monstr->GetAngle() ) & 0x7FFF;

  if (phi <= PLAYER_TURN)
     return IQClass::GO; //RANDOM

  return IQClass::TURN_ANGLE;
}

void    TurnToTarget::process (const IQParam& param)
{
  param.angle-param.monstr->GetAngle() < ANGLE_180 ?
              param.monstr->TurnRight (PLAYER_TURN) :
              param.monstr->TurnLeft  (PLAYER_TURN) ;
// param.monstr->TurnRight (PLAYER_TURN) ;
}

        //Преследование цели

IQClass::Actives  FolowTarget::analyse (const IQParam& param)
{
  Angle phi = param.angle - param.monstr->GetAngle();

  if ((phi & 0x7fff) > PLAYER_TURN)
     return IQClass::TURN_ANGLE;

        //Добавить анализ на дверь
        //И шаг в сторону ???

  CrossIndex index;
  Block*     block;

  FindCross (param.monstr->GetX(),
             param.monstr->GetY(),
             param.monstr->GetAngle(),
             PLAYER_STEP,
             param.owner->GetMap(),
             index);

  block = param.owner->GetBlock(index.crossIndex);

  if (index.crossIndex == -1)                return IQClass::GO;
  else if (block->type == Map::MONSTR)       return IQClass::GO;
  else if (block->type == Map::DOOR)         return IQClass::OPEN_DOOR;
  else if (block->type == Map::WALL &&
           phi > ANGLE_90 && phi < ANGLE_270)return IQClass::CHANGE_DIR;
  else if (block->type == Map::WALL)         return IQClass::FIND;
  else                                       return IQClass::GO;
}

void    FolowTarget::process (const IQParam& param)
{
  param.monstr->GoForward (PLAYER_STEP);
}

        //Поиск

IQClass::Actives  SearchForTarget::analyse (const IQParam& param)
{
  CrossIndex index;

  FindCross (param.monstr->GetX(),
             param.monstr->GetY(),
             param.monstr->GetAngle(),
             param.dist,
             param.owner->GetMap(),
             index);

  if (index.crossIndex == -1)
     return param.previos;

  FindCross (param.monstr->GetX(),
             param.monstr->GetY(),
             param.monstr->GetAngle()-ANGLE_90,
             PLAYER_STEP,
             param.owner->GetMap(),
             index);

  if (param.owner->GetBlock(index.crossIndex)->type == Map::DOOR)
  {
     param.monstr->TurnLeft (ANGLE_90);
     return IQClass::OPEN_DOOR;
  }
  else
     return IQClass::FIND;
}

void    SearchForTarget::process (const IQParam& param)
{
  CrossIndex index;

  FindCross (param.monstr->GetX(),
             param.monstr->GetY(),
             param.monstr->GetAngle()-ANGLE_90,
             PLAYER_STEP,
             param.owner->GetMap(),
             index);

  if (index.crossIndex != -1)
  {
    Block* block = param.owner->GetBlock (index.crossIndex);

    if (!block->cross)
    {
      FindCross (param.monstr->GetX(),
                 param.monstr->GetY(),
                 param.monstr->GetAngle(),
                 PLAYER_STEP,
                 param.owner->GetMap(),
                 index);

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

IQClass::Actives  RandomIQ::analyse (const IQParam& param)
{
  return param.previos; //!!!!!!!!!!!!
}

void    RandomIQ::process (const IQParam& param)
{
  param.monstr -> TurnRight (Random (PLAYER_TURN<<1)-PLAYER_TURN);
  param.monstr -> GoForward (PLAYER_STEP);
}

        //Открытие двери

IQClass::Actives  OpenDoor::analyse (const IQParam& param)
{
  CrossIndex index;
  Block*     block;

  FindCross (param.monstr->GetX(),
             param.monstr->GetY(),
             param.monstr->GetAngle(),
             PLAYER_STEP,
             param.owner->GetMap(),
             index);

  if (index.crossIndex == -1)
     return IQClass::RANDOM;

  block      = param.owner->GetBlock (index.crossIndex);

  if (block->type == Map::DOOR && block->id.door.state < DOOR_CROSS_STATE)
     return IQClass::OPEN_DOOR;
  else
     return param.previos;
}

void              OpenDoor::process (const IQParam& param)
{
  CrossIndex index;
  Block*     block;

  FindCross (param.monstr->GetX(),
             param.monstr->GetY(),
             param.monstr->GetAngle(),
             PLAYER_STEP,
             param.owner->GetMap(),
             index);

  block      = param.owner->GetBlock (index.crossIndex);

  if (block->type == Map::DOOR && block->id.door.state == 0)
     param.monstr->Press();
}

        //Изменение направления движения

IQClass::Actives  ChangeDir::analyse (const IQParam& param)
{
  if (param.previos == IQClass::CHANGE_DIR)
     return IQClass::GO;

  CrossIndex index;
  Block*     block;

  FindCross (param.monstr->GetX(),
             param.monstr->GetY(),
             param.monstr->GetAngle(),
             PLAYER_STEP,
             param.owner->GetMap(),
             index);

  if (index.crossIndex == -1)
     return IQClass::RANDOM;

  block  =   param.owner->GetBlock (index.crossIndex);

  if (block->type == Map::WALL)
     return IQClass::CHANGE_DIR;
  else
     return param.previos;      //????
}

void              ChangeDir::process (const IQParam& param)
{
  param.monstr->TurnRight (ANGLE_90);
}
