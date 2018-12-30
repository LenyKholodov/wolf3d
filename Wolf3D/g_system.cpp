#include "g_system.h"
#include "g_multi.h"
#include "g_load.h"

#include "g_door.h"

#include <debug\debug.h>
#include <time.h>

GameSystem*     gameSystem = NULL;

make<GameServer,GAME_SERVER> makeServer;

////////////////////////////implementation of class GameSystem//////////////

void*     GameSystem::CreateObject (GameObjects type)
{
  PoolIndex index = gamePool->CreateObject();

  ID id = gameTypeTable->Include (type,index);

  gamePool->GetObject (index)->mID = id;

  return gamePool->GetObject (index);
}

void*     GameSystem::CreateObject (ID id)
{
  PoolIndex index = gamePool->CreateObject();

  gamePool->GetObject (index)->mID = id;        //wrong! ERROR_TYPE

  gameTypeTable->SetPoolIndex (id,index);

  return gamePool->GetObject (index);
}

ID       GameSystem::CreateObject (GameObjects type,ArgPtr ptr,ID callBack)
{
  GameObject* obj = (*makeFn [type] )(ptr,callBack,this);

  return obj->id();
}

void     GameSystem::DeleteObject (ID id)
{
  delete GetObject(id);
}

ID       GameSystem::LoadObject  (int cacheId,int idRes,ArgPtr param,int _size,ID owner)
{
  NameCache* cache = gameLevel->GetCache (cacheId);

  if (cache == NULL || idRes >= cache->GetNamesNum())
     return ID(-1,-1);

  char* nameRes = (char*)cache->GetName (idRes);

  if (nameRes [0] == 0)
     return ID(-1,-1);

  ResourceHeader             header;
  char                       data [4096];
  CreateExemplar<char,char>* res = (CreateExemplar<char,char>*)&data;

  GameObjects type = header.Load (nameRes,&res->resourceClass);

  if (makeFn[type] == NULL)
  {
    dout<<"Game system: Can't load object. It does not have make function "<<type<<endl;
    return ID(-1,-1);
  }

  memcpy (res->paramData,param,_size);

  GameObject* object = (*makeFn[type])(res,owner,this);

  return object ? object->id() : ID(-1,-1);
}

void     GameSystem::DisposeObject (ID id)
{
  PoolIndex index = gameTypeTable->GetObject (id);

  gameTypeTable->Exclude  (id);
  gamePool->DeleteObject  (index);
}

int           GameSystem::GetAttr   (ID id)
{
  return GetObject(id)->attr();
}

void          GameSystem::SetAttr   (ID id,int attr)
{
  GetObject(id)->SetAttr (attr);
}

void          GameSystem::AddAttr   (ID id,int attr)
{
  SetAttr (id,GetAttr(id)|attr);
}

GameObjects   GameSystem::GetType   (uint index)
{
  return GetObject(GetObject(index))->type();
}

GameObject*   GameSystem::GetObject (ID id)
{
  return gamePool->GetObject (gameTypeTable->GetObject(id));
}

ID            GameSystem::GetObject (uint mapIndex)
{
  return gameMap->GetObject(mapIndex);
}

void          GameSystem::SetObject   (uint index ,ID id)
{
  gameMap->PutObject (index,id);
}

ID            GameSystem::GetDispatch ()
{
  return gameServer->id();
}

Ring*         GameSystem::GetRing   (GameObjects type)
{
  return gameTypeTable->GetRing (type);
}

void          GameSystem::PutMessage  (GameMessage* mes,ID id,int size,ID cb)
{
#ifndef NDEBUG
  if (id.type < 0 || id.type > MAX_TYPES ||
      id.id   < 0 || id.id   > MAX_SLOTS)
     return;
  else
#endif
  if (cb.type<-1 || cb.id <-1)
    dout<<"Mes ID "<<mes->id<<" CB "<<cb<<" ID "<<id<<endl;

  if (!pauseState)
    gameMessageQueue->PutMessage (mes,id,size,cb);
  else if (reTrans)
  {
    mes->callBack = cb;
    mes->size     = size;

    GetObject(reTrans)->Exec (mes);
  }
}

void          GameSystem::SendMessage (uint index,GameMessage* mes,int size)
{
  gameMap->SendMessage (index,mes,size);
}

void          GameSystem::DispatchMessage (GameMessage* mes)
{
  Flag flag ("DISPATCH_IN",TRUE);

#ifndef NDEBUG
  if (mes == NULL)
  {
     dout<<"FUCK"<<endl;
     return;
  }
#endif

  if (!GetObject(mes->objectId)->Exec (mes))
  {
    if (mes->callBack != ID(-1,-1))
    {
       GameNotProcess   message (mes->objectId,mes->id);

       PutMessage (&message,mes->callBack,sizeof(message));
    }
#ifdef __NO_PROCESS__
    else
#endif
      dout<<"Object "<<mes->objectId<<" type "<<GetObject(mes->objectId)->type()<<
            " wrong message "<<mes->id<<" send from "<<mes->callBack<<endl;

  }
}

void     GameSystem::SetBlock    (uint index,const Block& block)
{
  GetMap() -> SetBlock (index,block);

  if (GetClientsCount ())
  {
    SequenceObjectMessage  mes (index,block);
    mes.callBack = ID(-1,-1);

    Sequence (&mes,sizeof(mes));
  }
}

Block*   GameSystem::GetBlock    (uint index) { return GetMap()->GetBlock (index); }
BOOL     GameSystem::IsCross    (uint index) { return GetMap()->IsCross(index); }
void     GameSystem::SetMap     (Map* map)
{
  memcpy (GetMap(),map,MAP_WIDTH*MAP_HEIGHT*sizeof(Block));

  if (GetClientsCount ())
  {
    SequenceMapPtrMessage _map (GetMap());

    Sequence (&_map,sizeof(_map));
  }
}

void          GameSystem::DispatchAll()
{
  for (clock_t x = clock();!gameMessageQueue->IsEmpty() && (clock()-x<SEQUENCE_VALUE);)
    DispatchMessage ((GameMessage*)gameMessageQueue->GetMessage () );
}

void   GameSystem::AddActive       (ID id,int pr) { gameActiveList->Add (id,pr);    }
void   GameSystem::DelActive       (ID id) { gameActiveList->Del (id);   }
void   GameSystem::ClearActive     ()      { gameActiveList->ClearAll (); }
void   GameSystem::NextTick        ()      { gameActiveList->NextTick();  }
void   GameSystem::TickAll         ()      { gameActiveList->TickAll();   }

BOOL          GameSystem::AddClient (ID id)
{
  for (int i=0;i<MAX_CLIENTS && clientMap [i] != ID(-1,-1);i++);

  if (i>=MAX_CLIENTS || clientMap [i] !=  ID(-1,-1))
     return FALSE;

  clientMap [i] = id;

  clientCount++;

        //Синхронизация карты (на текущей машине!)

  SequenceMapPtrMessage _map (GetMap());

  PutMessage (&_map,id,sizeof(_map));

  return TRUE;
}

void          GameSystem::DelClient (ID id)
{
  for (int i=0;i<MAX_CLIENTS;i++)
      if (clientMap [i] == id)
      {
        clientMap [i] = ID (-1,-1);
        return;
      }
}

void          GameSystem::Sequence (GameMessage* mes,int size)
{
  for (int i=0;i<MAX_CLIENTS;i++)
      if (clientMap [i] != ID(-1,-1) )
        PutMessage (mes,clientMap [i],size,mes->callBack);
}

/////////////////////////////////////////////////////////////////////////////
//////Эхо
/////////////////////////////////////////////////////////////////////////////

void          GameSystem::PutEcho         (int x,int y,int volume,ID who)
{
  if (who == ID(-1,-1))
     return;

  int  xstart = x - (volume>>1),
       ystart = y - (volume>>1),
       xend   = x + (volume>>1),
       yend   = y + (volume>>1);

  xstart     = xstart<0 ? 0 : xstart;
  ystart     = ystart<0 ? 0 : ystart;
  xend       = xend>MAX_X_TILE ? MAX_X_TILE : xend;
  yend       = yend>MAX_Y_TILE ? MAX_Y_TILE : yend;

  int  index  = Index (ystart,xstart);
  ID   obj;

  EchoOnMessage message (who);

  for (int _y=ystart;_y<yend;_y++,index=Index(_y,xstart)) //inc
      for (int _x=xstart;_x<xend;_x++,index++)
          if (GetObject(index) != ID(-1,-1))
             if (GetBlock(index)->type == Map::MONSTR)
               PutMessage (&message,GetObject(index),sizeof(message));
}

void          GameSystem::Pause (ID who,BOOL state)
{
  pauseState = state;
  reTrans    = who;
}

void          GameSystem::tick ()
{
  if (!pauseState)
  {
    TickAll();
    DispatchAll();
  }
}

void          GameSystem::Load   (BinFile* file,Level* lev)
{
  ServerTag     tag;

  file->read    (&tag,sizeof(tag));

  if (tag.tag != SERVER_TAG)
  {
    dout<<"Game system: Error in-out file. Load failed"<<endl;
    return ;
  }

  gameLevel = lev;              //Установка текущего уровня

  if (!GetMap()->Load (file))
  {
    dout<<"Game system: Error load map. Load failed"<<endl;
    return ;
  }

  gamePool         -> ClearAll ();
  gameMessageQueue -> ClearAll ();
  gameMap          -> Load     (file);
  gameTypeTable    -> Load     (file);

  gameServer      =  (GameServer*)GetObject (tag.gameServer);
  gameActiveList  =  (ActiveList*)GetObject (tag.gameActiveList);

          //Устанавливаем атрибуты

  AddAttr (gameActiveList->id(),OBJ_UNLOAD_PROTECT);
  AddAttr (gameServer->id(),OBJ_UNLOAD_PROTECT);

  clientCount     =  tag.clientCount;
  pauseState      =  tag.pauseState;
  reTrans         =  tag.reTrans;

  memcpy (clientMap,tag.clientMap,sizeof(ID)*MAX_CLIENTS);

  if (gameServer->type() != GAME_SERVER || gameActiveList->type() != ACTIVE_LIST)
  {
    dout<<"Game system: Error in in-out file format. Load failed"<<endl;
    dout<<"Active list "<<gameActiveList->type()<<endl;
    dout<<"Server "<<gameServer->type()<<endl;
    return ;
  }
}

void          GameSystem::Save   (BinFile* file)
{
  ServerTag     tag;

  tag.gameServer        = gameServer->id();
  tag.gameActiveList    = gameActiveList->id();
  tag.clientCount       = clientCount;
  tag.pauseState        = pauseState;
  tag.reTrans           = reTrans;

  memcpy (tag.clientMap,clientMap,sizeof(ID)*MAX_CLIENTS);

  file->write    (&tag,sizeof(tag));

  GetMap()      -> Save (file);
  gameMap       -> Save (file);
  gameTypeTable -> Save (file);
}

void    GameSystem::CreateEnvironment ()
{
  Flag  flag1 ("CREATE_ENVIROMENT",TRUE);

  EnvironmentMap*            map = gameLevel->GetSubjectMap ();
  ResourceHeader             header;
  char                       data [4096];
  CreateExemplar<char,char>* res = (CreateExemplar<char,char>*)&data;

  const char*                oldName = "";
  GameObjects                type    = NULL_OBJECT;

  for (int i=0;i<map->count();i++)
  {
        //Если ресурс не загружен

    Exemplar exempl  = map->GetObject(i);

    if (strcmp(oldName,exempl.resName))
      type = header.Load (oldName=map->GetObject(i).resName,&res->resourceClass);

    if (makeFn[type] == NULL)
    {
      dout<<"Game system: Can't create object. It does not have make function "<<type<<endl;
      continue;
    }

    memcpy (res->paramData,exempl.resourceParam,MAX_RES_PARAM_LEN);

    (*makeFn[type])(res,ID(-1,-1),this);
  }

                //Расстановка стандартных объектов

  int   openSound  = GetRes ("SND_STD_DOOR_OPEN"),
        closeSound = GetRes ("SND_STD_DOOR_CLOSE"),
        textId     = GetRes ("TEXT_STD_DOOR"),
        step       = GetRes ("STD_DOOR_STEP"),
        waitTime   = GetRes ("STD_DOOR_WAIT_TIME"),
        cross      = GetRes ("STD_DOOR_CROSS_STATE");

  if (textId==-1) textId = 0;

  Map*  renderMap  = gameLevel->GetMap();

  for (int y=0;y<MAP_HEIGHT;y++)
      for (int x=0;x<MAP_WIDTH;x++)
          switch (renderMap->GetType (Index(y,x) ))
          {
            case Map::DOOR:
            {
              CreateDoor create;

              create.param().x            = x;
              create.param().y            = y;
              create.param().side         = NORTH; //??????????
              create.resourceClass.textId = textId;
              create.resourceClass.openSound  = openSound;
              create.resourceClass.closeSound = closeSound;
              create.resourceClass.doorCrossState = cross;
              create.resourceClass.doorWaitTime   = waitTime;
              create.resourceClass.doorStep       = step;

              new (DOOR,ID(-1,-1),this) Door (&create);

              break;
            }
          }
}

void    GameSystem::UnloadLevel ()
{
  for (int i=0;i<MAX_TYPES;i++)
    for (int j=0;j<MAX_SLOTS;j++)
    {
      ID id (i,j);

      if (GetAttr(id) & OBJ_UNLOAD_PROTECT || GetObject(id)->type()==ERROR_OBJECT)
         continue;

      dout<<"Unload object "<<id<<" type "<<GetObject(id)->type()<<endl;

      delete GetObject (id);
  }
}

void    GameSystem::ChangeLevel (Level* lev)
{
  Flag  flag1 ("CHANGE_LEVEL",TRUE);

  UnloadLevel();
  gameMap->ClearAll();
  gameMessageQueue->ClearAll();

  flag1 = FALSE;

  if (lev == NULL)      return;
  else                  gameLevel = lev;

  CreateEnvironment ();
}

void    GameSystem::CreateClient (const addrnode_t& addr)
{
  NetPacket           packet;
  ConnectMessage*     connect = (ConnectMessage*)packet.data;
  CreateWeaponPlayer  create;
  CreateFarClient     argClient;
  ResourceHeader      header;

  argClient.addr       = addr;
  argClient.player     = ID(-1,-1);

  connect->id          = CONNECT;
  connect->client      = CreateObject (MULTIPLAYER,&argClient,ID(-1,-1));

  if (connect->client == ID(-1,-1))
  {
     dout<<"Game system: Error create FarClient for "<<addr<<endl;
     return ;
  }

  uint  x = gameLevel->GetRespawn()[0].x,
        y = gameLevel->GetRespawn()[0].y;
  gameLevel->GetMap()->FindClear (x,y);

  create.param().x       = int2fixed(x);
  create.param().y       = int2fixed(y);
  create.param().angle   = 0;
  create.param().health  = PLAYER_HEALTH;
  create.param().protect = PLAYER_PROTECT;

  if (header.Load (DEFAULT_PLAYER,&create.resourceClass) != GAME_MONSTR)
  {
    DeleteObject (connect->client);
    dout<<"Game system: Wrong resource file format "<<DEFAULT_PLAYER<<endl;
    return;
  }

  connect->mainPlayer  =    CreateObject (USER_PLAYER,&create,connect->client);

  if (connect->mainPlayer == ID(-1,-1))
  {
    DeleteObject (connect->client);
    dout<<"Game system: Error create MONKEY for "<<addr<<endl;
    return;
  }

  strcpy (connect->levName,gameLevel->GetLevelFile());

  ((FarClient*)GetObject(connect->client))->player (connect->mainPlayer);

  GetLocal (connect->addr);

  packet.objectId    = connect->client;
  packet.size        = sizeof(packet)-GAME_MESSAGE_MAX_SIZE+sizeof(ConnectMessage);
  packet.callBack    = ID(-1,-1);
  packet.index       = 0;
  packet.socket      = NO_SOCKET;

  SendIPX  (addr,SERVER_WRITE,&packet,packet.size);
}

void    GameSystem::Close ()
{
  HaltSystemMessage    halt;

  Sequence (&halt,sizeof(halt));

  if (gameActiveList)   delete gameActiveList;
  if (gameServer)       delete gameServer;
  if (gameMap)          delete gameMap;
  if (gameMessageQueue) delete gameMessageQueue;
  if (gameTypeTable)    delete gameTypeTable;
  if (gamePool)         delete gamePool;

  gamePool              = NULL;
  gameTypeTable         = NULL;
  gameMap               = NULL;
  gameMessageQueue      = NULL;
  gameActiveList        = NULL;
  gameServer            = NULL;

  gameLevel             = NULL;

  for (int i=0;i<MAX_CLIENTS;i++)
      clientMap [i] = ID (-1,-1);
}

GameSystem::GameSystem(Level* lev,int poolSize,int queueSize)
          : gamePool         (new GamePool(poolSize)),
            gameMessageQueue (new MainQueue (queueSize,MAX_MESSAGES)),
            gameLevel        (lev),
            clientCount      (0),
            pauseState       (FALSE),
            reTrans          (ID(-1,-1))

{
  gameTypeTable       = new (this)                         TypeTable;
  gameMap             = new (this)                         GameMap;
  gameActiveList      = new (ACTIVE_LIST,ID(-1,-1),this)   ActiveList;
  gameServer          = new (GAME_SERVER,ID(-1,-1),this)   GameServer;

  dout<<"III "<<gameMessageQueue<<endl;

        //Устанавливаем атрибуты

  AddAttr (gameActiveList->id(),OBJ_UNLOAD_PROTECT);
  AddAttr (gameServer->id(),OBJ_UNLOAD_PROTECT);

        //Инициализация таблицы клиентов

  for (int i=0;i<MAX_CLIENTS;i++)
      clientMap [i] = ID(-1,-1);

  CreateEnvironment ();
}

GameSystem::GameSystem(Level* lev,BinFile* saveGame,int poolSize,int queueSize)
          : gamePool         (new GamePool(poolSize)),
            gameMessageQueue (new MainQueue (queueSize,MAX_MESSAGES)),
            gameLevel        (lev),
            clientCount      (0),
            pauseState       (FALSE),
            reTrans          (ID(-1,-1))

{
  gameTypeTable       = new (this)                         TypeTable;
  gameMap             = new (this)                         GameMap;
  gameActiveList      = new (ACTIVE_LIST,ID(-1,-1),this)   ActiveList;
  gameServer          = new (GAME_SERVER,ID(-1,-1),this)   GameServer;

  Load (saveGame,lev);
}

GameSystem::~GameSystem ()
{
  Close ();
}

////////////////////implementation of class GameServer/////////////////////

void            GameServer::Load (BinFile* file)
{
}

void            GameServer::Save (BinFile* file)
{
}

BOOL            GameServer::Exec (GameMessage* mes)
{
  switch (mes->id)
  {
        case CREATE_OBJECT:
        {
             __CreateObjectMessage*  message = (__CreateObjectMessage*)mes;

             ID object = owner->CreateObject (message->type,
                                              message->data,
                                              message->creater);

             return TRUE;
        }
        case DELETE_OBJECT:
        {
             DeleteObjectMessage*  message = (DeleteObjectMessage*)mes;
             GameObject*           obj     = owner->GetObject (message->obj);

             owner->DeleteObject (message->obj);

             return TRUE;
        }
        case LOAD_OBJECT:
        {
             _LoadObjectMessage*    message = (_LoadObjectMessage*)mes;

             ID object = owner->LoadObject (message->cacheId,
                                            message->idRes,
                                            message->data,
                                            message->parSize,
                                            message->callBack);

             if (object != ID(-1,-1))
             {
               AlreadyLoadMessage ok (object,message->cacheId,message->idRes);

               owner->PutMessage (&ok,message->callBack,sizeof(ok));
             }

             return TRUE;
        }
        case ADD_IN_ACTIVE:
        {
             AddInActiveMessage*  message = (AddInActiveMessage*)mes;

             owner->AddActive (message->obj,message->priority);

             return TRUE;
        }
        case DEL_FROM_ACTIVE:
        {
             DeleteFromActiveMessage*  message = (DeleteFromActiveMessage*)mes;

             owner->DelActive (message->obj);

             return TRUE;
        }
        case SEQUENCE_OBJECT:
        {
             SequenceObjectMessage*    message = (SequenceObjectMessage*)mes;

             owner->SetBlock (message->mapIndex,message->block);

             return TRUE;
        }
        case ADD_CLIENT:
        {
          AddClientMessage* message = (AddClientMessage*)mes;

          owner->AddClient (message->client);

          return  TRUE;
        }
        case DEL_CLIENT:
        {
          DelClientMessage* message = (DelClientMessage*)mes;

          owner->DelClient (message->client);

          return  TRUE;
        }
        case PLAY_SOUND:
        {
             PlaySoundMessage * message = (PlaySoundMessage*)mes;

             owner->PutEcho  (message->x,message->y,VOLUME_QUAD,mes->callBack);
             owner->Sequence (mes,sizeof(PlaySoundMessage));

             return TRUE;
        }
        case NET_SEQUENCE:
        {
          return ExecNet ((NetPacket*)mes);
        }
        default:        return FALSE;
  }
}

BOOL            GameServer::ExecNet      (NetPacket* packet)
{
  GameMessage* mes = (GameMessage*)packet->data;

  switch (mes->id)
  {
    case SERVER_QUESTION:
    {
         NetPacket             packet;
         GetServerInfoMessage* message = (GetServerInfoMessage*)mes;
         ServerInfoMessage*    info = (ServerInfoMessage*)packet.data;

         info->id = SERVER_INFO;

         strcpy    (info->serverName,"Wolf3D");
         strcpy    (info->levName,owner->GetLevel()->GetLevelName());
         GetLocal  (info->addr);

         info->clientsCount = owner->GetClientsCount();
         info->maxClients   = MULTI_MAX;
         info->server       = id();
         info->objectId     = message->callBack;
         info->callBack     = id();

         packet.objectId    = message->callBack;
         packet.size        = sizeof(packet)-GAME_MESSAGE_MAX_SIZE+sizeof(ServerInfoMessage);
         packet.callBack    = id();
         packet.index       = 0;
         packet.socket      = NO_SOCKET;

         SendIPX (message->clientAddr,SERVER_WRITE,&packet,packet.size);

         return TRUE;
    }
    case CONNECT:
    {
         ConnectMessage* message = (ConnectMessage*)mes;

         owner->CreateClient (message->addr);

         return TRUE;
    }
    default:    return FALSE;
  }
}


void            GameServer::dump (ostream& os) const
{
  os<<"'Wolfenstein 3D: Final' game dispatch"<<endl;
}

GameServer::GameServer  (ArgPtr arg)
{  }

GameServer::GameServer  (BinFile* file)
{
  Load (file);
}


GameServer::~GameServer ()
{  }

////////////////////implementation of class  GameClient///////////////////////

void          GameClient::PutMessage  (GameMessage* mes,ID id,int size,ID cb)
{
  Flag  flag ("GC::PutMessage");

  mes->size           = size;
  mes->callBack       = cb;
  mes->objectId       = id;

  if (mes->objectId == game->id() || game->id() == ID(-1,-1))
    game->Exec (mes);
  else
    game->Send (mes,game->id());
}

void          GameClient::PutMessage  (GameMessage* x,int size,ID cb)
{
  PutMessage (x,game->id(),size,cb);
}

Map*          GameClient::GetMap      () const
{
  return gameLevel->GetMap ();
}

BOOL          GameClient::IsCross     (uint index)
{
  return gameLevel->GetMap()->GetBlock (index)->cross;
}

void          GameClient::SetBlock    (uint index,const Block& block)
{
  gameLevel->GetMap()->SetBlock (index,block);
}

Block*        GameClient::GetBlock    (uint index)
{
  return gameLevel->GetMap()->GetBlock (index);
}

void*         GameClient::CreateObject (GameObjects)
{
  return NULL;
}

void*         GameClient::CreateObject (ID id)
{
  PoolIndex index = gamePool->CreateObject();

  gamePool->GetObject (index)->mID = id;        //wrong! ERROR_TYPE

  return gamePool->GetObject (index);
}

void          GameClient::DisposeObject (ID)
{
  return;
}

void          GameClient::ChangeLevel     (Level* lev)
{
  gameLevel = lev;
}

GameClient::GameClient(MultiGame* _game,addrnode_t addr,Level* lev,int poolSize)
          : gamePool         (new GamePool(poolSize)),
            gameLevel        (lev),
            game             (_game),
            server           (addr)
{     }

GameClient::~GameClient ()
{
  if (gamePool) delete gamePool;
}