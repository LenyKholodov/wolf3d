#include "t_weap.h"

#include <sbinfile.h>

#include <string.h>
#include <debug/debug.h>


///////////////////Загрузка - восстановление ресурса игрока//////////////////

void            PlayerRes::Save (BinFile* file)
{
  ResourceHeader::Save (file,GAME_PLAYER);

  file->write (this,sizeof(PlayerRes));
}

void            PlayerRes::Save (const char* name)
{
  stdBinFile    file;

  if (!file.open (name,BinFile::OPEN_CR))
  {
    dout<<"Player Resource: Cannot create file "<<name<<endl;
    return ;
  }

  Save(&file);

  file.close();
}

BOOL            PlayerRes::Load (BinFile* file)
{
  file->seek (sizeof(ResourceHeader));
  file->read (this,sizeof(PlayerRes));

  return TRUE;
}

BOOL            PlayerRes::Load (const char* name)
{
  stdBinFile    file;

  if (!file.open (name,BinFile::OPEN_RO))
  {
    dout<<"Player Resource: File not found "<<name<<endl;
    return FALSE;
  }

  BOOL  state = Load (&file);

  file.close();

  return state;
}
