#include <memory/clpool.h>

//////////////////implementation of class FixedPool//////////////////////////

void    FixedPool::_prepare()
{
          uchar* data = (uchar*)getbuf();

          if (data == NULL)
             return;

          for (long i=0;i<size();i+=mClaster,data+=mClaster)
          {
                      claster_t*  block = (claster_t*)data;
                      block->id         = MEM_BLOCK_ID;
                      block->next       = (claster_t*)(data+mClaster);
          }

          ((claster_t*)(data-mClaster))->next = NULL;   //end of list

          mFirst = (claster_t*)getbuf();                //first free block

}

void*   FixedPool::_alloc(size_t length)
{
          if (mFirst)
          {
             claster_t* _free = mFirst;
             mFirst           = mFirst->next;
             mFree           -= mClaster;
             return          _free->data;
          }
          else
             return NULL;
}

void    FixedPool::_free(void* data)
{
          claster_t*    block = (claster_t*)((uchar*)data
                                -sizeof(claster_t)+sizeof(uchar));

#ifndef NDEBUG
          if (block->id != MEM_BLOCK_ID)
             D_Error("Memory: Error free memory. Id is modifed!");
#endif

          block->next = mFirst;
          mFirst      = block;
          mFree      -= mClaster;
}

FixedPool::FixedPool (Pool* pool,size_t cl_size)
   :Space(pool,pool->size()), mClaster(cl_size+sizeof(claster_t)),
    mFree(size()),mFirst(NULL)
{
  _prepare();
}

static int claster_size = sizeof (claster_t);

FixedPool::FixedPool ( Pool* pool,size_t sz,size_t cl_size)
      :Space(pool,(mClaster=cl_size+claster_size)*sz),
       mFree(size()),mFirst(NULL)
{
  _prepare();
}

size_t  FixedPool::free()    const { return mFree; }
size_t  FixedPool::maxfree() const { return claster(); }
size_t  FixedPool::claster() const { return mClaster; }