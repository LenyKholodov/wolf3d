#include "g_player.h"
#include "g_system.h"
#include "g_view.h"
#include "map.h"

#include <keyboard.h>
#include <debug.h>

GameSystem*     gameSystem     = NULL;
ID              systemDispatch = ID(-1,-1);
ID              testPlayer;

Map*            map            = new Map ("maps/wolf3d.map");

void            CreateMP   ();
void            UpdateInfo ();

void    main()
{
  Keyboard        kb;

  gameSystem = new GameSystem (map,MAX_SLOTS*10,0xFFFF);

  CreateMP();

  dout<<"Поехали!!!"<<endl;

  while (!kb.IsPress ( KEY_ESC ) )
    gameSystem->tick();

  delete gameSystem;
}


void    CreateMP ()
{
  CreatePlayer        create;

  create.x              = FX_ONE<<2;
  create.y              = FX_ONE<<2;
  create.angle          = 0;
  create.spriteIndex    = 0;

  testPlayer = gameSystem->CreateObject(GAME_PLAYER,&create,sizeof(create));

  gameSystem->SetMainPlayer ( testPlayer );
}

