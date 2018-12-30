IQClass::Actives FarFollowTarget::process (const IQParam& param)
{
  CrossIndex left, right;
  Player*    monstr     = param.monstr;
  Angle      no_process = monstr->GetAngle() & 0x3FFF; //3FFF

  if (no_process)
     no_process < ANGLE_45 ? monstr->TurnLeft  (no_process) :
                             monstr->TurnRight (no_process);

  FindCross (monstr->GetX(),
             monstr->GetY(),
             monstr->GetAngle()-ANGLE_90,
             param.step,
             param.owner->GetMap(),
             left);

  FindCross (monstr->GetX(),
             monstr->GetY(),
             monstr->GetAngle()+ANGLE_90,
             param.step,
             param.owner->GetMap(),
             right);

  if (left.crossIndex != -1)
  {
    Block* block = param.owner->GetBlock (left.crossIndex);

    if (block->type == Map::DOOR)
    {
       monstr->TurnLeft  (ANGLE_90);
       OpenDoor          (monstr,block);
       monstr->TurnRight (ANGLE_90);
       return IQClass::FAR_FOLLOW;
    }
//    else if (block->type == Map::SPRITE || block->type == Map::MONSTR)
//      return IQClass::NEAR_FOLLOW;

    FindCross (monstr->GetX(),
               monstr->GetY(),
               monstr->GetAngle(),
               param.step,
               param.owner->GetMap(),
               left);

    if (left.crossIndex == -1)          monstr->GoForward (param.step);
    else                                monstr->TurnRight (ANGLE_90);
  }
  else if (right.crossIndex != -1)
  {
    Block* block = param.owner->GetBlock (right.crossIndex);

    if (block->type == Map::DOOR)
    {
       monstr->TurnRight  (ANGLE_90);
       OpenDoor          (monstr,block);
       monstr->TurnLeft (ANGLE_90);
       return IQClass::FAR_FOLLOW;
    }
//    else if (block->type == Map::SPRITE || block->type == Map::MONSTR)
//      return IQClass::NEAR_FOLLOW;

    FindCross (monstr->GetX(),
               monstr->GetY(),
               monstr->GetAngle(),
               param.step,
               param.owner->GetMap(),
               right);

    if (right.crossIndex == -1)          monstr->GoForward (param.step);
    else                                 monstr->TurnLeft (ANGLE_90);
  }
  else
  {
    BOOL leftWall  = IsWallLeft (monstr,param.owner->GetMap()),
         rightWall = IsWallRight (monstr,param.owner->GetMap());

    if (IsInTonel(monstr,param.owner->GetMap()))
    {
        //check for possible
      monstr->GoForward (param.step);
//      dout<<"X "<<fixed2int(monstr->GetX())<<" Y "<<fixed2int(monstr->GetY())<<
//            " angle "<<angle2int(monstr->GetAngle())<<endl;
    }
    else if (leftWall)
    {
      monstr->TurnLeft  (ANGLE_90);
      monstr->GoForward (param.step);
    }
    else if (rightWall)
    {
      monstr->TurnRight  (ANGLE_90);
      monstr->GoForward (param.step);
    }
    else
      return IQClass::NEAR_FOLLOW;
  }

  return IQClass::FAR_FOLLOW;
}