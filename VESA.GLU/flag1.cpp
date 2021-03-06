#include <vga.h>
#include <vesalo.h>
#include <vgalo.h>
#include <math.h>
#include <dpmi32gw.h>
#include <conio.h>
#include <picture.h>

const float M_PI=3.141592645f;
uchar* buffer=(uchar*)0xA0000;

class vScreen:public vga13hScreen
{
  public:
   BOOL open();
   virtual void _PutPalette(int index,int count,const RGBStruct* palette);
};

void vScreen::_PutPalette(int index,int count,const RGBStruct* palette)
{
  ::SetPalette(0,256,palette);
}

BOOL vScreen::open()
{
 if(!VESALOSetMode(0x13))
  return FALSE;
 mWidth=320;
 mHeight=200;
 mBpp=1;
 mData=(uchar*)0xA0000;
 mClipRect.setup(0,0,mWidth-1,mHeight-1);
 return TRUE;
}

vScreen screen;

const int speed=1;
const int size=2;
const int curve=1;
const int sOfs=30;
const int sAmp=6;
const int sLen=255;
const int xMax=240/size;
const int yMax=150/size;

int  screenY[1300];
int  sTab[sLen+1];

void setFlag(int width)
{
  for (int i=0;i<200;screenY[i++]=i*width);
}

void cSin()
{
  for (int i=0;i<sLen+1;sTab[i++]=sin(i*4*M_PI/sLen)*sAmp+sOfs);
}

void Flag()
{
  cPicture picture;
  if (!picture.load("c:\\gluks\\sextris\\cu-cu3.bmp"))
    return;
  ShiftPalette(256,(RGBStruct*)picture.GetPalette(),-2);
  screen.PutPalette((RGBStruct*)picture.GetPalette());
//  screen.PutPalette(0,0,0,0);
  uchar* bmp=(uchar*)picture.GetImage();
  int xMax=picture.width();
  int yMax=picture.height()-1;
  int post[200][320];
  int xp,yp,sidx(0);
  int y;
  for (int x=0;x<xMax;x++)
    for (y=0;y<yMax;y++)
      post[y][x]=0;
  do
  {
    for (x=0;x<xMax;x++)
      for (y=yMax;y>0;y--)
      {
        buffer[post[y][x]]=0;
        xp=size*x+sTab[(sidx+curve*(x+y)) % sLen];
        yp=size*y+sTab[(sidx+4*x+(curve+1)*y) % sLen];
        post[y][x]=xp+screenY[yp];
        buffer[post[y][x]]=bmp[y*xMax+x];
      }
      sidx=(sidx+speed) % sLen;
      delay(1);
  }
  while(!kbhit());
}

void main()
{
  cSin();
  screen.open();
  setFlag(320);
  Flag();
  screen.close();
  SetMode(3);
}