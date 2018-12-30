#include "sound.h"
#include "defs.h"
#include "g_types.h"

#include <string.h>

#include <debug\debug.h>

MidasSoundShell*      SoundPak::CreateShell (int ccount,BOOL runConfig,const char* cfgname)
{
  MidasSoundShell* sbShell = new MidasSoundShell;

  if (sbShell == NULL)
     return NULL;

  if(!sbShell->open(&mDirPak,ccount,runConfig,cfgname))
  {
    dout<<"Sound pak: Failed load sound dir pack"<<endl;
    return NULL;
  }
  if(!sbShell->load(mSamples,mDirPak.count()))
  {
    dout<<"Sound pak: Error assosiate list"<<endl;
    return NULL;
  }

  return sbShell;
}

int                   SoundPak::GetIndex    (const char* name)
{
  for (int i=0;i<mDirPak.count();i++)
      if (!strcmp (mSamples[i].name,name))
         return i;

  return 0;     //error -1
}

BOOL                  SoundPak::Load (BinFile* file)
{
  if (!mDirPak.openDP (file))   //PACK_CACHE_SIZE
     return FALSE;

  if (mSamples) delete mSamples;

  mSamples = new LoadSampleInfo [mDirPak.count()];

  const tFileListEntry*       list = mDirPak.GetDir();

  for (int i=0;i<mDirPak.count();i++,list++)
  {
      strcpy (mSamples[i].name,list->name);
      mSamples[i].loop = FALSE;
  }

  return TRUE;
}

BOOL                  SoundPak::Load (const char* name)
{
  if (!mDirPak.openDP (name,SOUND_CACHE_SIZE))
     return FALSE;

  if (mSamples) delete mSamples;

  mSamples = new LoadSampleInfo [mDirPak.count()];

  const tFileListEntry*       list = mDirPak.GetDir();

  for (int i=0;i<mDirPak.count();i++,list++)
  {
      strcpy (mSamples[i].name,list->name);

      if (!memcmp ("BK_",mSamples[i].name,3) ||
          !memcmp ("bk_",mSamples[i].name,3))  mSamples[i].loop = TRUE;
      else                                     mSamples[i].loop = FALSE;
  }

  return TRUE;
}

SoundPak::SoundPak  (const char* name)
         : mSamples (NULL)
       { Load (name); }

SoundPak::SoundPak  (BinFile* file)
         : mSamples (NULL)
       { Load (file); }

SoundPak::~SoundPak ()
{
  if (mSamples) delete mSamples;
  mDirPak.closeDP();
}
