#include <vbscreen.h>
#include <conio.h>
#include <picture.h>

BOOL vbScreen8bit::IsValid() const
{
 return TRUE;
}

vbScreen8bit screen;
int mode=VESA1024x768x8;

void main()
{
 screen.open(mode);
 cPicture picture;
 if (!picture.load("c:\\gluks\\Sextris\\2222.bmp"))
  return;
 tImage* image=picture.GetImage();
 ShiftPalette(256,(RGBStruct*)picture.GetPalette(),-2);
 screen.PutPalette(0,256,(RGBStruct*)picture.GetPalette());
 screen.PutImage(0,0,image);
 getch();
 screen.close();
 SetMode(3);
}