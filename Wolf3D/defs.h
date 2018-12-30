#ifndef __MAIN_GAME_DEFINES__
#define __MAIN_GAME_DEFINES__

#include <types.h>
#include <fixed.h>

#include <video.h>

extern double NORM_FOCUS;
extern double NORM_VIEW_ANGLE;
extern double NORM_WALL_HEIGHT;
extern double NORM_DETAIL_ANGLE;
extern double NORM_WALL_FEEL;

const  int  MAX_VIS_SPRITES = 0x200;
const  int  NOSPRITE        = 255;
const  int  NOFACE          = 31;
extern long PACK_CACHE_SIZE;

const int  MAX_PIC_MIP     = 6;

/////////////////////////////////////////////////////////////////////////////
////Возможные графические движки
/////////////////////////////////////////////////////////////////////////////
enum    RenderEngine
{
                        //Stage (wall render)
        STAGE_SIMPLE                    = 1,
        STAGE_TEXTURE_8BIT,
        STAGE_SHADOW_TEXTURE_8BIT,
        STAGE_LIGHTMAP_TEXTURE_8BIT,

                        //Floor (floor render)

        FLOOR_SIMPLE_PLANE              = 1<<8,
        FLOOR_TEXTURE_8BIT              = 2<<8,
        FLOOR_SHADOW_8BIT               = 3<<8,

                        //Sky (sky render)
        SKY_SIMPLE_PLANE                = 1<<16,
        FLOOR_UP_TEXTURE_8BIT           = 2<<16,
        FLOOR_UP_SHADOW_8BIT            = 3<<16,
        SKY                             = 4<<16,
};

/////////////////////////////////////////////////////////////////////////////
////Структуры для описания запроса к классу Render
/////////////////////////////////////////////////////////////////////////////
struct  EngineParam
{
  RenderEngine  type;   //Тип движка
};

struct  RenderParam
{
        EngineParam*     sky;
        EngineParam*     floor;
        EngineParam*     stage;
};

const  int     DEFAULT_RENDER =  STAGE_LIGHTMAP_TEXTURE_8BIT |
                                  FLOOR_SHADOW_8BIT | SKY;

const  int       modes[]         = { 0x13, VESA640x480x8,VESA800x600x8,
                                VESA1024x768x8 };

const  int     picmipTable [2][MAX_PIC_MIP] =
{
  {
     STAGE_LIGHTMAP_TEXTURE_8BIT | FLOOR_SHADOW_8BIT  | FLOOR_UP_SHADOW_8BIT,
     STAGE_LIGHTMAP_TEXTURE_8BIT | FLOOR_TEXTURE_8BIT | FLOOR_UP_TEXTURE_8BIT,
     STAGE_SHADOW_TEXTURE_8BIT   | FLOOR_TEXTURE_8BIT | FLOOR_UP_TEXTURE_8BIT,
     STAGE_TEXTURE_8BIT          | FLOOR_TEXTURE_8BIT | SKY_SIMPLE_PLANE,
     STAGE_TEXTURE_8BIT          | FLOOR_SIMPLE_PLANE | SKY_SIMPLE_PLANE,
     STAGE_SIMPLE                | FLOOR_SIMPLE_PLANE | SKY_SIMPLE_PLANE
  },
  {
     STAGE_LIGHTMAP_TEXTURE_8BIT | FLOOR_SHADOW_8BIT  | SKY,
     STAGE_LIGHTMAP_TEXTURE_8BIT | FLOOR_TEXTURE_8BIT | SKY,
     STAGE_SHADOW_TEXTURE_8BIT   | FLOOR_TEXTURE_8BIT | SKY,
     STAGE_TEXTURE_8BIT          | FLOOR_TEXTURE_8BIT | SKY_SIMPLE_PLANE,
     STAGE_TEXTURE_8BIT          | FLOOR_SIMPLE_PLANE | SKY_SIMPLE_PLANE,
     STAGE_SIMPLE                | FLOOR_SIMPLE_PLANE | SKY_SIMPLE_PLANE
  },
};

#endif
