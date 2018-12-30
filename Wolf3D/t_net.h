#ifndef __GAME_NET__
#define __GAME_NET__

#include "t_multi.h"
#include "map.h"

//////////////////////////////////////////////////////////////////////////////
///����饭�� ��� ����뫪� �� �� � ��᫥�������� ᨭ�஭���樨
//////////////////////////////////////////////////////////////////////////////
struct  NetPacket: public GameMessage
{
        NetPacket () { id = NET_SEQUENCE; }

        ulong      index;          //����� �����
        socket_t   socket;         //�� ����� ᮪�� �ਭ������� call-back
        char       data [GAME_MESSAGE_MAX_SIZE];
};

//////////////////////////////////////////////////////////////////////////////
//����� �� ����
//////////////////////////////////////////////////////////////////////////////
struct  NetNoPacket: public GameMessage
{
        NetNoPacket (ulong _index): index(_index) { id = NET_NO_PACKET; }

        ulong   index;          //����� ����� �� ����
};

//////////////////////////////////////////////////////////////////////////////
//��� ����⮢ 㦥 ��諠 - ����� 㤠����
//////////////////////////////////////////////////////////////////////////////
struct  NetOkPacket: public GameMessage
{
        NetOkPacket (ulong _index) : index(_index) { id = NET_OK_PACKET; }

        ulong   index;
};

/////////////////////////////////////////////////////////////////////////////
//��窠 ᨭ�஭���樨
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