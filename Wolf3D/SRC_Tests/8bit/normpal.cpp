#include <picture.h>
#include <iostream.h>
#include <mem.h>
#include <debug\debug.h>

int    rgb(int r,int g,int b)
{
  return ((r>>5)<<5)+((g>>5)<<2)+(b>>6);
}

int     main(int argc,char* argv[])
{
  dout<<"UNT Games Studio. Palette Normilize Utility"<<endl;
  if (argc<2)
  {
    cout<<"source.bmp destin.bmp"<<endl;
    return 1;
  }
  cPicture picture;
  if (!picture.load(argv[1]))
  {
    cout<<"This file is not BMP-picture: "<<argv[1]<<endl;
    return 1;
  }
  if (picture.GetPaletteSize() != 256)
  {
    cout<<"Picture must be '256 colors' palette image"<<endl;
    return 1;
  }
  tImage*       image = picture.GetImage();
  uchar*        tmp   = image->data;
  RGBStruct*    pal   = picture.GetPalette();
  long          size  = image->width*image->height;
  for (int i=0;i<size;i++,tmp++)
      *tmp = rgb(pal[*tmp].red,pal[*tmp].green,pal[*tmp].blue);
  for (i=0;i<256;i++)
  {
    pal[i].red   = ((i>>5)<<5)*1.138;
    pal[i].green = ((i & 0x1C)<<3)*1.138;
    pal[i].blue  = ((i & 3)<<6)*1.328;
  }
  picture.BMPsave(argv[2]);

  return 0;
}


