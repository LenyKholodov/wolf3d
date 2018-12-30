#include "t_telep.h"
#include "t_res.h"

#include <sbinfile.h>

#include <string.h>
#include <debug/debug.h>

///////////////////Загрузка - восстановление ресурса оружия//////////////////

void            TeleportRes::Save (BinFile* file,GameObjects type)
{
  ResourceHeader::Save (file,type);

  file->write (this,sizeof(TeleportRes));
}

void            TeleportRes::Save (const char* name,GameObjects type)
{
  stdBinFile    file;

  if (!file.open (name,BinFile::OPEN_CR))
  {
    dout<<"Teleport resource: Cannot create file "<<name<<endl;
    return ;
  }

  Save(&file,type);

  file.close();
}

BOOL            TeleportRes::Load (BinFile* file)
{
  file->seek (sizeof(ResourceHeader));
  file->read (this,sizeof(TeleportRes));

  return TRUE;
}

BOOL            TeleportRes::Load (const char* name)
{
  stdBinFile    file;

  if (!file.open (name,BinFile::OPEN_RO))
  {
    dout<<"Teleport resource: File not found "<<name<<endl;
    return FALSE;
  }

  BOOL  state = Load (&file);

  file.close();

  return state;
}