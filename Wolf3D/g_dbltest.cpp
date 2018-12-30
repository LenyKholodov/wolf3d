#include <mscreen.h>

#include "level.h"
#include "render.h"

#include "g_object.h"
#include "g_view.h"
#include "g_input.h"

#include "g_system.h"

class   PlayerClient: public GameObject
{
  public:
                                PlayerClient (Screen*,Level*);
                virtual         ~PlayerClient();
  
    virtual     void            Load (BinFile*)    {}
    virtual     void            Save (BinFile*)    {}
    virtual     BOOL            Exec (GameMessage*);
  
  protected:
             Render*            render;
             InputTranslator*   input;
             GameView*          view;

             ID                 player;             
};

class   DoubleGame
{
  friend class PlayerClient;
  public:
                        DoubleGame  (Screen* dest,const char* levelName);
                        DoubleGame  (Screen* dest,BinFile* saveGame);
            virtual     ~DoubleGame ();

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*)    {}
    virtual     void            Save (BinFile*)    {}
    virtual     BOOL            Exec (GameMessage*);

/////////////////////////////////////////////////////////////////////////////
/////////Воспроизведение звука
/////////////////////////////////////////////////////////////////////////////
                        void    PlaySound (int);

/////////////////////////////////////////////////////////////////////////////
/////////Отрисовка
/////////////////////////////////////////////////////////////////////////////
        virtual         void    DrawView ();

/////////////////////////////////////////////////////////////////////////////
/////////Главный цикл
/////////////////////////////////////////////////////////////////////////////
        virtual         void    MainLoop ();

  protected:
                        void    CreateEnv (Map*);

  protected:
             MidasSoundShell*   soundShell;
             GameSystem*        server;

             Level*             level;
             Screen*            screen; //double buffer

             BOOL               isContinue;
};

BOOL            PlayerClient::Exec (GameMessage* mes)
{
}

PlayerClient::PlayerClient (Screen* scr,Level* level)
{
  render = new Render(screen,FALSE);

  render->StagePtr() -> SetTexture  (level->GetTextCache());
  render->StagePtr() -> SetMap      (level->GetMap());
  render->StagePtr() -> SetLightMap (level->GetLightMap());
  render->StagePtr() -> SetCache    (level->GetSpriteCache());
  render->FloorPtr() -> SetTexture  (level->GetFloorText());
  render->SkyPtr()   -> SetTexture  (level->GetSkyText());

  render             -> Reset();

  int x = 1,y = 1;

  level->GetMap()->FindClear (x,y);

  CreatePlayer        create;

  create.x            = int2fixed(x);
  create.y            = int2fixed(y);
  create.angle        = 0;
  create.spriteIndex  = 2;

  CreateObjectMessage<CreatePlayer> createObj (id());

  createObj.type      = GAME_PLAYER;
  createObj.size      = sizeof(create);
  createObj.arg       = create;

  owner->PutMessage (&createObj,sizeof(createObj),id());
}

PlayerClient::~PlayerClient()
{
  if (render) delete render;
  if (view)   delete view;
  if (input)  delete input;
}

