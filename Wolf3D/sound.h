#ifndef __GAME_SOUND__
#define __GAME_SOUND__

#include <dirpak.h>
#include <midasshl.h>
//#include <varpool.h>

class   SoundPak
{
  public:
                        SoundPak  (const char*);
                        SoundPak  (BinFile*);
          virtual       ~SoundPak ();

          MidasSoundShell*      CreateShell (int ccount=8,
                                             BOOL runConfig=FALSE,
                                             const char* cfgname="midas.cfg");
          int                   GetIndex    (const char*);

          BOOL                  Load (const char*);
          BOOL                  Load (BinFile*);

  protected:
          DirPak                mDirPak;
          LoadSampleInfo*       mSamples;
};


#endif