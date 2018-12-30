#include "palette.h"

Palette::Palette(Screen* scr):palette(new RGBStruct [256]),dest(scr)
{   ResetPalette(); }

void    Palette::ResetPalette()
{
  RGBStruct pal[256];
  for (int i=0;i<256;i++)
  {
    palette[i].red   = (i>>5)*9;
    palette[i].green = (i & 0x1C)*2.25;
    palette[i].blue  = (i & 3)*21;
  }

  if (dest)
     dest->PutPalette(palette);
}

static  void    Palette::ResetPalette(Screen* dest)
{
  RGBStruct pal[256];
  for (int i=0;i<256;i++)
  {
    pal[i].red   = (i>>5)*9;
    pal[i].green = (i & 0x1C)*2.25;
    pal[i].blue  = (i & 3)*21;
  }

  if (dest)
     dest->PutPalette(pal);
}