#include "ipxpool.h"

#include <iostream.h>
#include <process.h>
#include <debug/debug.h>

////////////////////implemenmtation of class PacketPool////////////////////

void          PacketPool::ClearAll()
{
  char far* data = (char far*)mPool;

  for (long i=0;i<mSize;i++,data+=mClaster)
  {
     lump_t far*  mb = (lump_t far*)data;
     mb->next        = (lump_t far*)(data+mClaster);
  }

  ((lump_t far*)(data-mClaster))->next = NULL; //Конец списка//!!

  mFree = (lump_t far*)mPool;
}

void far*     PacketPool::Alloc   ()
{
  lump_t far* _free=mFree;

//  dout<<"This "<<(void*)_free<<endl;

  if (mFree)
  {
     mFree = mFree->next;
//     dout<<"Next "<<(void*)_free->next<<endl;
  }

  return _free != NULL ? _free->data : NULL;
}

void          PacketPool::Free    (void far* data)
{
  lump_t far* tmp = (lump_t far*)((char far*)data-sizeof(lump_t));

  tmp->next       = mFree;
  mFree           = tmp;
}

int           PacketPool::belong  (void far* _data)
{
  char far* data = (char far*)data,
       far* pool = (char far*)mPool;

  return data > pool && data < pool + mSize;
}

PacketPool::PacketPool   (int size,int cl_size,void far* data)
           : mSize (size), mClaster (cl_size+sizeof(lump_t)),
             mPool (data)
{
  if (mPool == NULL)
  {
    cout<<"No free memory for PacketPool"<<endl;
    exit (-1);
  }

  ClearAll ();
}

PacketPool::~PacketPool  ()
{   }

