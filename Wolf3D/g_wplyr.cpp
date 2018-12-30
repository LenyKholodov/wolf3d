#include "g_monstr.h"
#include "g_system.h"
#include "g_weap.h"

#include "t_wplr.h"

make <WeaponPlayer,WEAPON_PLAYER>       makeWeaponPlayer;

void            WeaponPlayer::Load (BinFile* file)
{
  Player::Load (file);

  file->read (&weapon,sizeof(ID));
}

void            WeaponPlayer::Save (BinFile* file)
{
  Player::Save (file);

  file->write (&weapon,sizeof(ID));
}

BOOL            WeaponPlayer::Exec (GameMessage* mes)
{
  switch (mes->id)
  {
    case ADD_FIRE: mActives [FIRE]++;   return TRUE;
    case SUB_FIRE: mActives [FIRE]--;   return TRUE;
    default:       return Player::Exec (mes);
  }
}

void    WeaponPlayer::Fire ()
{
  Weapon*       weaponPtr = (Weapon*)owner->GetObject(weapon);

  weaponPtr->Fire (GetX(),GetY(),GetAngle());

  if (mState != PlayerRes::ATACK)
  {
    mState    = PlayerRes::ATACK;
    mStatePos = 0;
  }
}

void    WeaponPlayer::tick ()
{
  if (mActives[FIRE])
    Fire();

  Player::tick();
}

WeaponPlayer::WeaponPlayer  (ArgPtr ptr)
            : Player (ptr)
{
  if (ptr == NULL)
     return;

  CreateWeaponPlayer* arg    = (CreateWeaponPlayer*)ptr;

        //создаём оружие

  CreateWeapon  _weapon;

  _weapon.resourceClass.Load (
      owner->GetCache(arg->resourceClass.weaponCache)->
             GetName (arg->resourceClass.weaponId));

  _weapon.param().count = -1;
  _weapon.param().box   = ID(-1,-1); //бесконечный запас

  weapon = owner->CreateObject (WEAPON,&_weapon,id());
}

WeaponPlayer::WeaponPlayer  (BinFile* file)
{
  Load (file);
}

WeaponPlayer::~WeaponPlayer ()
{
  owner->DeleteObject (weapon);
}
