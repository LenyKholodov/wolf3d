#ifndef __NET_RING__
#define __NET_RING__

#include "g_types.h"

struct  NetRingNode
{
  int16         next,
                prev;
  BOOL          inUse;
  BOOL          already;        //Уже послан

  int16         nextFree;
  NetPacket     message;
};

/////////////////////////////////////////////////////////////////////////////
/////Закольцованный итератор
/////////////////////////////////////////////////////////////////////////////
struct  NetRingNode;
class   NetRing
{
  public:
                                NetRing (int max = MAX_NET_SEQUENCE) ;
                  virtual       ~NetRing ();

////////////////////////////////////////////////////////////////////////////
//////Работа с итератором
////////////////////////////////////////////////////////////////////////////
                BOOL            More ()   const;
                BOOL            IsEmpty() const { return mFirst == -1;  }

                NetPacket*      Next ();       //in active iterator
                NetPacket*      Prev ();       //in active iterator
                NetPacket*      Current () const;

                int             Add         (NetPreparePacket*);
                void            Del         (int);
                void            Del         () { Del (mCurrent); }
                void            ClearAll    ();

                void            Reset       (int x);
                void            Reset       () { Reset (mFirst); }

                int             count () const { return mActiveCount; }
                void            dump  (ostream&) const;

                BOOL            already (int) const;
                BOOL            already () const { return already(mCurrent); }
                void            lock    (int,BOOL);
                void            lock    (BOOL x) { lock(mCurrent,x); }


  protected:
             int                mCount;
             int                mActiveCount;
             int                mFirst;
             int                mFirstFree;
             int                mCurrent;
             NetRingNode*       mPool   ;
};

#endif
