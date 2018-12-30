#include <picture.h>
#include <vesa.h>

#include <iostream.h>
#include <conio.h>

cPicture        picture;
VESAdevice      vesa;
Screen*         screen;
int             mode = VESA1024x768x8;

void    main()
{
  if (vesa.open()!=VESAdevice::OK)
  {
    cout<<"Error open VESA"<<endl;
    return;
  }
  if (vesa.SetMode(mode)!=VESAdevice::OK)
  {
    cout<<"Error open VESA mode"<<endl;
    return;
  }
  screen = vesa.CurrentScreen();
  if (!picture.load("snow2.bmp"))
     return;
  ShiftPalette(256,(RGBStruct*)picture.GetPalette(),-2);
  screen->PutPalette((RGBStruct*)picture.GetPalette());
  screen->PutImage(10,10,picture.GetImage());
  if (!picture.load("atoll2.bmp"))
     return;
  screen->PutImage(10,300,picture.GetImage());
  getch();
  vesa.close();
}