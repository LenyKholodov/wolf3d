#ifndef __WOLF3D_MAP__
#define __WOLF3D_MAP__

#include <types.h>
#include <binfile.h>

#include "fixed.h"

#include <iostream.h>
#include <types.h>

#include <debug\debug.h>

        //Продумать заголовок файла

const int MAP_WIDTH     = 64;
const int MAP_HEIGHT    = 64;
const int MAX_X_TILE    = MAP_WIDTH-1;
const int MAX_Y_TILE    = MAP_HEIGHT-1;
const int MAP_SHIFT     = 6;
const int OUT_OF_MAP    = 128;

///////////////////////////////////////////////////////////////////////////
////Считается, что карта имеет размер 64x64
///////////////////////////////////////////////////////////////////////////
inline uint Index (uint y,uint x)
{
  return (y<<6)+x;   //Y*64+X
}

/////////////////////////////////////////////////////////////////////////////
/////////Стороны света
/////////////////////////////////////////////////////////////////////////////
typedef uchar   Side;

enum    Sides
{
  NORTH = 0,
  EAST,
  SOURTH,
  WEST
};

enum    ZSpriteCorrection
{
  SPRITE_TOP = 0,
  SPRITE_CENTER,
  SPRITE_BOTTOM
};

Side    AngleToSide (Angle);    //4 side

/////////////////////////////////////////////////////////////////////////////
///////Клетка, единица - вобщем единица карты (рендер карта)
/////////////////////////////////////////////////////////////////////////////

#pragma pack(1)
struct      Block
{
   uint            type    : 3 ;        //4
   uint            cross   : 1 ;
   union _ID
   {
      struct Door
      {
        uint          id    : 7;
        uint          state : 21;
      }door;
      struct Wall
      {
        uint  id_N            : 7;
        uint  id_E            : 7;
        uint  id_S            : 7;
        uint  id_W            : 7;
      }wall;
      struct Sprite
      {
        uint    alpha         : 3;      //8 sectors
        uint    already       : 1;      //1 bit
        uint    id            : 8;      //максимальное кол-во спрайтов 256
        uint    face          : 5;      //фаза max = 32
        uint    xfrac         : 4;
        uint    yfrac         : 4;
        uint    z             : 2;
      }sprite;
      uint            id      : 28;
   }id;
};
#pragma pack(0)

/////////////////////////////////////////////////////////////////////////////
////Карта для графического движка
/////////////////////////////////////////////////////////////////////////////
class   Map
{
  private:
           Block*       map;                    //Stage map
           Block        error;                  //Error block out of map
  protected:
  public:
                //ВСЁ!!!!!
           enum BlockType{
                FREE = 0,
                WALL,
                MONSTR,
                DOOR,
                SPRITE,
                PORTAL,
                LIGHT,
                TRIGER
           };
          Map(): map(NULL)                    {  }
          Map(Block* new_map): map(new_map)   {  }
          Map(Map* m):       map(m->map)      {  }
          Map(BinFile* file): map(NULL)       { Load(file); }
          Map(const char* path): map(NULL)    { Load(path); }
          virtual ~Map() { delete map; }

          void  operator = (Map* m) { map = m->map; }

///////////////////////////////////////////////////////////////////////////
/////Загружает карту
///////////////////////////////////////////////////////////////////////////
   static Map*          CreateMap();

          BOOL          Load(BinFile*);
          BOOL          Load(const char*);
          void          Save(BinFile*);
          void          Save(const char*);

///////////////////////////////////////////////////////////////////////////
/////Работа с характеристиками клетки
///////////////////////////////////////////////////////////////////////////
          Block*        GetBlock(uint index);
          int           GetType(uint index)    { return GetBlock(index)->type; }
          BOOL          IsFree(uint index)     { return GetType(index) == FREE; }
          BOOL          IsDoor(uint index)     { return GetType(index) == DOOR;}
          BOOL          IsPortal(uint index)   { return GetType(index) == PORTAL;}
          BOOL          IsMonstr(uint index)   { return GetType(index) == MONSTR;}
          BOOL          IsWall(uint index)     { return GetType(index) == WALL;}
          BOOL          IsSprite(uint index)   { return GetType(index) == SPRITE;}
          BOOL          IsCross(uint index);
          uint          FindClear(uint& x,uint& y);

///////////////////////////////////////////////////////////////////////////
/////Изменение состояния клетки
///////////////////////////////////////////////////////////////////////////
          void          SetBlock(uint index,Block block);
          Block*        GetMap() { return map; }

};

#endif