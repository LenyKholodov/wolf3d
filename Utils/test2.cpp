#include <grtypes.h>
#include <vesa.h>

#include <iostream.h>
#include <conio.h>

tImageList*     il = tImageList::load("wall.ilc");
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

  ShiftPalette(256,il->GetPalette()->palette,-2);
  screen->PutPalette(il->GetPalette()->palette);
  screen->PutImage(10,10,il->GetImage(3));
  getch();
  vesa.close();
}