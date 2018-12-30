#include <grtypes.h>
#include <picture.h>

#include <iostream.h>
#include <malloc.h>
#include <mem.h>

#include <debug.h>

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

void    main(int argc,char* argv[])
{
  if (argc<2)
  {
     cout<<"picture.bmp mipmap.ilc"<<endl;
     return;
  }
  cout<<"Mip-map utility: Loading source image... ";
  const char* sourceFile = argv[1];//1
  const char* destinFile = argv[2];//2
  cPicture picture;
  if(!picture.load(sourceFile))
  {
     cout<<endl<<"Picture not open. Wrong path or not BMP image!"<<endl;
     return;
  }
  if (picture.bpp()!=8)
  {
    cout<<endl<<"Picture must be in 8-bit color"<<endl;
    return;
  }
  int   width  = picture.width(),
        height = picture.height();
  cout<<"OK"<<endl;
  cout<<"Picture:"<<endl;
  cout<<"\t\tWidth           = "<<width<<endl;
  cout<<"\t\tHeight          = "<<height<<endl;
  cout<<"\t\tBytes Per Pixel = "<<BytesPP<<endl;
  tImage* list[100];
  tImage* data    = picture.GetImage();
  RGBStruct* pal  = picture.GetPalette();
  list[0]         = (tImage*)malloc(width*height+sizeof(tImage)-1);
  list[0]->width  = width;
  list[0]->height = height;
  memcpy(list[0],data,width*height);
  int w = width,
      h = height;
  cout<<"Mip-map utility: Create new image #0"<<":\t";
  cout<<"Width  = "<<w;
  cout<<"\tHeight = "<<h<<endl;
  for (int level = 1;w>16 && h>16;level++)
  {
    int pw              = w;
    w/=2;
    h/=2;
    cout<<"Mip-map utility: Create new image #"<<level<<":\t";
    cout<<"Width  = "<<w;
    cout<<"\tHeight = "<<h;
    cout<<endl;
    uchar*      prev  = list[level-1]->data;
    list[level]       = (tImage*)malloc(w*h+sizeof(tImage)-1);
    list[level]->width  = w;
    list[level]->height = h;
    uchar*      tmp   = list[level]->data;

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
  cout<<"Mip-map utility: Create image list  ...";
  RGBPalette* palette = RGBPalette::create(0,256);
  memcpy(palette->palette,pal,256*sizeof(RGBStruct));
  tImageList* ilc     = tImageList::create(level,BPP,list,palette);
  cout<<" OK"<<endl;
  cout<<"Mip-map utility: Total size is "<<level<<endl;
  cout<<"Mip-map utility: Save list "<<destinFile<<" ...";
  ilc->save(destinFile);
  cout<<" OK"<<endl;
  cout<<"Mip-map utility: Free memory ...";
  for (int i=0;i<level;i++)
      free(list[i]);
  cout<<" OK"<<endl;
}
