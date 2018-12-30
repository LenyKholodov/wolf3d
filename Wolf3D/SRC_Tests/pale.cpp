#include <vga.h>

#include <iostream.h>
#include <conio.h>

vga13hScreen    vga;

int rgb(int red,int green,int blue)
{
   return ((red>>5)<<5)+((green>>5)<<2)+(blue>>6);
}

void    NormPalette()
{
  RGBStruct pal[256];
  for (int i=0;i<256;i++)
  {
    pal[i].red   = ((i>>5)<<3)*1.125;
    pal[i].green = ((i & 0x1C)<<1)*1.125;
    pal[i].blue  = ((i & 3)<<4)*1.3125;
  }
  vga.PutPalette(pal);
}

void    main()
{
  if (!vga.open())
  {
    cout<<"Error open VGA 13h mode"<<endl;
    return;
  }
  NormPalette();
  vga.cls(rgb(255,0,0));
  getch();
  vga.cls(rgb(0,255,0));
  getch();
  vga.cls(rgb(0,0,255));
  getch();
  vga.cls(rgb(255,255,255));
  getch();
  vga.close();
}