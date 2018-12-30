#include "net.h"

#include <debug/debug.h>

void       Net::Send (GameMessage* mes,ID _id)
{
  static NetPreparePacket packet;

  packet.id          = NET_SEQUENCE;
  packet.objectId    = _id;//mes->objectId;
  packet.size        = sizeof(NetPacket)-GAME_MESSAGE_MAX_SIZE+mes->size;
  packet.callBack    = mes->callBack;
  packet.index       = writePos++;
  packet.socket      = readSocket;
  packet.message     = mes;

  ring->Add (&packet);
}

void               Net::Receive (NetPacket* mes,ID callBack)
{
  if (mes->id != NET_SEQUENCE)
    return ;

  GameMessage* message = (GameMessage*)mes->data;

  if (message->id == NET_NO_PACKET)
  {
    NetNoPacket* noPak = (NetNoPacket*)mes->data;

    ulong  pos       = noPak->index;
    int    count     = ring->count();
    int    sendCount = ring->count() < NET_ANSWER_COUNT ? ring->count() : NET_ANSWER_COUNT;
    int    send      = 0;

    ring->Reset();

    for (int i=0;i<count && send<sendCount;ring->Next(),i++)
        if (ring->Current())
        {
           if (ring->Current()->index == pos)
           {
                SendIPX (addr,writeSocket,ring->Current(),ring->Current()->size);
                send++;
           }
        }

    return;
  }

  if (message->id == NET_OK_PACKET)
  {
    NetOkPacket* pak = (NetOkPacket*)mes->data;

    ulong  pos   = pak->index;
    int    count = ring->count();

    ring->Reset();

    for (int i=0;i<count;ring->Next(),i++)
        if (ring->Current())
        {
           if (ring->Current()->index<pos)
                ring->Del ();
        }

    writeAnswerPos = pak->index;

    return;
  }

  if (mes->index == readPos)
  {
     if (readPos - readAnswerPos >= NET_ANSWER_COUNT)
     {
       NetPacket      packet;
       NetOkPacket*   ok  = (NetOkPacket*)packet.data;

       packet.size      = sizeof (packet)-GAME_MESSAGE_MAX_SIZE
                          +sizeof(NetOkPacket);
       packet.objectId  = callBack;//mes->callBack;
       packet.callBack  = callBack;//mes->objectId;
       packet.socket    = readSocket;
       packet.index     = readPos;

       ok               = (NetOkPacket*)packet.data;
       ok   ->id        = NET_OK_PACKET;
       ok   ->size      = sizeof (NetOkPacket);
       ok   ->objectId  = callBack;
       ok   ->callBack  = callBack;
       ok   ->index     = readPos;

       SendIPX (addr,writeSocket,&packet,packet.size);

       readAnswerPos    = readPos;
     }

     waitPacket       = FALSE;
     readPos++;

     if (!in->Put (mes))
        dout<<"Net: In buffer is full (1) "<<in->count()<<endl;

     return;
  }
  else if (mes->index > readPos && !waitPacket)// || (waitPacket && clock()-last > NET_NO_PACKET_REQUEST)))
  {
     NetPacket      packet;
     NetNoPacket*   noPak;

     packet.size      = sizeof (packet)-GAME_MESSAGE_MAX_SIZE
                        +sizeof(NetNoPacket);
     packet.objectId  = callBack;//mes->callBack;
     packet.callBack  = callBack;//mes->objectId;
     packet.socket    = readSocket;
     packet.index     = readPos;

     noPak            = (NetNoPacket*)packet.data;
     noPak->id        = NET_NO_PACKET;
     noPak->size      = sizeof (NetNoPacket);
     noPak->objectId  = mes->objectId;
     noPak->callBack  = mes->callBack;
     noPak->index     = readPos;

     waitPacket       = TRUE;

     SendIPX (addr,writeSocket,&packet,packet.size);

     if (!in->Put (mes))
        dout<<"Net: In buffer is full (2.5) "<<in->count()<<endl;
  }
  else if (mes->index < readPos)
     return;
  else
  {
     if (!in->Put (mes))
        dout<<"Net: In buffer is full (2) "<<in->count()<<endl;
  }

  return;
}

GameMessage*    Net::Dispatch ()
{
  if (in->count () == 0 || in->Next() != dispPos)
     return NULL;

  const NetPacket *  pak = in->Get();

  if (readPos == pak->index)
  {
    readPos++;
    waitPacket = FALSE;
  }

  dispPos++;

  return (GameMessage*)pak->data;
}

void       Net::tick ()
{
  int   count     = ring->count();

  ring->Reset();

  for (int i=0;i<count;ring->Next(),i++)
    if (ring->Current() && !ring->already())
    {
       SendIPX (addr,writeSocket,ring->Current(),ring->Current()->size);
       ring->lock (TRUE);
    }
}

void    Net::Clear ()
{
  writePos      = 0;
  readPos       = 0;

  ring->ClearAll ();
  in->Clear();
}

Net::Net (size_t size,addrnode_t _addr,socket_t wSocket,socket_t rSocket)
    : ring (new NetRing (size)),
      in   (new NetInQueue),
      writePos (0), readPos (0), waitPacket (FALSE),
      writeSocket (wSocket), readSocket (rSocket),
      addr (_addr), readAnswerPos (0), writeAnswerPos (0),
      last (clock()), dispPos (0)
{     }

Net::~Net ()
{
  if (ring) delete ring;
  if (in)   delete in;
}
