#include "g_alist.h"
#include "g_system.h"

make<ActiveList,ACTIVE_LIST>   makeActiveList;

void     ActiveList::Load (BinFile* file)
{
  Ring::Load (file);

  if (mPriority) delete mPriority;

  mPriority = new uchar [count()];

  file->read (&mPos,sizeof(mPos));
  file->read (mPriority,count()*sizeof(uchar));
}

void     ActiveList::Save (BinFile* file)
{
  Ring::Save (file);

  file->write (&mPos,sizeof(mPos));
  file->write (mPriority,count()*sizeof(uchar));
}

BOOL     ActiveList::Exec (GameMessage* mes)
{
  switch (mes->id)
  {
    case NOT_PROCESS:
    {
      GameNotProcess* message = (GameNotProcess*)mes;

      Del (message->obj);

      return TRUE;
    }
    default: return Ring::Exec(mes);
  }
}

void     ActiveList::NextTick ()
{
  mPos = (mPos++) & PRIOR_MASK;

  if (!IsEmpty())
  {
    GameTickMessage  seq ;

    if (mPriority[Current()] >= mPos)
      owner->PutMessage (&seq,Current(),sizeof(GameTickMessage),id());

    Next();
  }
}

void     ActiveList::TickAll ()
{
  if (!IsEmpty())
     if (mPool[mFirst].next == mFirst)
        NextTick();
     else
     {
       while (More())  NextTick();
       NextTick();
     }
}

int     ActiveList::Add (ID x,priority_t prior)
{
  if (!IsBelong (x))
  {
    int _add = Ring::Add (x);

    if (_add == -1)
       return -1;

    mPriority [_add] = prior;

    return _add;
  }
  else
  {
    for (int i = mPool [mFirst].next;mPool[i].id!=x;i = mPool [i].next);

    mPriority [i] = prior;
  }

  return -1;
}

ActiveList::ActiveList (int max)
           : Ring (max), mPriority (new uchar [max]),
             mPos (0)
{
}

ActiveList::ActiveList (ArgPtr)
          : Ring(), mPriority (NULL), mPos (0)
{
  mPriority = new uchar [count()];
}

ActiveList::ActiveList (BinFile* file)
          : mPriority (NULL)
{
  Load (file);
}

ActiveList::~ActiveList ()
{
  if (mPriority) delete mPriority;
}

