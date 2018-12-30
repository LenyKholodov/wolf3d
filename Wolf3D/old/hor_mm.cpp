#include <math.h>
#include <mem.h>
#include <iostream.h>
#include <time.h>

#include <types.h>
#include <vga.h>
#include <picture.h>
#include <mscreen.h>

#include "textures.h"
#include "palette.h"
#include "keyboard.h"
#include "tables.h"
#include "fixed.h"

#include "debug.h"

#define    TEXT_SIZE 0xFFFF

const   int     SKY_COLOR   = 3;
const   Fixed16 H           = 100.0; //256l*100
const   double  DELTA       = 1.0;   //DELTA * NumLines = H
const   double  C           = 0.01;      //0.01
const   double  D0          = 100.0;         //Относительная высота
const   double  ROTATE_STEP = 2.0*FL_PI/180.0;

const  ulong    wMask  = MAKELONG(127,0xFFFF),
                hMask  = MAKELONG(127,0xFFFF),
                wShift = 7;

Keyboard        kb;
vga13hScreen    vga;
mScreen8bit*    screen;
Pool*           text_pool;
MipMapText*     mmt;
Palette*        palette;
int             bmpWidth,   //Texture width
                bmpHeight;  //Texture height
int             ilcCount;
uchar           *image,
                *frame;  //Frame buffer

Angle           angle;
Fixed16         x,y;

Fixed16         distTable[100];
Fixed16         CSin[8192],
                CCos[8192];

void            SetTables();
void            DrawView ();

void    main()
{
  if (!vga.open())
  {
    cout<<"Error open VGA mode"<<endl;
    return;
  }
  screen  = (mScreen8bit*)GimmeMirror(&vga);
  palette = new Palette(&vga);
  screen->cls();
  frame = screen->GetPtr();

  SetTables();

  text_pool = new Pool(1,TEXT_SIZE);
  MipMapText::SetPool(text_pool); //1 текстура!
  mmt = new MipMapText("wall.mmt");
  ilcCount = mmt->count();

  long    totalFrames = 0;
  clock_t start = clock();
//  D_String("begin");
  while (!kb.IsPress(KEY_ESC))
  {
    DrawView();
    screen->refresh();
    totalFrames++;
    if (!kb.IsEmpty())
    {
      Fixed16   vx = cos (angle);
      Fixed16   vy = sin (angle);
      if (kb.IsPress(KEY_LEFTARROW))
         angle += ROTATE_STEP;
      if (kb.IsPress(KEY_RIGHTARROW))
         angle -= ROTATE_STEP;
      if (kb.IsPress(KEY_UPARROW))
      {
        x += vx;
        y += vy;
      }
      if (kb.IsPress(KEY_DOWNARROW))
      {
        x -= vx;
        y -= vy;
      }
    }
  }
  clock_t end       = clock();
  double  totalTime = (end-start)/CLK_TCK;
  vga.close();
  cout<<"Hor text: FPS = "<<totalFrames/totalTime<<endl;
  delete palette;
  delete mmt;
  delete text_pool;
}

void    DrawView()
{
  uchar*        vptr1 = frame + 32000;
  uchar*        vptr2 = vptr1 + 160; //161

  Fixed16*      dist  = distTable;
  Fixed16       Csin  = CSin[angle.GetData()>>3],
                Ccos  = CCos[angle.GetData()>>3];
  Fixed16       _sin  = sin(angle),
                _cos  = cos(angle);

  Fixed16       du,             //Delta texture x
                dv,             //Delta texture y
                u1, v1,         //Texture x, y for first draw
                u2, v2,         //Texture x, y for second draw
                i,              //Image offsets
                j;

  ulong         wm, hm;
  int           shift,
                k,
                level;
  Fixed16       scale;

  for (int row = 0;row<100;row++,dist++,vptr1+=320,vptr2+=160)
  {
    shift = wShift;
    wm    = wMask;
    hm    = hMask;
    k     = 64;
    du    = *dist*Csin;
    dv    = *dist*Ccos;
    u1    =  u2 = (x+*dist*_cos) & wMask;
    v1    =  v2 = (y+*dist*_sin) & hMask;

    level = 0;
    scale = *dist*C>>1;

    while (scale>FX_ONE && level<ilcCount-1)
    {
      wm     -= k;
      hm     -= k;
      u1      = u1>>1;
      u2      = u2>>1;
      v1      = v1>>1;
      v2      = v2>>1;
      du.SetData(du.GetData()>>1);
      dv.SetData(dv.GetData()>>1);
      scale   = scale>>1;
      shift--;
      k     >>= 1;
      level++;
    }

    image = (uchar*)mmt->GetTexture(level);

    vptr1 += 160;       //opt

    for (int col = 159;col>=0;col--)
    {
      i =  u1 & wm;
      j =  (v1 & wm)<<shift;
      *vptr1-- = *(image+i+j);   //8-bit
      i =  u2 & wm;
      j =  (v2 & hm)<<shift;
      *vptr2++  = *(image+i+j);  //8-bit
      u1 -= du; u2 += du;
      v1 -= dv; v2 += dv;
    }
  }
}

void    SetTables()
{
  for (int row = 0;row<100;row++)
    distTable[row] = H*D0/((1+row)*DELTA);
  Angle a;
  for (long ij=0;ij<8192;ij++)
  {
    a.SetData(ij<<3);
    double d = a;
    CSin[ij] = sin(d)*C;
    CCos[ij] = -cos(d)*C;
  }
}
