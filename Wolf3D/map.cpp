#include <cbinfile.h>
#include <sbinfile.h>

#include "map.h"

#include <mem.h>
#include <debug\debug.h>

Map*    Map::CreateMap()
{
    dout<<"Map: Create new map..."<<endl;

    Map*    map = new Map;
    map->map    = new Block[MAP_HEIGHT*MAP_WIDTH];

    map->error.type  = Map::WALL;
    map->error.cross = 0;
    map->error.id.wall.id_N = 0;
    map->error.id.wall.id_W = 0;
    map->error.id.wall.id_E = 0;
    map->error.id.wall.id_S = 0;

    memset(map->map,0,MAP_WIDTH*MAP_HEIGHT*sizeof(Block));

    return map;
}

void  Map::SetBlock(uint index,Block block)
{
  #ifndef NDEBUG
          if (index>MAP_WIDTH*MAP_HEIGHT)
          {
             dout<<"Map: Index = "<<(int)index<<endl;
//             D_Error("Map: Array bound error");
             return;
          }
  #endif
  map[index] = block;
}

Block* Map::GetBlock(uint index)
{
  if (index>MAP_HEIGHT*MAP_WIDTH)
  {
    return  &error;
  }

  return &map[index];
}

BOOL    Map::Load(BinFile* file)
{
  if (file == NULL)
     return FALSE;
  if (file->length()<MAP_HEIGHT*MAP_WIDTH*sizeof(Block))
     return FALSE;
  dout<<"Map: All right begin loading..."<<endl;
  if (!map)
  {
     dout<<"Map: First initialization. Memory alloc..."<<endl;
     map = new Block[MAP_WIDTH*MAP_HEIGHT];
  }
  dout<<"Map: Now loading data of this file..."<<endl;

  file->read(map,MAP_HEIGHT*MAP_WIDTH*sizeof(Block));
  file->read(&error,sizeof (Block));

  return TRUE;
}

BOOL    Map::Load(const char* path)
{
  stdBinFile f;

  if (!f.open(path,BinFile::OPEN_RO))
  {
    dout<<"Map: File not found "<<path<<endl;
    return FALSE;
  }

  BOOL status=Load(&f);

  f.close();

  return status;
}

void    Map::Save(BinFile* file)
{
  if (file == NULL)
     return;
//  file->reset();
  file->write(map,MAP_HEIGHT*MAP_WIDTH*sizeof(Block));
  file->write(&error,sizeof(Block));
}

void    Map::Save(const char* path)
{
  stdBinFile f;
  f.open(path,BinFile::OPEN_RW);
  Save(&f);
  f.close();
}

uint    Map::FindClear(uint& x,uint& y)
{
  uint xstart = x;
  for (;y<MAX_Y_TILE;y++)
      for (x=xstart;x<MAX_X_TILE;x++)
          if (map[Index(y,x)].type == Map::FREE)
             return Index(y,x);
  return -1;
}

BOOL    Map::IsCross (uint index)
{
  if (index > MAP_WIDTH*MAP_HEIGHT)
     return FALSE;

  return GetBlock(index)->cross;
}

Side    AngleToSide (Angle angle)
{
  Side side;

  if      (angle<=ANGLE_90)                     side = NORTH;
  else if (angle>ANGLE_90  && angle<=ANGLE_180) side = EAST;
  else if (angle>ANGLE_180 && angle<=ANGLE_270) side = SOURTH;
  else                                          side = WEST;

  return  side;
}

