#include "t_effect.h"

#include <sbinfile.h>

#include <string.h>
#include <debug/debug.h>

///////////////////Загрузка - восстановление ресурса эффекта//////////////////

void            EffectRes::Save (BinFile* file)
{
  ResourceHeader::Save (file,EFFECT);

  file->write (this,sizeof(EffectRes));
}

void            EffectRes::Save (const char* name)
{
  stdBinFile    file;

  if (!file.open (name,BinFile::OPEN_CR))
  {
    dout<<"Effect Resource: Cannot create file "<<name<<endl;
    return ;
  }

  Save(&file);

  file.close();
}

BOOL            EffectRes::Load (BinFile* file)
{
  file->seek (sizeof(ResourceHeader));
  file->read (this,sizeof(EffectRes));

  return TRUE;
}

BOOL            EffectRes::Load (const char* name)
{
  stdBinFile    file;

  if (!file.open (name,BinFile::OPEN_RO))
  {
    dout<<"Effect Resource: File not found "<<name<<endl;
    return FALSE;
  }

  BOOL  state = Load (&file);

  file.close();

  return state;
}

//////////////Загрузка - восстановление ресурса спрайтового эффекта//////////////////

void            SpriteEffectRes::Save (BinFile* file)
{
  ResourceHeader::Save (file,SPRITE_EFFECT);

  file->write (this,sizeof(SpriteEffectRes));
}

void            SpriteEffectRes::Save (const char* name)
{
  stdBinFile    file;

  if (!file.open (name,BinFile::OPEN_CR))
  {
    dout<<"Sprite Effect Resource: Cannot create file "<<name<<endl;
    return ;
  }

  Save(&file);

  file.close();
}

BOOL            SpriteEffectRes::Load (BinFile* file)
{
  file->seek (sizeof(ResourceHeader));
  file->read (this,sizeof(SpriteEffectRes));

  return TRUE;
}

BOOL            SpriteEffectRes::Load (const char* name)
{
  stdBinFile    file;

  if (!file.open (name,BinFile::OPEN_RO))
  {
    dout<<"Sprite Effect Resource: File not found "<<name<<endl;
    return FALSE;
  }

  BOOL  state = Load (&file);

  file.close();

  return state;
}

//////////////Загрузка - восстановление ресурса спрайтового эффекта//////////////////

void            LightEffectRes::Save (BinFile* file)
{
  ResourceHeader::Save (file,LIGHT_EFFECT);

  file->write (this,sizeof(LightEffectRes));
}

void            LightEffectRes::Save (const char* name)
{
  stdBinFile    file;

  if (!file.open (name,BinFile::OPEN_CR))
  {
    dout<<"Light Effect Resource: Cannot create file "<<name<<endl;
    return ;
  }

  Save(&file);

  file.close();
}

BOOL            LightEffectRes::Load (BinFile* file)
{
  file->seek (sizeof(ResourceHeader));
  file->read (this,sizeof(LightEffectRes));

  return TRUE;
}

BOOL            LightEffectRes::Load (const char* name)
{
  stdBinFile    file;

  if (!file.open (name,BinFile::OPEN_RO))
  {
    dout<<"Light Effect Resource: File not found "<<name<<endl;
    return FALSE;
  }

  BOOL  state = Load (&file);

  file.close();

  return state;
}