IQClass::Actives  FarFollowTarget::process (const IQParam& param)
{
  Player*    monkey  = param.monstr;
  CrossIndex index;

  Angle      no_process = monkey->GetAngle() & 0x3FFF;

  if (no_process)
     no_process < ANGLE_45 ? monkey->TurnLeft  (no_process) :
                             monkey->TurnRight (no_process);

  FindCross (monkey->GetX(),monkey->GetY(),monkey->GetAngle()-ANGLE_90,
             param.step,param.owner->GetMap(),index);

  if (index.crossIndex != -1)
  {
    Block* block = param.owner->GetBlock (index.crossIndex);

    if (block->type == Map::DOOR)
       if (block->id.door.state == 0)
       {
         monkey->TurnLeft (ANGLE_90);
         monkey->Press();
       }
       else
         return IQClass::FAR_FOLLOW;

    if (!param.owner->GetBlock (index.crossIndex)->cross)
    {
      FindCross (monkey->GetX(),monkey->GetY(),monkey->GetAngle(),
               param.step,param.owner->GetMap(),index);

      if (index.crossIndex == -1)         monkey->GoForward(param.step);
      else                                monkey->TurnRight(ANGLE_90);
    }
  }
  else
  {
    monkey->TurnLeft (ANGLE_90);
    monkey->GoForward (param.step);
  }

  return IQClass::FAR_FOLLOW;
}