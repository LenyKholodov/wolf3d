#include "g_multi.h"
#include "g_mac.h"
#include "g_load.h"
#include "g_system.h"
#include "g_player.h"

#include "cross.h"

#include "palette.h"

#include <keydef.h>

make <FarClient,MULTIPLAYER>    makeMultiplayer;

/////////////////implementation of class FarClient///////////////////////////

BOOL    FarClient::Exec (GameMessage* mes)
{
  Flag  flag ("MULTIPLAYER_IN");

  switch (mes->id)
  {
    case NOT_PROCESS:
    {
      GameNotProcess* notProc = (GameNotProcess*)mes;

      dout<<"Far client: No process "<<notProc->mesId<<" from "<<notProc->obj<<endl;
      return TRUE;
    }
    case TICK:
    {
      NetMapSequenceMessage        seq; //static
      int                          count = netMap.count ();
      int                          cnt   = NET_SEQUENCE_PACKET;

      seq.size          = GAME_MESSAGE_MAX_SIZE;
      seq.callBack      = id();
      seq.objectId      = id();

      for (int pos=0;pos<count;)
      {
          seq.count = count-pos>cnt ? cnt : count-pos;

          for (int j=0;j<cnt && pos<count;pos++,j++)
            netMap.GetBlock (pos,seq.table[j].mapIndex,seq.table[j].block);

          net->Send (&seq,id());
      }

      netMap.Clear ();

      GameMessage* message = net->Dispatch();

      while (message)
      {
        owner->PutMessage (message,message->objectId,message->size,message->callBack);
        message = net->Dispatch ();
      }

      if (clock()-last > NET_VIEW_REFRESH)
      {
        GetPlayerInfoMessage getInfo (id());
        owner->PutMessage (&getInfo,mainPlayer,sizeof(getInfo));

        last = clock();
      }

      if (clock()-writeAnswer > NET_REANSWER_W)
      {
        OnlineMessage  online;

        online.size     = sizeof (online);
        online.objectId = id();
        online.callBack = id();

        net->Send (&online,id());

        writeAnswer = clock();
      }

        //Если нет ответа от клиента ... - КАЮK!

      if (clock()-readAnswer > NET_REANSWER_R)
      {
        delete this;
        return FALSE;
      }

      net->tick();

      return TRUE;
    }
    case ONLINE:
    {
      readAnswer = clock();

      return TRUE;
    }
    case DISCONNECT:
    {
      delete this;

      return TRUE;
    }
    case PLAYER_INFO:
         happy_blood();
    case MOVE_ITEM_TO:
    case MOVE_ITEM_REL:
    case MOVE_ITEM_START:
    case RELOAD_ITEM_VIEW:
    case CHANGE_ITEM_VIEW:  {  net->Send (mes,id()); return TRUE; }
    case ALREADY_DELETE:    { return TRUE; }
    case PLAYER_DEATH:
    {
       CreateWeaponPlayer  create;
       ResourceHeader      header;

       int   index  = random(owner->GetLevel()->GetRespawnCount());
       uint  x      = owner->GetLevel()->GetRespawn()[index].x,
             y      = owner->GetLevel()->GetRespawn()[index].y;
       owner->GetLevel()->GetMap()->FindClear (x,y);

       create.param().x       = int2fixed(x);
       create.param().y       = int2fixed(y);
       create.param().angle   = 0;
       create.param().health  = PLAYER_HEALTH;
       create.param().protect = PLAYER_PROTECT;

       if (header.Load (DEFAULT_PLAYER,&create.resourceClass) != GAME_MONSTR)
       {
         dout<<"Far client: Wrong resource file format "<<DEFAULT_PLAYER<<endl;
         return TRUE;
       }

       RespawnPlayerMessage newPlayer (
                            owner->CreateObject (
                                                 USER_PLAYER,
                                                 &create,
                                                 id())
                            );

       mainPlayer = newPlayer.mainPlayer;

       if (newPlayer.mainPlayer == ID(-1,-1))
       {
         dout<<"Far client: Error create MONKEY "<<endl;
         return TRUE;
       }

       newPlayer.size       = sizeof(RespawnPlayerMessage);
       newPlayer.objectId   = id();
       newPlayer.callBack   = id();

       net->Send (&newPlayer,id());

      return TRUE;
    }
    case CHANGE_LEVEL:
    {
      net->Send (mes,id());

      SequenceMapPtrMessage message (owner->GetMap());

      owner->PutMessage (&message,id(),sizeof(message));

           //Refresh для главного игрока

      UserRefreshMessage refrUser;

      owner->PutMessage (&refrUser,player(),sizeof(refrUser));

      return TRUE;
    }
    case HALT_SYSTEM:
    case LUCKY_SHOT:
    case CROSS_SHOT:
    case PLAY_SOUND:
    case SLEEP_OFF:             { net->Send (mes,id()) ; return TRUE; }
    case NET_OK_PACKET:
    case NET_NO_PACKET:
    case NET_SEQUENCE:
    {
      net->Receive ((NetPacket*)mes,id());

      return TRUE;
    }
    case SEQUENCE_OBJECT:
    {
      Flag      flag1 ("SEQUENCE_OBJECT");

      SequenceObjectMessage* message = (SequenceObjectMessage*)mes;

      netMap.SetIndex (message->mapIndex,message->block);

      return TRUE;
    }
    case SEQUENCE_MAP_PTR:
    {
      LevelCompile*   lComp  = new LevelCompile (owner->GetLevel()->GetLevelFile());
      Map*            srcMap = new Map(lComp->GetMap());
      Map*            trgMap = owner->GetMap();

      netMap.Clear ();

      for (int i=0;i<MAP_WIDTH*MAP_HEIGHT;i++)
          if (memcmp(trgMap->GetBlock(i),srcMap->GetBlock(i),sizeof(Block)))
          {
            SequenceObjectMessage  seq (i,*trgMap->GetBlock(i));
            seq.callBack    = ID(-1,-1);
            seq.size        = sizeof (seq);
            seq.objectId    = id();

            Exec (&seq);
//            owner->Sequence (&seq,sizeof(seq));
          }

      delete srcMap;
      delete lComp;

      return TRUE;
    }
    case ALREADY_CREATE:
    {
      AlreadyCreateMessage* message = (AlreadyCreateMessage*)mes;

      if (owner->GetObject (message->obj)->type()!=USER_PLAYER)
         return TRUE;

      owner->AddAttr (message->obj,OBJ_UNLOAD_PROTECT);
      owner->AddAttr (id(),OBJ_UNLOAD_PROTECT);

      return TRUE;
    }
    case I_SEE_YOU:
    case ECHO_ON:           return TRUE;
    default:                return FALSE;
  }
}

void    FarClient::dump (ostream& os) const
{
//  os<<"Client "<<id()<<" net address "<<clientAddr<<endl;
}

void    FarClient::happy_blood ()
{
  PlayerISeeYou message (mainPlayer);
  uint          visible [MAX_VIS_SPRITES];
  ID            obj;
  Player*       monkey     = (Player*)owner->GetObject (mainPlayer);
  Player*       vis_monstr;

  int           count   = FindSprites (monkey->GetX(),
                                       monkey->GetY(),
                                       monkey->GetAngle(),
                                       visible,
                                       owner->GetMap());

        //Happy New Year!

  for (int i=0;i<count;i++)
  {
      obj=owner->GetObject (visible[i]);

      if (obj != ID(-1,-1) && obj != mainPlayer)
      {
         vis_monstr = (Player*)owner->GetObject(obj);
         Angle ang  =  FindAngle(vis_monstr->GetX(),
                                 vis_monstr->GetY(),
                                 monkey->GetX(),
                                 monkey->GetY())
                       -vis_monstr->GetAngle();

         if (ang>=ANGLE_270 || ang<=ANGLE_90)
         {
            owner->PutMessage (&message,owner->GetObject(visible[i]),sizeof(message));
         }
      }
  }
}

FarClient::FarClient (ArgPtr ptr)
          : net (NULL), last (clock()), netMap (),
            writeAnswer (clock()), readAnswer (clock())
{
  CreateFarClient* arg = (CreateFarClient*)ptr;

  mainPlayer = arg->player;
  net        = new Net (MAX_NET_SEQUENCE,arg->addr,SERVER_WRITE,SERVER_READ);

  grout<<"Player "<<arg->addr<<" join the game"<<endl;

  owner->AddClient (id());
  owner->AddActive (id(),PR_MULTI);
  owner->AddAttr   (id(),OBJ_UNLOAD_PROTECT);
}

FarClient::~FarClient ()
{
  HaltSystemMessage   ok;

  ok.size      = sizeof (ok);
  ok.objectId  = id();
  ok.callBack  = id();

  net->Send (&ok);
  net->tick ();

  owner->DelClient    (id());
  owner->DelActive    (id());
  owner->DeleteObject (player());

  if (net) delete net;

//  grout<<"Player "<<clientAddr<<" left the game "<<endl;
  grout<<"Player "<<id()<<" left the game"<endl;
}

/////////////////implementation of class MultiGame///////////////////////////

static BOOL viewChange = TRUE;

BOOL            MultiGame::Exec (GameMessage* mes)
{
  Flag  flag ("MG::EXEC");

  switch (mes->id)
  {
    case  CHANGE_SCREEN:
    {
      if (isNet )
      {
        view->Exec   (mes);
        viewChange = TRUE;
      }
      else
      {
        ViewChangeScreenMessage* message = (ViewChangeScreenMessage* )mes;

        _screen = message->screen;

        if (console) delete console;

        console->open (_screen,tBinFont::load("fonts/font.fnt"));
        console->show ();

        Palette::ResetPalette (_screen);
      }

      return TRUE;
    }
    case  PLAYER_INFO:
    case  PIC_MIP:
    case  MOVE_ITEM_TO:
    case  MOVE_ITEM_REL:
    case  MOVE_ITEM_START:
    case  RELOAD_ITEM_VIEW:
    case  CHANGE_ITEM_VIEW:
    {
      Flag      flag ("VIEW_MESSAGE");

      if (isNet)
      {
        view->Exec   (mes);
        viewChange = TRUE;
      }

      return TRUE;
    }
    case  PLAY_SOUND:
    {
      Flag flag1 ("PLAY_SOUND");

      PlaySoundMessage *message = (PlaySoundMessage *) mes;

      if (soundShell == NULL)
      {
         dout<<"Play sound #"<<message->sound<<endl;
         return TRUE;
      }

      Fixed dist  = abs(int2fixed(message->x)-view->getx())<<1;

      Angle angle = FindAngle (view->getx(),view->gety(),
                    int2fixed(message->x),int2fixed(message->y));

      dist        = 256 - fixed2int( abs( mul(dist,icos(angle)) ) ); //128
      dist        = (dist*iFace [IF_SFX_VOLUME])>>VOLUME_SHIFT;

      if (dist>MIN_VOLUME_LIMIT)
         soundShell->play (message->sound,
                           dist,
                           angle2float(angle-view->getangle())
                          );

      flag1 = TRUE;

      return TRUE;
    }
    case  TICK:     { return TRUE; }
    case  HALT_SYSTEM:
    {
      Flag flag ("HALT_SYSTEM");

      if (!isNet || isDisconnect)
         return TRUE;

      DisconnectMessage disc;

      disc.size       = sizeof (disc);
      disc.objectId   = id();
      disc.callBack   = id();

      net->Send (&disc,id());
      net->tick();

      isDisconnect = TRUE;

      return TRUE;
    }
    case  SEQUENCE_OBJECT:
    {
      Flag      flag ("SQEUNCE_OBJETC");

      SequenceObjectMessage* message = (SequenceObjectMessage*)mes;

      server->SetBlock (message->mapIndex,message->block);

      viewChange = TRUE;

      return TRUE;
    }
    case  NET_MAP_SEQUENCE:
    {
      Flag      flag ("NET_MAP_SEQUENCE");

      if (isNet)
      {
        NetMapSequenceMessage* seq   = (NetMapSequenceMessage*)mes;
        int                    count = seq->count;

        if (count) viewChange = TRUE;

        for (int i=0;i<count;i++)
          server->SetBlock (seq->table[i].mapIndex,seq->table[i].block);
      }

      return TRUE;
    }
    case  NET_OK_PACKET:
    case  NET_NO_PACKET:
    case  NET_SEQUENCE:
    {
      Flag      flag ("NET_SEQUENCE");
      SearchVar<int>  var1 ("isNet",&isNet);

      if (isDisconnect)
         return TRUE;

      if (isNet)
        net->Receive ((NetPacket*)mes,id());
      else
      {
        NetPacket*    packet  = (NetPacket*)mes;
        GameMessage*  message = (GameMessage*)packet->data;

        return Exec (message);
      }

      return TRUE;
    }
    case INPUT_KEY_DOWN:
    {
      Flag      flag ("KEY_DOWN");

      KeyDownMessage*  message =  (KeyDownMessage*)mes;

      if (message->key == KEY_TILDA)
      {
        if (isNet)
        {
          PlayerResetActiveMessage reset;
          server->PutMessage (&reset,player,sizeof(reset));
        }
        else  return TRUE;
      }

      if (message->key == KEY_ESC)
         delete fin;

      if (isNet && video && view)
      {
        video->putc  (message->ascii,message->key);
        view->Redraw ();
      }
      else if (console)
      {
        console->putc(message->ascii,message->key);
      }

      return TRUE;
    }
    case SERVER_QUESTION:
    {
      grout<<"Wait... (4 sec)"<<endl;

      NetPacket             packet;
      GetServerInfoMessage* getInfo  = (GetServerInfoMessage*)packet.data;

      getInfo->objectId  = ID(-1,-1);
      getInfo->callBack  = id();
      getInfo->id        = SERVER_QUESTION;
      GetLocal (getInfo->clientAddr);

      packet.objectId    = ID(-1,-1);
      packet.size        = sizeof(packet)-GAME_MESSAGE_MAX_SIZE+sizeof(GetServerInfoMessage);
      packet.callBack    = id();
      packet.index       = 0;
      packet.socket      = NO_SOCKET;

      for (int i=0;i<8;i++)
          memset (servers[i].node,0xFF,6);

      clock_t     _last = clock();
      addrnode_t  quest;

      memset (quest.node,0xFF,6);

      for (clock_t x=clock();clock()-x<NET_REQUEST_TIME;)
      {
        NetTick();

        if (clock()-_last > REQUEST_TIME)
        {
          SendIPX (quest,CLIENT_WRITE,&packet,packet.size);
          _last = clock();
        }
      }

      return TRUE;
    }
    case SERVER_INFO:
    {
      ServerInfoMessage* serverInfo = (ServerInfoMessage*)mes;

      addrnode_t test;
      memset (test.node,0xFF,6);

      for (int i=0;i<8;i++)
          if (!memcmp (servers[i].node,test.node,6))
          {
            memcpy (servers[i].node,serverInfo->addr.node,6);

            grout<<"Server #"<<i<<endl;
            grout<<"Players "<<serverInfo->clientsCount<<"/"<<serverInfo->maxClients<<endl;
            grout<<"Server name "<<serverInfo->serverName<<endl;
            grout<<"Map name "<<serverInfo->levName<<endl;

            break;
          }

      if (i==8)
      {
        grout<<"No free slots for server"<<endl;
        return TRUE;
      }

      return TRUE;
    }
    case CONNECT_TO:
    {
      NetPacket             packet;
      ConnectMessage*       connect  = (ConnectMessage*)packet.data;
      ConnectToServerMessage* message = (ConnectToServerMessage*)mes;

      connect->objectId  = ID(-1,-1);
      connect->callBack  = id();
      connect->id        = CONNECT;
      GetLocal (connect->addr);

      packet.objectId    = ID(-1,-1);
      packet.size        = sizeof(packet)-GAME_MESSAGE_MAX_SIZE+sizeof(ConnectMessage);
      packet.callBack    = id();
      packet.index       = 0;
      packet.socket      = NO_SOCKET;

      SendIPX (message->addr,CLIENT_WRITE,&packet,packet.size);

      return TRUE;
    }
    case CONNECT:
    {
      ConnectMessage* message    = (ConnectMessage*)mes;

      InNet (message->addr,message->client,message->mainPlayer,message->levName);

      return TRUE;
    }
    case RESPAWN:
    {
      RespawnPlayerMessage* message = (RespawnPlayerMessage*)mes;

      input->player (message->mainPlayer);
      view->player  (message->mainPlayer);

      player        = message->mainPlayer;

      return TRUE;
    }
    case ONLINE:
    {
      readAnswer = clock();

      return TRUE;
    }
    case CHANGE_LEVEL:
    {
      PortalChangeLevelMessage* message = (PortalChangeLevelMessage*)mes;

      LoadLevel(message->levelName,message->x,message->y);

      return TRUE;
    }
    default: return FALSE;
  }
}

void            MultiGame::dump (ostream& os) const
{
  os<<"Fucking multiplayer!!!"<<endl;
}

void    MultiGame::OutNet      ()
{
  if (video)  delete video;
  if (server) delete server;
  if (level)  delete level;
  if (net)    delete net;
  if (soundShell) delete soundShell;

  video      = NULL;
  view       = NULL;
  server     = NULL;
  input      = NULL;
  net        = NULL;
  level      = NULL;
  soundShell = NULL;
  farClient  = ID(-1,-1);
  player     = ID(-1,-1);

  addrnode_t addr ;

  server  = new GameClient  (this,addr,level);
  console = new Console     (_screen,tBinFont::load("fonts/font.fnt"));
  console->height           (_screen->height());
  console->show();

  input   = new (farClient,farClient,(GameSystem*)server) InputTranslator (player);
  input->OnlyConsole (TRUE);

  isNet      = FALSE;
}

void    MultiGame::InNet       (addrnode_t serv,ID _client,ID _player,const char* levelName)
{
  if (console) delete console;
  if (server)  delete server;

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

  dout<<"Multi game: Load level '"<<levelName<<"'"<<endl;

  level    = LevelCompile::LoadLevel (levelName);

  if (level == NULL)
  {
    dout<<"Multi game: Error open level '"<<levelName<<"'"<<endl;
    grout<<"Error open level '"<<levelName<<"'"<<endl;

    delete fin;
  }

  farClient = _client;
  player    = _player;

  net       = new Net (MAX_NET_SEQUENCE,serv,CLIENT_WRITE,CLIENT_READ);

  server    = new GameClient  (this,serv,level);
  video     = new VideoSystem (level,_screen);
  input     = new (farClient,farClient,(GameSystem*)server) InputTranslator (player);
  view      = new (farClient,farClient,(GameSystem*)server) GameView        (video,player);

  InitSound();
  LoadAutoexec("config.cfg");

  isNet       = TRUE;
  readAnswer  = clock();
  writeAnswer = clock();
  loadTable   = NULL;
}

void            MultiGame::MainLoop  ()
{
  Flag  flag ("MULTI_TICK");

  static clock_t last = clock();
  const clock_t  VIEW_REFRESH = NET_VIEW_REFRESH;

  clock_t start = clock ();

  while (isContinue)
  {
    if (isDisconnect)
    {
      OutNet ();
      isDisconnect = FALSE;
      continue;
    }
    if (isNet)
    {
      net->tick();
      input->Translate   ();

      for (GameMessage* message=net->Dispatch();message;message=net->Dispatch())
           Exec (message);

      if (viewChange)
      {
        view->Redraw ();
        viewChange = FALSE;
      }

      if (clock()-writeAnswer > NET_REANSWER_W)
      {
        OnlineMessage online;

        online.size     = sizeof (online);
        online.objectId = id();
        online.callBack = id();

        net->Send (&online,id());

        writeAnswer = clock();
      }

      if (clock()-readAnswer > NET_REANSWER_R)
      {
        grout<<"SERVER NOT FOUND!"<<endl;

        OutNet();

        continue;
      }
    }
    else
    {
      console->tick();
      input->Translate ();
    }
  }
}

void    MultiGame::SendLoadMessage (const char* message)
{
  if (loadTable) loadTable->SetMessage (LM_CURRENT_LOAD,"Loading... ",message);
  delay (LOAD_RESOURCE_DELAY);
}

void    MultiGame::SendLevelName (const char* message)
{
  if (loadTable) loadTable->SetMessage (LM_LEVEL_NAME,message);
}

void            MultiGame::LoadLevel       (const char* name,int x,int y)
{
        //Создаём таблицу загрузки

  char shotName [128];

  LoadTable  lTable (_screen,mesPoint,
                     "fonts/trip.chr",
                     LevelCompile::ScreenShotName(name,shotName));

  loadTable = &lTable;

  loadTable->SetMessage (LM_LEVEL_FILE,"Loading    ",name);
  loadTable->SetMessage (LM_OPERATION_ID,"LOAD LEVEL");
  loadTable->SetMessage (LM_LEVEL_NAME,"NONAME");
  loadTable->SetMessage (LM_CURRENT_LOAD,"Wait, system download resources");

  dout<<"Multi game: Reload level '"<<name<<"'"<<endl;

  level    = LevelCompile::ReloadLevel (name,level);

  if (level == NULL)
  {
    dout<<"Multi game: Error open level '"<<name<<"'"<<endl;
    grout<<"Error open level '"<<name<<"'"<<endl;

    delete fin;
  }

  video ->refresh     (level);
  server->ChangeLevel (level);

  LoadAutoexec("config.cfg");

  dout<<"LOAD OK!"<<endl;

  loadTable = NULL;
}

void            MultiGame::InitSound   ()
{
  if (loadTable) loadTable->SetMessage (LM_CURRENT_LOAD,"Play back ground music");

  if (soundShell == NULL && SOUND_ON)
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
}

void    MultiGame::SfxVolume (int vol)
{
    if (soundShell == NULL) return;

    iFace [IF_SFX_VOLUME] = vol &= MAX_VOLUME;

    for (int i=0;i<MAX_CHANNELS;i++)
        if (i!=iFace[IF_MUSIC_CHANNEL])
          soundShell->volume(i,(soundShell->volume(i)*vol)>>VOLUME_SHIFT);
}

void    MultiGame::MusicVolume (int vol)
{
    if (soundShell == NULL) return;

    int i                   = iFace [IF_MUSIC_CHANNEL];
    iFace [IF_MUSIC_VOLUME] = vol;

    soundShell->volume(i,vol);
}

MultiGame::MultiGame  (VESAdevice* vesa,Screen* dest)
          :_screen (dest), farClient (ID(-1,-1)), player (ID(-1,-1)),
           level (NULL), net (NULL), server (NULL), view (NULL),
           video(NULL), soundShell (NULL), loadTable (NULL),
           writeAnswer (clock()), readAnswer (clock()),
           isDisconnect (FALSE)
{
  Palette::ResetPalette (dest);

  OutNet ();

  isContinue = TRUE;
}

MultiGame::MultiGame  (addrnode_t& addr,ID _farClient,ID _player,VESAdevice*,Screen* dest,const char* levelName)
          :_screen (dest), farClient (_farClient), player (_player),
           soundShell (NULL), loadTable (NULL), isDisconnect(FALSE)
{
  level    = LevelCompile::LoadLevel (levelName);

  if (level == NULL)
  {
    dout<<"Multi game: Error open level '"<<levelName<<"'"<<endl;
    grout<<"Error open level '"<<levelName<<"'"<<endl;

    delete fin;
  }

  net     = new Net (MAX_NET_SEQUENCE,addr,CLIENT_WRITE,CLIENT_READ);

  server  = new GameClient  (this,addr,level);
  video   = new VideoSystem (level,_screen);

  input   = new (farClient,farClient,(GameSystem*)server) InputTranslator (player);
  view    = new (farClient,farClient,(GameSystem*)server) GameView        (video,player);

  isContinue = TRUE;
  isNet      = TRUE;
}

MultiGame::~MultiGame ()
{  }
