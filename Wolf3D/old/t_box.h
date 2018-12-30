#ifndef __TYPE_GAME_BOX__
#define __TYPE_GAME_BOX__

#include "t_defs.h"
#include "t_res.h"

#include <binfile.h>

/////////////////Описание схем ресурсов для GameBox//////////////////////////

struct   GameBoxRes: public ResourceHeader
{
  int             count;
  int             x, y;

  int             takeSoundId,
                  useSoundId,
                  spriteId;

     BOOL        Load (const char*);
     BOOL        Load (BinFile*);
     void        Save (const char*);
     void        Save (BinFile*);
};

/////////////////////////////////////////////////////////////////////////////
//////Создание коробки
/////////////////////////////////////////////////////////////////////////////
struct   CreateGameBox
{
  ID            playerOwner;      //maybe ID(-1,-1)
  GameBoxRes    box;
};

/////////////////////////////////////////////////////////////////////////////
/////Таг для сохранения GameBox
/////////////////////////////////////////////////////////////////////////////
struct    GameBoxTag
{
  GameBoxRes    box;
};

#endif