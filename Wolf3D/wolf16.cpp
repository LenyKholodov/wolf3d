#include "render.h"
#include "level.h"

#include <debug/debug.h>
#include <tables.h>
#include <keyboard.h>
#include <video.h>

VESAdevice      vesa;
Keyboard        kb;
Level*          level;
Screen*         screen;

void    main ()
{
  if (vesa.open () != VESAdevice::OK)
  {
    dout<<"Error open VESA"<<endl;
    return;
  }
  if (vesa.SetMode(VESA640x480x16) != VESAdevice::OK)
  {
    dout<<"Error open VESA mode "<<endl;
    return;
  }
  if ((level = LevelCompile::LoadLevel("maps/wolf3d/wolf3d.lev"))== NULL)
  {
    dout<<"Error open level"<<endl;
    return;
  }

  screen = GimmeMirror(vesa.CurrentScreen());

  RayCast16bit* draw = new RayCast16bit ((fScreen16bit*)screen,FALSE);

  draw->SetTexture(level->GetTextCache());
  draw->SetMap(level->GetMap());
  draw->SetLightMap(level->GetLightMap());
  draw->SetCache(level->GetSpriteCache());

  draw->Reset();

  Angle   angle=0;
  Fixed   x=0,y=0;

  while (!kb.IsPress(KEY_ESC))
  {
    screen->cls (1);
    draw->DrawView (x,y,angle);
    screen->refresh();
    if (kb.IsPress (KEY_CLEFT))
       angle-=ANGLE_1<<2;
    if (kb.IsPress (KEY_CRIGHT))
       angle+=ANGLE_1<<2;
    if (kb.IsPress (KEY_CUP)  || kb.IsPress(KEY_W))
    {
      Fixed _x = x + (cos (angle)>>1),
            _y = y + (sin (angle)>>1);

      if (level->GetMap()->IsCross(Index(fixed2int(_y),fixed2int(_x))))
      {
        x = _x;
        y = _y;
      }
    }
    if (kb.IsPress (KEY_CDOWN) || kb.IsPress(KEY_S))
    {
      Fixed _x = x - (cos (angle)>>1),
            _y = y - (sin (angle)>>1);

      if (level->GetMap()->IsCross(Index(fixed2int(_y),fixed2int(_x))))
      {
        x = _x;
        y = _y;
      }
    }
  }

  dout<<"Done"<<endl;

  vesa.close();

  delete draw;
  delete screen;
  delete level;
}

void            SendLoadMessage (const char*)
{  }

void            SendLevelName (const char*)
{  }