#include <binfile.h>
#include <grtypes.h>
//#include "memory.h"

const int  SPRITE_SIZE       =  40000;
const int  MAX_SPRITES       =  16;
const int  MAX_SPRITE_WIDTH  =  256;
const int  MAX_SPRITE_HEIGHT =  256;

//Pool*      wsprites_pool;   //pool of sprites

struct  WSFHeader
{
  uchar         id[4];
  uchar         reserv[12];
};

class  SimpleSprite
{
  public:
    int           width;
    int           height;
    int           z_color;
    uchar         data[1];

                  SimpleSprite (tImage*,int z=0);

    long          size (int bpp);

    void*         operator           new        (size_t,size_t);
    void*         operator           new        (size_t,void* x) { return x; }
    void          operator           delete     (void*);
};

class  Sprite3D
{
  public:
    int           num;
    int           bpp;
    uchar         data[1];

                  Sprite3D  (tImageList*,int z=0);
                  Sprite3D  (SimpleSprite**,int,int bitpp=8);

    void*         operator           new        (size_t,size_t);
    void*         operator           new        (size_t,void* x) { return x; }
    void          operator           delete     (void*);
};

