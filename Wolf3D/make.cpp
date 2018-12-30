#include "sprite.h"
#include "sprite.h"
#include "level.h"

#include "t_weap.h"
#include "t_wplr.h"
#include "t_box.h"
#include "t_subj.h"
#include "t_door.h"
#include "t_telep.h"
#include "t_iq.h"
#include "t_effect.h"

#include <process.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <picture.h>
#include <sbinfile.h>
#include <cbinfile.h>

#include <debug\debug.h>

enum  LineType {
      SPRITE_HEADER = 1,
      FACE_HEADER,
      IMAGE_HEADER,
      TEXT_LIST,
      SPRITE_LIST,
      END,
      COMMENT,
      DEST_DIR,
      SRC_DIR,
      CACHE_FILE,
      MAP_HEADER,
      LEVEL_HEADER,
      FLOOR,
      SKY,
      BGI_FONT,
      BIN_FONT,
      SPR_FONT,
      SOUND_PAK_HEADER,
      MUSIC,
      RESOURCE_LIST,
      SPRITELIST_LIST,
      SPRITELIST_HEADER,
      RENDER_TYPE,
      SCREEN_SHOT,
      KLIP,
      DOOM2WSF,
      WAD_DIR,
      GAME_USE_LIST,
      G_WEAPON,
      G_BOX,
      G_SUBJECT_LIST,
      G_DOOR,
      G_PLAYER,
      G_TELEPORT,
      G_PORTAL,
      G_SPRITE_EFFECT,
      G_LIGHT_EFFECT,
      GRAPH_BPP,
      RESPAWN
      };

enum ObjectType
{
     RES_SUBJECT,
     RES_DOOR,
     RES_PLAYER,
     RES_TELEPORT,
     RES_PORTAL,
     RES_SPRITE_EFFECT,
     RES_LIGHT_EFFECT
};

struct  Param
{
        LineType        type;           //type of string-line
        char            name[128];      //name (file)
        char            string [128];   //Дополнительная строка
        int             num;            //count of objects
        int             index;          //index in list
        int             z_color;        //for simple sprite
};

char*           NextLine        ();
BOOL            ProcessLine     (char*,Param&);
char*           NextWord        (char*,int& pos);

BinFile*        OpenFile        (const char*);
void            CloseFile       ();
void            CloseAll        ();
int             current         ();

int             Process              ();
int             ProcessLevel         ();
GameSprite3D*   ProcessSpriteHeader  (const Param&);
Sprite3D*       ProcessSpriteFace    (const Param&);
SimpleSprite*   ProcessSpriteImage   (const Param&);
void            ProcessSprListHeader (const Param&);
tSprite*        ProcessSprListImage  (char* name,int z_color);
int             ProcessText          (char* []);
int             ProcessSpriteList    (char* []);
int             ProcessSpriteListList(char* names[]);
BOOL            ProcessResourceList  ();
BOOL            ProcessSoundPak      (const char*,ResourceTable*);
BOOL            ProcessRender        (int&);
int             ProcessRespawn       (RespawnPoint*);

void            AddResFileEntry      (const char*,const char* prefix,int);
char*           TranslateFileName    (const char*,char*,char* mainDirName);

void            ProcessStateList     (char* line,char* stateList,int _pos);
WeaponRes*      ProcessWeapon        ();
WeaponRes::Shot ProcessShotHeader    ();
SubjectRes*     ProcessSubject       ();
void            ProcessTeleport      (Param& param,int);
void            ProcessSpriteEffect  (Param& param);
void            ProcessLightEffect   (Param& param);
void            ProcessDoor          (Param&);
void            ProcessPlayer        (Param&);
EnvironmentMap* ProcessSubjectList   (Param&);

BOOL            ProcessSubjectEx     (char*,Exemplar&);
BOOL            ProcessTeleportEx    (char*,Exemplar&);
BOOL            ProcessPortalEx      (char*,Exemplar&);
BOOL            ProcessDoorEx        (char*,Exemplar&);
BOOL            ProcessPlayerEx      (char*,Exemplar&);
BOOL            ProcessLightEffectEx (char*,Exemplar&);
BOOL            ProcessSpriteEffectEx(char*,Exemplar&);

void            SaveSprite          (const char*,GameSprite3D*);
void            MakeSpriteCache     (const char*,char* files[],int count);
void            MakeTextCache       (const char*,char* files[],int count);
void            MakeSpriteListCache (const char* name,char* files[],int count);
void            MakeMap             (const char*);
void            MakeLevel           (const char*,const char*);
int             MakeBGIFont         ();
int             MakeBinFont         ();
int             MakeSpriteFont      ();
void            MakeGameCache       (int id,const char* prefix);
void            MakeDOOM2WSF        (const char*,const char*);

char            tempBuffer   [1024];
char*           temp1        = tempBuffer;
char*           temp2        = tempBuffer+512;

int             curFile      = 0;
stdBinFile      stdBinFileTable [16];
BinFile*        fileTable       [16];
char            fileName        [16][128];
int             lines           [16];

char            destDirName     [128]      = "";
char            srcDirName      [128]      = "";
char            wadDirName      [128]      = "";

char            gameCacheName   [128]      = "";
char            spriteCacheName [128]      = "default.wsc";
char            spriteListCacheName [128]  = "default.wsl";
char            textCacheName   [128]      = "default.wtf";
char            srcMapName      [128]      = "default.txt";
char            stageMapName    [128]      = "default.map";
char            lightMapName    [128]      = "default.lmf";
int             floorText                  = 0;
int             skyText                    = 0;
char            resName         [128]      = "default.res";
char            soundPak        [128]      = "default.pak";
char            subMap          [128]      = "default.sbj";
char            screenShot      [128]      = "default.bmp";
char            klipFile        [128]      = "default.klp";

char            bgiFont         [16][128];
char            binFont         [16][128];
char            sprFont         [16][128];

int             bgiCount, binCount, sprCount;
int             renderType;
int             bkMusic;

int             respCount;
RespawnPoint    resp [MAX_RESPAWN];

char            subjectTable    [GAME_CACHE_NUM][64][128];
char            gameCacheTable  [GAME_CACHE_NUM][128];

ResourceIndex   res;

int                     line    = 0;
int                     grBPP   = 1;

char*                   nPal         = "normpal.exe";
char*                   mmtMake8bit  = "mmt8.exe";
char*                   mmtMake16bit = "mmt16.exe";

#define                 NO_ERROR 0
#define                 MAX_TEXT 4096
#define                 MAX_SPRITELIST 4096

int     main(int argc,char* argv[])
{
  dout<<endl<<"UNT Game Studio. Resource compile utility v 1.0"<<endl;
  if (argc<2 || argc>3)
  {
    dout<<"make.txt"<<endl;
    return 1;
  }

  for (int i=0;i<16;i++)
    fileTable [i] = NULL;

  if (OpenFile (argv[1]) == NULL)
    return 1;

  int l;

  if (l = Process())
  {
    dout<<"File '"<<fileName[current()]<<"' Error in line "<<l<<endl;
    CloseAll();
    return 1;
  }

  CloseAll();

  return 0;
}

int             current         ()
{
  return curFile-1;
}

BinFile*        OpenFile        (const char* name)
{
  if (curFile==16)
     return fileTable [current()];

  if (!stdBinFileTable[curFile].open(name,BinFile::OPEN_RO))
  {
    dout<<"Make file not found '"<<name<<"' ("<<current()<<")"<<endl;
    return fileTable[curFile];
  }

  lines [curFile] = 1;
  strcpy (fileName[curFile],name);

  cacheBinFile* cf = new cacheBinFile (&stdBinFileTable[curFile],DEFAULT_CACHE_SIZE);

  if (cf!=NULL && !cf->bad())
  {
    return fileTable [curFile++] = cf;
  }
  else
  {
    if (cf) delete cf;
    fileTable [curFile] = &stdBinFileTable [curFile++];
  }

  return fileTable[curFile];
}

void            CloseFile       ()
{
  if (current()<0)
     return;

  if (fileTable [curFile] == NULL)
  {
     if (curFile!=0) curFile--;
     return;
  }
  if (fileTable[curFile] != &stdBinFileTable[curFile])
  {
     delete fileTable [curFile];
     fileTable [curFile]=NULL;
  }

  stdBinFileTable[curFile--].close();
}

void            CloseAll        ()
{
  while (current()>=0)
        CloseFile();
}

/////////////////////Добавление имени ресурса////////////////////////////////

void            AddResFileEntry          (const char* name,const char* prefix,int x)
{
   char         dest [128];
   int          len = strlen (prefix);

   memcpy (dest,prefix,len);

   TranslateFileName (name,dest+len,destDirName);

   len = strlen (dest);

   for (int i=0;i<len;dest[i]=toupper(dest[i]),i++);

   dout<<"\t\t "<<dest<<endl;

   res.SetIndex (dest,x);
}

///////////////Сохраняет 3D-Sprite///////////////////////////////////////////

void    SaveSprite (const char* name,GameSprite3D* s3D)
{
  char    dest [256];

  strcpy (dest,destDirName);
  strcat (dest,"/");
  strcat (dest,name);

  dout<<"Save sprite: "<<dest<<endl;

  s3D->Save (dest);
}

/////////////////Создаёт кэш спрайтов///////////////////////////////////////

void    MakeSpriteCache    (const char* name,char* files[],int count)
{
  NameCache* wsf = new NameCache(files,count,count,1,SPRITE_SIZE);

  cout<<"Save Sprites Cache "<<name<<endl;
  wsf->Save(name);

  delete wsf;
}

//////////////////Создаёт кэш текстур////////////////////////////////////////

void    MakeTextCache    (const char* name,char* files[],int count)
{
  NameCache* wsf = new NameCache(files,count,count,1,TEXT_SIZE);

  cout<<"Save Textures Cache "<<name<<endl;
  wsf->Save(name);

  delete wsf;
}

//////////////////Создаём кэш SpriteList////////////////////////////////////

void    MakeSpriteListCache    (const char* name,char* files[],int count)
{
  NameCache* wsf = new NameCache(files,count,count,1,SPRITE_LIST_SIZE);

  cout<<"Save Sprite List Cache "<<name<<endl;

  wsf->Save(name);

  delete wsf;
}

////////////////////Препроцессинг звуков/////////////////////////////////////

BOOL    ProcessSoundPak (const char* _name,ResourceTable* rTable)
{
  DirPak dpak;

  if(!dpak.openDP(_name))
  {
    dout<<"File not found '"<<_name<<"'"<<endl;
    return FALSE;
  }

  const tFileListEntry*       list = dpak.GetDir();
  char                        dest [128];

  for (int i=0;i<dpak.count();i++,list++)
  {
      const char* fst = list->name;
      const char* lst = strchr (fst,'.');

      memcpy (dest,"SND_",4);
      memcpy (dest+4,fst,(long)(lst-fst));

      dest [(long)(lst-fst)+4] = '\0';

      rTable->SetIndex (dest,i);
  }

  return TRUE;
}

void    MakeRes (const char* name)
{
  res.Save (name);
}

////////////////////Создаётся карта//////////////////////////////////////////

void    MakeMap (const char* name)
{
  char* argList [4];

  argList [1] = srcMapName;
  argList [2] = stageMapName;
  argList [3] = NULL;

  dout<<"Create stage map "<<stageMapName<<endl;

  spawnv (0,"crmap.exe",argList);

  argList [1] = stageMapName;
  argList [2] = "190";
  argList [3] = lightMapName;
  argList [4] = NULL;

  dout<<"Create light map "<<lightMapName<<endl;

  spawnv (0,"crlmap.exe",argList);
}

//////////////////Создаётся игровой список///////////////////////////////////

void    MakeGameCache (int i,const char* prefix)
{
  char*  names [4096];

  int   pos       = 0;
  int   index     = 0;

  char  name [128];

  for (char* procStr = NextWord(NextLine(),pos);memcmp(procStr,"END",3);pos=0,
             procStr = NextWord(NextLine(),pos),index++)
  {
    strcpy (name,destDirName);
    strcat (name,"/");
    strcat (name,procStr);

    names [index] = new char [strlen(name)+1];

    strcpy (names[index],name);

    dout<<"\t\t\t"<<name<<endl;

    strcpy(subjectTable [i][index],name);

    AddResFileEntry (names[index],prefix,index);
  }

  char  dest [128];

  strcpy (dest,gameCacheName);
  strcat (dest,itoa (i,name,16));
  strcat (dest,".wgc");

  strcpy (gameCacheTable[i],dest);

  NameCache* cache = new NameCache (names,index,index,1,GAME_RES_SIZE);

  cache->Save (dest);

  delete cache;

  for (i=0;i<index;delete names [i++]);
}

//////////////////Создание спрайта из DOOM///////////////////////////////////

void            MakeDOOM2WSF        (const char* mName,const char* mPath)
{
  char dest [128];

  strcpy (dest,destDirName);
  strcat (dest,"/");
  strcat (dest,mPath);

  char* argList [5];

  argList [1] = (char*)mName;
  argList [2] = dest;
  argList [3] = wadDirName;
  argList [4] = NULL;

  spawnv (0,"d2wspr.exe",argList);

}

//////////////////Создайтся новый уровень////////////////////////////////////

void    MakeLevel (const char* fileName,const char* levelName)
{
  LevelCompile  lComp;

  lComp.SetLevelName       (levelName);
  lComp.SetMap             (stageMapName);
  lComp.SetLightMap        (lightMapName);
  lComp.SetTextCache       (textCacheName);
  lComp.SetSpriteCache     (spriteCacheName);
  lComp.SetSpriteListCache (spriteListCacheName);
  lComp.SetFloorText       (floorText);
  lComp.SetSkyText         (skyText);
  lComp.SetRenderType      (renderType);
  lComp.SetSound           (soundPak);
  lComp.SetResourceTable   (resName);
  lComp.SetSubjectMap      (subMap);
  lComp.SetBkMusic         (bkMusic);
  lComp.SetScreenShot      (screenShot);
  lComp.SetKlip            (klipFile);
  lComp.SetRespawn         (resp,respCount);

  for (int i=0;i<GAME_CACHE_NUM;i++)
      lComp.SetCache (i,gameCacheTable[i]);

  for (i=0;i<bgiCount;i++)
    lComp.SetBGIFont     (i,bgiFont[i]);

  for (i=0;i<binCount;i++)
    lComp.SetBinFont     (i,binFont[i]);

  for (i=0;i<sprCount;i++)
    lComp.SetSpriteFont     (i,sprFont[i]);

  lComp.Save           (fileName);

  cout<<"Create game level "<<levelName<<endl;
}

//////////////////Функция процессинга всего файла////////////////////////////

int     Process      ()
{
  Param         param;

  char          curDestDir [128];
  char          curSrcDir  [128];

  for (char* curLine=NextLine();curLine;curLine=NextLine())
  {
        if (!ProcessLine (curLine,param))
           return line;

        switch (param.type)
        {
                case DEST_DIR:
                {
                        strcpy (destDirName,param.name);
                        break;
                }
                case SRC_DIR:
                {
                        strcpy (srcDirName,param.name);
                        break;
                }
                case LEVEL_HEADER:
                {
                        sprCount = bgiCount = binCount = 0;

                        strcpy (curDestDir,destDirName);
                        strcpy (curSrcDir,srcDirName);

                        if ( ProcessLevel () )
                           return line;

                        strcpy (destDirName,curDestDir);
                        strcpy (srcDirName,curSrcDir);

                        char dest [128];

                        strcpy (dest,destDirName);
                        strcat (dest,"/");
                        strcat (dest,param.name);
                        strcat (dest,".lev");

                        MakeLevel (dest,param.string);

                        break;
                }
                case CACHE_FILE:    return line;
                case MAP_HEADER:    return line;
                case SPRITE_HEADER: return line;
                case TEXT_LIST:     return line;
                case FACE_HEADER:   return line;
                case IMAGE_HEADER:  return line;
                case END:           return line;
                case COMMENT:       continue;
                default:            return line;
        }
  }

  return NO_ERROR;
}

int     ProcessLevel ()
{
  Param         param;
  char          destFileName [128];
  char*         spriteCacheList      [MAX_SPRITES];
  int           spriteCacheCount     = 0;
  char*         textCacheList        [MAX_TEXT];
  int           textCacheCount       = 0;
  char*         spriteListCacheList  [MAX_SPRITELIST];
  int           spriteListCacheCount = 0;

  char          curDestDir [128];
  char          curSrcDir  [128];

  renderType                 = 0;
  GameSprite3D* s3D          = NULL;

  res.Clear ();

  for (int i=0;i<GAME_CACHE_NUM;i++)
      gameCacheTable[i][0] = 0;

        //Разнести спрайты

  for (char* curLine=NextLine();curLine;curLine=NextLine())
  {
        if (!ProcessLine (curLine,param))
           return line;

        switch (param.type)
        {
                case WAD_DIR:
                {
                        strcpy (wadDirName,param.name);
                        break;
                }
                case DEST_DIR:
                {
                        strcpy (destDirName,param.name);
                        break;
                }
                case SRC_DIR:
                {
                        strcpy (srcDirName,param.name);
                        break;
                }
                case MUSIC:
                {
                        bkMusic = res.GetIndex(param.name);
                        break;
                }
                case SOUND_PAK_HEADER:
                {
                        strcpy (soundPak,destDirName);
                        strcat (soundPak,"/");
                        strcat (soundPak,param.name);
                        strcat (soundPak,".pak");

                        if (!ProcessSoundPak(soundPak,&res))
                           return line;

                        break;
                }
                case FLOOR:
                {
                        floorText = param.index<0 ? 0 :param.index;

                        break;
                }
                case SKY:
                {
                        skyText = param.index<0 ? 0 : param.index;

                        break;
                }
                case GRAPH_BPP:
                {
                        grBPP   = param.index<1 ? 1 : param.index;
                        break;
                }
                case CACHE_FILE:
                {
                        if (strlen (param.name) > 6)
                        {
                           dout<<"Name of cache must be lower of 6"<<endl;
                           return line;
                        }

                        strcpy (gameCacheName,destDirName);
                        strcat (gameCacheName,"/");
                        strcat (gameCacheName,param.name);

                        strcpy (textCacheName,gameCacheName);
                        strcat (textCacheName,".wtf");

                        strcpy (spriteCacheName,gameCacheName);
                        strcat (spriteCacheName,".wsc");

                        strcpy (spriteListCacheName,gameCacheName);
                        strcat (spriteListCacheName,".wsl");

                        strcpy (resName,gameCacheName);
                        strcat (resName,".res");

                        break;
                }
                case MAP_HEADER:
                {
                        strcpy (srcMapName,srcDirName);
                        strcat (srcMapName,"/");
                        strcat (srcMapName,param.name);
                        strcat (srcMapName,".map");

                        strcpy (stageMapName,destDirName);
                        strcat (stageMapName,"/");
                        strcat (stageMapName,param.name);
                        strcat (stageMapName,".map");

                        strcpy (lightMapName,destDirName);
                        strcat (lightMapName,"/");
                        strcat (lightMapName,param.name);
                        strcat (lightMapName,".lmf");

                        break;
                }
                case SPRITELIST_HEADER:
                {
                        strcpy (curDestDir,destDirName);
                        strcpy (curSrcDir,srcDirName);

                        strcpy (destFileName,destDirName);
                        strcat (destFileName,"/");
                        strcat (destFileName,param.name);
                        strcat (destFileName,".slc");
                        strcpy (param.name,destFileName);

                        ProcessSprListHeader (param);

                        strcpy (destDirName,curDestDir);
                        strcpy (srcDirName,curSrcDir);

                        break;
                }
                case SPRITE_HEADER:
                {
                        strcpy (curDestDir,destDirName);
                        strcpy (curSrcDir,srcDirName);

                        strcpy (destFileName,param.name);
                        strcat (destFileName,".wsf");

                        param.index = spriteCacheCount;

                        if ((s3D = ProcessSpriteHeader(param))==NULL)
                           return line;
                        else
                        {
                          SaveSprite (destFileName,s3D);

                          delete s3D;
                        }

                        strcpy (destDirName,curDestDir);
                        strcpy (srcDirName,curSrcDir);

                        break;
                }
                case DOOM2WSF:
                {
                        MakeDOOM2WSF (param.name,param.string);

                        break;
                }
                case SPRITE_LIST:
                {
                      if ((spriteCacheCount=ProcessSpriteList(spriteCacheList))==-1)
                        return line;

                        break;
                }
                case SPRITELIST_LIST:
                {
                      if ((spriteListCacheCount=
                           ProcessSpriteListList(spriteListCacheList)
                           )==-1)
                        return line;

                        break;
                }
                case TEXT_LIST:
                {
                  if ((textCacheCount=ProcessText(textCacheList))==-1)
                     return line;

                  break;
                }
                case RENDER_TYPE:
                {
                  if (!ProcessRender (renderType))
                     return line;

                  break;
                }
                case RESOURCE_LIST:
                {
                  if (!ProcessResourceList ())
                     return line;

                  break;
                }
                case END:
                {
                   MakeSpriteCache (spriteCacheName,spriteCacheList,spriteCacheCount);
                   MakeTextCache   (textCacheName,textCacheList,textCacheCount);
                   MakeSpriteListCache (spriteListCacheName,
                                        spriteListCacheList,
                                        spriteListCacheCount);
                   MakeMap         (srcMapName);
                   MakeRes         (resName);

                   for (int i=0;i<spriteCacheCount;i++)
                       delete spriteCacheList[i];

                   for (i=0;i<textCacheCount;i++)
                       delete textCacheList[i];

                   for (i=0;i<spriteListCacheCount;i++)
                       delete spriteListCacheList[i];

                   return NO_ERROR;
                }
                case BGI_FONT:
                {
                  if ((bgiCount = MakeBGIFont()) == 0)
                     return line;
                  break;
                }
                case BIN_FONT:
                {
                  if ((binCount = MakeBinFont()) == 0)
                     return line;
                  break;
                }
                case SPR_FONT:
                {
                  if ((sprCount = MakeSpriteFont()) == 0)
                     return line;
                  break;
                }
                case SCREEN_SHOT:
                {
//                  strcpy (screenShot,destDirName);
//                  strcat (screenShot,"/");
                  strcpy (screenShot,param.name);

                  break;
                }
                case KLIP:
                {
//                  strcpy (screenShot,destDirName);
//                  strcat (screenShot,"/");
                  strcpy (klipFile,param.name);

                  break;
                }
                case GAME_USE_LIST:
                {
                  MakeGameCache (param.index,param.string);

                  break;
                }
                case G_WEAPON:
                {
                  WeaponRes* weap = ProcessWeapon ();

                  if (weap == NULL)
                     return line;

                  char dest [128];

                  strcpy (dest,destDirName);
                  strcat (dest,"/");
                  strcat (dest,param.name);
                  strcat (dest,".wpn");

                  weap->Save (dest);

                  delete weap;

                  break;
                }
                case G_BOX:
                {
                  SubjectRes* box = ProcessSubject ();

                  if (box == NULL)
                     return line;

                  char dest [128];

                  strcpy (dest,destDirName);
                  strcat (dest,"/");
                  strcat (dest,param.name);
                  strcat (dest,".box");

                  box->Save (dest);

                  delete box;

                  break;
                }
                case G_DOOR:     ProcessDoor     (param); break;
                case G_PLAYER:   ProcessPlayer   (param); break;
                case G_TELEPORT: ProcessTeleport (param,TELEPORT); break;
                case G_PORTAL:   ProcessTeleport (param,PORTAL); break;
                case G_SPRITE_EFFECT:
                                 ProcessSpriteEffect(param); break;
                case G_LIGHT_EFFECT:
                                 ProcessLightEffect(param); break;
                case RESPAWN:    respCount = ProcessRespawn (resp); break;
                case G_SUBJECT_LIST:
                {
                  EnvironmentMap* map = ProcessSubjectList (param);

                  if (map == NULL)
                     return line;

                  char dest [128];

                  strcpy (dest,destDirName);
                  strcat (dest,"/");
                  strcat (dest,param.name);
                  strcat (dest,".sbj");

                  strcpy (subMap,dest);

                  map->Save (dest);

                  delete map;

                  break;
                }
                case LEVEL_HEADER:  return line;
                case FACE_HEADER:   return line;
                case IMAGE_HEADER:  return line;
                case COMMENT:       continue;
                default:            return line;
        }
  }


  return line;
}

////////////////////////////Компиляция спрайта////////////////////////////////

void            ClearSHeaderList (Sprite3D** list,int count)
{
  for (int i=0;i<count;i++)
      delete list[i];
}

GameSprite3D*   ProcessSpriteHeader (const Param& pIn)
{
  Sprite3D**      list = new Sprite3D* [pIn.num];
  Param           param;
  int             count = 0;

  char          curDestDir [128];
  char          curSrcDir  [128];

  dout<<"Create sprite header: #"<<pIn.index<<" "<<
        pIn.name<<" num = "<<pIn.num<<endl;

  for (char* curLine=NextLine();curLine;curLine=NextLine())
  {
        if (!ProcessLine (curLine,param))
           return NULL;

        switch (param.type)
        {
                case FACE_HEADER:
                {
                        strcpy (curDestDir,destDirName);
                        strcpy (curSrcDir,srcDirName);

                        if ((list [param.index] = ProcessSpriteFace(param)) == NULL)
                        {
                           delete list;
                           return NULL;
                        }

                        strcpy (destDirName,curDestDir);
                        strcpy (srcDirName,curSrcDir);

                        count++;

                        break;
                }
                case CACHE_FILE:
                case SPRITE_HEADER: return NULL;
                case IMAGE_HEADER:  return NULL;
                case DEST_DIR:
                {
                        strcpy (destDirName,param.name);
                        break;
                }
                case SRC_DIR:
                {
                        strcpy (srcDirName,param.name);
                        break;
                }
                case COMMENT:       continue;
                case END:
                {
                        if (count!=pIn.num || count == 0)
                        {
                          dout<<"Error in num sprite faces"<<endl;
                          delete list;
                          return NULL;
                        }

                        GameSprite3D*   s3D = makeGameSprite3D(list,pIn.num);

                        ClearSHeaderList (list,count);
                        delete list;

                        return s3D;
                }

        }

  }

  delete list;
  return NULL;
}

void            ClearFHeaderList (SimpleSprite** list,int count)
{
  for (int i=0;i<count;i++)
      delete list[i];
}

Sprite3D*       ProcessSpriteFace(const Param& pIn)
{
  SimpleSprite**  list = new SimpleSprite* [pIn.num];

  Param           param;
  int             count = 0;

  dout<<"Create sprite face: #"<<pIn.index<<" num_images = "<<pIn.num<<endl;

  for (char* curLine=NextLine();curLine;curLine=NextLine())
  {
        if (!ProcessLine (curLine,param))
           return NULL;

        switch (param.type)
        {
                case IMAGE_HEADER:
                {
                        char dest [512];

                        strcpy(dest,srcDirName);
                        strcat(dest,"/");
                        strcat(dest,param.name);
                        strcpy(param.name,dest);

                        if ((list [param.index] = ProcessSpriteImage(param)) == NULL)
                        {
                           delete list;
                           return NULL;
                        }
                        count++;

                        break;
                }
                case DEST_DIR:
                {
                        strcpy (destDirName,param.name);
                        break;
                }
                case SRC_DIR:
                {
                        strcpy (srcDirName,param.name);
                        break;
                }
                case CACHE_FILE:
                case FACE_HEADER:   return NULL;
                case SPRITE_HEADER: return NULL;
                case COMMENT:       continue;
                case END:
                {
                        if (count!=pIn.num || count==0)
                        {
                          dout<<"Error in num sprite face images"<<endl;
                          delete list;
                          return NULL;
                        }

                        Sprite3D*   s3D = makeSprite3D(list,count);

                        ClearFHeaderList (list,count);
                        delete list;

                        return s3D;
                }

        }

  }

  delete list;
  return NULL;
}

SimpleSprite*   ProcessSpriteImage (const Param& pIn)
{
  cPicture      picture;

  char          name [128];

  strcpy (name,pIn.name);
//  strcat (name,".bmp");

  char*   argList [4];

  argList [1] = name;
  argList [2] = name;
  argList [3] = NULL;

  if (spawnv (0,nPal,argList))
    return NULL;

  dout<<"Create sprite plane: #"<<pIn.index<<" "<<name<<endl;

  if (!picture.load(name))
  {
    dout<<"Error open simple image"<<endl;
    return NULL;
  }

  int  bpp = picture.bpp() < 8 ? picture.bpp() : picture.bpp()/8;

  return makeSimpleSprite((tImage*)picture.GetImage(),pIn.z_color,bpp);
}

//////////////////Компиляция списка спрайтов SpriteList//////////////////////

void           ProcessSprListHeader (const Param& pIn)
{
  tSprite*      spriteArray [MAX_SPRITELIST];
  char          names       [MAX_SPRITELIST][128];

  int   pos       = 0;
  int   index     = 0;

  char  name [128];
  char* curLine;

  for (char* procStr = NextWord(curLine=NextLine(),pos);memcmp(procStr,"END",3);pos=0,
             procStr = NextWord(curLine=NextLine(),pos))
  {
    strcpy (name,srcDirName);
    strcat (name,"/");
    strcat (name,procStr);

    if (spriteArray [index] = ProcessSprListImage (name,atoi(NextWord (curLine,pos))))
    {
       strcpy (names[index],name);
       index++;
    }
  }

  tSpriteList* list = tSpriteList::create (index,8,spriteArray);

  dout<<"Save sprite list ("<<index<<"): "<<pIn.name<<endl;

  list->save (pIn.name);

  free (list);

  for (int i=0;i<index;free (spriteArray[i++]));
}

tSprite*        makeSprite (const tImage* image,int z_color)
{
  int         len = image->width*image->height;
  tSprite* sprite = (tSprite*)malloc(len+sizeof(tSprite)+image->height*5+16);

  sprite->width  = image->width;
  sprite->height = image->height;
  sprite->x      = 0;
  sprite->y      = 0;

  for(int j=0;j<sprite->height;j++)
  {
     uint16* data = (uint16*)(sprite->data+j*(sprite->width+5));
     data[0]      = image->width+5;
     data[1]      = image->width|0x8000;

     uchar* dc    = (uchar*)data;
     dc[4]        = z_color;

     memcpy(dc+5,image->data+image->width*j,image->width);
  }

  return sprite;
}

tSprite*        ProcessSprListImage (char* _name,int z_color)
{
  cPicture      picture;

  char*   argList [4];

  argList [1] = _name;
  argList [2] = _name;
  argList [3] = NULL;

  if (spawnv (0,nPal,argList))
    return NULL;

  dout<<"Create sprite list image "<<_name<<endl;

  if (!picture.load(_name))
  {
    dout<<"Error open image"<<endl;
    return NULL;
  }

  return makeSprite (picture.GetImage(),z_color);
}

///////////////////////////Процедуры для компиляции строковых лексем//////////

char*   NextLine ()
{
  BinFile* fPtr = fileTable [current()];

  if (fPtr->eof() && current()==0)
     return NULL;
  else if (fPtr->eof())
  {
    CloseFile();
    return NextLine ();
  }

  int i    = 0;

  for (char simb = fPtr->getc();simb!=10 && !fPtr->eof();temp1[i++]=simb,simb=fPtr->getc());

  temp1 [i] = 0; //end of line

  if (strlen(temp1)<=1 && (temp1[0] == ' ' || temp1[0] == 10 || temp1[0] == 13))
     return NextLine();

  int pos = 0;

  if (!strcmp(NextWord(temp1,pos),"INCLUDE"))
  {
    char* name = NextWord (temp1,pos);

    dout<<"Include file '"<<name<<"' from '"<<fileName[current()]
        <<"' line "<<lines[current()]<<endl;

    lines [current()]++;
    OpenFile (name);

    return NextLine();
  }

  line=lines [current()]++;

  return temp1;
}

char*   NextWord (char* str,int& pos)
{
  for (char simb = str[pos++];!isalnum(simb) && simb != '_' && simb != '"' &&
       simb != '-';
       simb=str[pos++]);

  int i         = 0;
  int len       = strlen (str);
  temp2 [0]     = 0;

  if (simb!=10 && simb != '"') //2???
    for (;pos<=len && (isalnum(simb) || simb=='_' || simb=='.' || simb =='-');
          temp2[i++]=simb,simb=str[pos++]);

  if (simb == '"' && pos<=len)
     for (simb=str[pos++];
          pos<=len && (isalnum (simb) || simb =='_' || simb=='.'
          || simb=='/' || simb=='\\' || simb ==':' || simb == '-');
          temp2[i++]=simb,simb=str[pos++]);

  temp2 [i] = 0; //end of line

  return temp2;
}

char*   TranslateFileName (const char* src,char* dest,char* dirName)
{
  char* ret = dest;

  for (;*src && toupper(*src) == toupper(*dirName);dirName++,src++);
  for (;*src && !isalnum(*src) && *src!='_';src++);

  for (;*src;src++,dest++)
    if (*src == '/') *dest = '_';
    else     *dest = *src;

  *dest = 0;

  if (strchr (ret,'.') != ret)
     *strchr (ret,'.') = 0;

  return ret;
}

BOOL            ProcessLine     (char* str,Param& param)
{
        const int         typeCount = 39;
        const char*       types [typeCount] = {"SPRITE","FACE","IMAGE",
              "DEST","SRC","END","CACHE","TEXTURES","MAP","LEVEL","FLOOR",
              "SKY","BGIFONT","BINFONT","SPRITEFONT","REM","SOUND","SPRITES",
              "RESOURCE","SPRITE_LISTS","SPRITE_LIST","WEAPON",
              "USE_LIST","DOOM_SPRITE","WAD_DIR","SUBJECT",
              "SUBJECT_MAP","DOOR","PLAYER","RENDER_TYPE","TELEPORT","PORTAL",
              "MUSIC","SPRITE_ANIMATION","LIGHT_EFFECT","SCREEN_SHOT","KLIP",
              "GRAPH_BPP","RESPAWN"};
        const LineType    realTypes [typeCount] = {SPRITE_HEADER,FACE_HEADER,
              IMAGE_HEADER,DEST_DIR,SRC_DIR,END,CACHE_FILE,TEXT_LIST,
              MAP_HEADER,LEVEL_HEADER,FLOOR,SKY,BGI_FONT,BIN_FONT,SPR_FONT,
              COMMENT,SOUND_PAK_HEADER,SPRITE_LIST,RESOURCE_LIST,
              SPRITELIST_LIST,SPRITELIST_HEADER,G_WEAPON,GAME_USE_LIST,
              DOOM2WSF,WAD_DIR,G_BOX,G_SUBJECT_LIST,G_DOOR,G_PLAYER,RENDER_TYPE,
              G_TELEPORT,G_PORTAL,MUSIC,G_SPRITE_EFFECT,G_LIGHT_EFFECT,SCREEN_SHOT,
              KLIP,GRAPH_BPP,RESPAWN};

                //Если строка пустая
        if (strlen(str) <= 3)
        {
          param.type = COMMENT;
          return TRUE;
        }

        char            list [100][128];
        int             count = 0;
        int             tLen  = strlen(str);

        for (int pos = 0;pos<tLen;)
            strcpy (list[count++],NextWord(str,pos));

        if (count == 0)
           return FALSE;

                //Find type
        for (int j=0;j<typeCount;j++)
            if (!strcmp(types[j],list[0]))
               break;

        if (j != typeCount)
           param.type = realTypes [j];
        else
           return FALSE;

        switch (param.type)
        {
                case SPRITE_HEADER:
                {
                  if (count<3)
                     return FALSE;

                  strcpy(param.name,list[1]);
                  param.num = atoi (list[2]);

                  break;
                }
                case FACE_HEADER:
                {
                  if (count<3)
                     return FALSE;

                  param.index = atoi(list[1]);
                  param.num   = atoi(list[2]);

                  break;
                }
                case IMAGE_HEADER:
                {
                  if (count<3)
                     return FALSE;

                  strcpy(param.name,list[1]);
                  param.index   = atoi(list[2]);
                  param.z_color = atoi(list[3]);

                  break;
                }
                case MAP_HEADER:
                {
                  if (count<2)
                     return FALSE;

                  strcpy(param.name,list[1]);

                  break;
                }
                case SPRITELIST_HEADER:
                {
                  if (count<2)
                     return FALSE;

                  strcpy (param.name,list[1]);

                  break;
                }
                case GRAPH_BPP:
                {
                  if (count<2)
                     return FALSE;

                  param.index = atoi(list[1]);

                  break;
                }
                case FLOOR:
                case SKY:
                {
                  if (count<2)
                     return FALSE;

                  param.index = res.GetIndex (list[1]);

                  break;
                }
                case END: return TRUE;
                case SRC_DIR:
                case DEST_DIR:
                {
                  if (count<2)
                     return FALSE;

                  strcpy(param.name,list[1]);

                  break;
                }
                case CACHE_FILE:
                {
                  if (count<2)
                     return FALSE;

                  strcpy (param.name,list[1]);

                  break;
                }
                case LEVEL_HEADER:
                {
                  if (count<3)
                     return FALSE;

                  strcpy (param.name,list[1]);
                  strcpy (param.string,list[2]);

                  break;
                }
                case SOUND_PAK_HEADER:
                {
                  if (count<2)
                     return FALSE;

                  strcpy (param.name,list[1]);

                  break;
                }
                case MUSIC:
                {
                  if (count<2)
                     return FALSE;

                  strcpy (param.name,list[1]);

                  break;
                }
                case RESOURCE_LIST:
                {
                  if (count>2)
                    strcpy (param.name,list[1]);

                  break;
                }
                case WAD_DIR:
                {
                  if (count<2)
                     return FALSE;

                  strcpy (param.name,list[1]);

                  break;
                }
                case DOOM2WSF:
                {
                  if (count<3)
                     return FALSE;

                  strcpy (param.name,list[1]);
                  strcpy (param.string,list[2]);

                  break;
                }
                case GAME_USE_LIST:
                {
                  if (count<2)
                     return FALSE;

                  param.index = res.GetIndex (list[1]);

                  if (count == 3)
                  {
                     strcpy (param.string,list[2]);
                     strcat (param.string,"_");
                  }
                  else
                  {
                     char name [128];

                     strcpy (param.string,"UL");
                     strcat (param.string,itoa(param.index,name,16));
                     strcat (param.string,"_");
                  }

                  break;
                }
                case KLIP:
                case SCREEN_SHOT:
                {
                  if (count<2)
                     return FALSE;

                  strcpy (param.name,list[1]);

                  break;
                }
                case G_WEAPON:
                {
                  if (count<2)
                     return FALSE;

                  strcpy (param.name,list[1]);

                  break;
                }
                case G_BOX:
                {
                  if (count<2)
                     return FALSE;

                  strcpy (param.name,list[1]);

                  break;
                }
                case G_PLAYER:
                {
                  if (count<2)
                     return FALSE;

                  strcpy (param.name,list[1]);

                  break;
                }
                case G_LIGHT_EFFECT:
                case G_SPRITE_EFFECT:
                {
                  if (count<2)
                     return FALSE;

                  strcpy (param.name,list[1]);

                  break;
                }
                case G_PORTAL:
                case G_TELEPORT:
                {
                  if (count<2)
                     return FALSE;

                  strcpy (param.name,list[1]);

                  break;
                }
                case G_SUBJECT_LIST:
                {
                  if (count<3)
                     return FALSE;

                  strcpy (param.name,list[1]);
                  param.num = count == 2 ? atoi (list[2]) : 4096;

                  break;
                }
                case G_DOOR:
                {
                  if (count<2)
                     return FALSE;

                  strcpy (param.name,list[1]);

                  break;
                }
                case RESPAWN:
                case RENDER_TYPE:
                case BIN_FONT:
                case SPR_FONT:
                case BGI_FONT:  return TRUE;
                case TEXT_LIST: return TRUE;
                case SPRITE_LIST: return TRUE;
                case SPRITELIST_LIST: return TRUE;
        }

  return TRUE;
}

////////////////////////////Компиляция текстур////////////////////////////////

int      ProcessText     (char* names[])
{
  int   pos       = 0;
  int   index     = 0;

  char  name    [128];
  char  name1   [128];
  char  tempX   [128];
  char  procStr [128];

  char* curLine;
  char* argList [4];

  argList [3] = NULL;   //end of list

  for (strcpy(procStr,NextWord(curLine=NextLine(),pos));memcmp(procStr,"END",3);pos=0,
       strcpy(procStr,NextWord(curLine=NextLine(),pos)))
  {
    strcpy (name,destDirName);
    strcat (name,"/");
    strcat (name,TranslateFileName(procStr,tempX,destDirName));
    strcat (name,".mmt");

    if (strcmp(NextWord (curLine,pos),"NO_COMPILE"))
    {
      strcpy (name1,srcDirName);
      strcat (name1,"/");
      strcat (name1,procStr);

      if (grBPP == 1)
      {
        argList [1] = name1;
        argList [2] = name1;

        if (spawnv (0,nPal,argList))
          return -1;

        argList [1] = name1;
        argList [2] = name;

        if (spawnv (0,mmtMake8bit,argList))
          return -1;
      }
      else if (grBPP == 2)
      {
        argList [1] = name1;
        argList [2] = name;

        if (spawnv (0,mmtMake16bit,argList))
          return -1;
      }
    }

    if (strcmp(NextWord (curLine,pos),"ONLY_COMPILE"))
    {
      AddResFileEntry (name,"TEXT_",index);
      names [index] = new char [strlen(name)+1];
      strcpy (names[index++],name);
    }

  }

  return index;
}

/////////////////////Компиляция списка спрайтов/////////////////////////////

int      ProcessSpriteList     (char* names[])
{
  int   pos       = 0;
  int   index     = 0;

  char  name [128];

  for (char* procStr = NextWord(NextLine(),pos);memcmp(procStr,"END",3);pos=0,
             procStr = NextWord(NextLine(),pos),index++)
  {
    strcpy (name,destDirName);
    strcat (name,"/");
    strcat (name,procStr);
//    strcat (name,".wsf");

    names [index] = new char [strlen(name)+1];

    strcpy (names[index],name);

    AddResFileEntry (names[index],"SPR_",index);
  }

  return index;
}

////////////////////Компиляция SpriteList////////////////////////////////////

int      ProcessSpriteListList     (char* names[])
{
  int   pos       = 0;
  int   index     = 0;

  char  name [128];

  for (char* procStr = NextWord(NextLine(),pos);memcmp(procStr,"END",3);pos=0,
             procStr = NextWord(NextLine(),pos),index++)
  {
    strcpy (name,destDirName);
    strcat (name,"/");
    strcat (name,procStr);

    names [index] = new char [strlen(name)+1];

    strcpy (names[index],name);

    AddResFileEntry (names[index],"SL_",index);
  }

  return index;
}

///////////////////////////Компиляция имён ресурсов//////////////////////////

BOOL    ProcessResourceList ()
{
  char resName  [128];
  char resIndexStr [128];
  int  resIndex;

  int  pos = 0;

  for (char* nextLine = NextLine();memcmp(NextWord(nextLine,pos),"END",3);pos = 0,
             nextLine = NextLine())
  {
    pos = 0;

    strcpy (resName,NextWord(nextLine,pos));
    strcpy (resIndexStr,NextWord(nextLine,pos));

    if (!strcmp (resName,"END") || !strcmp (resIndexStr,"END"))
       return FALSE;

    resIndex = atoi (resIndexStr);

    if (resIndex == 0 && resIndexStr [0] != '0')
      res.SetIndex (resName,res.GetIndex (resIndexStr));
    else
      res.SetIndex (resName,resIndex);

    dout<<"\t"<<resName<<" = "<<res.GetIndex(resName)<<endl;
  }

  return TRUE;
}

///////////////////////////Компиляция шрифтов////////////////////////////////

int     MakeBGIFont ()
{
  int index = 0;
  int   pos       = 0;

  for (char* procStr = NextWord(NextLine(),pos);memcmp(procStr,"END",3);pos=0,
             procStr = NextWord(NextLine(),pos),index++)
  {
    strcpy (bgiFont[index],destDirName);
    strcat (bgiFont[index],"/");
    strcat (bgiFont[index],procStr);
//    strcat (bgiFont[index],".chr");

    AddResFileEntry (bgiFont[index],"BGIFNT_",index);
  }

  return index;
}

int     MakeBinFont ()
{
  int index = 0;
  int   pos       = 0;

  for (char* procStr = NextWord(NextLine(),pos);memcmp(procStr,"END",3);pos=0,
             procStr = NextWord(NextLine(),pos),index++)
  {
    strcpy (binFont[index],destDirName);
    strcat (binFont[index],"/");
    strcat (binFont[index],procStr);
//    strcat (binFont[index],".fnt");

    AddResFileEntry (binFont[index],"BFNT_",index);
  }

  return index;
}

int     MakeSpriteFont ()
{
  int index = 0;
  int   pos       = 0;

  for (char* procStr = NextWord(NextLine(),pos);memcmp(procStr,"END",3);pos=0,
             procStr = NextWord(NextLine(),pos),index++)
  {
    strcpy (sprFont[index],destDirName);
    strcat (sprFont[index],"/");
    strcat (sprFont[index],procStr);
    strcat (sprFont[index],".slc");

    AddResFileEntry (sprFont[index],"SPRFNT_",index);
  }

  return index;
}

////////////////////Конфигурация Render//////////////////////////////////////

BOOL            ProcessRender        (int& type)
{
  enum {
       STAGE_R,
       FLOOR_R,
       SKY_R
  };

  const         int count = 3;

  char*         members [count] = {"STAGE_RENDER","FLOOR_RENDER","SKY_RENDER"};

  int           realMembers [count] = {STAGE_R,FLOOR_R,SKY_R};

  int                   pos       = 0;
  type                            = 0;
  char*                 curLine;

  for (char* procStr = NextWord(curLine=NextLine(),pos);
             memcmp(procStr,"END",3);pos=0,
             procStr = NextWord(curLine=NextLine(),pos))
  {
                //Find type ????
        for (int j=0;j<count;j++)
            if (!strcmp(procStr,members[j]))
               break;

        if (j==count) return FALSE;

        char*  str    = NextWord (curLine,pos);

        switch (realMembers[j])
        {
          case STAGE_R: type |= res.GetIndex(str);     break;
          case FLOOR_R: type |= res.GetIndex(str)<<8;  break;
          case SKY_R:   type |= res.GetIndex(str)<<16; break;
          default:    return FALSE;
        }
  }

  return TRUE;
}

///////////////////Слздание оружия///////////////////////////////////////////

WeaponRes::Shot            ProcessShotHeader    ()
{
  enum {
       FLY = 0,
       BAH,
       SPEED,
       FORCE,
       RADIUS,
       MAX_DIST,
       SPRITES,
       BAH_ANIM,
       FLY_ANIM,
       BACK
  };

  const         int count = 10;

  char*         members [count] = {"FLY_SND","BAH_SND","SPEED","FORCE",
                "RADIUS","MAX_DIST","SPRITES","BAH_ANIM","BACK_FORCE","FLY_ANIM"};

  int           realMembers [count] = { FLY,BAH,SPEED,FORCE,RADIUS,MAX_DIST,
                SPRITES,BAH_ANIM, BACK,FLY_ANIM};


  WeaponRes::Shot       shot;
  int                   pos       = 0;
  char*                 curLine;

  for (char* procStr = NextWord(curLine=NextLine(),pos);
             memcmp(procStr,"END",3);pos=0,
             procStr = NextWord(curLine=NextLine(),pos))
  {
                //Find type
        for (int j=0;j<count;j++)
            if (!strcmp(procStr,members[j]))
               break;

        int    realPos  = pos;
        char*  str      = NextWord (curLine,pos);
        int    value    = atoi (str);
        float  fvalue   = atof (str);

        switch (realMembers[j])
        {
          case FLY:        shot.fly_sound = res.GetIndex(str);    break;
          case BAH:        shot.bah_sound = res.GetIndex(str);    break;
          case SPEED:      shot.speed     = float2fixed(fvalue);  break;
          case FORCE:      shot.force     = value;                break;
          case BACK:       shot.back_force= res.GetIndex (str);   break;
          case RADIUS:     shot.radius    = value;                break;
          case MAX_DIST:   shot.max_dist  = int2fixed(value);     break;
          case SPRITES:    shot.sprites   = res.GetIndex(str);    break;
          case BAH_ANIM:   ProcessStateList (curLine,shot.bah_anim,realPos);  break;
          case FLY_ANIM:   ProcessStateList (curLine,shot.fly_anim,realPos);  break;
        }
  }

  return shot;
}

WeaponRes*      ProcessWeapon        ()
{
  enum {
       SHOT = 0,
       GBOX,
       SPEED,
       P_SOUND,
       P_NUM,
       P_CHANGE_TIME,
       A_SOUND,
       A_NUM,
       A_CHANGE_TIME,
       SPRITES,
       FIRST_COUNT,
       MAX_COUNT
  };

  const         int count = 12;

  char*         members [count] = {"SHOT","BOX","SPEED","P_SOUND","P_ANIM",
                "P_CHANGE_TIME","A_SOUND","A_ANIM","A_CHANGE_TIME","SPRITES",
                "FIRST_COUNT","MAX_COUNT"};

  int           realMembers [count] = { SHOT,GBOX,SPEED,P_SOUND,P_NUM,
                P_CHANGE_TIME,A_SOUND,A_NUM,A_CHANGE_TIME,SPRITES,
                FIRST_COUNT,MAX_COUNT };


  WeaponRes*      weapon = new WeaponRes;
  int             pos       = 0;
  char*           curLine;

  for (char* procStr = NextWord(curLine=NextLine(),pos);
             memcmp(procStr,"END",3);pos=0,
             procStr = NextWord(curLine=NextLine(),pos))
  {
                //Find type
        for (int j=0;j<count;j++)
            if (!strcmp(procStr,members[j]))
               break;

        if (j != count)
        {
           int    realPos = pos;
           char*  str     = NextWord (curLine,pos);
           int    value   = atoi (str);

           switch (realMembers[j])
           {
             case SHOT:
                  weapon->shot = ProcessShotHeader ();
                  break;
             case GBOX:       weapon->gameBox = (BoxTypes)res.GetIndex(str);    break;
             case SPEED:      weapon->speed   = value;                break;
             case P_SOUND:    weapon->p_sound = res.GetIndex(str);    break;
             case P_NUM:      ProcessStateList (curLine,weapon->p_anim,realPos); break;
             case P_CHANGE_TIME:
                              weapon->p_changeTime = value;           break;
             case A_SOUND:    weapon->a_sound = res.GetIndex(str);    break;
             case A_NUM:      ProcessStateList (curLine,weapon->a_anim,realPos); break;
             case A_CHANGE_TIME:
                              weapon->a_changeTime = value;           break;
             case SPRITES:    weapon->sprites      = res.GetIndex(str);break;
             case FIRST_COUNT:weapon->count        = value;           break;
             case MAX_COUNT:  weapon->max          = value;           break;
             default:          return NULL;
           }
        }
        else
             return NULL;
  }

  return weapon;
}

SubjectRes*     ProcessSubject       ()
{
  enum {
       FIRST_COUNT = 1,
       GIVE_SND_ID,
       SPRITE_ID,
       GAME_CACHE,
       GAME_ID,
       BOX_CLASS,
       RESPAWN_TIME,
       RESPAWN_SND
  };

  const         int count = 8;

  char*         members [count] = {"FIRST_COUNT",
                "GIVE_SOUND","SPRITE","GAME_CACHE",
                "ID","GAME_BOX","RESPAWN_TIME","RESPAWN_SOUND"};

  int           realMembers [count] = { FIRST_COUNT,
                GIVE_SND_ID,SPRITE_ID,GAME_CACHE,GAME_ID,BOX_CLASS,
                RESPAWN_TIME,RESPAWN_SND};

  SubjectRes*    box = new SubjectRes;
  int            pos       = 0;
  char*          curLine;

  for (char* procStr = NextWord(curLine=NextLine(),pos);
             memcmp(procStr,"END",3);pos=0,
             procStr = NextWord(curLine=NextLine(),pos))
  {
                //Find type
        for (int j=0;j<count;j++)
            if (!strcmp(procStr,members[j]))
               break;

        if (j != count)
        {
           char*        str     = NextWord (curLine,pos);
           int          value   = atoi (str);

           switch (realMembers[j])
           {
             case FIRST_COUNT:  box->count     = value;      break;
             case GIVE_SND_ID:  box->giveSound = res.GetIndex(str);    break;
             case SPRITE_ID:    box->mapSprite = res.GetIndex (str);  break;
             case GAME_CACHE:   box->gameCache = res.GetIndex (str);  break;
             case GAME_ID:      box->id        = res.GetIndex (str);  break;
             case BOX_CLASS:    box->boxClass  = res.GetIndex (str);  break;
             case RESPAWN_TIME: box->respawnTime = value; break;
             case RESPAWN_SND:  box->respawnSound=res.GetIndex (str);  break;
             default:           return NULL;
           }
        }
        else
             return NULL;
  }

  return box;
}

void         ProcessDoor       (Param& param)
{
  enum {
       TEXT_ID,
       OPEN_SOUND,
       CLOSE_SOUND,
       DR_STEP,
       DR_WAIT_TIME,
       DR_CROSS_STATE
  };

  const         int count = 6;

  char*         members [count] = {"TEXT_ID",
                "OPEN_SOUND","CLOSE_SOUND","STEP","WAIT_TIME","CROSS"};

  int           realMembers [count] = {TEXT_ID,
                OPEN_SOUND,CLOSE_SOUND,DR_STEP,DR_WAIT_TIME,DR_CROSS_STATE};

  DoorRes*       door = new DoorRes;
  int            pos  = 0;
  char*          curLine;

  for (char* procStr = NextWord(curLine=NextLine(),pos);
             memcmp(procStr,"END",3);pos=0,
             procStr = NextWord(curLine=NextLine(),pos))
  {
                //Find type
        for (int j=0;j<count;j++)
            if (!strcmp(procStr,members[j]))
               break;

        if (j != count)
        {
           char*        str     = NextWord (curLine,pos);
           int          value   = atoi (str);

           switch (realMembers[j])
           {
             case OPEN_SOUND:  door->openSound = res.GetIndex(str);    break;
             case CLOSE_SOUND: door->closeSound = res.GetIndex(str);    break;
             case TEXT_ID:     door->textId  = res.GetIndex (str);  break;
             case DR_WAIT_TIME:door->doorWaitTime = value; break;
             case DR_STEP:     door->doorStep = value; break;
             case DR_CROSS_STATE:door->doorCrossState = value; break;
             default:          return;
           }
        }
        else
             return ;
  }

  char dest [128];

  strcpy (dest,destDirName);
  strcat (dest,"/");
  strcat (dest,param.name);
  strcat (dest,".gdr");

  door->Save (dest);

  delete door;
}

void         ProcessStateList    (char* line,char* stateList,int _pos)
{
  char* str  = line + _pos + 1;
  char* tmp  = str;
  int   pos  = 0;

  memset (stateList,-1,sizeof(char)*ANIM_NUM);

  for (int i=0;i<ANIM_NUM;i++)
  {
    str = NextWord (tmp,pos);

    if (!isdigit (str [0]) || pos > strlen(tmp))
    {
      stateList [i] = PlayerRes::END_OF_STATE;
      break;
    }

    stateList [i] = atoi(str);
  }
}

void         ProcessPlayer       (Param& param)
{
  enum {
       SPRITE_ID,
       DEATH_SND,
       CROSS_SND,
       ATACK_SND,
       LUCK_SND,
       HEY_SND,
       DEATH_SPR,
       CROSS_SPR,
       ATACK_SPR,
       NORM_SPR,
       WP_CACHE,
       WP_ID,
       IQ_ATACK,
       IQ_FAR_FOLLOW,
       IQ_NEAR_FOLLOW,
       IQ_VIEW_OUT,
       IQ_RANDOM_MOVE,
       SPEED
  };

  const         int count = 21;

  char*         members [count] = {"SPRITE",
                "DEATH_SOUND","CROSS_SOUND","ATACK_SOUND","LUCK_SOUND","HEY_SOUND",
                "DEATH_SPRITE","CROSS_SPRITE","ATACK_SPRITE","NORM_SPRITE",
                "WEAPON_CACHE","WEAPON_ID","ATACK","FAR_FOLLOW","NEAR_FOLLOW",
                "VIEW_OUT","RANDOM_MOVE","SPEED"};

  int           realMembers [count] = {SPRITE_ID,
                DEATH_SND,CROSS_SND,ATACK_SND,LUCK_SND,HEY_SND,DEATH_SPR,CROSS_SPR,
                ATACK_SPR,NORM_SPR,WP_CACHE,WP_ID,IQ_ATACK,IQ_FAR_FOLLOW,
                IQ_NEAR_FOLLOW,IQ_VIEW_OUT,IQ_RANDOM_MOVE,SPEED};

  WeaponPlayerRes*  play = new WeaponPlayerRes;
  int               pos  = 0, realPos;
  char*             curLine;

  memset (play->iq,-1,sizeof(uint16)*IQ_MAX_PROBS);

  for (char* procStr = NextWord(curLine=NextLine(),pos);
             memcmp(procStr,"END",3);pos=0,
             procStr = NextWord(curLine=NextLine(),pos))
  {
                //Find type
        for (int j=0;j<count;j++)
            if (!strcmp(procStr,members[j]))
               break;

        if (j != count)
        {
           realPos              = pos;
           char*        str     = NextWord (curLine,pos);
           int          value   = atoi (str);
           float        fvalue  = atof (str);

           switch (realMembers[j])
           {
             case SPEED:     play->speed       = float2fixed(fvalue); break;
             case SPRITE_ID: play->mapSprite   = res.GetIndex(str);  break;
             case DEATH_SND: play->sound [PlayerRes::DEATH]      = res.GetIndex(str);  break;
             case CROSS_SND: play->sound [PlayerRes::CROSS_SHOT] = res.GetIndex(str);  break;
             case ATACK_SND: play->sound [PlayerRes::ATACK]      = res.GetIndex(str);  break;
             case LUCK_SND:  play->sound [PlayerRes::LUCK]       = res.GetIndex(str);  break;
             case HEY_SND:   play->sound [PlayerRes::HEY]        = res.GetIndex(str);  break;
             case DEATH_SPR: ProcessStateList (curLine,play->sprite[PlayerRes::DEATH],realPos);      break;
             case CROSS_SPR: ProcessStateList (curLine,play->sprite[PlayerRes::CROSS_SHOT],realPos); break;
             case ATACK_SPR: ProcessStateList (curLine,play->sprite[PlayerRes::ATACK],realPos);      break;
             case NORM_SPR:  ProcessStateList (curLine,play->sprite[PlayerRes::NORM],realPos);      break;
             case WP_ID:     play->weaponId    = res.GetIndex(str);  break;
             case WP_CACHE:  play->weaponCache = res.GetIndex(str);  break;

             case IQ_FAR_FOLLOW:  play->iq [IQClass::FAR_FOLLOW]  = value; break;
             case IQ_NEAR_FOLLOW: play->iq [IQClass::NEAR_FOLLOW] = value; break;
             case IQ_ATACK:       play->iq [IQClass::ATACK]       = value; break;
             case IQ_VIEW_OUT:    play->iq [IQClass::VIEW_OUT]    = value; break;
             case IQ_RANDOM_MOVE: play->iq [IQClass::RANDOM_MOVE] = value; break;
             default:          return;
           }
        }
        else
             return ;
  }

  char dest [128];

  strcpy (dest,destDirName);
  strcat (dest,"/");
  strcat (dest,param.name);
  strcat (dest,".plr");

  play->Save (dest);

  delete play;
}

void    ProcessTeleport      (Param& param,int type)
{
  enum {
       SOUND_ID,
       SPRITE_ID,
       ANIM_ID
  };

  const         int count = 3;

  char*         members [count] = {"SPRITE","TRANS_SOUND","WORK_ANIM"};

  int           realMembers [count] = {SPRITE_ID,SOUND_ID,ANIM_ID};

  TeleportRes*  telep    = new TeleportRes;
  int               pos  = 0, realPos;
  char*             curLine;

  for (char* procStr = NextWord(curLine=NextLine(),pos);
             memcmp(procStr,"END",3);pos=0,
             procStr = NextWord(curLine=NextLine(),pos))
  {
                //Find type
        for (int j=0;j<count;j++)
            if (!strcmp(procStr,members[j]))
               break;

        if (j != count)
        {
           realPos              = pos;
           char*        str     = NextWord (curLine,pos);
           int          value   = atoi (str);

           switch (realMembers[j])
           {
             case SPRITE_ID: telep->sprite = res.GetIndex(str);  break;
             case SOUND_ID:  telep->sound  = res.GetIndex(str);  break;
             case ANIM_ID:   ProcessStateList (curLine,telep->anim,realPos);break;
             default:          return;
           }
        }
        else
             return ;
  }

  char dest [128];

  strcpy (dest,destDirName);
  strcat (dest,"/");
  strcat (dest,param.name);
  strcat (dest,".tlp");

  telep->Save (dest,(GameObjects)type);

  delete telep;
}

void            ProcessSpriteEffect  (Param& param)
{
  enum {
       CHANGE_TIME,
       ANIM,
       CROSS,
       SPRITE
  };

  const         int count = 4;

  char*         members [count] = {"SPRITE","ANIM","CROSS","CHANGE_TIME"};

  int           realMembers [count] = {SPRITE,ANIM,CROSS,CHANGE_TIME};

  SpriteEffectRes   sprEf;
  int               pos  = 0, realPos;
  char*             curLine;

  for (char* procStr = NextWord(curLine=NextLine(),pos);
             memcmp(procStr,"END",3);pos=0,
             procStr = NextWord(curLine=NextLine(),pos))
  {
                //Find type
        for (int j=0;j<count;j++)
            if (!strcmp(procStr,members[j]))
               break;

        if (j != count)
        {
           realPos              = pos;
           char*        str     = NextWord (curLine,pos);
           int          value   = atoi (str);

           switch (realMembers[j])
           {
             case SPRITE: sprEf.mapSprite = res.GetIndex(str);  break;
             case CROSS:  sprEf.cross     = res.GetIndex(str);  break;
             case CHANGE_TIME:  sprEf.changeTime = value;  break;
             case ANIM:   ProcessStateList (curLine,sprEf.anim,realPos);break;
             default:          return;
           }
        }
        else
             return ;
  }

  char dest [128];

  strcpy (dest,destDirName);
  strcat (dest,"/");
  strcat (dest,param.name);
  strcat (dest,".spr");

  sprEf.Save (dest);
}

void            ProcessLightEffect   (Param& param)
{
  enum {
       CHANGE_TIME,
       ANIM,
       DIST,
  };

  const         int count = 3;

  char*         members [count] = {"ANIM","DIST","CHANGE_TIME"};

  int           realMembers [count] = {ANIM,DIST,CHANGE_TIME};

  LightEffectRes    lightEf;
  int               pos  = 0, realPos;
  char*             curLine;

  for (char* procStr = NextWord(curLine=NextLine(),pos);
             memcmp(procStr,"END",3);pos=0,
             procStr = NextWord(curLine=NextLine(),pos))
  {
                //Find type
        for (int j=0;j<count;j++)
            if (!strcmp(procStr,members[j]))
               break;

        if (j != count)
        {
           realPos              = pos;
           char*        str     = NextWord (curLine,pos);
           int          value   = atoi (str);
           float        fvalue  = atof (str);

           switch (realMembers[j])
           {
             case DIST  :       lightEf.dist  = float2fixed(fvalue);  break;
             case CHANGE_TIME:  lightEf.changeTime = value;  break;
             case ANIM:   ProcessStateList (curLine,lightEf.anim,realPos);break;
             default:          return;
           }
        }
        else
             return ;
  }

  char dest [128];

  strcpy (dest,destDirName);
  strcat (dest,"/");
  strcat (dest,param.name);
  strcat (dest,".lt");

  lightEf.Save (dest);
}

EnvironmentMap*     ProcessSubjectList   (Param& pIn)
{
  dout<<"Process subject map"<<endl;

  const int     count           = 7;
  char*         members [count] = {"SUBJECT","DOOR", "PLAYER","TELEPORT",
                                   "PORTAL","SPRITE_EFFECT","LIGHT_EFFECT"};

  int           realMembers [count] = { RES_SUBJECT,RES_DOOR, RES_PLAYER,
                                        RES_TELEPORT, RES_PORTAL,RES_SPRITE_EFFECT,
                                        RES_LIGHT_EFFECT};

  EnvironmentMap*   map = new EnvironmentMap (pIn.num);

  int            pos       = 0;
  char*          curLine;

  for (char* procStr = NextWord(curLine=NextLine(),pos);
             memcmp(procStr,"END",3);pos=0,
             procStr = NextWord(curLine=NextLine(),pos))
  {
    for (int lineType=0;lineType<count;lineType++)
        if (!strcmp(members[lineType],procStr))
           break;

    if (lineType==count)
    {
       dout<<"Undefined symbol '"<<procStr<<"' in line #"<<line<<endl;
       continue;
    }

    int gameCache      = res.GetIndex   (NextWord(curLine,pos)),
        id             = res.GetIndex   (NextWord(curLine,pos));

    Exemplar  resource;

    switch (lineType)
    {
      case RES_SUBJECT:
           if (!ProcessSubjectEx (curLine+pos,resource))
              continue;
           break;
      case RES_DOOR:
           if (!ProcessDoorEx (curLine+pos,resource))
              continue;
           break;
      case RES_PLAYER:
           if (!ProcessPlayerEx (curLine+pos,resource))
              continue;
           break;
      case RES_TELEPORT:
           if (!ProcessTeleportEx (curLine+pos,resource))
              continue;
           break;
      case RES_PORTAL:
           if (!ProcessPortalEx (curLine+pos,resource))
              continue;
           break;
      case RES_SPRITE_EFFECT:
           if (!ProcessSpriteEffectEx (curLine+pos,resource))
              continue;
           break;
      case RES_LIGHT_EFFECT:
           if (!ProcessLightEffectEx (curLine+pos,resource))
              continue;
           break;
      default: continue;
    }

    strcpy (resource.resName,subjectTable[gameCache][id]);

    map->AddObject (resource);
   }

  return map;
}

int     ProcessRespawn (RespawnPoint* table)
{
  int   pos       = 0;
  int   index     = 0;
  char* curLine  ;

  for (char* procStr = NextWord(curLine=NextLine(),pos);index<MAX_RESPAWN && memcmp(procStr,"END",3);pos=0,
             procStr = NextWord(curLine=NextLine(),pos),index++)
  {
    pos   = 0;
    int x = atoi (NextWord (curLine,pos)),
        y = atoi (NextWord (curLine,pos));

    table [index].x = x;
    table [index].y = y;

    dout<<"X "<<x<<" Y "<<y<<endl;
  }

  return index;
}

/////////////////////Ресурс-схемы для объектов///////////////////////////////

BOOL            ProcessTeleportEx     (char* string,Exemplar& exemplar)
{
  TeleportExempl* exempl = (TeleportExempl*)&exemplar.resourceParam;
  int             pos    = 0;

  exempl->xFrom   = atoi (NextWord(string,pos));
  exempl->yFrom   = atoi (NextWord(string,pos));
  exempl->xTo     = atoi (NextWord(string,pos));
  exempl->yTo     = atoi (NextWord(string,pos));
  exempl->isCross = res.GetIndex(NextWord(string,pos));

  return TRUE;
}

BOOL            ProcessPortalEx      (char* string,Exemplar& exemplar)
{
  PortalExempl*   exempl = (PortalExempl*)&exemplar.resourceParam;
  int             pos    = 0;

  exempl->xFrom   = atoi (NextWord(string,pos));
  exempl->yFrom   = atoi (NextWord(string,pos));
  exempl->xTo     = atoi (NextWord(string,pos));
  exempl->yTo     = atoi (NextWord(string,pos));
  exempl->isCross = res.GetIndex(NextWord(string,pos));

  strcpy (exempl->levelName,NextWord(string,pos));

  return TRUE;
}

BOOL            ProcessSubjectEx     (char* string,Exemplar& res)
{
  SubjectExempl* exempl = (SubjectExempl*)&res.resourceParam;
  int            pos    = 0;

  exempl->x = atoi (NextWord(string,pos));
  exempl->y = atoi (NextWord(string,pos));

  return TRUE;
}

BOOL            ProcessDoorEx     (char* string,Exemplar& exemplar)
{
  TrigerExempl*  exempl = (TrigerExempl*)&exemplar.resourceParam;
  int            pos    = 0;

  exempl->x    = atoi (NextWord(string,pos));
  exempl->y    = atoi (NextWord(string,pos));
  exempl->side = res.GetIndex(NextWord(string,pos));

  return TRUE;
}

BOOL            ProcessPlayerEx     (char* string,Exemplar& res)
{
  PlayerExempl*  exempl = (PlayerExempl*)&res.resourceParam;
  int            pos    = 0;
  int            temp;

  exempl->x       = int2fixed(atoi (NextWord(string,pos)))+FX_HALF;
  exempl->y       = int2fixed(atoi (NextWord(string,pos)))+FX_HALF;
  exempl->angle   = int2angle(temp=atoi (NextWord(string,pos)));
  exempl->health  = atoi (NextWord(string,pos));
  exempl->protect = atoi (NextWord(string,pos));

  return TRUE;
}

BOOL            ProcessSpriteEffectEx  (char* string,Exemplar& exemplar)
{
  SpriteEffectExempl*  exempl = (SpriteEffectExempl*)&exemplar.resourceParam;
  int            pos    = 0;

  exempl->x           = atoi (NextWord(string,pos));
  exempl->y           = atoi (NextWord(string,pos));
  exempl->changeTime  = atoi (NextWord(string,pos));

  return TRUE;
}

BOOL            ProcessLightEffectEx  (char* string,Exemplar& exemplar)
{
  LightEffectExempl*  exempl = (LightEffectExempl*)&exemplar.resourceParam;
  int            pos    = 0;

  exempl->x           = atoi (NextWord(string,pos));
  exempl->y           = atoi (NextWord(string,pos));
  exempl->dist        = float2fixed(atof (NextWord(string,pos)));
  exempl->changeTime  = atoi (NextWord(string,pos));

  return TRUE;
}

void            SendLoadMessage (const char*)
{  }

void            SendLevelName (const char*)
{  }

