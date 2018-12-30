#include "light.h"
#include "palette.h"

#include <vesa.h>
#include <conio.h>

#include "debug.h"

VESAdevice      vesa;

const int     left   = 16;
const int     right  = 784;
const int     top    = 50;
const int     bottom = 600-top;
const int     width  = 3;
const int     height = 25;

void    main()
{
  if (vesa.open() != VESAdevice::OK)
  {
    dout<<"Error open VESA"<<endl;
    return;
  }
  if (vesa.SetMode(VESA800x600x8) != VESAdevice::OK)
  {
    dout<<"Error open VESA mode"<<endl;
    return;
  }

  Screen*  screen    = vesa.CurrentScreen();
  Palette* palette   = new Palette(screen);
  LightTable* ltable = new LightTable;

  int level = 0;
  for (int y = top;y<bottom && level<MAX_LIGHT_LEVELS;y+=height*2,level++)
  {
    int i = 0;
    for (int x = left;x<right && i<256;x+=width,i++)
          screen->bar(x,y,x+width,y+height,ltable->light[level][i]);
  }

  getch();
  delete palette;
  vesa.close();
}