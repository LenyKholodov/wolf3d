#include <iostream.h>
#include <fstream.h>

#include <vesa.h>
#include <mscreen.h>
#include <fin.h>

#include "keyboard.h"
#include "render.h"
#include "level.h"

#include "palette.h"

#include "g_view.h"
#include "g_system.h"
#include "g_input.h"
#include "g_player.h"
#include "g_door.h"

#include <debug.h>

Angle     ROTATE_STEP       = float2angle(500.0/12000);
Fixed     WALK_STEP         = float2fixed(500.0/6000.0);
Fixed     STRAFE_STEP       = float2fixed(WALK_STEP/4);
int       modes[]           = { 0x13, VESA640x480x8,VESA800x600x8,
                                VESA1024x768x8 };

int       curModePos        = 1;
int       mode              = modes[curModePos];


float           xscale      = 1;
float           yscale      = 1;

Level*          level;
Map*            map;
LightMap*       lMap;
Render*         draw  = NULL;
Keyboard        kb;

VESAdevice      vesa;
Palette*        palette = NULL;

TextCache*      wtf;
SpriteCache*    wsc;
MMT*            floorT, *skyT;

ID                testPlayer;
GameView*         testRender;
GameSystem*       gameSystem;
InputTranslator*  gameInput;

Screen*         frame_screen = NULL;

void            CreateMP   ();
void            CreateInput ();
void            CreateTest  ();

void main()
{

 dout              = cout;

 dout<<"Wolfenstein 3D: Final"<<endl<<"Copyright by UNT Games Studio 2001"<<endl;

 dout<<"Wolf 3D: Load level "<<endl;

 level = LevelCompile::LoadLevel ("maps/wolf3d.lev",12,12);

// font     = level->GetBinFont(0);
// bgiFont  = level->GetBGIFont(0);
 wtf      = level->GetTextCache();
 wsc      = level->GetSpriteCache();
 floorT   = level->GetFloorText();
 skyT     = level->GetSkyText();
 map      = level->GetMap();
 lMap     = level->GetLightMap();

 lMap->SetMap (map);

 dout<<"Init VESA..."<<endl;

 if (vesa.open() != VESAdevice::OK)
 {
   dout<<"Error open VESA mode"<<endl;
   return;
 }

 dout<<"VESA open OK"<<endl;
 dout<<"Init graph mode..."<<endl;

 if (vesa.SetMode(mode) != VESAdevice::OK)
 {
   dout<<"Error open VESA mode "<<hex<<mode<<dec<<endl;
   return;
 }

 dout<<"Init graph mode OK"<<endl;

 gameSystem = new GameSystem (map,MAX_SLOTS*10,0xFFFF);

 CreateMP();
 CreateTest();
 CreateInput ();

 frame_screen = GimmeMirror  (vesa.CurrentScreen());

 draw       = new Render     (frame_screen,FALSE);

 draw->StagePtr()->SetTexture(wtf);
 draw->StagePtr()->SetMap(map);
 draw->StagePtr()->SetLightMap(lMap);
 draw->StagePtr()->SetCache(wsc);
 draw->FloorPtr()->SetTexture(floorT);
 draw->SkyPtr()->SetTexture(skyT);

 draw->Reset();

 testRender = new (TEST_OBJECT) GameView   (draw,testPlayer);

 dout<<"GO"<<endl;

 while (!kb.IsPress (KEY_ESC))
 {
       Flag in ("MAIN LOOP IN",TRUE);
       gameSystem->tick();
       frame_screen->refresh();
       in = FALSE;
 }

 delete testRender;
 delete gameInput;
 delete draw;
 delete frame_screen;
 delete gameSystem;
}

void    CreateMP ()
{
  CreatePlayer        create;

  create.x              = FX_ONE<<2;
  create.y              = FX_ONE<<2;
  create.angle          = 0;
  create.spriteIndex    = 2;

  testPlayer = gameSystem->CreateObject(GAME_PLAYER,&create,sizeof(create));

//  gameSystem->SetMainPlayer ( testPlayer );
}

void    CreateInput ()
{
  gameInput = new (INPUT_TRANSLATOR) InputTranslator (testPlayer);
}

void            CreateTest  ()
{
  CreateDoor create;

  create.x      = 8;
  create.y      = 2;
  create.side   = EAST;
  create.textId = 4;

  ID id         = gameSystem->CreateObject (DOOR,&create,sizeof(create));

  dout<<"Create door: ("<<id.type<<","<<id.id<<")"<<endl;

  //!!

//  TrigerPressMessage press (EAST);

//  gameSystem->PutMessage (&press,id,sizeof(press));
}