#include "g_netinq.h"

#include <debug/debug.h>

BOOL        NetInQueue::Put (NetPacket* pak)
{
  int   pos = FindPos (pak->index);

  if (pos == -1)
     return FALSE;

  memcpy (&mBuffer [pos].packet,pak,pak->size);
  mPrev                = pos;

  if (pak->index > mBuffer [mLast].packet.index)
     mLast             = pos;
  if (pak->index < mBuffer [mFirst].packet.index)
     mFirst            = pos;

  mCount++;

  return TRUE;
}

const  NetPacket*  NetInQueue::Get ()
{
  if (mFirst == -1)
    return NULL;

  const NetPacket* pak = &mBuffer [mFirst].packet;

  if (mCount != 1)
  {
    int next              = mBuffer [mFirst].next;
    Free (mFirst);
    mFirst                = next;
    mBuffer [mFirst].prev = -1;
  }
  else
  {
    Free (mFirst);
    mFirst = mPrev = mLast  = -1;
  }

  mCount--;

  return pak;
}

ulong       NetInQueue::Next () const
{
  if (mFirst == -1)
    return (ulong)-1;

  const NetPacket* pak = &mBuffer [mFirst].packet;

  return pak->index;
}

int         NetInQueue::FindPos (int index)
{
  if (mFirst == -1 || mPrev == -1 || mLast == -1)
  {
    mFirst = mLast = mPrev = Alloc ();

    mBuffer [mFirst].prev = -1;
    mBuffer [mFirst].next = -1;

    return mFirst;
  }
  else
  {
    int  firstSum = abs ((int)mBuffer[mFirst].packet.index-index),
         lastSum  = abs ((int)mBuffer[mLast].packet.index-index),
         prevSum  = abs ((int)mBuffer[mPrev].packet.index-index);

    if       (firstSum < lastSum && firstSum < prevSum)
      return FindPos (mFirst,index);
    if  (lastSum  < firstSum && lastSum < prevSum)
      return FindPos (mLast,index);
    else
      return FindPos (mPrev,index);     //opt
  }
}

int         NetInQueue::FindPos (int pos,int index)
{
  int   i;

  if (mBuffer [pos].packet.index < index)
  {
        //next
    for (int i=pos;i!=-1;i=mBuffer[i].next)
    {
        if (mBuffer [i].packet.index == index)
        {
           dout<<"0"<<endl;
           return -1;
        }
        if (mBuffer [i].packet.index < index && mBuffer [i].next != -1 &&
            mBuffer [mBuffer [i].next].packet.index > index)
        {
            int tmp = Alloc ();

            if (tmp == -1)
            {
               dout<<"1"<<endl;
               return -1;
            }

            mBuffer [tmp].next             = mBuffer [i].next;
            mBuffer [tmp].prev             = i;
            mBuffer [mBuffer[i].next].prev = tmp;
            mBuffer [i].next               = tmp;

            return tmp;
        }
        else if (mBuffer [i].next == -1)
        {
          mBuffer [i].next = Alloc ();

          if (mBuffer [i].next == -1)
          {
               dout<<"2"<<endl;
             return -1;
          }

          mBuffer [mBuffer [i].next].prev = i;
          mBuffer [mBuffer [i].next].next = -1;

          return mBuffer [i].next;
        }
    }
  }
  else
  {
        //prev
    for (int i=pos;i!=-1;i=mBuffer[i].prev)
    {
        if (mBuffer [i].packet.index == index)
        {
           dout<<"2.5"<<endl;
           return -1;
        }
        if (mBuffer [i].packet.index > index && mBuffer [i].prev != -1 &&
            mBuffer [mBuffer [i].prev].packet.index < index)
        {
          int tmp = Alloc ();

          if (tmp == -1)
          {
             dout<<"3"<<endl;
             return -1;
          }

          mBuffer [tmp].next             = i;
          mBuffer [tmp].prev             = mBuffer [i].prev;
          mBuffer [mBuffer[i].prev].next = tmp;
          mBuffer [i].prev               = tmp;

          return tmp;
        }
        else if (mBuffer [i].prev == -1)
        {
          mBuffer [i].prev = Alloc ();

          if (mBuffer[i].prev == -1)
          {
             dout<<"4"<<endl;
             return -1;
          }

          mBuffer [mBuffer [i].prev].prev = -1;
          mBuffer [mBuffer [i].prev].next = i;

          return mBuffer [i].prev;
        }
    }
  }

  dout<<"5"<<endl;

  return -1;
}

int         NetInQueue::Alloc   ()
{
  int _free=mFirstFree;

  if (mFirstFree != -1)
      mFirstFree = mBuffer [mFirstFree].next;

  return _free;
}

void        NetInQueue::Free    (int node)
{
  if (node == -1)
     dout<<"HALT"<<endl;

  mBuffer[node].next = mFirstFree;
  mFirstFree         = node;
}

void        NetInQueue::Clear   ()
{
  mFirstFree = 0;
  mFirst     = -1;
  mPrev      = -1;
  mLast      = -1;

  for (int j=0;j<mMax;j++)
     mBuffer [j].next = j+1;

  mBuffer [j-1].next = -1;
}

void    NetInQueue::dump ()
{
  dout<<"Net in queue: Net in queue count "<<mCount<<endl;

  for (int i=mFirst;i!=-1;i=mBuffer[i].next)
      dout<<mBuffer[i].packet.index<<endl;
}

NetInQueue::NetInQueue (int size)
           : mBuffer (new NetInNode [size]),
             mLast (-1), mFirst (-1), mPrev (-1),
             mMax    (size), mCount (0), mFirstFree (0)
{
  Clear ();
}

NetInQueue::~NetInQueue ()
{
  if (mBuffer) delete mBuffer;
}
