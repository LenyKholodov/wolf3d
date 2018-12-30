#ifndef __WEAPON_PLAYER_RESOURCE__
#define __WEAPON_PLAYER_RESOURCE__

#include "t_player.h"

//////////////////////Создание воруженного игрока////////////////////////////

struct  WeaponPlayerRes: public PlayerRes
{
  int           weaponCache,
                weaponId;

     BOOL        Load (const char*);
     BOOL        Load (BinFile*);
     void        Save (const char*);
     void        Save (BinFile*);
};

//////////////////////Создание вооруженного игрока///////////////////////////

typedef CreateExemplar <WeaponPlayerRes,PlayerExempl> CreateWeaponPlayer;

//////////////////////Сохранение вооруженного игрока////////////////////////

struct  WeaponPlayerTag
{
  PlayerTag     player;

  ID            weapon;
};

#endif