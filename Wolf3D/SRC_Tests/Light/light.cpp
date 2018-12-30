#include "d:\\projects\\project\\wolf3d\\palette.h"

LightTable::LightTable()
{
  float k       = 1.0;
  float DELTA   = 0.125; // 1/8
  for (int i=0;i<8;i++,k-=DELTA)
      form(i,k);
}

void    LightTable::form(int level,float k)
{
  int           r,g,b;
  uchar*        table = light[level];
  
  for (int src=0;src<256;src++)
  {
    Palette::rgb(src,r,g,b);
    r *= k;                  //??
    g *= k;
    b *= k;
    table[src] = Palette::rgb(r,g,b);
  }
}
