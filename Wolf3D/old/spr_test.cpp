#include "sprite.h"
#include "palette.h"

#include <vesa.h>
#include <mscreen.h>
#include <ascreen.h>
#include <keyboard.h>

#include <tables.h>
//#include <math.h>

#include <debug.h>
#include <time.h>

const int       mode = VESA1024x768x8;

Keyboard        kb;
VESAdevice      vesa;
Palette*        palette;

mScreen8bit*    screen;
aScreen*        scr;

Fixed*          zBuffer;
GameSprite3D*   s3D;
SimpleSprite*   sprite;
SpriteList*     list;
RenderSprite*   render;

uFixed          xSprite = int2fixed(6),
                ySprite = int2fixed(7);

Fixed           proj    = 0;

uFixed          x = int2fixed(10),
                y = int2fixed(10);
Angle           angle = 0;

void            InitSystem();
void            DeleteSystem();
void            DrawView(Fixed,Fixed,Angle);
VisSprite*      ProjectSprite (Fixed,Fixed,Fixed,Fixed,Angle);

void    main()
{
  if (vesa.open() != VESAdevice::OK)
  {
    dout<<"Error open VESA"<<endl;
    return ;
  }
  if (vesa.SetMode(mode) != VESAdevice::OK)
  {
    dout<<"Error open VESA mode"<<endl;
    return ;
  }

  Screen*   cur = vesa.CurrentScreen();

  scr     = new aScreen(10,10,cur->width()-10,cur->height()-10,cur);
  screen  = (mScreen8bit*)GimmeMirror(scr);

  proj    = int2fixed((screen->width()/2));

  palette = new Palette(vesa.CurrentScreen());

  InitSystem();

  int     totalFrames = 0;
  clock_t start       = clock();

  while (!kb.IsPress (KEY_ESC))
  {
    screen->cls();
    DrawView (x,y,angle);
    screen->refresh();

    totalFrames++;

    if (kb.IsPress(KEY_LEFTARROW))       angle += ANGLE_1;
    if (kb.IsPress(KEY_RIGHTARROW))      angle -= ANGLE_1;

    if (kb.IsPress (KEY_UPARROW))
    {
      x += cos (angle)>>4;
      y += sin (angle)>>4;
    }
    if (kb.IsPress (KEY_DOWNARROW))
    {
      x -= cos (angle)>>4;
      y -= sin (angle)>>4;
    }

  }

  clock_t end = clock();

  DeleteSystem();

  delete screen;
  delete scr;

  vesa.close();

  dout<<"FPS: "<<(double)totalFrames/((double)(end-start)/CLK_TCK)<<endl;
}

void    InitSystem ()
{
  zBuffer   = new Fixed [screen->width()];

  for (int i=0;i<screen->width();i++)
      zBuffer [i] = int2fixed (128);

  render    = new RenderSprite8bit (screen,zBuffer);
  s3D       = makeGameSprite3D ("textures/sprite.wsf"); //!!
  sprite    = (*(*s3D)[0])[0];
  list      = new SpriteList   (5);

}

void    DeleteSystem ()
{
  delete render;
  delete zBuffer;
}

void    DrawView (Fixed x,Fixed y,Angle angle)
{
  VisSprite* sprite = ProjectSprite (x,y,xSprite,ySprite,angle);

  if (sprite == NULL)
     return ;

//  list->AddSprite (*sprite);

  render->DrawSprites(sprite,1);        //!!??

//  list->Reset();

  delete sprite;
}

VisSprite*   ProjectSprite (Fixed xView,Fixed yView,
                       Fixed xSprite,Fixed ySprite,
                       Angle angle)
{
  Fixed  dx   = xSprite - xView;
  Fixed  dy   = ySprite - yView;

  Fixed  _sin = sin(angle);     //out
  Fixed  _cos = cos(angle);    //out

  Fixed locY  = mul(dx,_cos)+mul(dy,_sin);      //dist
  Fixed locX  = mul(dx,_sin)-mul(dy,_cos);

  Fixed xscale = div(proj,locY);
  int   xstart = fixed2int(proj-mul(locX,xscale)); //halfWidth

  Fixed dist     = abs(dx)+abs(dy);
//  Fixed dist   = abs ( div(locY,FX_HALF) ); //     /focus

  if (xstart>screen->width()) //<0
     return NULL;

  VisSprite*    vSprite = new VisSprite;

  vSprite->dist         = dist;
  vSprite->xstart       = xstart;

  vSprite->sprite       = sprite;

  return vSprite;

}