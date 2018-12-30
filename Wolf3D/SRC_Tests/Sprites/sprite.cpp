#include "sprite.h"

#include <debug.h>

SimpleSprite::SimpleSprite(tImage* image,int z):
   width(image->width),height(image->height),z_color(z)
{
  ulong*        offsets = (ulong*)data;

  for (int x=0;x<width;x++)
  {
      uchar* filePos = x+image->data;
      int pos        = offsets[x] = x*height+width*sizeof(ulong);

      for (int y=0;y<height;y++,filePos+=width)
          data[pos++] = *filePos;
  }
}

long   SimpleSprite::size (int bpp)
{
  return width*height*bpp+sizeof(SimpleSprite)-sizeof(uchar)+
         width*sizeof(ulong);
}

void*   SimpleSprite::operator new (size_t,size_t bytes)
{
  return ::operator new (bytes);
}

void    SimpleSprite::operator delete (void* del)
{
  ::operator delete (del);
}

Sprite3D::Sprite3D (tImageList* list,int z):num(list->count),bpp(list->bpp)
{
  ulong*        offset = (ulong*)data;
  ulong         pos    = num*sizeof(ulong);
  SimpleSprite* sprite;

  for (int i=0;i<num;i++)
  {
    sprite    = new(data+pos) SimpleSprite(list->GetImage(i),z);
    pos      += sprite->size(bpp);
    offset[i] = pos;
  }
}

Sprite3D::Sprite3D (SimpleSprite** sprite,int n,int bitpp):num(n),bpp(bitpp)
{
  ulong*        offset = (ulong*)data;
  ulong         pos    = num*sizeof(ulong);

  for (int i=0;i<num;i++)
  {
    memcpy(data+pos,sprite[i],pos+=sprite[i]->size(bpp));
    offset[i] = pos;
  }
}

void*   Sprite3D::operator new (size_t,size_t bytes)
{
  return ::operator new (bytes);
}

void    Sprite3D::operator delete (void* del)
{
  ::operator delete (del);
}

///////////////////////////////TEST SECTION/////////////////////////////////
#include <vesa.h>
#include <picture.h>
#include <conio.h>


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
  if    (vesa.SetMode(0x13) != VESAdevice::OK)
  {
        dout<<"Error open VESA mode"<<endl;
        return;
  }
  screen = vesa.CurrentScreen();

  if (!pict.load("d:\\resource\\bitmaps\\dima1.bmp"))
  {
        dout<<"Error open data file"<<endl;
        return;
  }
  ShiftPalette(256,(RGBStruct*)pict.GetPalette(),-2);
  screen->PutPalette((RGBStruct*)pict.GetPalette());
  screen->cls(15);

  SimpleSprite* sprite[2];

  sprite[0] = new (256*256) SimpleSprite ((tImage*)pict.GetImage(),0);

  uchar* data;
  ulong* offsets = (ulong*)sprite[0]->data;

        //Простейшая демонстрация масштабирования спрайта
  for (dist = 0;dist<12.0;dist+=0.2)
  {
      double width  = focus*320.0/dist;
      double height = focus*200.0/dist;
      double hStep  = sprite[0]->width/width;
      double vStep  = sprite[0]->height/height;

      double        dx = 0;
      double        dy;

      screen->cls();

      for (int x=0;x<(int)width;x++,dx+=hStep)
      {
        data = sprite[0]->data+offsets[(int)dx];
        dy   = 0;
        for (int y=0;y<(int)height;y++,dy+=vStep)
            screen->PutPixel(x,y,data[(int)dy]);
      }
      for (int i=0;i<50000000;i++);
  }

  getch();

  delete sprite[0];

  vesa.close();
}