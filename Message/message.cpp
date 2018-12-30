#include <message.h>
//#include <fin.h>
#include <misc.h>

#include <debug/debug.h>

//////////////////////implementation of class HandlerPtr/////////////////////

HandlerPtr         message_queue;

HandlerPtr::HandlerPtr(): ptr (new MessageHandler(0xFFF,0xFF)) {}
HandlerPtr::~HandlerPtr() { delete ptr; }

HandlerPtr::operator = (int size)
{
   delete ptr;
   ptr = new MessageHandler(size);
}

void      HandlerPtr::Recreate (int pool_size,int max_mes)
{
  if (ptr) delete ptr;
  ptr = new MessageHandler (pool_size,max_mes);
}

void      HandlerPtr::SetPool(void* p,int size)
{
  delete ptr;
  ptr = new MessageHandler(p,size);
}

void      HandlerPtr::PutMessage (const Message* m,int size)
{
  ptr->PutMessage(m,size);
}

void      HandlerPtr::PutMessage (const Message far* m,int size)
{
  ptr->PutMessage(m,size);
}

const Message* HandlerPtr::GetMessage ()
{
  return ptr->GetMessage();
}

BOOL     HandlerPtr::IsEmpty() const
{
  return ptr->IsEmpty();
}

void     HandlerPtr::Clear()
{
  ptr->Clear();
}

////////////////////implementation of class MessageList//////////////////////

struct  MessageList
{
  MessageList*  next;
  uchar*        data;
};

////////////////////implementation of class MessageHandler/////////////////////

void*  MessageHandler::Alloc(long size)
{
  ulong writePos = (ulong)(nextPos       - pool);       //writePtr - pool
  ulong readPos  = (ulong)(readPtr->data - pool);

  if ( (writePos >= readPos && writePos+size<pool_size) ||
       (writePos <  readPos && writePos+size<readPos))
  {
     nextPos   += size;
     return    nextPos-size;
  }
  else if (size<readPos)
  {
     nextPos = pool+size;
     return    pool;
  }

  return NULL;
}

BOOL  MessageHandler::PutMessage(const Message* message,int size)
{
  if (writePtr == NULL)
     return FALSE;

  writePtr->next     = (MessageList*)listPool->AllocBlock();
  writePtr->data     = (uchar*)Alloc (size);

  if (writePtr->data == NULL)
  {
     dout<<"Message: Message queue is full (1) "<<this<<endl;
     listPool->FreeBlock (writePtr->next);
     return FALSE;
  }
  if (writePtr->next == NULL)
  {
     dout<<"Message: Message queue if full (2) "<<this<<endl;
     return FALSE;
  }

  Message* dest = (Message*)writePtr->data;

  writePtr      = writePtr->next;

//  memcpy(dest,message,size); //????

    int i = 0;
    uchar*  src   = (uchar*)message;
    uchar*  _dest = (uchar*)dest;
    for (;i<size;i++)
        *_dest++ = *src++;

  dest->size = size;
//  wait       = FALSE;

  return TRUE;
}

BOOL  MessageHandler::PutMessage(const Message far* message,int size)
{
  if (writePtr == NULL)
     return FALSE;

  writePtr->next     = (MessageList*)listPool->AllocBlock();
  writePtr->data     = (uchar*)Alloc (size);

  if (writePtr->data == NULL)
  {
     dout<<"Message: Message queue is full (1-far) "<<this<<endl;
     listPool->FreeBlock (writePtr->next);
     return FALSE;
  }
  if (writePtr->next == NULL)
  {
     dout<<"Message: Message queue if full (2-far) "<<this<<endl;
     return FALSE;
  }

  Message* dest = (Message*)writePtr->data;

  writePtr      = writePtr->next;

  _fmemcpy(dest,message,size);

  dest->size    = size;

  return TRUE;
}

const Message* MessageHandler::GetMessage()
{
  if (IsEmpty())  return NULL;

  const Message* tmp = (Message*)readPtr->data;
  MessageList*   del = readPtr;
  readPtr            = readPtr->next;

  listPool->FreeBlock (del);

  return tmp;
}

BOOL    MessageHandler::IsEmpty() const
{
  return readPtr == writePtr;
}

void    MessageHandler::Clear()
{
  while (!IsEmpty())
        GetMessage ();
}

MessageHandler::MessageHandler(long pool_sz,long max)
               : listPool (new Pool(max+1,sizeof(MessageList))),
                 pool_size (pool_sz),
                 pool (new uchar [pool_sz]),
                 wait (FALSE)
{
  if (pool == NULL || listPool == NULL)
  {
    dout<<"Message: No enough memory for system buffers"<<endl;
//    delete fin;
    exit (-1);
  }

  nextPos = pool;
  readPtr = writePtr = (MessageList*)listPool -> AllocBlock ();

  readPtr->data = pool;
  readPtr->next = NULL;
}

MessageHandler::MessageHandler(void* p,long pool_sz,long max)
               :pool_size(pool_sz),
                pool((uchar*)p),
                nextPos ((uchar*)p),
                listPool (new Pool(max+1,sizeof(MessageList))),
                wait (FALSE)

{
  if (pool == NULL || listPool == NULL)
  {
    dout<<"Message: No enough memory for system buffers"<<endl;
//    delete fin;
    exit (-1);
  }

  readPtr = writePtr = (MessageList*)listPool -> AllocBlock ();

  readPtr->data = pool;
  readPtr->next = NULL;
}

MessageHandler::~MessageHandler()
{
  delete pool;
  delete listPool;
}