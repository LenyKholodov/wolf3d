#include <sbinfile.h>
#include <mbinfile.h>

#include <iostream.h>

#include "light.h"
#include "map.h"

//memBinFile      file;
stdBinFile      source;

BOOL    CreateMap (BinFile*,const char*);

int     main(int argc,char* argv[])
{
  cout<<"UNT Games Studio. Map make utility"<<endl;

  if (argc<3)
  {
    cout<<"source.txt dest.map"<<endl;
    return 1;
  }
  if (!source.open (argv[1],BinFile::OPEN_RO))
  {
    cout<<"File not found: "<<argv[1]<<endl;
    return 1;
  }

//  file.AssignFrom (source);

  if (!CreateMap(&source,argv[2]))
  {
    cout<<"Error in source file"<<endl;
    return 1;
  }

//  file.close();
  source.close();
  return 0;
}

BOOL    CreateMap (BinFile* fPtr,const char* name)
{
  if (fPtr == NULL)
     return FALSE;

  fPtr->reset();

  Map*          map  = Map::CreateMap();
  Block         bl;
  char          simb;

  for (int y=0;y<MAP_HEIGHT;y++)
      for (int x=0;x<MAP_WIDTH;x++)
      {
          if (fPtr->eof())
             break;

          simb = fPtr->getc();

          while (!fPtr->eof() && (simb == 10 || simb == 13)) //alnum
                simb = fPtr->getc();

          switch(simb)
          {
            case '*':
                         bl.type         = Map::WALL;
                         bl.cross        = 0;

                         bl.id.wall.id_N = 10; //0
                         bl.id.wall.id_E = 10; //1
                         bl.id.wall.id_S = 10; //2
                         bl.id.wall.id_W = 10; //3

                         map->SetBlock (Index(y,x),bl);

                         break;
            case 'D':
                         bl.type  = Map::DOOR; //door
                         bl.id.id = 4;         //!!
                         bl.cross = 0;
                         map->SetBlock (Index(y,x),bl);

                         break;
            case 'L':
                         bl.type  = Map::LIGHT;
                         bl.id.id = 0;
                         bl.cross = 1;
                         map->SetBlock (Index(y,x),bl);

                         break;
            case 'S':
                         bl.type              = Map::SPRITE; //sprite
                         bl.id.sprite.alpha   = 0;
                         bl.id.sprite.already = 0;
                         bl.id.sprite.id      = 2;
                         bl.id.sprite.face    = 0;
                         bl.id.sprite.xfrac   = 0xFFFF>>13;  //half
                         bl.id.sprite.yfrac   = 0xFFFF>>13;  //half
                         bl.id.sprite.z       = SPRITE_CENTER;
                         bl.cross = 1;
                         map->SetBlock (Index(y,x),bl);

                         break;

            case 'M':
                         bl.type              = Map::MONSTR; //sprite
                         bl.id.sprite.alpha   = 0;
                         bl.id.sprite.already = 0;
                         bl.id.sprite.id      = random(7);
                         bl.id.sprite.face    = 0;
                         bl.id.sprite.xfrac   = 0xFFFF>>13;  //half
                         bl.id.sprite.yfrac   = 0xFFFF>>13;  //half
                         bl.id.sprite.z       = SPRITE_CENTER;
                         bl.cross = 1;
                         map->SetBlock (Index(y,x),bl);

                         break;

            default:     bl.type = Map::FREE;
                         bl.id.id = 0;
                         bl.cross = 1;
                         map->SetBlock (Index(y,x),bl);

                         break;
          }
      }

  cout<<"Save new map: "<<name<<endl;

  map->Save(name);

  delete map;

  return TRUE;
}
