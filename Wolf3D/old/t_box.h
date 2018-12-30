#ifndef __TYPE_GAME_BOX__
#define __TYPE_GAME_BOX__

#include "t_defs.h"
#include "t_res.h"

#include <binfile.h>

/////////////////���ᠭ�� �奬 ����ᮢ ��� GameBox//////////////////////////

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
//////�������� ��஡��
/////////////////////////////////////////////////////////////////////////////
struct   CreateGameBox
{
  ID            playerOwner;      //maybe ID(-1,-1)
  GameBoxRes    box;
};

/////////////////////////////////////////////////////////////////////////////
/////��� ��� ��࠭���� GameBox
/////////////////////////////////////////////////////////////////////////////
struct    GameBoxTag
{
  GameBoxRes    box;
};

#endif