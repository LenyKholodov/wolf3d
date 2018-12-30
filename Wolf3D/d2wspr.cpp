#include "wad.h"
#include "sprite.h"
#include "palette.h"

#include <stdlib.h>
#include <debug.h>

//Debug   debug;

RGBStruct       palette   [256];
SimpleSprite*   faceTable [64][8];    //face : alpha
int             alphaSize [64];

WadFile*         wad = NULL;

char*   defaultWAD      = "d:\\games\\doom\\doom\\doom.wad";
char*   defaultMonstr   = "SARG";
char*   defaultFile     = "textures/sarg.wsf";

const   int z_color     = 0;
int         faceSize    = 0;

tImage*         drawSprite (const char*,int dir = 0) ;

int    rgb(int r,int g,int b)
{
  return ((r>>5)<<5)+((g>>5)<<2)+(b>>6);
}

void    main ( int argc, char* argv [])
{
  if (argc<3)
  {
    dout<<"<DOOM monstr name> <Wolf 3D:Final sprite file> <DOOM WAD file path>"<<endl;
    return ;
  }

  defaultMonstr = argv[1];
  defaultFile   = argv[2];
  defaultWAD    = argv[3];

  dout<<"Create DOOM sprite "<<defaultMonstr<<" in "<<defaultFile<<endl;

  wad = new WadFile (defaultWAD);

        //Загружаем палитру
  wad->loadPlayPal ();

  for ( int i = 0; i < 256; i++ )
  {
          palette [i].red   = playPal [0][i].red   ;
          palette [i].green = playPal [0][i].green ;
          palette [i].blue  = playPal [0][i].blue  ;
  }

        //Находим все имена
  char*  table [1024];

  dout<<"Search for monstr "<<defaultMonstr<<endl;

  int num = wad->findEntries (defaultMonstr,table);

  if (num == 0)
  {
    dout<<"Monstr not found"<<endl;
    delete wad;
    return ;
  }
  else dout<<"OK"<<endl;


  int firstPos = strlen (defaultMonstr);
  int faceNum  = 0;

        //Создаём плоскости

  for (i=0;i<num;i++)
  {
    int face   = table[i][firstPos]-'A';
    int alpha  = table[i][firstPos+1]-'1';

    if (alpha < 0)
    {
      alphaSize [face] = 1;
      alpha            = 0;
    }
    else
      alphaSize [face] = 8;

    if (faceSize < face+1) faceSize = face+1;

    tImage* image = drawSprite (table[i],0);

    faceTable [face][alpha] = makeSimpleSprite(image,z_color,1);

    free (image);

    if (isalnum(table[i][firstPos+2]))
    {
      face   = table[i][firstPos+2]-'A';
      alpha  = table[i][firstPos+3]-'1';

      if (alpha < 0)
      {
        alphaSize [face] = 1;
        alpha            = 0;
      }
      else
        alphaSize [face] = 8;

      if (faceSize < face+1) faceSize = face+1;

      tImage* image = drawSprite (table[i],1);

      faceTable [face][alpha] = makeSimpleSprite(image,z_color,1);

      free (image);
    }
  }

  for (i=0;i<num;i++) delete table[i];

        //Создаём фазы движения

  Sprite3D*  sprite3D [64];

  for (i=0;i<faceSize;i++)
  {
    sprite3D[i] = makeSprite3D (faceTable[i],alphaSize[i]);

    for (int j=0;j<alphaSize[i];j++)    delete faceTable [i][j];
  }

        //Создаём спрайт
  GameSprite3D* s3D = makeGameSprite3D (sprite3D,faceSize);

  dout<<"Create sprite (size = "<<s3D->size()<<")"
      <<" faces "<<faceSize<<endl;

  s3D->Save (defaultFile);

  delete s3D;

  for (i=0;i<faceSize;i++) delete sprite3D [i] ;

  delete wad;
}

tImage* drawSprite (const char* _name,int dir)
{
  tPic* pic              = wad->loadPic (_name);

  if (pic == NULL)
  {
     dout<<"Error load resource "<<_name<<endl;
     return NULL;
  }

  tImage* image = (tImage*)malloc (tImage::size(pic->width,pic->height,BPP_8bit)
                                   + 1024);

  if (image == NULL)
  {
    dout<<"No free memory"<<endl;
    return NULL;
  }

  image->width  = pic->width;
  image->height = pic->height;

  memset (image->data,z_color,pic->width*pic->height);

  PicColumn* column ;

  for (int x=0;x<pic->width;x++)
  {
      column            = (PicColumn*)(pic->data+pic->colOffsets[x]);

      char*      src    = column->pixels+column->row;
      uchar*     dest   = dir == 1 ? image->data+column->row*image->width+x :
                          image->data+column->row*image->width+pic->width-x-1;

      while (column->row != 0xFF)
      {
        for (int y=0;y<column->nonTransparentPixels;y++,src++,dest+=image->width)
        {
           *dest = rgb(palette[*src].red,
                       palette[*src].green,
                       palette[*src].blue
                      );
        }

        column            = (PicColumn*)((char*)column +
                                         column->nonTransparentPixels + 4);
      }
  }

  return image;
}