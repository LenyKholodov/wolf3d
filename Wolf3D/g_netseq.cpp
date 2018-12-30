#include "g_netseq.h"
#include "cons.h"

#include <debug\debug.h>
#include <string.h>


//////////////////////implementation of class NetNetRing///////////////////////////

BOOL     NetRing::More () const
{
//  return (mCurrent != -1) ? (mPool[mCurrent].next != mFirst) : FALSE;
  return mCurrent!=-1;
}

NetPacket*  NetRing::Next ()
{
  Flag  flag ("NET_RING_NEXT");

  if (mCurrent >= mCount || mCurrent<0)
     return NULL;

  NetPacket* x = &mPool [mCurrent].message;

  mCurrent = mPool [mCurrent].next;

  return x;
}

NetPacket*  NetRing::Prev ()
{
  if (mCurrent >= mCount || mCurrent<0)
     return NULL;

  NetPacket* x = &mPool [mCurrent].message;

  mCurrent = mPool [mCurrent].prev;

  return x;
}

NetPacket*  NetRing::Current () const
{
  Flag  flag ("NET_RING_CURRENT");

  if (mCurrent <0 || mCurrent>mCount || !mPool[mCurrent].inUse)
    return NULL;
  else
    return &mPool [mCurrent].message;
}

int      NetRing::Add         (NetPreparePacket* id)
{
  if (mFirstFree == -1)
  {
    grout<<"Net queue is full"<<endl;
    return -1;
  }
  else if (mCurrent == -1)
  {
                //Первый раз
    mPool [mFirstFree].next  = mPool [mFirstFree].prev
                             = mFirst = mCurrent = mFirstFree;
  }
  else
  {
    mPool [mPool [mFirst].prev].next    = mFirstFree;
    mPool [mFirstFree].prev             = mPool [mFirst].prev;
    mPool [mFirstFree].next             = mFirst;
    mPool [mFirst].prev                 = mFirstFree;
  }

  mPool[mFirstFree].message.id       = NET_SEQUENCE;
  mPool[mFirstFree].message.size     = id->size;
  mPool[mFirstFree].message.index    = id->index;
  mPool[mFirstFree].message.socket   = id->socket;
  mPool[mFirstFree].message.objectId = id->objectId;
  mPool[mFirstFree].message.callBack = id->callBack;

  if (id->message->size >= GAME_MESSAGE_MAX_SIZE)
  {
     dout<<"Net ring: Error in message size "<<id->message->size<<endl;;
     id->message->size = GAME_MESSAGE_MAX_SIZE;
  }

  memcpy (mPool [mFirstFree].message.data,id->message,id->message->size);

  mPool [mFirstFree].inUse            = TRUE;
  mPool [mFirstFree].already          = FALSE;
  mFirstFree                          = mPool [mFirstFree].nextFree;

  mActiveCount++;

  return mPool[mFirst].next;
}

void     NetRing::dump (ostream& os) const
{
  os<<"Ring list"<<endl;

  if (mFirst == -1)
     os<<"This ring is empty"<<endl;
  else
  {
    for (int i = mPool [mCurrent].next;i!=mCurrent;i=mPool [i].next)
    {
      os<<mPool[i].message.index<<endl;
    }
  }
}

void     NetRing::Del  (int node)
{
  Flag  flag ("NET_RING_DEL");

  if (node == -1 || !mPool [node].inUse) return;

  mPool [node].nextFree              = mFirstFree;
  mPool [node].inUse                 = FALSE;
  mFirstFree                         = node;

  if (mPool[node].next == mPool[node].prev)
    mCurrent = mFirst = -1;
  else
  {
    if (node == mCurrent)
       mCurrent  = mPool [mCurrent].prev;

    if (node == mFirst)
       mFirst = mPool[mFirst].next;

    mPool [mPool [node].next].prev = mPool [node].prev;
    mPool [mPool [node].prev].next = mPool [node].next;
  }

  --mActiveCount;
}

BOOL    NetRing::already (int node) const
{
  if (node <0 || node>mCount || !mPool[node].inUse)
    return TRUE;
  else
    return mPool [node].already;
}

void    NetRing::lock (int node,BOOL flag)
{
  if (node <0 || node>mCount || !mPool[node].inUse)
    return ;
  else
    mPool [node].already = flag;
}

void     NetRing::Reset       (int x)
{
  Flag  flag ("NET_RING_RESET");

  if (x<0 || x>=mCount)
     return;

  if (!mPool [x].inUse)
     return;

  mCurrent = x;
}

void     NetRing::ClearAll    ()
{
  mFirstFree = 0;
  mFirst     = -1;
  mCurrent   = -1;

  for (int j=0;j<mCount;j++)
  {
     mPool [j].nextFree = j+1;
     mPool [j].inUse    = FALSE;
  }

  mPool [j-1].nextFree = -1;
}

NetRing::NetRing (int max)
    : mFirstFree (0), mCurrent (-1),
      mPool (NULL), mCount (max), mActiveCount (0), mFirst (-1)
{
  mPool = new NetRingNode [mCount];
  ClearAll ();
}

NetRing::~NetRing ()
{
  if (mPool) delete mPool;
}

