#include <message.h>
#include <fin.h>
#include <misc.h>

#include <debug.h>

//////////////////////implementation of class HandlerPtr/////////////////////

HandlerPtr         message_queue;

HandlerPtr::HandlerPtr(): ptr (new MessageHandler(0xFFF,0xFF)) {}
HandlerPtr::~HandlerPtr() { delete ptr; }

HandlerPtr::operator = (int size)
{
   delete ptr;
   ptr = new MessageHandler(size);
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
//  if (wait) return FALSE;
//  else      wait = TRUE;

  MessageList* nextBlock =  (MessageList*)listPool->AllocBlock();

  if (nextBlock == NULL)
  {
//     wait = FALSE;
     return FALSE;
  }

  nextBlock->next    = NULL;
  nextBlock->data    = (uchar*)Alloc (size);

  if (nextBlock->data == NULL)
  {
     listPool->FreeBlock (nextBlock);
//     wait = FALSE;
     return FALSE;
  }

  writePtr      = writePtr->next = nextBlock;

  Message* dest = (Message*)nextBlock->data;

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
    delete fin;
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
    delete fin;
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