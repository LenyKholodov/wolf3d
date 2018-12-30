#include "g_single.h"
#include "g_single.h"
#include "g_user.h"
#include "t_wplr.h"

#include "g_door.h"

#include "cross.h"
#include "fail.h"
#include "g_mac.h"

#include <keyboard.h>
#include <vesa.h>
#include <klip.h>
#include <string.h>
#include <sbinfile.h>
#include <cbinfile.h>

#include "fin.h"
#include <debug\debug.h>

////////////////implementation of VESAdevice down class//////////////////////
class _VESA: public VESAdevice
{
  public:
          static int    getmode () { return mCurrentMode; }
};

////////////////implementation of class SingleGameClient/////////////////////

class   SingleGameClient: public GameObject
{
  public:
                                SingleGameClient (ArgPtr) {}
                                SingleGameClient (BinFile*) { }
                                SingleGameClient (SingleGame* own)
                                   : game (own)
                                {  }
                                SingleGameClient (SingleGame* own,BinFile*)
                                   :game (own)
                                {    }
             virtual            ~SingleGameClient ()
                                {    }

                void            SetSystem (SingleGame* own) { game = own; }

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile* fPtr)    { dout<<"HERE~!!!"<<endl;}
    virtual     void            Save (BinFile* fPtr)    { }
    virtual     BOOL            Exec (GameMessage* mes) { return game->Exec (mes); }

    virtual     GameObjects     type () const  { return game->type(); }
    virtual     void            dump (ostream& os) const { game->dump (os); }

  protected:
                SingleGame*     game;
};

make<SingleGameClient,SINGLE_GAME>      makeSingleGame;

//////////////////implementation of class SingleGame/////////////////////////

void            SingleGame::Load (const char* name)
{
  cacheBinFile      cache;
  stdBinFile    file;

  if (!file.open (name,BinFile::OPEN_RO))
  {
    dout<<"Single game: File not found "<<name<<" Load failed"<<endl;
    return;
  }

  if (cache.open (&file,DEFAULT_CACHE_SIZE))
  {
     Load (&cache);
     cache.close();
  }
  else
     Load (&file);

  file.close();
}

void            SingleGame::Save (const char* name)
{
  if (isError)
  {
    grout<<"Can not save game in error mode!"<<endl;
    return;
  }

  cacheBinFile      cache;
  stdBinFile    file;

  if (!file.open (name,BinFile::OPEN_CR))       //RW
  {
    dout<<"Single game: File not found "<<name<<" Save failed"<<endl;
    return;
  }

  if (cache.open (&file,DEFAULT_CACHE_SIZE))
  {
     Save (&cache);
     cache.close();
  }
  else
     Save (&file);

  file.close();

}

void            SingleGame::Load (BinFile* file)
{
  dout<<"Single game: Load game"<<endl;

  SingleGameTag tag;

  file->read (&tag,sizeof(tag));

  if (strcmp (tag.levelFile,level->GetLevelFile ()))// && server->GetLevel()==level)
  {
    level    = LevelCompile::ReloadLevel (tag.levelFile,level);

    if (level == NULL)
    {
      dout<<"Single game: Error open level"<<tag.levelFile<<endl;
      grout<<"Error open level"<<tag.levelFile<<endl;

      ErrorLoop();
      return ;
    }

    video->refresh(level);
    InitSound();
  }

  server->Load (file,level);

        //Создаём окружающее управление

  isPause   = tag.isPause;
  player    = tag.player;
  fragCount = tag.fragCount;
  client    = (SingleGameClient*)server->GetObject(tag.face);
  client->SetSystem(this);

  view      = new (GAME_VIEW,client->id(),server)    //client->id
                     GameView        (video,player);

  input     = new (INPUT_TRANSLATOR,client->id(),server)
                     InputTranslator (player);

  if (server->GetObject(client->id())->type () != SINGLE_GAME)
  {
     dout<<server->GetObject(client->id())->id ()<<endl;
     D_Error ("Fuck")
  }

        //Устанавливаем его атрибуты

  server->AddAttr (player,OBJ_UNLOAD_PROTECT);
  server->AddAttr (client->id(),OBJ_UNLOAD_PROTECT);
  server->AddAttr (view->id(),OBJ_UNLOAD_PROTECT);
  server->AddAttr (input->id(),OBJ_UNLOAD_PROTECT);

        //Отменяем консольный режим

  video->OnlyConsole(FALSE);
  input->OnlyConsole(FALSE);

        //Refresh для главного игрока

  UserRefreshMessage refrUser;

  server->PutMessage (&refrUser,player,sizeof(refrUser));

        //refresh for this

  ReloadSingleOk rel;

  server->PutMessage (&rel,id(),sizeof(rel));

  dout<<"Load savegame OK!"<<endl;

  isError = FALSE;
}

void            SingleGame::Save (BinFile* file)
{
  dout<<"Single game: Save game"<<endl;

  SingleGameTag         tag;

  tag.player    = player;
  tag.face      = client->id();
  tag.isPause   = isPause;
  tag.fragCount = fragCount;

  strcpy (tag.levelFile,level->GetLevelFile());

  file->write (&tag,sizeof (tag) );

  server->Save (file);
}

BOOL            SingleGame::Exec (GameMessage* mes)
{
  Flag  flag ("SINGLE_EXEC");

  switch (mes->id)
  {
    case LOAD_GAME:
    {
      GameLoadMessage* message = (GameLoadMessage*)mes;

      Load (message->name);

      return TRUE;
    }
    case SAVE_GAME:
    {
      GameSaveMessage* message = (GameSaveMessage*)mes;

      Save (message->name);

      return TRUE;
    }
    case CHANGE_LEVEL:
    {
      if (mes->callBack != id())
      {
        PortalChangeLevelMessage* message = (PortalChangeLevelMessage*)mes;

        LoadLevel(message->levelName,message->x,message->y);

        message->callBack = id();

        server->Sequence (message,message->size);
      }

      return TRUE;
    }
    case SEQUENCE_MAP_PTR:
    {
      Flag      flag1 ("SEQUENCE_MAP_PTR");

      SequenceMapPtrMessage*   message = (SequenceMapPtrMessage*)mes;

      if (message->map == level->GetMap())
         return TRUE;
      else
         memcpy (level->GetMap(),message->map,MAP_WIDTH*MAP_HEIGHT*sizeof(Block));

      return TRUE;
    }
    case SEQUENCE_OBJECT:
    {
      SequenceObjectMessage* message = (SequenceObjectMessage*)mes;

      if (server->GetMap() == level->GetMap())
         return TRUE;

      level->GetMap()->SetBlock (message->mapIndex,message->block);

      return TRUE;
    }
    case RELOAD_SINGLE:
    {
      if (isPause)
         PauseLoop();

      return TRUE;
    }
    case TICK:
    {
      Flag      flag1 ("SINGLE_TICK");

      happy_blood ();

      return TRUE;
    }
    case PLAYER_DEATH:
    {
      PlayerDeath* message = (PlayerDeath*)mes;

      dout<<"You are killed by "<<message->beach<<endl;

      RespawnPoint* respawn = level->GetRespawn ();
      int           index   = random (level->GetRespawnCount());

      fragCount             = ((UserPlayer*)server->GetObject (player))->GetFrags();

      CreateMainPlayer (DEFAULT_PLAYER,respawn[index].x,respawn[index].y);

      input->player (ID(-1,-1));
      view->player  (ID(-1,-1));

      return TRUE;
    }
    case INPUT_KEY_UP: return TRUE;
    case INPUT_KEY_DOWN:
    {
      KeyDownMessage*  message =  (KeyDownMessage*)mes;

      if (isError && message->key == KEY_TILDA)
         return TRUE;
      else if (message->key == KEY_TILDA)
      {
        PlayerResetActiveMessage reset;
        server->PutMessage (&reset,player,sizeof(reset));
      }

      video->putc (message->ascii,message->key);

      return TRUE;
    }
    case PAUSE_SINGLE_GAME:
    {
      if (isError)
      {
         grout<<"Can not go in console mode in error mode!"<<endl;
         return TRUE;
      }

      PauseSingleGameMessage* message = (PauseSingleGameMessage*)mes;

      if (isPause && !message->state)
      {
         isPause = FALSE;
         dout<<"Pause game OFF"<<endl;

         ViewChangeStrItemMessage change (IM_CENTER_MES,"");
         PlayerResetActiveMessage reset;

         view->Exec (&change);
         server->PutMessage (&reset,player,sizeof(reset));
      }
      else if (!isPause && message->state)
      {
        dout<<"Pause game ON"<<endl;

        ViewChangeStrItemMessage change (IM_CENTER_MES,"Pause");

        view->Exec (&change);

        PauseLoop();
      }

      return TRUE;
    }
    case ALREADY_CREATE:
    {
      AlreadyCreateMessage* message = (AlreadyCreateMessage*)mes;

                //убрать когда сделаешь Input
      if (server->GetObject (message->obj)->type()!=USER_PLAYER)
         return TRUE;

      if (player != ID(-1,-1))
      {
        player  = message->obj;

        input->player (player);
        view->player (player);

        PlayerHeyMessage hey;

        server->PutMessage (&hey,player,sizeof(hey));
        server->AddAttr    (player,OBJ_UNLOAD_PROTECT);

        UserRefreshMessage refrUser;

        server->PutMessage (&refrUser,player,sizeof(refrUser));

        dout<<"Main player: "<<player
           <<" TYPE "<<server->GetObject(player)->type()<<endl;

        ((UserPlayer*)server->GetObject(player))->SetFrags (fragCount);

        return TRUE;
      }

      player  = message->obj;

      view    = new (GAME_VIEW,ID(-1,-1),server)        //client->id
                     GameView        (video,player);

      input   = new (INPUT_TRANSLATOR,client->id(),server) //client->id
                     InputTranslator (player);

         //Устанавливаем атрибуты окружения

      server->AddAttr (player,OBJ_UNLOAD_PROTECT);
      server->AddAttr (view->id(),OBJ_UNLOAD_PROTECT);
      server->AddAttr (input->id(),OBJ_UNLOAD_PROTECT);

      dout<<"Main player: "<<player
          <<" TYPE "<<server->GetObject(player)->type()<<endl;
      dout<<"Main view: "<<view->id()
          <<" TYPE "<<view->type()<<endl;
      dout<<"Main input: "<<input->id()
          <<" TYPE "<<input->type()<<endl;

      server->AddActive (client->id(),PR_CLIENT);
      server->AddClient (client->id());

      LoadAutoexec("config.cfg");

      return TRUE;
    }
    case ALREADY_DELETE:
    {
      AlreadyDeleteMessage * message = (AlreadyDeleteMessage *)mes;

      UserRefreshMessage refrUser;

      server->PutMessage (&refrUser,player,sizeof(refrUser));

      return TRUE;
    }
    case SFX_VOLUME:
    {
      SFXVolumeMessage* message = (SFXVolumeMessage*)mes;

      SfxVolume (message->volume);

      return TRUE;
    }
    case MUSIC_VOLUME:
    {
      MusicVolumeMessage* message = (MusicVolumeMessage*)mes;

      MusicVolume (message->volume);

      return TRUE;
    }
    case PLAY_SOUND:
    {
      PlaySoundMessage *message = (PlaySoundMessage *) mes;

      if (soundShell == NULL)
      {
         dout<<"Play sound #"<<message->sound<<endl;
         return TRUE;
      }

      Player* monkey = (Player*)server->GetObject(player);

      Fixed dist  = abs(int2fixed(message->x)-monkey->GetX())<<1;

      Angle angle = FindAngle (monkey->GetX(),monkey->GetY(),
                    int2fixed(message->x),int2fixed(message->y));

      dist        = 256 - fixed2int( abs( mul(dist,icos(angle)) ) ); //128
      dist        = (dist*iFace [IF_SFX_VOLUME])>>VOLUME_SHIFT;

      if (dist>MIN_VOLUME_LIMIT)
         soundShell->play (message->sound,
                           dist,
                           angle2float(angle-monkey->GetAngle())
                          );

      return TRUE;
    }
    case  PIC_MIP:
    case  CHANGE_SCREEN:
    case  MOVE_ITEM_TO:
    case  MOVE_ITEM_REL:
    case  MOVE_ITEM_START:
    case  RELOAD_ITEM_VIEW:
    case  CHANGE_ITEM_VIEW:
    {
      if (view) view->Exec (mes);
      return TRUE;
    }
    case ECHO_ON:      return TRUE;
    case I_SEE_YOU:    return TRUE;
    case LUCKY_SHOT:
    {
      PlayerLuckyShot* message = (PlayerLuckyShot*)mes;

      grout<<"You are kill monstr "<<message->dest<<endl;

      if (server && server->GetObject(player)->type()!=ERROR_OBJECT)
        server->GetObject(player)->Exec (mes);

      return TRUE;
    }
    case SLEEP_OFF:    {  return TRUE;    }
    default:
    {
      dout<<"Single game: Wrong message: "<<mes->id<<endl;
      return TRUE;
    }
  }
}

void            SingleGame::dump (ostream& os) const
{
  os<<"Single game; main player "<<player<<endl;
}

void            SingleGame::MainLoop ()
{
  while (isContinue)
    server->tick();
}

void            SingleGame::PauseLoop ()
{
  if (!input || !view || !server)
     return;

  isPause = TRUE;

  input->pause (TRUE);
  server->Pause (id(),TRUE);

  while (isPause)
  {
    input->Translate ();
    view->Redraw();
  }

  input->pause (FALSE);
  server->Pause (ID(-1,-1),FALSE);
}

void            SingleGame::ErrorLoop ()
{
  if (!input || !view || !server)
     return;    //delete fin;

  isError = TRUE;

  input->OnlyConsole(TRUE);
  video->OnlyConsole(TRUE);
  server->Pause (id(),TRUE);

  while (isError)
  {
    input->Translate ();
    view->Redraw();
  }

  input->OnlyConsole(FALSE);
  video->OnlyConsole(FALSE);
  server->Pause (ID(-1,-1),FALSE);
}

void            SingleGame::happy_blood ()
{
  PlayerISeeYou message (player);
  uint          visible [MAX_VIS_SPRITES];
  ID            obj;
  Player*       monkey     = (Player*)server->GetObject (player);
  Player*       vis_monstr;

  int           count   = FindSprites (monkey->GetX(),
                                       monkey->GetY(),
                                       monkey->GetAngle(),
                                       visible,
                                       level->GetMap());

        //Happy New Year!

  for (int i=0;i<count;i++)
  {
      obj=server->GetObject (visible[i]);

      if (obj != ID(-1,-1) && obj != player)
      {
         vis_monstr = (Player*)server->GetObject(obj);
         Angle ang  =  FindAngle(vis_monstr->GetX(),
                                 vis_monstr->GetY(),
                                 monkey->GetX(),
                                 monkey->GetY())
                       -vis_monstr->GetAngle();

         if (ang>=ANGLE_270 || ang<=ANGLE_90)
         {
            server->PutMessage (&message,server->GetObject(visible[i]),sizeof(message));
         }
      }
  }
}

ID         SingleGame::id   ()         const
{
  return client ? client->id() : ID(-1,-1);
}

void    SingleGame::SfxVolume (int vol)
{
    iFace [IF_SFX_VOLUME] = vol &= MAX_VOLUME;

    for (int i=0;i<MAX_CHANNELS;i++)
        if (i!=iFace[IF_MUSIC_CHANNEL])
          soundShell->volume(i,(soundShell->volume(i)*vol)>>VOLUME_SHIFT);
}

void    SingleGame::MusicVolume (int vol)
{
    int i                   = iFace [IF_MUSIC_CHANNEL];
    iFace [IF_MUSIC_VOLUME] = vol;

    soundShell->volume(i,vol);
}

void       SingleGame::CreateMainPlayer (const char* name,int x,int y)
{
  CreateWeaponPlayer  create;
  ResourceHeader      header;

  create.param().x       = int2fixed(x);
  create.param().y       = int2fixed(y);
  create.param().angle   = 0;
  create.param().health  = PLAYER_HEALTH;
  create.param().protect = PLAYER_PROTECT;

  if (header.Load (name,&create.resourceClass) != GAME_MONSTR) //!!!
  {
    dout<<"Player: Wrong resource file format "<<name<<endl;
    delete fin;
  }

  CreateObjectMessage<CreateWeaponPlayer> createObj (client->id());

  createObj.type      = USER_PLAYER;
  createObj.size      = sizeof(create);
  createObj.arg       = create;

  server->PutMessage (&createObj,sizeof(createObj),client->id());
}

void    SingleGame::InitSound ()
{
  if (loadTable) loadTable->SetMessage (LM_CURRENT_LOAD,"Play back ground music");

  if (soundShell == NULL)
  {
    iFace [IF_SFX_VOLUME]    = DEF_SFX_VOLUME;
    iFace [IF_MUSIC_VOLUME]  = DEF_MUSIC_VOLUME;

    soundShell = level->GetSound()->CreateShell(MAX_CHANNELS);
  }

  if (soundShell == NULL)
    dout<<"Single game: Error open sound system"<<endl;
  else
  {
    for (int i=0;i<MAX_CHANNELS;i++)
    {
        soundShell->stop  (i);
        soundShell->volume(i,iFace[IF_SFX_VOLUME]);
    }

    if (MUSIC_PLAY)
      iFace [IF_MUSIC_CHANNEL] = soundShell->play (level->GetBkMusic(),
                                                 iFace[IF_MUSIC_VOLUME],0);
  }

  if (loadTable) loadTable->SetMessage (LM_CURRENT_LOAD,"Play game");
}

void    SingleGame::LoadLevel  (const char* levelName,int x,int y)
{
  Flag flag ("LOAD LEVEL",TRUE);

        //Создаём таблицу загрузки

  char shotName [128];

  LoadTable  lTable (_screen,mesPoint,
                     "fonts/trip.chr",
                     LevelCompile::ScreenShotName(levelName,shotName));

  loadTable = &lTable;

  loadTable->SetMessage (LM_LEVEL_FILE,"Loading    ",levelName);
  loadTable->SetMessage (LM_OPERATION_ID,"LOAD LEVEL");
  loadTable->SetMessage (LM_LEVEL_NAME,"NONAME");
  loadTable->SetMessage (LM_CURRENT_LOAD,"Wait, system download resources");

  dout<<"Single game: Load level '"<<levelName<<"'"<<endl;

  if (player)    ReloadLevel(levelName,x,y);
  else           InitSystem (levelName,x,y);

        //Refresh for sound system

  InitSound();
  LoadAutoexec("config.cfg");

  if (input && isError) input->OnlyConsole(FALSE);
  if (video && isError) video->OnlyConsole(FALSE);

  isPause   = FALSE;
  isError   = FALSE;
  loadTable = NULL;
}

void    SingleGame::InitSystem (const char* levelName,int x,int y)
{
  level    = LevelCompile::LoadLevel (levelName);

  if (level == NULL)
  {
    dout<<"Single game: Error open level '"<<levelName<<"'"<<endl;
    grout<<"Error open level '"<<levelName<<"'"<<endl;

    loadTable = NULL;
    ErrorLoop();

    return ;
  }

  if (KLIP_PLAY)
  {
    soundShell = level->GetSound()->CreateShell(MAX_CHANNELS);
    PlayKlip (level->GetKlip());
  }

  video   = new VideoSystem (level,_screen);
  server  = new GameSystem  (level,MAX_OBJECTS,QUEUE_SIZE);

        //Создаём посредника

  client  = new (SINGLE_GAME,ID(-1,-1),server)
                 SingleGameClient (this);

  server->AddAttr (client->id(),OBJ_UNLOAD_PROTECT);

  isContinue = TRUE;

        //Создаём игрока

  CreateMainPlayer (DEFAULT_PLAYER,x,y);

  dout<<"Single game: Create system OK"<<endl;
}

void    SingleGame::ReloadLevel(const char* name,int x,int y)
{
  server->UnloadLevel();

  level    = LevelCompile::ReloadLevel (name,level);

  if (level == NULL)
  {
    dout<<"Single game: Error open level '"<<name<<"'"<<endl;
    grout<<"Error open level '"<<name<<"'"<<endl;

    ErrorLoop();

    return ;
  }

  if (KLIP_PLAY)
  {
    PlayKlip (level->GetKlip());
    video ->refresh    (_screen);
  }

  server->ChangeLevel(level);
  video ->refresh    (level);

  dout<<"LOAD OK!"<<endl;

        //refresh для GameView

  PlayerInfoMessage refrView(-1,-1,0);

  server->PutMessage (&refrView,view->id(),sizeof(refrView));

  if (server->GetObject(player)->type() != ERROR_OBJECT)
    ((Player*)server->GetObject(player))->SetPos (int2fixed(x),int2fixed(y));
  else
  {
    dout<<"ID "<<player<<endl;
    delete fin;
  }

        //Refresh для главного игрока

  UserRefreshMessage refrUser;

  server->PutMessage (&refrUser,player,sizeof(refrUser));
}

void    SingleGame::PlayKlip         (KlipPlayer* player)
{
  if (!vesa)
     return;

  if (loadTable)
  {
    loadTable->SetMessage (LM_CURRENT_LOAD,"Loading klip...");
    loadTable->freeze     ();
  }

  int prevMode = ((_VESA*)vesa)->getmode();

  if (player)   //IsCorrect
  {
    if (vesa->SetMode (DEFAULT_KLIP_MODE) == VESAdevice::OK)
    {
      player->InitSound (soundShell);
      player->GoMusic   ();
      player->SetScreen (vesa->CurrentScreen());

      while (!Keyboard::IsPress (KEY_ESC) && !player->end())
        player->tick();
    }

    if (vesa->SetMode (prevMode) != VESAdevice::OK)
    {
      dout<<"Single game: Error restore video mode after play klip"<<endl;
      delete fin;
    }

    _screen = vesa->CurrentScreen();

    if (soundShell)
    {
      delete soundShell;
      soundShell = NULL;
    }
  }

  if (loadTable)
     loadTable->refresh (_screen,mesPoint);
}

void    SingleGame::SendLoadMessage (const char* message)
{
  if (loadTable) loadTable->SetMessage (LM_CURRENT_LOAD,"Loading... ",message);
  delay (LOAD_RESOURCE_DELAY);
}

void    SingleGame::SendLevelName (const char* message)
{
  if (loadTable) loadTable->SetMessage (LM_LEVEL_NAME,message);
}

SingleGame::SingleGame  (VESAdevice* _vesa,Screen* dest)
          : level (NULL), server (NULL),
            view (NULL),  input (NULL), player (ID(-1,-1)),
            isContinue (FALSE), soundShell (NULL),
            _screen (dest), consLine(FALSE), isPause(FALSE),
            isError (FALSE), loadTable(NULL), vesa(_vesa)
{
  Palette::ResetPalette (dest);
}

SingleGame::SingleGame  (VESAdevice* _vesa,Screen* dest,const char* levelName)
          : level (NULL), server (NULL),
            view (NULL),  input (NULL), player (ID(-1,-1)),
            isContinue (FALSE), soundShell (NULL),
            _screen (dest), consLine(FALSE), isPause(FALSE),
            isError (FALSE), loadTable(NULL), vesa(_vesa)
{
  LoadLevel (levelName,1,1);
}

SingleGame::SingleGame  (VESAdevice* _vesa,Screen* dest,BinFile*)
{
}

SingleGame::~SingleGame ()
{
  dout<<"Single game: Delete system"<<endl;

     //Удаление
  if (soundShell) delete soundShell;
  if (server) delete server;
  if (level)  delete level;
  if (video)  delete video;
}
