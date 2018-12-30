#ifndef __LEVEL__
#define __LEVEL__

#include <types.h>
#include <grtypes.h>
#include <klip.h>

#include "map.h"
#include "light.h"
#include "sprite.h"
#include "textures.h"
#include "sprlist.h"
#include "sound.h"

#include "g_defs.h"
#include "t_res.h"
#include "t_resp.h"

const   int     LEVEL_NAME_LENGTH = 32;
const   int     MAX_FONTS         = 8;
const   int     VERSION           = MAKELONG (0,1);  //v 1.0
const   int     GAME_CACHE_NUM    = 8;
const   int     GAME_CACHE_SIZE   = 8;
const   int     GAME_RES_SIZE     = 256;
const   int     MAX_RESPAWN       = 16;

extern  int     TEXT_CACHE_SIZE;
extern  int     SPRITE_CACHE_SIZE;
extern  int     SPRLIST_CACHE_SIZE;
extern  int     GAME_OBJ_CACHE_SIZE;
extern  int     LOAD_RESOURCE_DELAY;
extern  BOOL    KLIP_PLAY;

/////////////////////////////////////////////////////////////////////////////
///Индикатор загрузки
/////////////////////////////////////////////////////////////////////////////
extern  void    SendLoadMessage (const char*);
extern  void    SendLevelName   (const char*);

/////////////////////////////////////////////////////////////////////////////
////Wolfenstein 3D Level File Header
/////////////////////////////////////////////////////////////////////////////
struct  WLFHeader
{
  char          id        [4];    //"WLF "
  ulong         version;          // 1.0
  char          copyright [128];
};

/////////////////////////////////////////////////////////////////////////////
////Структура необходимая для работы с уровнем
/////////////////////////////////////////////////////////////////////////////
class   LevelCompile;
class   Level
{
  friend class LevelCompile;
  protected:
        char            levelName[LEVEL_NAME_LENGTH];
        char            levelFile[LEVEL_NAME_LENGTH];
        KlipPlayer*     klip;

        ResourceIndex*  resTable;

        int             renderType;

        Map*            stageMap;
        LightMap*       lightMap;

        TextCache*       textCache;
        SpriteCache*     spriteCache;
        SpriteListCache* sprlistCache;

        int             textCount;
        int             spriteCount;

        MMT*            floorText;
        MMT*            skyText;

        tBGIFont*       bgiFont [MAX_FONTS];
        tBinFont*       binFont [MAX_FONTS];
        tSpriteFont*    sprFont [MAX_FONTS];

        int             bgiCount;
        int             binCount;
        int             sprCount;

        SoundPak*       sound;
        int             bkMusic;

        NameCache*       gameCache [GAME_CACHE_NUM];
        EnvironmentMap*  subjectMap;

        RespawnPoint*    resp;
        int              respCount;

  protected:
                Level ();
  public:

   virtual      ~Level ();

/////////////////////////////////////////////////////////////////////////////
/////Установки
/////////////////////////////////////////////////////////////////////////////

   const char*  GetLevelName ()  const        { return levelName; }
   const char*  GetLevelFile ()  const        { return levelFile; }

   KlipPlayer*  GetKlip      ()  const        { return klip;      }

   SoundPak*    GetSound     ()  const        { return sound;    }
   int          GetBkMusic   ()  const        { return bkMusic;  }

   Map*         GetMap       ()  const        { return stageMap; }
   LightMap*    GetLightMap  ()  const        { return lightMap; }
   int          GetRenderType()  const        { return renderType; }

   TextCache*       GetTextCache   ()  const        { return textCache;   }
   SpriteCache*     GetSpriteCache ()  const        { return spriteCache; }
   SpriteListCache* GetSpriteListCache () const     { return sprlistCache;}

   MMT*         GetFloorText   ()  const      { return floorText; }
   MMT*         GetSkyText     ()  const      { return skyText;   }

   tBGIFont*    GetBGIFont   (int i)  const   { return i<bgiCount ? bgiFont[i] : NULL; }
   tBinFont*    GetBinFont   (int i)  const   { return i<binCount ? binFont[i] : NULL; }
   tSpriteFont* GetSpriteFont(int i)  const   { return i<sprCount ? sprFont[i] : NULL; }

   ResourceIndex* GetResourceTable () const { return resTable ;}
   NameCache*     GetCache   (int cacheId) const;

   EnvironmentMap*    GetSubjectMap() const;

   int            GetRespawnCount  () const { return respCount; }
   RespawnPoint*  GetRespawn       () const { return resp;      }
};

/////////////////////////////////////////////////////////////////////////////
////Класс производящий все опреции (на файловом уровне) для загрузки уровня
/////////////////////////////////////////////////////////////////////////////
////Внимание! Потомков с виртуальными функциями быть не должно!
/////////////////////////////////////////////////////////////////////////////
class   LevelCompile
{
  protected:
        char            levelName   [LEVEL_NAME_LENGTH];
        char            levelFile   [LEVEL_NAME_LENGTH];
        char            screenShot  [LEVEL_NAME_LENGTH];
        char            klipFile    [LEVEL_NAME_LENGTH];

        char            resTable [128];

        char            bgiFile [MAX_FONTS][128];
        char            binFile [MAX_FONTS][128];
        char            sprFile [MAX_FONTS][128];

        char            stageMap    [128];
        char            lightMap    [128];
        char            spriteCache [128];
        char            textCache   [128];
        char            sprlistCache[128];

        int             textCount;
        int             spriteCount;
        int             renderType;

        int             floorText, skyText;

        int             bgiCount;
        int             binCount;
        int             sprCount;

        char            sound       [128];
        int             bkMusic;

        char            gameCache   [GAME_CACHE_NUM][128];
        char            subjectMap  [128];

        RespawnPoint    resp  [MAX_RESPAWN];
        int             respCount;

  public:
                         LevelCompile ();
                         LevelCompile (BinFile*);
                         LevelCompile (const char*);

            void         SetLevelName (const char*);
            void         SetLevelFile (const char*);
            void         SetScreenShot(const char*);
            void         SetKlip      (const char*);

            void         SetMap       (const char*);
            void         SetLightMap  (const char*);
            void         SetRenderType(int);

   const    char*        GetMap       () const { return stageMap; }
   const    char*        GetLightMap  () const { return lightMap; }

            void         SetSpriteCache     (const char*);
            void         SetSpriteListCache (const char*);
            void         SetTextCache       (const char*);

            void         SetBGIFont   (int i,const char*);
            void         SetBinFont   (int i,const char*);
            void         SetSpriteFont(int i,const char*);

            void         SetSkyText   (int);
            void         SetFloorText (int);

            void         SetSound     (const char*);
            void         SetBkMusic   (int i) { bkMusic = i; }

            void         SetResourceTable (const char*);

            void         SetCache (int cacheId,const char*);
            void         SetSubjectMap (const char*);

            void         SetRespawn (const RespawnPoint*,int);

/////////////////////////////////////////////////////////////////////////////
//////Загрузка уровня
/////////////////////////////////////////////////////////////////////////////
///////Дополнительные параметры - размеры кэша (текстур и спрайтов)
/////////////////////////////////////////////////////////////////////////////

   static   Level*       LoadLevel     (BinFile*);
   static   Level*       LoadLevel     (const char*);

   static   Level*       ReloadLevel   (BinFile*,LevelCompile*,Level*);
   static   Level*       ReloadLevel   (const char*,Level* = NULL);

   static   const char*  ScreenShotName (const char*,char* shotName);
   static   const char*  ScreenShotName (BinFile*,char* shotName);

/////////////////////////////////////////////////////////////////////////////
/////Загрузка - сохранение
/////////////////////////////////////////////////////////////////////////////
            BOOL         Load (BinFile*);
            BOOL         Load (const char*);
            void         Save (BinFile*);
            void         Save (const char*);

};


#endif