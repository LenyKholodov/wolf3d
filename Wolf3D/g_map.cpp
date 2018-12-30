#include "g_map.h"
#include "g_system.h"

#include <debug\debug.h>

void*   GameMap::operator new (size_t bytes ,GameSystem* owner)
{
  void* _dest = ::operator new (bytes);

  if (_dest == NULL)
     return NULL;

  GameMap* map = (GameMap*)_dest;

  map->owner   = owner;

  return _dest;
}

void    GameMap::Load (BinFile* file)
{
  GameObject::Load (file);
//  ClearAll ();

  file->read (mMap,sizeof(ID)*MAP_WIDTH*MAP_HEIGHT);
}

void    GameMap::Save (BinFile* file)
{
  GameObject::Save (file);

  file->write (mMap,sizeof(ID)*MAP_WIDTH*MAP_HEIGHT);
}

void    GameMap::dump (ostream& os) const
{
  os<<"This is Game Map ("<<hex<<(void*)this<<dec<<")"<<endl;
}

void    GameMap::PutObject (uint index,ID id)
{
//#ifndef NDEBUG
  if (index>MAP_WIDTH*MAP_HEIGHT)
     return;
//#endif
  mMap [index] = id;
}

ID      GameMap::GetObject (uint index)
{
//#ifndef NDEBUG
  if (index>MAP_WIDTH*MAP_HEIGHT)
     return ID(-1,-1);
//#endif
  return mMap [index];
}

void    GameMap::SendMessage (uint index,GameMessage* mes,int size)
{
  owner->PutMessage (mes,GetObject(index),size);
}

void    GameMap::ClearAll ()
{
  for (int y = 0;y<MAP_HEIGHT;y++)
      for (int x = 0;x<MAP_WIDTH;x++)
          mMap [Index (y,x)] = ID(-1,-1);
}

GameMap::GameMap ()
        :mMap(new ID [MAP_WIDTH*MAP_HEIGHT])
{
  ClearAll ();
}

GameMap::~GameMap ()
{
  if (mMap)
     delete mMap;
}

//                        GameMap (Map*);
