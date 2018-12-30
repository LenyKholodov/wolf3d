#include "fail.h"

#include <keyboard.h>
#include <debug/debug.h>

Destruct::~Destruct()
{
  Done ();
}

BOOL    Process ()
{
  while (!Keyboard::IsPress (KEY_N) && !Keyboard::IsPress (KEY_Y));

  if (Keyboard::IsPress (KEY_Y)) return TRUE;
  else                           return FALSE;
}

BOOL    Error (FailErrors error,Screen* screen)
{
  switch (error)
  {
    case NO_ERROR: return TRUE;
    case LEVEL_NO_MEMORY:
    {
      LevelNoMemory (screen);

      break;
    }
  }

  return Process();
}

void    LevelNoMemory (Screen* screen)
{
  screen->cls ();

  cPicture picture;

  if (!picture.load("face/error/levnomem.bmp"))
  {
    dout<<"No free memory for load level"<<endl;
    return;
  }

  int width  = picture.width(),
      height = picture.height();

  screen->PutImage ((screen->width()-width)/2,(screen->height()-height)/2,
                    picture.GetImage());
}
