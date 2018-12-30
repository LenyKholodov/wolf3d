#include "textures.h"
#include "palette.h"

#include <vesa.h>
#include <iostream.h>
#include <conio.h>

VESAdevice      vesa;
Screen*         screen;
Palette*        palette;

void    main()
{
  if (vesa.open() != VESAdevice::OK)
  {
    cout<<"Error open VESA"<<endl;
    return;
  }
  if (vesa.SetMode(VESA800x600x8)!= VESAdevice::OK)
  {
    cout<<"Error open VESA mode"<<endl;
    return;
  }
  screen  = vesa.CurrentScreen();
  palette = new Palette(screen);
  int x = 100;
  MipMapText* mmt = makeMMT("textures/wall.mmt");
  for (int i=0;i<mmt->count();x += mmt->width(i)+10,i++)
    screen->PutImage(x,100,mmt->GetImage(i));
  getch();
  vesa.close();
  delete palette;
  delete mmt;
}
