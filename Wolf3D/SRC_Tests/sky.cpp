#include <types.h>
#include <vga.h>
#include <picture.h>
#include <iostream.h>

#include <tables.h>
#include <keyboard.h>

Keyboard        kb;

uchar*          frame;
vga13hScreen    vga;

cPicture        picture;
tImage*         image;
int             bmpWidth;
ulong           wMask;

Angle           angle = 0;
const Angle     ROTATE_STEP = ANGLE_1;

void    DrawSky(Angle ang)
{
  int           shift   = angle>>7;
  uchar*        data    = image->data;
  uchar*        video   = frame;

  for (int row = 0;row<100;row++,data+=bmpWidth)
      for (int col = 0;col<320;col++)
            *video++ = data[(col-shift) & wMask];
}

void    main()
{
  if (!picture.load("wall.bmp"))
  {
    cout<<"Error open image"<<endl;
    return;
  }
  if (!vga.open())
  {
    cout<<"Error open VGA mode"<<endl;
    return;
  }

  frame = vga.GetPtr();

  image    = picture.GetImage();
  bmpWidth = picture.width();
  wMask    = bmpWidth-1;

  while (!kb.IsPress(KEY_ESC))
  {
    DrawSky(angle);
    if (kb.IsPress(KEY_CLEFT))
       angle += ROTATE_STEP;
    if (kb.IsPress(KEY_CRIGHT))
       angle -= ROTATE_STEP;
  }
  vga.close();
//  delete image;
}