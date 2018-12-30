#ifndef __WEAPON_PLAYER_RESOURCE__
#define __WEAPON_PLAYER_RESOURCE__

#include "t_player.h"

//////////////////////�������� ���㦥����� ��ப�////////////////////////////

struct  WeaponPlayerRes: public PlayerRes
{
  int           weaponCache,
                weaponId;

     BOOL        Load (const char*);
     BOOL        Load (BinFile*);
     void        Save (const char*);
     void        Save (BinFile*);
};

//////////////////////�������� ����㦥����� ��ப�///////////////////////////

typedef CreateExemplar <WeaponPlayerRes,PlayerExempl> CreateWeaponPlayer;

//////////////////////���࠭���� ����㦥����� ��ப�////////////////////////

struct  WeaponPlayerTag
{
  PlayerTag     player;

  ID            weapon;
};

#endif