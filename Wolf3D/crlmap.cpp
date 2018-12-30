#include "map.h"
#include "light.h"

#include <iostream.h>

int     main(int argc,char* argv[])
{
  cout<<"UNT Games Studio. Light Map make utility"<<endl;
  if (argc<4)
  {
    cout<<"stage.map bkGround dest.lmf"<<endl;
    return 1;
  }

  Map*  map = new Map (argv[1]);

  if (map == NULL)
  {
    cout<<"Error open map file"<<endl;
    return 1;
  }

  LightMap* lMap = new LightMap (map);

//  lMap->reset(MAX_LIGHT_LEVELS-5);
  lMap->reset(atoi(argv[2]));
  lMap->recalc();

  cout<<"Save maps...";

  lMap->Save(argv[3]);
  map->Save(argv[1]);

  cout<<"OK"<<endl;

  delete lMap;
  delete map;

  return 0;
}