#ifndef __TYPE_GAME_DOOR__
#define __TYPE_GAME_DOOR__

#include "t_res.h"
#include "t_triger.h"

/////////////////////////////////////////////////////////////////////////////
//////Создание двери (ArgPtr)
/////////////////////////////////////////////////////////////////////////////
struct   DoorRes: public ResourceHeader
{
  int           textId;
  int           openSound,
                closeSound;

  int           doorWaitTime;
  uint          doorStep;
  int           doorCrossState;

     BOOL        Load (const char*);
     BOOL        Load (BinFile*);
     void        Save (const char*);
     void        Save (BinFile*);
};

struct   DoorExempl
{
  Side          side;   // (N,E,S,W)
  int           x,y;    // map coords (integer format)
};

typedef CreateExemplar <DoorRes,DoorExempl> CreateDoor;

/////////////////////////////////////////////////////////////////////////////
/////Таг для сохранения двери
/////////////////////////////////////////////////////////////////////////////
struct    DoorTag
{
  DoorRes    res;

  int        mDoorState;      //насколько открыта
  int        mDoorStep;
  int        mDoorWait;
};

#endif