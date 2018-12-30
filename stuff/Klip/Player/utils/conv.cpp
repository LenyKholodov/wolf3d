#include <iostream.h>
#include <picture.h>

void    main (int argc,char* argv[])
{
  if (argc<3)
  {
    cout<<"24bit.bmp 16bit.bmp"<<endl;
    return;
  }

  cPicture      p (argv[1]);

  if (p.bpp() != 24)
  {
    cout<<"first file must be in 24 bit color model"<<endl;
    return;
  }

  tImage* src     = p.GetImage();

  cPicture conv (0,0,src->width,src->height,16,0);

  int       len      = src->width*src->height;
  tPicture* dest     = conv.GetPicture();
  uchar*    readPos  = src->data;
  uint16*   writePos = (uint16*)dest->image.data;

  for (int i=0;i<len;i++,readPos+=3,writePos++)
  {
    uchar red   = readPos[0],
          green = readPos[1],
          blue  = readPos[2];

    *writePos = ((blue>>3)<<11)+((green>>2)<<5)+(red>>3);
  }

  conv.BMPsave (argv[2]);
}