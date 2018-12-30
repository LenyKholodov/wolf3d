#include "g_user.h"
#include "g_weap.h"
#include "g_system.h"

#include "palette.h"

make<UserPlayer,USER_PLAYER>            makeLamer;

tPoint          UserPlayer::mMoveTable [MAX_WEAPON_MOVE] =
{
  {3,4}, {3,4}, {3,4}, {3,4},
  {0,-4}, {0,-4}, {0,-4}, {0,-4},
  {-3,4},{-3,4},{-3,4},{-3,4},
  {0,-4}, {0,-4}, {0,-4}, {0,-4}
};

void            UserPlayer::Load (BinFile* file)
{
  Player::Load (file);

        //Загрузка

  file->read (&mCurWeapon,sizeof(mCurWeapon));
  file->read (&mFrags,sizeof(mFrags));
  file->read (mWeapons,sizeof(ID)*USER_WEAPONS);        //оружие

        //Загрузка коробок

  ID* boxes = new ID [USER_WEAPONS];

  if (boxes == NULL)
     D_Error ("User player: Load");

  file->read (boxes,sizeof(ID)*USER_WEAPONS);

        //Обновление коробок

  UserReloadBoxesMessage message (boxes);

  owner->PutMessage (&message,id(),sizeof(message));

        //Обнуление положения оружия

  ViewMoveStartMessage moveStart (IM_AM_HAND);

  owner->PutMessage (&moveStart,callBack,sizeof(moveStart));
}

void            UserPlayer::Save (BinFile* file)
{
  Player::Save (file);

        //Сохранение

  file->write (&mCurWeapon,sizeof(mCurWeapon));
  file->write (&mFrags,sizeof(mFrags));
  file->write (mWeapons,sizeof(ID)*USER_WEAPONS);

  ID empty = ID(-1,-1);

  for (int i=0;i<USER_WEAPONS;i++)
    if (mBoxes [i])
    {
      ID temp = mBoxes[i]->id();

      file->write (&temp,sizeof(ID));
    }
    else
      file->write (&empty,sizeof(ID));

        //Пересохранять Health & Protect
}

BOOL            UserPlayer::Exec (GameMessage* mes)
{
  switch (mes->id)
  {
    case ADD_FIRE:
    {
      mActives[FIRE]++;

      if (mCurWeapon == -1)
         return TRUE;

      WeaponShotOnMessage   on;

      owner->PutMessage (&on,mWeapons[mCurWeapon],sizeof(on));

      return TRUE;
    }
    case SUB_FIRE:
    {
      mActives[FIRE]--;

      if (mCurWeapon == -1 || mActives[FIRE]>0)
         return TRUE;

      WeaponShotOffMessage   off;

      owner->PutMessage (&off,mWeapons[mCurWeapon],sizeof(off));

      return TRUE;
    }
    case CROSS_SHOT:
    {
      BOOL state = Player::Exec (mes);

      RefreshFace();

      return state;
    }
    case LUCKY_SHOT:
    {
      mFrags++;
      RefreshFace ();
      return TRUE;
    }
    case CHANGE_LEVEL:
    {
      owner->PutMessage (mes,callBack,sizeof(PortalChangeLevelMessage),id());

      mMovePos        = 0;
      mOldBlock.type  = Map::FREE;
      mOldBlock.cross = 1;
      mOldId          = ID(-1,-1);

      PortalChangeLevelMessage* message = (PortalChangeLevelMessage*)mes;

      mX              = int2fixed(message->x);
      mY              = int2fixed(message->y);

      return TRUE;
    }
    case NEXT_WEAPON: NextWeapon () ; return TRUE;
    case PREV_WEAPON: PrevWeapon () ; return TRUE;
    case SELECT_WEAPON:
    {
      UserSelectWeaponMessage* message = (UserSelectWeaponMessage*)mes;

      SelectWeapon (message->weapon);

      return TRUE;
    }
    case PRESENT_WEAPON:
    {
      UserIsPresentWeaponMessage* message = (UserIsPresentWeaponMessage*)mes;
      UserPresentWeaponMessage    answer (message->weapon);

      if (IsPresent (message->weapon))
        owner->PutMessage (&answer,message->callBack,sizeof(answer),id());

      return TRUE;
    }
    case SUBJECT_INFO:
    {
      SubjectInfoMessage*    message = (SubjectInfoMessage*)mes;

      if (mBoxes [message->res.boxClass] && mBoxes [message->res.boxClass]->full())
         return TRUE;

      SubjectIncludeMessage       include (id());

      owner->PutMessage (&include,message->callBack,sizeof(include));

      if (mOldId == message->callBack)
      {
        mOldId          = ID(-1,-1);
        mOldBlock.type  = Map::FREE;
        mOldBlock.cross = 1;
      }

      return TRUE;
    }
    case ALREADY_INCLUDE:
    {
      SubjectIncludeOkMessage* message = (SubjectIncludeOkMessage*)mes;

      if (mBoxes [message->res.boxClass])   //Увеличиваем колличестов патронов
      {
         mBoxes[message->res.boxClass]->AddStore (message->res.count);

         if (message->res.id == mCurWeapon)
         {
           WeaponGetCountMessage getCount;
           owner->PutMessage (&getCount,mWeapons[mCurWeapon],sizeof(getCount),id());
         }
         else
           RefreshFace();
      }

      if (message->res.id != -1 && mWeapons [message->res.id] == ID(-1,-1))
         LoadWeapon (message->res.gameCache,message->res.id,message->res.boxClass,message->res.count);

      return TRUE;
    }
    case ALREADY_LOAD:
    {
      AlreadyLoadMessage* message = (AlreadyLoadMessage*)mes;

      mWeapons [message->idRes] = message->object;

      owner->AddAttr (message->object,OBJ_UNLOAD_PROTECT);

                //Создаем коробку

      WeaponGetInfoMessage getInfo;

      owner->PutMessage (&getInfo,message->object,sizeof(getInfo),id());

                //Если индекс оружия выше - меняем оружие

      if (message->idRes >= mCurWeapon)
         SelectWeapon (message->idRes);

      return TRUE;
    }
    case WP_NO_PATRON:
    {
      dout<<"No patron!!"<<endl;

      NextWeapon();

      return TRUE;
    }
    case  WP_CHANGE_COUNT:
    {
      if (mes->callBack == mWeapons [mCurWeapon])
      {
        WeaponChangeCountMessage* inMes = (WeaponChangeCountMessage*)mes;
        GameBox*                  box   = (GameBox*)owner->GetObject(inMes->box);
        ViewChangeItemMessage    message  (IM_WP_COUNT,box->count());

        owner->PutMessage (&message,callBack,sizeof(ViewChangeItemMessage));
      }

      return TRUE;
    }
    case  WP_ANIM_CHANGE:
    {
      if (mCurWeapon!=-1 && mes->callBack == mWeapons [mCurWeapon])
      {
        WeaponChangeFaceMessage* inMes = (WeaponChangeFaceMessage*)mes;
        ViewChangeItemMessage    message (IM_AM_HAND,inMes->face);

        owner->PutMessage (&message,callBack,sizeof(ViewChangeItemMessage));
      }

      return TRUE;
    }
    case  WP_ANIM_MODE_ANSWER:
    {
      WeaponSetChangeModeOkMessage* inMes = (WeaponSetChangeModeOkMessage*)mes;
      ViewChangeItemMessage    message (IM_AM_HAND,inMes->face);

      owner->PutMessage (&message,callBack,sizeof(ViewChangeItemMessage));

      return TRUE;
    }
    case WP_INFO:
    {
      WeaponInfoMessage* message = (WeaponInfoMessage*)mes;

      if (mBoxes[message->res.gameBox] != NULL)
      {
        ViewReloadItemMessage change (IM_AM_HAND,message->res.sprites);
        ViewChangeItemMessage  view  (IM_WP_COUNT,mBoxes[message->res.gameBox]
                                     ->count());

        owner->PutMessage (&view,callBack,sizeof(ViewChangeItemMessage));
        owner->PutMessage (&change,callBack,sizeof(change));

          //Разрешение смены фаз

        WeaponSetChangeModeMessage changeOn (id());

        owner->PutMessage (&changeOn,mWeapons[mCurWeapon],sizeof(changeOn));
      }
      else
      {
        WeaponSetBoxMessage setBox (CreateBox(message->res.gameBox,0,message->res.max));

        owner->PutMessage (&setBox,message->callBack,sizeof(setBox));
      }

      return TRUE;
    }
    case  RELOAD_BOXES:
    {
      Flag flag2 ("RELOAD_BOXES",TRUE);

      ID* boxes = ((UserReloadBoxesMessage*)mes)->boxes;

      for (int i=0;i<USER_WEAPONS;i++)
          if (owner->GetObject(boxes[i])->type() != ERROR_OBJECT)
             mBoxes [i] = (GameBox*)owner->GetObject (boxes[i]);
          else
             mBoxes [i] = NULL;

      mBoxes [owner->GetRes("BOX_HEALTH")]       = &mHealth;
      mBoxes [owner->GetRes("BOX_PROTECT")]      = &mProtect;

      delete boxes;

      return TRUE;
    }
    case USER_REFRESH:
    {
      ViewChangeItemMessage message1 (IM_PLAYER_FACE,(100-mHealth.count())/20);
      WeaponGetInfoMessage  message3;
      WeaponGetCountMessage message2;

      owner->PutMessage (&message3,mWeapons[mCurWeapon],sizeof(message3),id());
      owner->PutMessage (&message2,mWeapons[mCurWeapon],sizeof(message2),id());
      owner->PutMessage (&message1,callBack,sizeof(message1));

      RefreshFace();

      return TRUE;
    }
    default:    return Player::Exec (mes);
  }
}

void            UserPlayer::dump (ostream& os) const
{
  os<<"Current weapon "<<mCurWeapon<<endl;
}

void            UserPlayer::RefreshFace  ()
{
  int playerFace = (100-mHealth.count())/20;

  if (playerFace<0) playerFace = 0;
  else if (playerFace>4) playerFace = 4;

  ViewChangeItemMessage message (IM_PLAYER_FACE,playerFace);
  ViewChangeItemMessage message3(IM_FRAG_COUNT,mFrags);
  ViewReloadItemMessage message1(IM_HEALTH,Palette::rgb(255-mHealth.count(),
                                                       140+mHealth.count(),
                                                       140+mHealth.count()));
  ViewReloadItemMessage message2(IM_PROTECT,Palette::rgb(255-mHealth.count(),
                                                       140+mHealth.count(),
                                                       140+mHealth.count()));

  owner->PutMessage (&message,callBack,sizeof(message));
  owner->PutMessage (&message1,callBack,sizeof(message1));
  owner->PutMessage (&message2,callBack,sizeof(message2));
  owner->PutMessage (&message3,callBack,sizeof(message3));
}

int             UserPlayer::IsPresent    (int index) const
{
  if (index < 0 || index >= USER_WEAPONS)
     return FALSE;

  if (ID(-1,-1) == mWeapons [index])
     return FALSE;

  return TRUE;
}

ID              UserPlayer::CreateBox    (int _box,int _cnt,int _max)
{
  if (_box<0)
     return ID(-1,-1);

        //Сoздаём коробку для патронов, или подключаем существующую

  if (mBoxes [_box] == NULL)
    mBoxes[_box] = new (GAME_BOX,id(),owner) GameBox (_cnt,_max);
  else
    mBoxes[_box]->AddStore(_cnt);

      //Защита от выгрузки во время перезагрузки

  owner->AddAttr (mBoxes[_box]->id(),OBJ_UNLOAD_PROTECT);

  return mBoxes [_box]->id();
}

void            UserPlayer::LoadWeapon  (int cache,int idRes,int _box,int cnt)
{
  WeaponExempl  param;

  param.box   = ID(-1,-1);
  param.count = 0;

        //Собственно загружаем новый вид оружия (предмета)

  LoadObjectMessage<WeaponExempl> message (cache,idRes,param);

  owner->PutMessage (&message,sizeof(message),id());
}

void            UserPlayer::SelectWeapon (int index)
{
  if (!IsPresent (index))
     return;

  if (mCurWeapon != -1)
  {
      //Отключаем анимацию текущего оружия

    WeaponSetOffChangeModeMessage message (id());

    owner->PutMessage (&message,mWeapons[mCurWeapon],sizeof(message));

      //Отменяем стрельбу

    WeaponShotOffMessage shotOff;

    owner->PutMessage (&shotOff,mWeapons[mCurWeapon],sizeof(shotOff));
  }

      //Анимация смены оружия

      //Смена вида оружия в руках

  WeaponGetInfoMessage message;

  owner->PutMessage (&message,mWeapons[index],sizeof(message),id());

      //Обнуляем положение

  ViewMoveStartMessage moveStart (IM_AM_HAND);
  owner->PutMessage (&moveStart,callBack,sizeof(moveStart));

  mMovePos   = 0;
  mCurWeapon = index;
}

int             UserPlayer::NextWeapon   ()
{
  int previos = mCurWeapon;

  for (int wpn=mCurWeapon+1;!IsPresent (wpn);)
    if (wpn++ >= USER_WEAPONS)
       wpn = 0;

  SelectWeapon (wpn);

  return previos;
}

int             UserPlayer::PrevWeapon   ()
{
  int previos = mCurWeapon;

  for (int wpn = mCurWeapon-1;!IsPresent (wpn);)
    if (wpn-- < 0)
       wpn = USER_WEAPONS - 1;

  SelectWeapon (wpn);

  return previos;
}

void            UserPlayer::Fire         ()
{
  WeaponShotOnMessage   on;
  WeaponShotOffMessage  off;

  owner->PutMessage (&on,mWeapons[mCurWeapon],sizeof(on));
  owner->PutMessage (&off,mWeapons[mCurWeapon],sizeof(off));
}

void            UserPlayer::tick        ()
{
  Player::tick ();

                //Если под нами предмет - взять его

  if (owner->GetObject(mOldId)->type() == SUBJECT)
  {
     SubjectGetInfoMessage      message (id());

     owner->PutMessage (&message,mOldId,sizeof(message));
  }

  if (!mDeath && !mActives[FIRE] && (mActives[FORWARD]-mActives[BACK] || mActives[STRAFE_LEFT]-mActives[STRAFE_RIGHT]))
  {
    if (clock()-mMoveLast>WEAPON_MOVE_TIME)
    {
      ViewMoveRelMessage message (IM_AM_HAND,mMoveTable[mMovePos].x,mMoveTable[mMovePos].y);

      owner->PutMessage (&message,callBack,sizeof(message));

      mMovePos  = ++mMovePos & WEAPON_MOVE_MASK;
      mMoveLast = clock();
    }
  }
  else if (mMovePos)
  {
        //Reset weapon view
    ViewMoveStartMessage moveStart (IM_AM_HAND);
    owner->PutMessage (&moveStart,callBack,sizeof(moveStart));
    mMovePos = 0;
  }
}

UserPlayer::UserPlayer(ArgPtr ptr)
           : Player (ptr), mCurWeapon (-1), mMovePos(0), mMoveLast(clock()),
             mFrags (0)
{
  memset (mWeapons,-1,sizeof(ID)*USER_WEAPONS);
  memset (mBoxes,0,sizeof(GameBox*)*USER_WEAPONS);

  mBoxes [owner->GetRes("BOX_HEALTH")]        = &mHealth;
  mBoxes [owner->GetRes("BOX_PROTECT")]       = &mProtect;

  CreateWeaponPlayer*  arg = (CreateWeaponPlayer*)ptr;

  LoadWeapon (arg->resourceClass.weaponCache,arg->resourceClass.weaponId);

        //Reset weapon view
  ViewMoveStartMessage moveStart (IM_AM_HAND);
  owner->PutMessage (&moveStart,callBack,sizeof(moveStart));

  RefreshFace ();
}

UserPlayer::UserPlayer(BinFile* file)
           : mMoveLast(clock()), mMovePos (0)
{
  Load (file);
}

UserPlayer::~UserPlayer()
{
  int health  = owner->GetRes("BOX_HEALTH"),
      protect = owner->GetRes("BOX_PROTECT");

  for (int i=0;i<USER_WEAPONS;i++)
  {
      if (i!=health && i!=protect && mBoxes [i])
         delete mBoxes [i];
      if (mWeapons [i] && owner->GetObject(mWeapons[i])->type() != ERROR_OBJECT)
         delete owner->GetObject(mWeapons[i]);
  }

  //Удаление взятыx предметов, сброс на пол
  //Опотрашение коробок

        //Reset weapon view
  ViewMoveStartMessage moveStart (IM_AM_HAND);
  owner->PutMessage (&moveStart,callBack,sizeof(moveStart));
}
