#include "t_wplr.h"

#include <sbinfile.h>

#include <string.h>
#include <debug/debug.h>

///////////////////Загрузка - восстановление ресурса игрока//////////////////

void            WeaponPlayerRes::Save (BinFile* file)
{
  ResourceHeader::Save (file,GAME_MONSTR);    //WEAPON_PLAYER

  file->write (this,sizeof(WeaponPlayerRes));
}

void            WeaponPlayerRes::Save (const char* name)
{
  stdBinFile    file;

  if (!file.open (name,BinFile::OPEN_CR))
  {
    dout<<"Weapon Player Resource: Cannot create file "<<name<<endl;
    return ;
  }

  Save(&file);

  file.close();
}

BOOL            WeaponPlayerRes::Load (BinFile* file)
{
  file->seek (sizeof(ResourceHeader));
  file->read (this,sizeof(WeaponPlayerRes));

  return TRUE;
}

BOOL            WeaponPlayerRes::Load (const char* name)
{
  stdBinFile    file;

  if (!file.open (name,BinFile::OPEN_RO))
  {
    dout<<"Weapon Player Resource: File not found "<<name<<endl;
    return FALSE;
  }

  BOOL  state = Load (&file);

  file.close();

  return state;
}
