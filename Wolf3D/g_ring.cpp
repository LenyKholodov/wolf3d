#include "g_ring.h"
#include "g_pool.h"
#include "g_system.h"

#include <debug\debug.h>
#include <string.h>


//////////////////////implementation of class Ring///////////////////////////

void     Ring::Load (BinFile* file)
{
  GameObject::Load (file);

  RingTag       tag;

  file->read    (&tag,sizeof(tag));

  mFirst        = tag.mFirst;
  mFirstFree    = tag.mFirstFree;
  mCurrent      = tag.mCurrent;
  mCount        = tag.mReadSize / sizeof(RingNode);

  if (mPool) delete mPool;

  mPool = new RingNode [mCount];

  if (mPool)
    file->read (mPool,tag.mReadSize);
  else
    dout<<"Ring: Error! Too large load table"<<endl;
}

void     Ring::Save (BinFile* file)
{
  GameObject::Save (file);

  RingTag       tag;

  tag.mFirst        = mFirst;
  tag.mFirstFree    = mFirstFree;
  tag.mCurrent      = mCurrent;
  tag.mReadSize     = mCount*sizeof(RingNode);

  file->write (&tag,sizeof(tag));
  file->write (mPool,tag.mReadSize);
}

BOOL     Ring::Exec (GameMessage* mes)
{
  Reset();

  do
  {
    owner->PutMessage (mes,Next(),mes->size,mes->callBack);
  } while (More());

  return TRUE;
}

BOOL     Ring::IsBelong    (ID x) const
{
  if (mFirst == -1)
     return FALSE;

  if (x == mPool[mFirst].id)
     return TRUE;

  for (int i = mPool [mFirst].next;i!=mFirst;i = mPool [i].next)
    if (mPool[i].id == x)
       return TRUE;

  return FALSE;
}

void     Ring::dump (ostream& os) const
{
  os<<"Ring list"<<endl;

  if (mFirst == -1)
     os<<"This ring is empty"<<endl;
  else
  {
    ID    id = mPool[mFirst].id;

    os<<"ID ("<<id.type<<","<<id.id<<")"<<endl;


    for (int i = mPool [mFirst].next;i!=mFirst;i  = mPool [i].next)
    {
      id = mPool[i].id;

      os<<"ID ("<<id.type<<","<<id.id<<")"<<endl;
    }
  }
}

BOOL     Ring::More () const
{
  return (mCurrent != -1) ? (mPool[mCurrent].next != mFirst) : FALSE;
}

ID  Ring::Next ()
{
  ID x = mPool [mCurrent].id;

  mCurrent = mPool [mCurrent].next;

  return x;
}

ID  Ring::Prev ()
{
  ID x = mPool [mCurrent].id;

  mCurrent = mPool [mCurrent].prev;

  return x;
}

ID  Ring::Current () const
{
  if (mCurrent == -1)
  {
    ID  id;

    id.type = -1;
    id.id   = -1;

    return id;
  }
  else return mPool [mCurrent].id;
}

void  Ring::Reset()
{
  mCurrent = mFirst;
}

int      Ring::Add         (ID id)
{
  if (mFirstFree == -1)    return -1;
  else if (mCurrent == -1)
  {
                //Первый раз
    mPool [mFirstFree].next  = mPool [mFirstFree].prev =
                mFirst       = mCurrent = mFirstFree;
  }
  else
  {
    mPool [mPool [mCurrent].next].prev  = mFirstFree;
    mPool [mFirstFree].prev             = mCurrent;
    mPool [mFirstFree].next             = mPool [mCurrent].next;
    mPool [mCurrent].next               = mFirstFree;
  }

  mPool [mFirstFree].id               = id;
  mFirstFree                          = mPool [mFirstFree].nextFree;

  return mPool[mCurrent].next;
}

void     Ring::Del  ()
{
  if (mCurrent == -1) return;

  mPool [mCurrent].nextFree          = mFirstFree;
  mFirstFree                         = mCurrent;

  if (mPool[mCurrent].next == mPool[mCurrent].prev)
    mCurrent = mFirst = -1;
  else
  {
    if (mCurrent == mFirst) mFirst = mPool [mCurrent].next;

    mPool [mPool [mCurrent].next].prev = mPool [mCurrent].prev;
    mPool [mPool [mCurrent].prev].next = mPool [mCurrent].next;

    mCurrent                           = mPool [mCurrent].next;
  }
}

void    Ring::Del (ID id)
{
  int   pos = GetPos();

  do
  {
    if (Current().id == id.id && Current().type == id.type)
    {
      Del();
      pos = GetPos();
//      break;
    }

    Next();
  } while (pos != GetPos());

}

void     Ring::ClearAll    ()
{
  mFirstFree = 0;
  mFirst     = -1;
  mCurrent   = -1;

  for (int j=0;j<mCount;j++)
     mPool [j].nextFree = j+1;

  mPool [j-1].nextFree = -1;
}

Ring::Ring (int max)
    : mFirstFree (0), mFirst (-1), mCurrent (-1),
      mPool (NULL), mCount (max)
{
  mPool = new RingNode [mCount];
  ClearAll ();
}

Ring::Ring (BinFile* file)
     : mPool (NULL), mCount (0)
{ Load (file); }

Ring::~Ring ()
{
  if (mPool) delete mPool;
}

