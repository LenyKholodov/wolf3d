#ifndef __GAME_NET__
#define __GAME_NET__

#include "t_multi.h"
#include "map.h"

//////////////////////////////////////////////////////////////////////////////
///Сообщение для пересылки по сети с отслеживанием синхронизации
//////////////////////////////////////////////////////////////////////////////
struct  NetPacket: public GameMessage
{
        NetPacket () { id = NET_SEQUENCE; }

        ulong      index;          //Номер пакета
        socket_t   socket;         //На каком сокете принимаетмся call-back
        char       data [GAME_MESSAGE_MAX_SIZE];
};

//////////////////////////////////////////////////////////////////////////////
//Пакет не пришёл
//////////////////////////////////////////////////////////////////////////////
struct  NetNoPacket: public GameMessage
{
        NetNoPacket (ulong _index): index(_index) { id = NET_NO_PACKET; }

        ulong   index;          //Какой пакет не дошёл
};

//////////////////////////////////////////////////////////////////////////////
//Куча пакетов уже пришла - можно удалять
//////////////////////////////////////////////////////////////////////////////
struct  NetOkPacket: public GameMessage
{
        NetOkPacket (ulong _index) : index(_index) { id = NET_OK_PACKET; }

        ulong   index;
};

/////////////////////////////////////////////////////////////////////////////
//Пачка синхронизации
/////////////////////////////////////////////////////////////////////////////
struct  BlockSequence
{
  uint     mapIndex;
  Block    block;
};

const   int      NET_SEQUENCE_PACKET    = (GAME_MESSAGE_MAX_SIZE-sizeof(GameMessage)
                                          -sizeof(int))/sizeof(BlockSequence);

struct  NetMapSequenceMessage: public GameMessage
{
        NetMapSequenceMessage () { id = NET_MAP_SEQUENCE; }

        int             count;
        BlockSequence   table [NET_SEQUENCE_PACKET];
};

struct  NetPreparePacket: public GameMessage
{
        ulong           index;
        socket_t        socket;
        GameMessage*    message;
};

#endif