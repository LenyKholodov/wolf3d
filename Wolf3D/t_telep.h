#ifndef __GAME_TELEPORT_RES__
#define __GAME_TELEPORT_RES__

#include "t_res.h"
#include "t_mes.h"

#include <string.h>

/////////////////////////////////////////////////////////////////////////////
////������-�奬� ��� ⥫�����
/////////////////////////////////////////////////////////////////////////////
struct  TeleportRes:public ResourceHeader
{
  int   sprite;                 //������ �ࠩ� ⥫�����
  int   sound;                  //��� ��७��
  char  anim [TELEP_ANIM_NUM];  //������� ࠡ��� ⥫�����

     BOOL        Load (const char*);
     BOOL        Load (BinFile*);
     void        Save (const char*,GameObjects = TELEPORT);
     void        Save (BinFile*,GameObjects = TELEPORT);
};

struct  TeleportExempl
{
  int   xFrom,yFrom,            //���न���� ���⠫� � ��७��
        xTo,yTo;
  BOOL  isCross;                //����������� ����祭��
};

/////////////////////////////////////////////////////////////////////////////
////������ �奬� ��� ���⠫�
/////////////////////////////////////////////////////////////////////////////
struct  PortalExempl: public TeleportExempl
{
  char  levelName [128];        //�������� �஢��, � ����� ��६�頥� ���⠫
};

typedef CreateExemplar <TeleportRes,TeleportExempl> CreateTeleport;
typedef CreateExemplar <TeleportRes,PortalExempl>   CreatePortal;

/////////////////////////////////////////////////////////////////////////////
////��� ��� ��࠭���� ⥫�����
/////////////////////////////////////////////////////////////////////////////
struct TeleportTag
{
  TeleportRes      res;
  TeleportExempl   exempl;

  int              animPos;
};

/////////////////////////����饭��///////////////////////////////////////////

enum TeleportMesId
{
  TELEPORT_TO = TELEPORT_LIMIT,
  CHANGE_LEVEL
};

/////////////////////////////////////////////////////////////////////////////
////����饭�� � ⥫�����樨
////////////////////////////////////////////////////////////////////////////
struct  TeleportToMessage: public GameMessage
{
        TeleportToMessage (int _x,int _y): x(_x),y(_y)
                          { id=TELEPORT_TO; }

        int     x, y;   //���न���� ⥫�����樨
};

/////////////////////////////////////////////////////////////////////////////
////���ᥪ�� ���⠫ - ����㦠�� ᫥���騩 �஢���
/////////////////////////////////////////////////////////////////////////////
struct  PortalChangeLevelMessage: public GameMessage
{
        PortalChangeLevelMessage (const char* name,int _x,int _y)
          : x(_x), y(_y)
                       { id = CHANGE_LEVEL; strcpy (levelName,name); }

        int     x, y;           //���न���� ⥫�����樨
        char    levelName [64]; //�஢��� ⥫�����樨
};

#endif