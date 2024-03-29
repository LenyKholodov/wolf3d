#include <message.h>

#include <misc.h>

#include <debug.h>

HandlerPtr         message_queue;

HandlerPtr::HandlerPtr(): ptr (new MessageHandler(0xFF)) {}
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

void*  MessageHandler::Alloc(long size)
{

  last += ((Message*)last)->size;

  if ((ulong)(last-pool+size)<pool_size)
     return last;
  else if (pool+size<first)
  {
     ((Message*)last)->size = pool_size;     //too large

     return last = pool;
  }
  else
     return NULL;
}

BOOL    MessageHandler::PutMessage(const Message* message,int size)
{
  Message* dest = (Message*)Alloc(size);

  if (dest == NULL)
     return FALSE;

//  _memcpy(dest,message,size); //????
    int i = 0;
    uchar*  src   = (uchar*)message;
    uchar*  _dest = (uchar*)dest;
    for (;i<size;i++)
        *_dest++ = *src++;

  dest->size = size;

  return TRUE;
}

const Message* MessageHandler::GetMessage()
{
  if (IsEmpty())
     return NULL;

  const Message* tmp = (Message*)first;

  if (first-pool+tmp->size<pool_size)
  {
     first += tmp->size;
     return tmp;
  }
  else
     return (const Message*)(first = pool);
}

BOOL    MessageHandler::IsEmpty() const  { return first == last; }

void    MessageHandler::Clear() { first = last = pool; }        //not

MessageHandler::MessageHandler(long pool_sz)
{
  pool = new uchar [pool_size=pool_sz];

  ((Message*)pool)->size = 0;   //for first write

  Clear();
}

MessageHandler::MessageHandler(void* p,long pool_sz)
        :pool_size(pool_sz), pool((uchar*)p), first((uchar*)p), last((uchar*)p)
{
  ((Message*)pool)->size = 0;   //for first write
}

MessageHandler::~MessageHandler()
{
  delete pool;
}