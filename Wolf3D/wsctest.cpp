#include <vesa.h>
#include <grtypes.h>

#include <conio.h>
#include <iostream.h>

#include "sprite.h"

#include <debug\debug.h>
#include <time.h>

#define TOTAL 1000l

VESAdevice      vesa;
Screen*         screen;
const int       mode = VESA800x600x8;

void    main()
{
  if (vesa.open() != VESAdevice::OK)
  {
    cout<<"Error open VESA"<<endl;
    return;
  }
  if (vesa.SetMode(mode) != VESAdevice::OK)
  {
    cout<<"Error open VESA mode"<<endl;
    return;
  }
  screen = vesa.CurrentScreen();

  SpriteCache* wtf  = new SpriteCache ("textures\\w3d.wsc",2);
  if (wtf)
    cout<<"Load OK"<<endl;

  double  start = clock();

  for (long i = 0;i<TOTAL;i++)
  {
    wtf->GetData(0,0);
    wtf->GetData(1,0);
    wtf->GetData(2,0);
  }

  double end = clock();
  cout<<"Test OK"<<endl;

  cout<<"Total time    "<<(end-start)/CLK_TCK<<endl;
  cout<<"In one second "<<3*TOTAL*CLK_TCK/(end-start)<<endl;

  Sprite3D*   s3D   = (Sprite3D*)wtf->GetData(2);
  dout<<"!"<<endl;
  uchar*      image = (*s3D)[0]->data;

  if (image == NULL)
     D_Error ("Hey-ho!");


  for (int y=0;y<128;y++)
      for (int x=0;x<128;x++)
          screen->PutPixel(x+100,y+100,*image++);

  delete wtf;
  getch();
  vesa.close();
}