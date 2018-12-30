        //Атака и догонка

void            Monstr::think ()
{
  if ((target.x == -1 || target.y == -1) && clock()-lastTime < MONSTR_CHANGE_TIME)
     return;

  lastTime = clock();

  CrossIndex index;
  Player*    monkey = (Player*)owner->GetObject (player());
  Angle      angle  = FindAngle (monkey->GetX(),monkey->GetY(),
                                 target.x,target.y)-
                                monkey->GetAngle();

  Fixed dist        = abs (monkey->GetX()-target.x)+
                      abs (monkey->GetY()-target.y);

  int   friendly    = (MAX_TARGET_IQ-target.IQ)>>HARD_LEVEL;

        //Поворот лицом к цели

  if (angle > PLAYER_TURN || ANGLE_360-angle<ANGLE_360-PLAYER_TURN)
  {
    turn        = angle ;

    return;
  }
  else  //прицел
    monkey->TurnRight (angle + (friendly<<5));

        //Стрельба в цель если расстояние позволяет и монстр не зол
        //Поставить корректиролвку огня здесь

  if ( dist < MAX_ATACK_DIST )
  {
      FindCross (monkey->GetX(),monkey->GetY(),monkey->GetAngle(),
                     dist+FX_ONE,owner->GetMap(),index);

      if (index.crossIndex != -1 &&
          owner->GetBlock(index.crossIndex)->type==Map::MONSTR)
      {
          fire = TRUE;

      }
      else
          fire = FALSE;

  }

        //пройдёмся???

  FindCross (monkey->GetX(),monkey->GetY(),monkey->GetAngle(),
             FX_ONE,owner->GetMap(),index);

  if (index.crossIndex == -1)
      walk  = FX_ONE>>4;
  else
  {
      angle = angle+PLAYER_TURN;
      walk  = 0;
  }
}

        //Вертеться вокруг цели и стрелять

void            Monstr::think ()
{
  strafe = FX_HALF>>1;
  turn   = -ANGLE_1<<2;
  fire   = TRUE;
}

        //Убегаем не стреляя

void            Monstr::think ()
{
  CrossIndex index;
  Player*    monkey = (Player*)owner->GetObject (player());
  Angle      angle  = FindAngle (monkey->GetX(),monkey->GetY(),
                                 target.x,target.y)-
                                 monkey->GetAngle();

        //Поворот лицом к цели

  if (angle > PLAYER_TURN || ANGLE_360-angle<ANGLE_360-PLAYER_TURN)
  {
    turn        = angle ;

    return;
  }

        //бежим !!!

  FindCross (monkey->GetX(),monkey->GetY(),monkey->GetAngle()-ANGLE_180,
             FX_ONE,owner->GetMap(),index);

  if (index.crossIndex == -1)
      walk  = -FX_ONE>>4;
  else
  {
      angle = angle+PLAYER_TURN;
      walk  = 0;
  }
}

        //Бегаем по корридору
void            Monstr::think ()
{
  Player*    monkey = (Player*)owner->GetObject (player());
  CrossIndex index;

  FindCross (monkey->GetX(),monkey->GetY(),monkey->GetAngle()-ANGLE_90,
             FX_ONE,owner->GetMap(),index);

  if (index.crossIndex != -1)
  {
    Block* block = owner->GetBlock (index.crossIndex);

    if (block->type == Map::DOOR)
       if (block->id.door.state == 0)
       {
         monkey->TurnLeft (ANGLE_90);
         monkey->Press();
       }
       else
         return;

    if (!owner->GetBlock (index.crossIndex)->cross)
    {
      FindCross (monkey->GetX(),monkey->GetY(),monkey->GetAngle(),
               FX_ONE,owner->GetMap(),index);

      if (index.crossIndex == -1)         walk  = FX_ONE;
      else                                turn  = ANGLE_90;
    }
  }
  else
  {
    turn = -ANGLE_90;
    walk = FX_ONE;
  }
}

tick()
{
  WeaponPlayer* monkey = (WeaponPlayer*)owner->GetObject (player());

        //если пора покрутиться на месте

  if (turn)
  {
    monkey->TurnRight (turn);
    turn = 0;
  }

        //если пора походить

  if (walk)
  {
    monkey->GoForward(walk);
    walk = 0;
  }

        //если пора по-strafe'овать
  if (strafe)
  {
    monkey->StrafeRight (strafe);
    strafe = 0;
  }

        //Если пора пострелять

  if (fire)
  {
    monkey->Fire();
    fire = FALSE;
  }
}

        //Strafe

IQMark  StrafeProtect::analyse (const IQParam& param)
{
  int   ystart = fixed2int(param.monstr->GetY()),
        yend   = ystart+3,
        xstart = fixed2int(param.monstr->GetX()),
        xend   = xstart+3;
  int   index  = Index (ystart,xstart);

  GameSystem* owner = param.owner;

  if (xstart<0 || ystart<0 || xend > MAX_X_TILE || yend > MAX_Y_TILE)
     return 0;

  for (int y=ystart;y<yend;y++,index+=MAP_WIDTH)
      for (int x=xstart;x<xend;x++,index++)
          if (owner->GetObject(index) != ID(-1,-1) &&
              owner->GetObject(owner->GetObject(index))->type() == SHOT)
          {
            Angle ang = owner->GetBlock(index)->id.sprite.alpha;
            ang       =  (ang<<13)-param.monstr->GetAngle();

            if (ang>ANGLE_180)
            {
               dout<<"Strafe!"<<endl;
               return 100;
            }
          }

  return 0;
}

IQClass::Actives  StrafeProtect::process (const IQParam& param)
{
  CrossIndex    index;

  Angle angle = param.monstr->GetAngle ();

  FindCross (param.monstr->GetX(),
             param.monstr->GetY(),
             param.monstr->GetAngle()-ANGLE_90,
             param.step,
             param.owner->GetMap(),
             index);

  if (index.crossIndex == -1)
  {
     param.monstr->StrafeLeft (param.step);
     return IQClass::STRAFE_PROTECT;
  }

  FindCross (param.monstr->GetX(),
             param.monstr->GetY(),
             param.monstr->GetAngle()+ANGLE_90,
             param.step,
             param.owner->GetMap(),
             index);

  if (index.crossIndex == -1)
  {
     param.monstr->StrafeRight (param.step);
     return IQClass::STRAFE_PROTECT;
  }

  param.monstr->GoBack (param.step);

  return IQClass::ATACK;
}