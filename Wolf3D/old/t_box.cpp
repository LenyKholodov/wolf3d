#include "t_box.h"

#include <sbinfile.h>

#include <string.h>
#include <debug/debug.h>

///////////////////Загрузка - восстановление ресурса оружия//////////////////

void            GameBoxRes::Save (BinFile* file)
{
  ResourceHeader::Save (file,GAME_BOX);

  file->write (this,sizeof(GameBoxRes));
}

void            GameBoxRes::Save (const char* name)
{
  stdBinFile    file;

  if (!file.open (name,BinFile::OPEN_CR))
  {
    dout<<"Game Box Resource: Cannot create file "<<name<<endl;
    return ;
  }

  Save(&file);

  file.close();
}

BOOL            GameBoxRes::Load (BinFile* file)
{
  file->seek (sizeof(ResourceHeader));
  file->read (this,sizeof(GameBoxRes));

  return TRUE;
}

BOOL            GameBoxRes::Load (const char* name)
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
