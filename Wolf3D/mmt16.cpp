#include <video.h>
#include <debug/debug.h>

#include "textures.h"

tImage* from24 (tImage* src);
tImage* from8  (tImage* src,RGBStruct*);  //no shift!
MMT*    mmt    (tImage*);
int     conv   (uchar*,int,int,int,uchar**);
int     open   (const char*,cPicture& dest);

void    rgb       (long color,int* red,int* green,int* blue);
long    rgb       (int r,int g,int b);
int     findRed   (long);
int     findGreen (long);
int     findBlue  (long);

int     main (int argc,char* argv [])
{
  dout<<"UNT MMT16 convertional utility"<<endl;
  if (argc < 3)
  {
    dout<<"mmt16.exe src.bmp dest.mmt"<<endl;
    return 1;
  }

  tImage*       image;
  cPicture      picture;
  int bytesPP = open (argv[1],picture);

  switch (bytesPP)
  {
    case 1: image  = from8  (picture.GetImage(),(RGBStruct*)picture.GetPalette()); break;
    case 2: image  = picture.GetImage(); break;
    case 3: image  = from24 (picture.GetImage()); break;
    default:
    {
      dout<<"Wrong BPP format "<<bytesPP<<endl;
      return 1;
    }
  }

  MMT*  texture = mmt (image);

  texture->Save (argv[2]);

  delete texture;
  free   (image);

  dout<<"Save OK"<<endl;

  return 0;
}

tImage*    from24 (tImage* src)
{
  int       len      = src->width*src->height;
  tImage*   dest     = (tImage*)malloc (len*2+0x100);
  uchar*    readPos  = src->data;
  uint16*   writePos = (uint16*)dest->data;

  for (int i=0;i<len;i++,readPos+=3,writePos++)
  {
    uchar red   = readPos[0],
          green = readPos[1],
          blue  = readPos[2];

    *writePos = ((blue>>3)<<11)+((green>>2)<<5)+(red>>3);
  }

  dest->width        = src->width;
  dest->height       = src->height;

  return dest;
}

tImage*   from8  (tImage* src,RGBStruct* pal)
{
  int       len      = src->width*src->height;
  tImage*   dest     = (tImage*)malloc (len*2+0x100);
  uchar*    readPos  = src->data;
  uint16*   writePos = (uint16*)dest->data;

  for (int i=0;i<len;i++,readPos++,writePos++)
  {
    int         red   = pal [*readPos].red   >> 2,
                green = pal [*readPos].green >> 2,
                blue  = pal [*readPos].blue  >> 2;

    *writePos = ((red>>1)<<11)+((green>>0)<<5)+(blue>>1);
  }

  dest->width        = src->width;
  dest->height       = src->height;

  return dest;
}

MMT*    mmt    (tImage* src)
{
  uchar* list [128];

  memcpy (list[0]=(uchar*)malloc(src->width*src->height*2+0x100),
          src->data,src->width*src->height*2);

  int level = conv (src->data,src->width,src->height,1,list);

  cout<<"Create Mip-Map-Texture (size = "<<level<<"): "<<endl;

  MMT* text = makeMMT (list,level,src->width,src->height,2);

  for (int i=0;i<level;i++)
  {
      free(list[i]);
  }

  return text;
}

int     conv   (uchar* src,int width,int height,int level,uchar** list)
{
  if (width <= 4 || height <= 4)
     return level;

  list [level] = (uchar*)malloc (width*height*2+0x100);

  uint16 *readPos  = (uint16*)src,
         *writePos = (uint16*)list[level];

  for (int y=0;y<height;y+=2,readPos+=width)
      for (int x=0;x<width;x+=2,readPos+=2,writePos++)
      {
          int   r,g,b;

          r     = (findRed (*readPos)      + findRed (*(readPos+1))+
                   findRed (*(readPos+width)) + findRed (*(readPos+width+1)))/4;

          g     = (findGreen (*readPos)      + findGreen (*(readPos+1))+
                   findGreen (*(readPos+width)) + findGreen (*(readPos+width+1)))/4;

          b     = (findBlue (*readPos)      + findBlue (*(readPos+1))+
                   findBlue (*(readPos+width)) + findBlue (*(readPos+width+1)))/4;

          *writePos  = rgb (r,g,b);
      }

  return conv (list[level],width>>1,height>>1,level+1,list); //??
}

int     open   (const char* name,cPicture& dest)
{
  if (!dest.load (name))
  {
    dout<<"MMT16: Wron file path '"<<name<<"'"<<endl;
    return 0;
  }

  return dest.bpp() < 8 ? dest.bpp() : dest.bpp()/8;
}

long    rgb       (int r,int g,int b)
{
  return ((b & 31)<<11)+((g & 63)<<5)+(r&31);
}

void    rgb    (long color,int* red,int* green,int* blue)
{
  *red   = color & 31;
  *green = (color>>5) & 63;
  *blue  = color>>11;
}

int     findRed   (long color)
{
  int r,g,b;
  rgb (color,&r,&g,&b);

  return r;
}

int     findGreen (long color)
{
  int r,g,b;
  rgb (color,&r,&g,&b);

  return g;
}

int     findBlue  (long color)
{
  int r,g,b;
  rgb (color,&r,&g,&b);

  return b;
}
