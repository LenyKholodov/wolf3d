#ifndef __GAME_TELEPORT_RES__
#define __GAME_TELEPORT_RES__

#include "t_res.h"
#include "t_mes.h"

#include <string.h>

/////////////////////////////////////////////////////////////////////////////
////Ресурс-схема для телепорта
/////////////////////////////////////////////////////////////////////////////
struct  TeleportRes:public ResourceHeader
{
  int   sprite;                 //Индекс спрайта телепорта
  int   sound;                  //Звук переноса
  char  anim [TELEP_ANIM_NUM];  //Анимация работы телепорта

     BOOL        Load (const char*);
     BOOL        Load (BinFile*);
     void        Save (const char*,GameObjects = TELEPORT);
     void        Save (BinFile*,GameObjects = TELEPORT);
};

struct  TeleportExempl
{
  int   xFrom,yFrom,            //Координаты портала и переноса
        xTo,yTo;
  BOOL  isCross;                //Возможность пересечения
};

/////////////////////////////////////////////////////////////////////////////
////Ресурс схема для портала
/////////////////////////////////////////////////////////////////////////////
struct  PortalExempl: public TeleportExempl
{
  char  levelName [128];        //Название уровня, в который перемещает портал
};

typedef CreateExemplar <TeleportRes,TeleportExempl> CreateTeleport;
typedef CreateExemplar <TeleportRes,PortalExempl>   CreatePortal;

/////////////////////////////////////////////////////////////////////////////
////Таг для сохранения телепорта
/////////////////////////////////////////////////////////////////////////////
struct TeleportTag
{
  TeleportRes      res;
  TeleportExempl   exempl;

  int              animPos;
};

/////////////////////////Сообщения///////////////////////////////////////////

enum TeleportMesId
{
  TELEPORT_TO = TELEPORT_LIMIT,
  CHANGE_LEVEL
};

/////////////////////////////////////////////////////////////////////////////
////Сообщение о телепортации
////////////////////////////////////////////////////////////////////////////
struct  TeleportToMessage: public GameMessage
{
        TeleportToMessage (int _x,int _y): x(_x),y(_y)
                          { id=TELEPORT_TO; }

        int     x, y;   //Координаты телепортации
};

/////////////////////////////////////////////////////////////////////////////
////Пересекли портал - загружаем следующий уровень
/////////////////////////////////////////////////////////////////////////////
struct  PortalChangeLevelMessage: public GameMessage
{
        PortalChangeLevelMessage (const char* name,int _x,int _y)
          : x(_x), y(_y)
                       { id = CHANGE_LEVEL; strcpy (levelName,name); }

        int     x, y;           //Координаты телепортации
        char    levelName [64]; //Уровень телепортации
};

#endif