#ifndef __GAME_FAIL__
#define __GAME_FAIL__

#include "fin.h"
#include <video.h>

extern  void Done ();   //Выгрузчик

class   Destruct: public FinClient
{
  public:
          virtual ~Destruct();
};

enum FailErrors
{
  NO_ERROR = 0,
  LEVEL_NO_MEMORY,
};

BOOL    Error (FailErrors,Screen* = NULL);

void    LevelNoMemory (Screen*);

#endif