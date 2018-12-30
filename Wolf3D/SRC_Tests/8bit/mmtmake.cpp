#include "textures.h"

#include <grtypes.h>
#include <picture.h>

#include <iostream.h>
#include <malloc.h>
#include <mem.h>

#include <debug\debug.h>

int     BPP=8,BytesPP=1;

int     find(int r,int g,int b,RGBStruct* palette)
{
  int min=1024,iMin=0;
  for (int i=0;i<256;i++)
  {
    int d = abs(r-palette[i].red) + abs(g-palette[i].green) +
            abs(b-palette[i].blue);
    if (d<min)
    {
      min = d;
      iMin = i;
    }
  }
  return iMin;
}

int    main(int argc,char* argv[])
{
  cout<<"UNT Games Studio. Mip-map textures make utility v1.0"<<endl;
  if (argc<2)
  {
     cout<<"picture.bmp mipmap.mmt"<<endl;
     return 1;
  }
  cout<<"Mip-map utility: Loading source image: "<<argv[1]<<endl;
  const char* sourceFile = argv[1];
  const char* destinFile = argv[2];
  cPicture picture;
  if(!picture.load(sourceFile))
  {
     cout<<"Picture not open. Wrong path or not BMP image!"<<endl;
     return 1;
  }
  if (picture.bpp()!=8)
  {
    cout<<endl<<"Picture must be in 8-bit color"<<endl;
    return 1;
  }
  int   width  = picture.width(),
        height = picture.height();
  uchar*  list[100];
  uchar*     data = picture.GetImage()->data;
  RGBStruct* pal  = picture.GetPalette();
  list[0]         = (uchar*)malloc(width*height*BytesPP);
  memcpy(list[0],data,width*height*BytesPP);
  int w = width,
      h = height;
  for (int level = 1;w>4 && h>4;level++)
  {
    int pw              = w;
    w/=2;
    h/=2;
    uchar*      prev  = list[level-1];
    list[level]       = (uchar*)malloc(w*h*BytesPP);
    uchar*      tmp   = list[level];

    for (int j=0;j<h;j++,prev+=pw)
    {
        for (int i=0;i<w;i++,prev+=2,tmp++)
        {
          int   r,g,b;

          r     = ((int)pal[*prev].red+
                   (int)pal[*(prev+1)].red+
                   (int)pal[*(prev+pw)].red+
                   (int)pal[*(prev+pw+1)].red)/4;

          g     = ((int)pal[*prev].green+
                   (int)pal[*(prev+1)].green+
                   (int)pal[*(prev+pw)].green+
                   (int)pal[*(prev+pw+1)].green)/4;

          b     = ((int)pal[*prev].blue+
                   (int)pal[*(prev+1)].blue+
                   (int)pal[*(prev+pw)].blue+
                   (int)pal[*(prev+pw+1)].blue)/4;

          *tmp  = find (r,g,b,pal);
        }
    }
  }
  cout<<"Create Mip-Map-Texture (size = "<<level<<"): "<<destinFile;

  MMT* mmt = makeMMT (list,level,width,height);
  mmt->Save(destinFile);
  delete mmt;

  dout<<endl;

  for (int i=0;i<level;i++)
      free(list[i]);

  return 0;
}
