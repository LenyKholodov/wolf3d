#ifndef __NET__
#define __NET__

#include "g_netseq.h"
#include "g_netinq.h"
#include "g_object.h"

#include <message.h>
#include <ipx.h>

////////////////////////////////////////////////////////////////////////////
///Сетевой ретранслятор
////////////////////////////////////////////////////////////////////////////
class   Net
{
  public:
                        Net (size_t,
                             addrnode_t toMashine,
                             socket_t   wSocket,
                             socket_t   rSocket);
           virtual      ~Net ();

////////////////////////////////////////////////////////////////////////////
////Принять - послать - период
////////////////////////////////////////////////////////////////////////////
         void          Receive  (NetPacket*,ID callBack = ID(-1,-1));
         GameMessage*  Dispatch ();
         void          Send (GameMessage*,ID dest = ID(-1,-1));
         void          tick ();

         void          Clear ();

  protected:
         addrnode_t     addr;
         socket_t       writeSocket,
                        readSocket;
         NetRing*       ring;
         NetInQueue*    in;
         ulong          writePos,
                        readPos,
                        dispPos;
         ulong          readAnswerPos,
                        writeAnswerPos;
         BOOL           waitPacket;
         clock_t        last;
};

#endif
