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
////����� �� ���ଠ�� � �ࢥ�
/////////////////////////////////////////////////////////////////////////////
struct  GetServerInfoMessage: public GameMessage
{
        GetServerInfoMessage (addrnode_t& _addr):clientAddr (_addr) { id = SERVER_QUESTION; }

    addrnode_t    clientAddr;
};

/////////////////////////////////////////////////////////////////////////////
////���ଠ�� � �ࢥ�
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
////��ᮥ������� � �ࢥ��
/////////////////////////////////////////////////////////////////////////////
struct     ConnectMessage: public GameMessage
{
        ConnectMessage () { id = CONNECT; }

        ID            mainPlayer;     //������砥�� ��ப
        ID            client;         //������砥�� ������
        addrnode_t    addr;           //���� �ࢥ� (�⢥�), ������ (�����)
        char          levName [128];  //�������� �஢��
};

struct     ConnectToServerMessage: public GameMessage
{
        ConnectToServerMessage () { id = CONNECT_TO; }

        addrnode_t    addr;
};

/////////////////////////////////////////////////////////////////////////////
////��ᮥ������� �� �ࢥ�
/////////////////////////////////////////////////////////////////////////////
struct     DisconnectMessage: public GameMessage
{
        DisconnectMessage () { id = DISCONNECT; }

        ID      client;         //�⪫�砥�� ������
};

/////////////////////////////////////////////////////////////////////////////
///����� �������� ��ப� Respawn
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
///ONLINE �⢥�
/////////////////////////////////////////////////////////////////////////////
struct  OnlineMessage: public GameMessage
{
                OnlineMessage () { id = ONLINE; }
};

#endif