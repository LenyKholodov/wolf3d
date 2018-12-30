#include <malloc.h>
#include <stdlib.h>

#include <memory\memory.h>

#include <i86.h>
#include <malloc.h>

//#define  NDEBUG
#include <debug\debug.h>

Pool::Pool(long pool_sz,long cl_sz)
{
     cl_size    = cl_sz+sizeof(memblock_t)-sizeof(uchar);
     pool_size  = pool_sz;
     pool       = NULL;
     if (!InitPool())
        D_Error("Pool: Construct is halted");
}

Pool::~Pool()
{
    if (pool) free(pool);
    pool = NULL;
}

BOOL    Pool::InitPool()
{
          pool = (memblock_t*)malloc(cl_size*pool_size);
          if (pool == NULL)
          {
             dout<<"Pool: No free memory"<<endl;
             return FALSE;
          }
          uchar* data = (uchar*)pool;
          for (long i=0;i<pool_size;i++,data+=cl_size)
          {
                      memblock_t*  mb = (memblock_t*)data;
                      mb->id    = MEM_BLOCK_ID;
                      mb->owner = this;
                      mb->next  = (memblock_t*)(data+cl_size);
          }
          ((memblock_t*)(data-cl_size))->next = NULL; //Конец списка//!!
          firstFree = pool;
          return TRUE;
}

memblock_t*   Pool::Alloc()
{
           memblock_t* _free=firstFree;
           if (firstFree)
              firstFree = firstFree->next;
           return _free;
}

void    Pool::Free(memblock_t* mb)
{
  if (mb->id != MEM_BLOCK_ID)
     D_Error("Memory: Error free memory. Id is modifed!");
  mb->next = firstFree;
  firstFree = mb;
}

BOOL    Pool::CheckHeap()
{
        uchar* data = (uchar*)pool;
        for (long i=0;i<pool_size;i++,data+=cl_size)
            if (((memblock_t*)data)->id!=MEM_BLOCK_ID)
               return TRUE;
        return  FALSE;
}

void*          Pool::AllocBlock ()
{
  memblock_t*    tmp = Alloc();
  return tmp ? (void*)(tmp->data) : NULL;
}

void           Pool::FreeBlock  (void* el)
{
  memblock_t*    tmp = (memblock_t*)((uchar*)el
                                     -sizeof(memblock_t)
                                     +sizeof(uchar));
  if (tmp->id != MEM_BLOCK_ID)
     D_Error("Pool: Wrong delete element of pool");

  Free(tmp);
}