#include "t_weap.h"

#include <sbinfile.h>

#include <string.h>
#include <debug/debug.h>


///////////////////Загрузка - восстановление ресурса оружия//////////////////

void            WeaponRes::Save (BinFile* file)
{
  ResourceHeader::Save (file,WEAPON);

  file->write (this,sizeof(WeaponRes));
}

void            WeaponRes::Save (const char* name)
{
  stdBinFile    file;

  if (!file.open (name,BinFile::OPEN_CR))
  {
    dout<<"Weapon Resource: Cannot create file "<<name<<endl;
    return ;
  }

  Save(&file);

  file.close();
}

BOOL            WeaponRes::Load (BinFile* file)
{
  file->seek (sizeof(ResourceHeader));
  file->read (this,sizeof(WeaponRes));

  return TRUE;
}

BOOL            WeaponRes::Load (const char* name)
{
  stdBinFile    file;

  if (!file.open (name,BinFile::OPEN_RO))
  {
    dout<<"Weapon Resource: File not found "<<name<<endl;
    return FALSE;
  }

  BOOL  state = Load (&file);

  file.close();

  return state;
}
