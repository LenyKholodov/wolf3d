        //�⠪� � �������

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

        //������ ��殬 � 楫�

  if (angle > PLAYER_TURN || ANGLE_360-angle<ANGLE_360-PLAYER_TURN)
  {
    turn        = angle ;

    return;
  }
  else  //��楫
    monkey->TurnRight (angle + (friendly<<5));

        //��५졠 � 楫� �᫨ ����ﭨ� �������� � ������ �� ���
        //���⠢��� ���४�஫��� ���� �����

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

        //�ன����???

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

        //�������� ����� 楫� � ��५���

void            Monstr::think ()
{
  strafe = FX_HALF>>1;
  turn   = -ANGLE_1<<2;
  fire   = TRUE;
}

        //������� �� ��५��

void            Monstr::think ()
{
  CrossIndex index;
  Player*    monkey = (Player*)owner->GetObject (player());
  Angle      angle  = FindAngle (monkey->GetX(),monkey->GetY(),
                                 target.x,target.y)-
                                 monkey->GetAngle();

        //������ ��殬 � 楫�

  if (angle > PLAYER_TURN || ANGLE_360-angle<ANGLE_360-PLAYER_TURN)
  {
    turn        = angle ;

    return;
  }

        //����� !!!

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

        //������ �� ���ਤ���
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

        //�᫨ ��� ���������� �� ����

  if (turn)
  {
    monkey->TurnRight (turn);
    turn = 0;
  }

        //�᫨ ��� ��室���

  if (walk)
  {
    monkey->GoForward(walk);
    walk = 0;
  }

        //�᫨ ��� ��-strafe'�����
  if (strafe)
  {
    monkey->StrafeRight (strafe);
    strafe = 0;
  }

        //�᫨ ��� ����५���

  if (fire)
  {
    monkey->Fire();
    fire = FALSE;
  }
}