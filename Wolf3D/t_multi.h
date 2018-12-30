#ifndef __MULTIPLAYER__
#define __MULTIPLAYER__

#include "t_defs.h"
#include "t_mes.h"

#include <ipx.h>

struct NetPacket;

extern  void    SendIPX  (const addrnode_t&,
                          socket_t,
                          NetPacket*,
                          size_t);
extern  void    GetLocal (addrnode_t&);
extern  void    NetTick  ();

enum
{
  SERVER_QUESTION = MULTI_LIMIT,
  SERVER_INFO,
  CONNECT,
  CONNECT_TO,
  DISCONNECT,
  RESPAWN,
  ONLINE,
  NET_NO_PACKET = 0x500,
  NET_SEQUENCE,
  NET_OK_PACKET,
  NET_MAP_SEQUENCE
};

/////////////////////////////////////////////////////////////////////////////
////Запрос на информацию о сервере
/////////////////////////////////////////////////////////////////////////////
struct  GetServerInfoMessage: public GameMessage
{
        GetServerInfoMessage (addrnode_t& _addr):clientAddr (_addr) { id = SERVER_QUESTION; }

    addrnode_t    clientAddr;
};

/////////////////////////////////////////////////////////////////////////////
////Информация о сервере
/////////////////////////////////////////////////////////////////////////////
struct     ServerInfoMessage: public GameMessage
{
        ServerInfoMessage () { id = SERVER_INFO; }

        ID             server;
        int            clientsCount, maxClients;
        addrnode_t     addr;
        char           serverName [16];
        char           levName    [128];
};

/////////////////////////////////////////////////////////////////////////////
////Присоединение к серверу
/////////////////////////////////////////////////////////////////////////////
struct     ConnectMessage: public GameMessage
{
        ConnectMessage () { id = CONNECT; }

        ID            mainPlayer;     //Подключаемый игрок
        ID            client;         //Подключаемый клиент
        addrnode_t    addr;           //Адрес сервер (ответ), клиента (запрос)
        char          levName [128];  //Название уровня
};

struct     ConnectToServerMessage: public GameMessage
{
        ConnectToServerMessage () { id = CONNECT_TO; }

        addrnode_t    addr;
};

/////////////////////////////////////////////////////////////////////////////
////Отсоединение от сервера
/////////////////////////////////////////////////////////////////////////////
struct     DisconnectMessage: public GameMessage
{
        DisconnectMessage () { id = DISCONNECT; }

        ID      client;         //Отключаемый клиент
};

/////////////////////////////////////////////////////////////////////////////
///Смена главного игрока Respawn
/////////////////////////////////////////////////////////////////////////////
struct     RespawnPlayerMessage: public GameMessage
{
        RespawnPlayerMessage (ID mp): mainPlayer (mp) { id = RESPAWN; }

        ID      mainPlayer;
};

struct  CreateFarClient
{
  addrnode_t    addr;
  ID            player;
};

/////////////////////////////////////////////////////////////////////////////
///ONLINE ответ
/////////////////////////////////////////////////////////////////////////////
struct  OnlineMessage: public GameMessage
{
                OnlineMessage () { id = ONLINE; }
};

#endif