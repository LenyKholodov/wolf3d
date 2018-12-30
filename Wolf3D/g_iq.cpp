#include "g_iq.h"
#include "g_random.h"

#include "g_system.h"
#include "g_monstr.h"

#include "cross.h"

#include <debug.h>
#include <tables.h>

        //Доделать чтобы не убегал от пули

//////////////////Реализация модулей//////////////////////////////////////////

BOOL    IsWallNear      (Player* player,Map* map)
{
  int   x     = fixed2int(player->GetX()),
        y     = fixed2int(player->GetY());

  return !map->IsCross (Index(y+1,x))   || !map->IsCross (Index(y+1,x+1)) ||
         !map->IsCross (Index(y,x+1))   || !map->IsCross (Index(y-1,x+1)) ||
         !map->IsCross (Index(y-1,x))   || !map->IsCross (Index(y-1,x-1)) ||
         !map->IsCross (Index(y,x-1))   || !map->IsCross (Index(y+1,x-1));
}

BOOL    IsWallLeft (int x,int y,Angle angle,Map* map)
{
  int dir = angle < ANGLE_180 ? 1 : -1;

  switch (angle & 0x7FFF)
  {
    case ANGLE_0: return !map->IsCross (Index(y-dir,x+dir)) ||
                         !map->IsCross (Index(y-dir,x))   ||
                         !map->IsCross (Index(y-dir,x-dir));

    case ANGLE_90:return !map->IsCross (Index(y+dir,x+dir)) ||
                         !map->IsCross (Index(y,    x+dir))   ||
                         !map->IsCross (Index(y-dir,x+dir));
  };
  return FALSE;
}

BOOL    IsInTonel (Player* player,Map* map)
{
  int   x     = fixed2int(player->GetX()),
        y     = fixed2int(player->GetY());
  int dir     = player->GetAngle() < ANGLE_180 ? 1 : -1;

  switch (player->GetAngle() & 0x7FFF)
  {
    case ANGLE_0: return !map->IsCross (Index(y-dir,x));
    case ANGLE_90: return !map->IsCross (Index(y,x+dir));
  };
  return FALSE;
}

BOOL    IsWallLeft (Player* player,Map* map)
{
  return IsWallLeft (fixed2int(player->GetX()),fixed2int(player->GetY()),
                     player->GetAngle(),map);
}

BOOL    IsWallRight (Player* player,Map* map)
{
  return IsWallLeft (fixed2int(player->GetX()),fixed2int(player->GetY()),
                     player->GetAngle()+ANGLE_180,map);
}

BOOL    OpenDoor        (Player* monstr,const Block* block)
{
  if (block->type == Map::DOOR)
  {
    if (block->id.door.state == 0)                      monstr->Press();
    else if (block->id.door.state < DOOR_CROSS_STATE)   return FALSE;
    else                                                return TRUE;
   }

   return FALSE;
}

void    TurnToAngle     (Player* monstr,Angle angle)
{
  angle < ANGLE_180 ? monstr->TurnRight (MONSTR_TURN) :
                      monstr->TurnLeft  (MONSTR_TURN);
}
  //Атака

IQMark  AtackTarget::analyse (const IQParam& param)
{
  CrossIndex    index;

  Angle angle = param.angle - param.monstr->GetAngle ();

  if (param.dist > MAX_ATACK_DIST)
     return 15;

  FindCross (param.monstr->GetX(),
             param.monstr->GetY(),
             //param.monstr->GetAngle(),
             param.angle,
             param.dist+FX_ONE,
             param.owner->GetMap(),
             index);

  if (index.crossIndex != -1 &&
      param.owner->GetObject(index.crossIndex) == param.target)
    return  100;
  else
    return 30;
}

IQClass::Actives    AtackTarget::process       (const IQParam& param)
{
  CrossIndex index;

  WeaponPlayer*  monstr = param.monstr;
  Angle          angle  = param.angle - monstr->GetAngle();
  Fixed          dist   = param.dist;

        //Поворот лицом к цели

  if ((angle & 0x7fff) > MONSTR_TURN)
  {
    TurnToAngle (monstr,angle);
    return IQClass::ATACK;
  }
//  else
//    monstr->TurnRight (param.angle); //tuda_suda

        //Стрельба в цель если расстояние позволяет и монстр не зол

  if ( dist < MAX_ATACK_DIST )
  {
      FindCross (monstr->GetX(),
                 monstr->GetY(),
                 monstr->GetAngle(),
                 dist+FX_ONE,
                 param.owner->GetMap(),
                 index);

      if (index.crossIndex != -1 &&
          param.owner->GetObject(index.crossIndex) == param.target)
        monstr->Fire ();
  }

        //пройдёмся ?

  FindCross (monstr->GetX(),
             monstr->GetY(),
             monstr->GetAngle(),
             param.step,
             param.owner->GetMap(),
             index);

  if (index.crossIndex == -1)   monstr->GoForward (param.step);
  else
      return IQClass::FAR_FOLLOW;     //NEAR_FOLLOW
             //повернуться и вызвать far follow

  return IQClass::ATACK;
}

        //Поиск

IQMark  FarFollowTarget::analyse (const IQParam& param)
{
  if (!IsWallNear (param.monstr,param.owner->GetMap()))
     return 5;

  CrossIndex index;

  FindCross (param.monstr->GetX(),
             param.monstr->GetY(),
             param.angle,
             param.dist+FX_ONE,
             param.owner->GetMap(),
             index);

  if (index.crossIndex == -1)
     return 1;
  else
     return 80+fixed2int(param.dist);
}

IQClass::Actives FarFollowTarget::process (const IQParam& param)
{
  CrossIndex index;
  Player*    monstr     = param.monstr;
  Angle      no_process = monstr->GetAngle() & 0x3FFF;

  if (no_process)
     no_process < ANGLE_45 ? monstr->TurnLeft  (no_process) :
                             monstr->TurnRight (no_process);

  FindCross (monstr->GetX(),
             monstr->GetY(),
             monstr->GetAngle()-ANGLE_90,
             param.step,
             param.owner->GetMap(),
             index);

  if (index.crossIndex != -1)
  {
    Block* block = param.owner->GetBlock (index.crossIndex);

    if (block->type == Map::DOOR)
    {
       monstr->TurnLeft  (ANGLE_90);
       OpenDoor          (monstr,block);
       monstr->TurnRight (ANGLE_90);
       return IQClass::FAR_FOLLOW;
    }
    else if (block->type == Map::SPRITE || block->type == Map::MONSTR)
      return IQClass::NEAR_FOLLOW;

    FindCross (monstr->GetX(),
               monstr->GetY(),
               monstr->GetAngle(),
               param.step,
               param.owner->GetMap(),
               index);

    if (index.crossIndex == -1)         monstr->GoForward (param.step);
    else                                monstr->TurnRight (ANGLE_90);
  }
  else
  {
      monstr->TurnLeft  (ANGLE_90);  //'-'??
      monstr->GoForward (param.step);
  }

  return IQClass::FAR_FOLLOW;
}

        //Ближнее преследование

IQMark            NearFollowTarget::analyse (const IQParam& param)
{
  CrossIndex    index;

  Angle angle = param.angle - param.monstr->GetAngle ();

  if (param.dist > MAX_ATACK_DIST)
     return 25;

  FindCross (param.monstr->GetX(),
             param.monstr->GetY(),
             param.monstr->GetAngle(),
             param.step,
             param.owner->GetMap(),
             index);

  if (index.crossIndex == -1 )
    return  100;
  else
    return 80-fixed2int(param.dist);
}

IQClass::Actives  NearFollowTarget::process (const IQParam& param)
{
  CrossIndex    index;
  Player*       monstr     = param.monstr;
  Angle         angle      = param.angle - monstr->GetAngle();

         //Поворот лицом к цели

  if ((angle & 0x7fff) > MONSTR_TURN)
  {
    TurnToAngle (monstr,angle);
    return IQClass::ATACK;
  }

  FindCross (monstr->GetX(),
             monstr->GetY(),
             monstr->GetAngle(),
             param.step,
             param.owner->GetMap(),
             index);

  if (index.crossIndex != -1)
  {
    Block*        block  = param.owner->GetBlock (index.crossIndex);

    if (block->type == Map::DOOR)
    {
       OpenDoor (monstr,block);
       return IQClass::NEAR_FOLLOW;
    }
    else if (block->type == Map::MONSTR)
       return IQClass::ATACK;
    else
       return IQClass::FAR_FOLLOW;
  }
  else
     monstr->GoForward (param.step);

  return IQClass::NEAR_FOLLOW;
}

        //Осматривается

IQMark            ViewOut::analyse (const IQParam& param)
{
  if (param.angle & 0x7FFF > MONSTR_TURN)
     return 80 - fixed2int(param.dist);
  return 25;
}

IQClass::Actives  ViewOut::process (const IQParam& param)
{
  TurnToAngle (param.monstr,param.angle-param.monstr->GetAngle());

  return IQClass::VIEW_OUT;
}

        //Случайное

IQMark            RandomMove::analyse (const IQParam& param)
{
  return 0;
 // return Random (64-fixed2int(param.dist));
}

IQClass::Actives  RandomMove::process (const IQParam& param)
{
  TurnToAngle (param.monstr,Random(ANGLE_360)-ANGLE_180);

  param.monstr->GoForward (param.step);

  return IQClass::RANDOM_MOVE;
}



