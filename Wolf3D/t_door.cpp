#include "g_types.h"

#include <sbinfile.h>

#include <string.h>
#include <debug/debug.h>


///////////////////Загрузка - восстановление ресурса оружия//////////////////

void            DoorRes::Save (BinFile* file)
{
  ResourceHeader::Save (file,DOOR);

  file->write (this,sizeof(DoorRes));
}

void            DoorRes::Save (const char* name)
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

BOOL            DoorRes::Load (BinFile* file)
{
  file->seek (sizeof(ResourceHeader));
  file->read (this,sizeof(DoorRes));

  return TRUE;
}

BOOL            DoorRes::Load (const char* name)
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
