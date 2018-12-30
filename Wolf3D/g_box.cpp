#include "g_box.h"
#include "g_system.h"
#include "map.h"

GameBox        gigaBox (-1,-1);

make<GameBox,GAME_BOX>          makeBox;

void            GameBox::Load (BinFile* file)
{
  GameBoxTag    tag;

  file->read (&tag,sizeof(tag));

  mCount = tag.count;
  mMax   = tag.max;
}

void            GameBox::Save (BinFile* file)
{
  GameBoxTag    tag;

  tag.count = mCount;
  tag.max   = mMax;

  file->write (&tag,sizeof(tag));
}

BOOL           GameBox::Exec (GameMessage* mes)
{
  switch (mes->id)
  {
    case SET_STORE: SetStore (((SetBoxStoreMessage*)mes)->store); return TRUE;
    case GET_STORE:
    {
      ID object = ((GetBoxStoreMessage*)mes)->object;

      BoxStoreStateMessage state (mCount);

      owner->PutMessage (&state,object,sizeof(state));

      return TRUE;
    }
    default: return FALSE;
  }
}

void            GameBox::dump (ostream& os) const
{
  os<<"Game box "<<type()<<" ("<<count()<<")"<<endl;
}

void            GameBox::AddStore      (int x)
{
  if (mCount != -1 && mCount+x<mMax)
     mCount += x;
  else if (mCount != -1 && mCount+x>mMax)
     mCount = mMax;
}

void            GameBox::SetStore      (int x)
{
  if (mCount!=-1 && x<mMax)
     mCount = x;
}

void            GameBox::SubStore ( int c )
{
  mCount -= c;

  if      (mCount<0 && mCount+c>=0)     mCount = 0;
  else if (mCount<0)                    mCount = -1;
}

GameBox::GameBox (int cnt,int max)
        : mCount (cnt), mMax (max)
{    }

GameBox::GameBox  (ArgPtr ptr)
        : mCount (*(int*)ptr), mMax (-1)
{    }

GameBox::GameBox  (BinFile* file)
{
  Load (file);
}

GameBox::~GameBox ()
{        }
