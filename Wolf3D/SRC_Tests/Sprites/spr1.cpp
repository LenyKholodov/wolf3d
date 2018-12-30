#include <vesa.h>
#include <picture.h>
#include <conio.h>

#include "sprite.h"

#include <debug.h>

VESAdevice      vesa;
Screen*         screen;
cPicture        pict;

double          dist;
double          focus   = 1.5;

void    main()
{
  if    (vesa.open() != VESAdevice::OK)
  {
        dout<<"Error open VESA"<<endl;
        return;
  }
  if    (vesa.SetMode(VESA800x600x8) != VESAdevice::OK)
  {
        dout<<"Error open VESA mode"<<endl;
        return;
  }
  screen = vesa.CurrentScreen();

//  if (!pict.load("d:\\resource\\bitmaps\\dima1.bmp"))
//  {
//        dout<<"Error open data file"<<endl;
//        return;
//  }
//  ShiftPalette(256,(RGBStruct*)pict.GetPalette(),-2);
//  screen->PutPalette((RGBStruct*)pict.GetPalette());
//  screen->cls(15);

  SimpleSprite* s   [2];
  Sprite3D*     spr [2];

//  s   [0]                = makeSimpleSprite((tImage*)pict.GetImage(),0);
//  spr [0]                = makeSprite3D    (s,1);
//  GameSprite3D* s3D      = makeGameSprite3D(spr,1);
  GameSprite3D*   s3D    = makeGameSprite3D ("textures/sprite.wsf");

  uchar* data;
//  SimpleSprite* sprite = (*spr[0])[0];
  SimpleSprite* sprite = (*(*s3D)[0])[0];
  ulong* offsets       = (ulong*)sprite->data;

        //Простейшая демонстрация масштабирования спрайта
  for (dist = 0;dist<12.0;dist+=0.2)
  {
      double width  = focus*320.0/dist;
      double height = focus*200.0/dist;
      double hStep  = sprite->width/width;
      double vStep  = sprite->height/height;

      double        dx = 0;
      double        dy;
      screen->cls();
      for (int x=0;x<(int)width;x++,dx+=hStep)
      {
        data = (*sprite)[(int)dx];
        dy   = 0;
        for (int y=0;y<(int)height;y++,dy+=vStep)
            screen->PutPixel(x,y,data[(int)dy]);
      }
//      for (int i=0;i<50000000;i++);
  }

  getch();

  delete s[0];
  delete spr[0];
  delete s3D;

  vesa.close();
}