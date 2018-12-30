#include <iostream.h>
#include <conio.h>

#include <grtypes.h>
#include <vesa.h>

VESAdevice      vesa;
Screen*         screen;
int             mode = VESA640x480x8;

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
  tImageList* ilc = tImageList::load("atoll1.ilc");
  if (ilc == NULL)
     return;
  screen = vesa.CurrentScreen();
  if (ilc->palsize>0)
  {
    ShiftPalette(256,ilc->GetPalette()->palette,-2);
    screen->PutPalette(ilc->GetPalette()->palette);
  }
  int x = 0,
      y = 0;
  for (int i=0;i<ilc->count;i++,x+=ilc->GetImage(i)->width/2,
                                y+=ilc->GetImage(i)->height/2)
    screen->PutImage(x,y,ilc->GetImage(i));
  getch();
  vesa.close();
}
