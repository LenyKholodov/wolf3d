#include "level.h"

#include <sbinfile.h>

#include <string.h>
#include <debug\debug.h>

BOOL    Check (void* lev)
{
  if (lev == NULL)
     return FALSE;
  else
     return TRUE;
}

int     TEXT_CACHE_SIZE       = -1;
int     SPRITE_CACHE_SIZE     = -1;
int     SPRLIST_CACHE_SIZE    = -1;
int     GAME_OBJ_CACHE_SIZE   = -1;
int     LOAD_RESOURCE_DELAY   = 0;
BOOL    KLIP_PLAY             = FALSE;
BOOL    MUSIC_PLAY            = TRUE;

///////////////////////implementation of class Level/////////////////////////

Level::Level ()
      : stageMap (NULL), lightMap(NULL),
        textCache(NULL), spriteCache(NULL),
        sprlistCache(NULL), resTable(NULL),
        sound(NULL),
        floorText (NULL), skyText (NULL),
        textCount (0),  spriteCount(0),
        bgiCount (0),   binCount (0), sprCount (0),
        subjectMap (NULL), klip (NULL), resp(NULL),
        respCount (0)
{
  for (int i=0;i<MAX_FONTS;i++)
  {
    bgiFont [i] = NULL;
    binFont [i] = NULL;
    sprFont [i] = NULL;
  }

  for (i=0;i<GAME_CACHE_NUM;i++)
      gameCache [i] = NULL;

  strcpy (levelName,"DEFAULT");
  strcpy (levelFile,"DEFAULT.LEV");
}

Level::~Level()
{
  Flag  flag ("~Level!");

  if (stageMap)    delete stageMap;
  if (lightMap)    delete lightMap;
  if (textCache)   delete textCache;
  if (spriteCache) delete spriteCache;
  if (sprlistCache)delete sprlistCache;
  if (sound)       delete sound;
  if (resTable)    delete resTable;
  if (subjectMap)  delete subjectMap;
  if (klip)        delete klip;
  if (resp)        delete resp;

  for (int i=0;i<MAX_FONTS;i++)
  {
    if (bgiFont [i])   free(bgiFont);
    if (binFont [i])   free(binFont);
    if (sprFont [i])   free(sprFont);
  }

  flag = TRUE;

  for (i=0;i<GAME_CACHE_NUM;i++)
      if (gameCache [i])
         delete gameCache [i];
}

NameCache*      Level::GetCache (int id) const { return gameCache [id]; }

EnvironmentMap*    Level::GetSubjectMap() const
{
  return subjectMap;
}

/////////////////////implementation of class LevelCompile////////////////////

void    LevelCompile::SetRespawn (const RespawnPoint* _resp,int count)
{
  if (count > MAX_RESPAWN) count = MAX_RESPAWN;
  respCount = count;
  memcpy (resp,_resp,count*sizeof(RespawnPoint));
}

void    LevelCompile::SetSubjectMap (const char* name)
{
  strcpy (subjectMap,name);
}

void    LevelCompile::SetCache     (int id,const char* name)
{
  strcpy (gameCache [id],name);
}

void    LevelCompile::SetSound     (const char* name)
{
  strcpy (sound,name);
}

void    LevelCompile::SetBGIFont   (int i,const char* name)
{
  if (i>MAX_FONTS) return;

  if (!bgiFile [i][0]) bgiCount++;

  strcpy (bgiFile [i],name);
}

void    LevelCompile::SetBinFont   (int i,const char* name)
{
  if (i>MAX_FONTS) return;

  if (!binFile [i][0]) binCount++;

  strcpy (binFile [i],name);
}

void    LevelCompile::SetSpriteFont   (int i,const char* name)
{
  if (i>MAX_FONTS) return;

  if (!sprFile [i][0]) sprCount++;

  strcpy (sprFile [i],name);
}

void    LevelCompile::SetSpriteListCache (const char* name)
{
  strcpy (sprlistCache,name);
}

void    LevelCompile::SetSpriteCache (const char* name)
{
  strcpy (spriteCache,name);
}

void    LevelCompile::SetTextCache   (const char* name)
{
  strcpy (textCache,name);
}

void    LevelCompile::SetMap       (const char* name)
{
  strcpy (stageMap,name);
}

void    LevelCompile::SetLightMap  (const char* name)
{
  strcpy (lightMap,name);
}

void    LevelCompile::SetLevelName (const char* name)
{
  strcpy (levelName,name);
}

void    LevelCompile::SetLevelFile (const char* name)
{
  strcpy (levelFile,name);
}

void    LevelCompile::SetScreenShot (const char* name)
{
  strcpy (screenShot,name);
}

void    LevelCompile::SetKlip (const char* name)
{
  strcpy (klipFile,name);
}

void    LevelCompile::SetResourceTable (const char* name)
{
  strcpy (resTable,name);
}

void    LevelCompile::SetSkyText   (int i)
{
  skyText = i;
}

void    LevelCompile::SetFloorText (int i)
{
  floorText = i;
}

void    LevelCompile::SetRenderType (int i)
{
  renderType = i;
}

BOOL    LevelCompile::Load (BinFile* file)
{
  dout<<"Level compile: Load level"<<endl;

  file->reset();

  WLFHeader  wlf;

  file->read (&wlf,sizeof(WLFHeader));

  if (memcmp(wlf.id,"WLF ",4))
  {
    dout<<"Level compile: Wrong file format"<<endl;
    return FALSE;
  }

  if (wlf.version != VERSION)  dout<<"Level compile: Wrong version. Continue loading"<<endl;
  if (wlf.copyright[0])        dout<<wlf.copyright<<endl;
        //levelName - первый член класса
  file->read (&levelName,sizeof(LevelCompile));

  return TRUE;
}

BOOL    LevelCompile::Load (const char* name)
{
  stdBinFile file;

  if (!file.open(name,BinFile::OPEN_RO))
  {
    dout<<"Level compile: File not found "<<name<<endl;
    return FALSE;
  }
  else
    dout<<"Level compile: load level '"<<name<<"'"<<endl;

  BOOL  state = Load(&file);

  file.close();

  strcpy (levelFile,name);

  return state;
}

void    LevelCompile::Save (BinFile* file)
{
  file->reset();

  WLFHeader  wlf;

  memcpy (wlf.id,"WLF ",4);
  strcpy (wlf.copyright,"Copyright by UNT Games Studio 2001-2002");

  wlf.version = VERSION;

  file->write (&wlf,sizeof(WLFHeader));
  file->write (&levelName,sizeof(LevelCompile)); //levelName - first member
}

void    LevelCompile::Save (const char* name)
{
  stdBinFile file;

  if (!file.open(name,BinFile::OPEN_CR))
  {
    dout<<"Level compile: Can't create file "<<name<<endl;
    return;
  }

  strcpy (levelFile,name);

  Save (&file);

  file.close();
}

Level*  LevelCompile::LoadLevel (BinFile* file)
{
  SendLoadMessage ("Process load files");

  Level*          level = new Level ();
  LevelCompile*   lComp = new LevelCompile (file);

  if (!Check(level)) { delete level; delete lComp; return NULL; }
  if (!Check(lComp)) { delete level; delete lComp; return NULL; }

  strcpy (level->levelName,lComp->levelName);
  strcpy (level->levelFile,lComp->levelFile);

  SendLevelName   (level->levelName);
  SendLoadMessage ("Resource table");

  level->resTable       = new ResourceIndex  (lComp->resTable);

  if (!Check(level->resTable)) { delete level; delete lComp;return NULL; }

  int sprlistCacheSize  = SPRLIST_CACHE_SIZE == -1 ?
                          level->resTable->GetIndex ("SPRLIST_CACHE_SIZE") :
                          SPRLIST_CACHE_SIZE;
  int textCacheSize     = TEXT_CACHE_SIZE == -1 ?
                          level->resTable->GetIndex ("TEXT_CACHE_SIZE") :
                          TEXT_CACHE_SIZE;
  int sprCacheSize      = SPRITE_CACHE_SIZE == -1 ?
                          level->resTable->GetIndex ("SPRITE_CACHE_SIZE") :
                          SPRITE_CACHE_SIZE;
  int gameCacheSize     = GAME_OBJ_CACHE_SIZE == -1 ?
                          level->resTable->GetIndex ("GAME_CACHE_SIZE") :
                          GAME_OBJ_CACHE_SIZE;

  if (sprlistCacheSize < 1) sprlistCacheSize = 5;
  if (sprCacheSize < 1)     sprCacheSize     = 5;
  if (textCacheSize < 1)    textCacheSize    = 5;
  if (gameCacheSize < 1)    textCacheSize    = 5;

  SendLoadMessage ("Game sounds");

  level->sound          = new SoundPak       (lComp->sound);
  level->bkMusic        = lComp->bkMusic;

  if (!Check(level->sound)) { delete level; delete lComp; return NULL;}

  SendLoadMessage ("Stage map");

  level->stageMap       = new Map            (lComp->stageMap);

  if (!Check(level->stageMap)) { delete level; delete lComp; return NULL; }

  SendLoadMessage ("Light map");

  level->lightMap       = new LightMap       (lComp->lightMap,level->stageMap);
  SendLoadMessage ("Textures cache");
  level->textCache      = new TextCache      (lComp->textCache,textCacheSize);
  SendLoadMessage ("Sprites cache");
  level->spriteCache    = new SpriteCache    (lComp->spriteCache,sprCacheSize);
  SendLoadMessage ("Sprite list cache");
  level->sprlistCache   = new SpriteListCache(lComp->sprlistCache,sprlistCacheSize);

  if (!Check(level->lightMap))  { delete level; delete lComp; return NULL; }
  if (!Check(level->textCache)) { delete level; delete lComp; return NULL; }
  if (!Check(level->spriteCache)){ delete level; delete lComp; return NULL; }
  if (!Check(level->sprlistCache)) { delete level; delete lComp; return NULL; }

  SendLoadMessage ("Floor/Sky textures");

  level->floorText      = makeMMT            (level->textCache->GetName(lComp->floorText));
  level->skyText        = makeMMT            (level->textCache->GetName(lComp->skyText));
  level->renderType     = lComp->renderType;

  if (!Check(level->floorText)) { delete level; delete lComp; return NULL; }
  if (!Check(level->skyText))   { delete level; delete lComp; return NULL; }

  SendLoadMessage ("Game objects cache");

  for (int i=0;i<GAME_CACHE_NUM;i++)
      if (lComp->gameCache[i][0])
      {
          level->gameCache[i]  = new NameCache (lComp->gameCache[i],gameCacheSize);
          if (!Check(level->gameCache[i])) { delete level; delete lComp; return NULL; }
      }

  level->bgiCount       = lComp->bgiCount;
  level->binCount       = lComp->binCount;
  level->sprCount       = lComp->sprCount;

  if (level->spriteCache) level->spriteCount = level->spriteCache->GetNum();
  if (level->textCache)   level->textCount   = level->textCache->GetNum();

  SendLoadMessage ("Enviroment map");

  level->subjectMap     = new EnvironmentMap (lComp->subjectMap);
  level->resp           = new RespawnPoint [lComp->respCount];
  level->respCount      = lComp->respCount;

  memcpy (level->resp,lComp->resp,lComp->respCount*sizeof(RespawnPoint));

  if (!Check(level->subjectMap)) { delete level; delete lComp; return NULL; }

  SendLoadMessage ("Game fonts");

  for (i=0;i<MAX_FONTS;i++)
  {
    if (lComp->bgiFile[i][0])
    {
         level->bgiFont [i] = tBGIFont::load (lComp->bgiFile[i]);

         if (!Check(level->bgiFont)) { delete level; delete lComp; return NULL; }
    }

      if (lComp->binFile[i][0])
      {
         level->binFont [i] = tBinFont::load (lComp->binFile[i]);

         if (!Check(level->binFont)) { delete level; delete lComp; return NULL; }
      }

      if (lComp->sprFile[i][0])
      {
         level->sprFont [i] = tSpriteFont::load (lComp->sprFile[i]);

         if (!Check(level->sprFont)) { delete level; delete lComp; return NULL; }
      }
  }

  if (KLIP_PLAY)
  {
    SendLoadMessage ("Level klip");

    level->klip = new KlipPlayer (lComp->klipFile);
  }
  else
    level->klip = NULL;

  delete lComp;

  return level;
}

Level*  LevelCompile::LoadLevel (const char* name)
{
  stdBinFile file;

  if (!file.open(name,BinFile::OPEN_RO))
  {
    dout<<"Level compile: File not found "<<name<<endl;
    return NULL;
  }
  else
    dout<<"Level compile: Load level '"<<name<<"'"<<endl;

  Level*  level = LoadLevel(&file);

  file.close();

  return level;
}

Level*       LevelCompile::ReloadLevel   (BinFile* file,LevelCompile* prev,Level* level)
{
  if (level == NULL || prev == NULL)
     return LoadLevel(file);

  SendLoadMessage ("Process load files");

  LevelCompile*   lComp = new LevelCompile (file);

  strcpy (level->levelName,lComp->levelName);
  strcpy (level->levelFile,lComp->levelFile);

  SendLevelName   (level->levelName);

  if (strcmp(prev->resTable,lComp->resTable))
  {
    delete level->resTable;
    SendLoadMessage ("Resource table");
    level->resTable       = new ResourceIndex  (lComp->resTable);
    if (!Check (level->resTable)) { delete level; delete lComp; return NULL; }
  }

  if (strcmp(prev->sound,lComp->sound))
  {
    delete level->sound;
    SendLoadMessage ("Game sounds");
    level->sound          = new SoundPak       (lComp->sound);
    if (!Check (level->sound)) { delete level; delete lComp; return NULL; }
  }

  level->bkMusic        = lComp->bkMusic;

  int sprlistCacheSize  = SPRLIST_CACHE_SIZE == -1 ?
                          level->resTable->GetIndex ("SPRLIST_CACHE_SIZE") :
                          SPRLIST_CACHE_SIZE;
  int textCacheSize     = TEXT_CACHE_SIZE == -1 ?
                          level->resTable->GetIndex ("TEXT_CACHE_SIZE") :
                          TEXT_CACHE_SIZE;
  int sprCacheSize      = SPRITE_CACHE_SIZE == -1 ?
                          level->resTable->GetIndex ("SPRITE_CACHE_SIZE") :
                          SPRITE_CACHE_SIZE;
  int gameCacheSize     = GAME_OBJ_CACHE_SIZE == -1 ?
                          level->resTable->GetIndex ("GAME_CACHE_SIZE") :
                          GAME_OBJ_CACHE_SIZE;

  if (sprlistCacheSize < 1) sprlistCacheSize = 5;
  if (sprCacheSize < 1)     sprCacheSize     = 5;
  if (textCacheSize < 1)    textCacheSize    = 5;
  if (gameCacheSize < 1)    textCacheSize    = 5;

  delete level->stageMap;
  delete level->lightMap;
  delete level->textCache;
  delete level->spriteCache;
  delete level->sprlistCache;

  SendLoadMessage ("Stage map");

  level->stageMap       = new Map            (lComp->stageMap);

  if (!Check(level->stageMap)) { delete level; delete lComp; return NULL; }

  SendLoadMessage ("Light map");
  level->lightMap       = new LightMap       (lComp->lightMap,level->stageMap);
  SendLoadMessage ("Textures cache");
  level->textCache      = new TextCache      (lComp->textCache,textCacheSize);
  SendLoadMessage ("Sprites cache");
  level->spriteCache    = new SpriteCache    (lComp->spriteCache,sprCacheSize);
  SendLoadMessage ("Sprites list cache");
  level->sprlistCache   = new SpriteListCache(lComp->sprlistCache,sprlistCacheSize);

  if (!Check(level->lightMap)) { delete level; delete lComp; return NULL; }
  if (!Check(level->textCache)) { delete level; delete lComp; return NULL; }
  if (!Check(level->spriteCache)){ delete level; delete lComp; return NULL; }
  if (!Check(level->sprlistCache)){ delete level; delete lComp; return NULL; }

  if (level->floorText) delete level->floorText;
  if (level->skyText)   delete level->skyText;

  SendLoadMessage ("Floor/Sky textures");

  level->floorText      = makeMMT            (level->textCache->GetName(lComp->floorText));
  level->skyText        = makeMMT            (level->textCache->GetName(lComp->skyText));
  level->renderType     = lComp->renderType;

  if (!Check(level->floorText)) { delete level; delete lComp; return NULL; }
  if (!Check(level->skyText))   { delete level; delete lComp; return NULL; }

  SendLoadMessage ("Game cache");

  for (int i=0;i<GAME_CACHE_NUM;i++)
    if (strcmp(lComp->gameCache[i],prev->gameCache[i]))
      if (lComp->gameCache[i][0])
      {
          delete level->gameCache [i];
          level->gameCache[i]  = new NameCache (lComp->gameCache[i],gameCacheSize);

          if (!Check(level->gameCache[i])) { delete level; delete lComp; return NULL; }
      }

  level->bgiCount       = lComp->bgiCount;
  level->binCount       = lComp->binCount;
  level->sprCount       = lComp->sprCount;

  if (level->spriteCache) level->spriteCount = level->spriteCache->GetNum();
  if (level->textCache)   level->textCount   = level->textCache->GetNum();

  if (strcmp(lComp->subjectMap,prev->subjectMap))
  {
    delete level->subjectMap;

    SendLoadMessage ("Subject map");
    level->subjectMap     = new EnvironmentMap (lComp->subjectMap);

    if (!Check(level->subjectMap)) { delete level; delete lComp; return NULL; }
  }

  if (level->resp) delete level->resp;

  level->resp           = new RespawnPoint [lComp->respCount];
  level->respCount      = lComp->respCount;

  memcpy (level->resp,lComp->resp,lComp->respCount*sizeof(RespawnPoint));

  SendLoadMessage ("Game fonts");

  for (i=0;i<MAX_FONTS;i++)
  {
    if (strcmp(lComp->bgiFile[i],prev->bgiFile[i]))
    {
         free (level->bgiFont[i]);
         level->bgiFont [i] = tBGIFont::load (lComp->bgiFile[i]);

         if (!Check (level->bgiFont[i])) { delete level; delete lComp; return NULL; }
    }

    if (strcmp(lComp->binFile[i],prev->binFile[i]))
    {
         free (level->binFont[i]);
         level->binFont [i] = tBinFont::load (lComp->binFile[i]);

         if (!Check (level->binFont[i])) { delete level; delete lComp; return NULL; }
    }

    if (strcmp(lComp->sprFile[i],prev->sprFile[i]))
    {
         free (level->sprFont[i]);
         level->sprFont [i] = tSpriteFont::load (lComp->sprFile[i]);

         if (!Check (level->sprFont[i])) { delete level; delete lComp; return NULL; }
    }
  }

  if (KLIP_PLAY)
  {
    SendLoadMessage ("Level klip");

    if (level->klip && strcmp (level->klip->GetName(),lComp->klipFile))
    {
      delete level->klip;
      level->klip = new KlipPlayer (lComp->klipFile);
    }
    else if (level->klip)
      level->klip->Reset();
    else
      level->klip = new KlipPlayer (lComp->klipFile);
  }
  else
    level->klip = NULL;

  delete lComp;

  return level;
}

Level*       LevelCompile::ReloadLevel   (const char* cur,Level* level)
{
  Flag  flag1 ("RELOAD_LEVEL");

  if (level == NULL)
    return LoadLevel(cur);

  stdBinFile file, file1;

  if (!file.open(cur,BinFile::OPEN_RO))
  {
    dout<<"Level compile: File not found '"<<cur<<"'"<<endl;
    return NULL;
  }

  if (!file1.open(level->levelFile,BinFile::OPEN_RO))
  {
    dout<<"Level compile: File not found '"<<level->levelFile<<"'"<<endl;
    return NULL;
  }

  dout<<"Level compile: Reload level '"<<level->levelFile<<"' to '"<<cur<<"'"<<endl;

  LevelCompile*   lComp = new LevelCompile (&file1);

  if (!Check(lComp)) { delete lComp; return NULL; }

  if (lComp)
  {
    level = ReloadLevel(&file,lComp,level);
    delete lComp;
  }

  file.close();
  file1.close();
  return level;
}

const char*  LevelCompile::ScreenShotName (const char* name,char* dest)
{
  stdBinFile file;

  if (!file.open(name,BinFile::OPEN_RO))
  {
    dout<<"Level compile: File not found "<<name<<endl;
    return NULL;
  }

  ScreenShotName(&file,dest);

  file.close();

  return dest;
}

const char*  LevelCompile::ScreenShotName (BinFile* file,char* dest)
{
  LevelCompile lComp (file);

  strcpy (dest,lComp.screenShot);

  return dest;
}

LevelCompile::LevelCompile (BinFile* file)      { Load (file); }
LevelCompile::LevelCompile (const char*name)    { Load (name); }

LevelCompile::LevelCompile ()
             :bgiCount (0), binCount (0), sprCount (0),
              textCount (0), spriteCount (0), floorText (0), skyText(0)
{
  stageMap   [0] = 0;
  lightMap   [0] = 0;
  textCache  [0] = 0;
  spriteCache[0] = 0;
  sprlistCache[0]=0;
  sound      [0] = 0;
  resTable   [0] = 0;
  subjectMap [0] = 0;

  for (int i=0;i<MAX_FONTS;i++)
  {
    bgiFile [i][0] = 0;
    binFile [i][0] = 0;
    sprFile [i][0] = 0;
  }

  for (i=0;i<GAME_CACHE_NUM;i++)
      gameCache [i][0] = 0;

  strcpy (levelName,"DEFAULT");
}
