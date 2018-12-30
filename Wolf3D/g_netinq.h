#ifndef __NET_IN_QUEUE__
#define __NET_IN_QUEUE__

#include "t_net.h"

struct  NetInNode
{
  int           next, prev;
  NetPacket     packet;
};

/////////////////////////////////////////////////////////////////////////////
////Буфер сетевого приёма
/////////////////////////////////////////////////////////////////////////////
class    NetInQueue
{
  friend class Net;
  public:
                        NetInQueue (int size = NET_IN_QUEUE_SIZE);
            virtual     ~NetInQueue ();

            BOOL        Put  (NetPacket*);
   const    NetPacket*  Get  ();
            ulong       Next () const;

            int         count () const { return mCount; }
            void        dump ();

  protected:
            int         FindPos (int index);
            int         FindPos (int pos,int index);

            int         Alloc   ();
            void        Free    (int);
            void        Clear   ();

  private:
           int          mFirst, mLast, mPrev;
           int          mFirstFree;
           int          mCount, mMax;
           NetInNode*   mBuffer ;
};

#endif