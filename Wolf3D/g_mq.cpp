#include "g_mq.h"
#include "g_pool.h"
#include "g_ttable.h"

#include <debug\debug.h>

void     MainQueue::Load (BinFile* file)
{
  GameObject::Load (file);
  ClearAll ();
}

void     MainQueue::Save (BinFile* file)
{
  GameObject::Save (file);
}

void    MainQueue::dump (ostream& os) const
{
  os<<"No implementation"<<endl;
}

void     MainQueue::PutMessage (GameMessage* mes,ID id,int size,ID cb)
{
#ifndef NDEBUG
  if (id.type < 0 || id.type > MAX_TYPES ||
      id.id   < 0 || id.id   > MAX_SLOTS)
     return;
#endif
  mes->objectId = id;
  mes->callBack = cb;

  MessageHandler::PutMessage (mes,size);
}

MainQueue::MainQueue (long size,long maxMessages)
          :MessageHandler (size,maxMessages), GameObject()
{  }

MainQueue::MainQueue (BinFile* file)
          :MessageHandler (0l), GameObject ()
{ Load(file); }
