#include <malloc.h>
#include <stdlib.h>

#include <memory\memory.h>

#include <i86.h>
#include <malloc.h>

#define  NDEBUG
#include "debug.h"

Pool::Pool(long pool_sz,long cl_sz)
{
     cl_size    = cl_sz+sizeof(memblock_t)-sizeof(uchar);
     pool_size  = pool_sz;
     pool       = NULL;
     if (!InitPool())
        D_Error("Pool: Construct is halted");
     D_String("Pool: Constructor OK");
}

Pool::~Pool()
{
    free(pool);
    pool = NULL;
    D_String("Pool: Destructor OK");
}

BOOL    Pool::InitPool()
{
          D_String("Pool: Allocate memory for this pool");
          pool = (memblock_t*)malloc(cl_size*pool_size);
          if (pool == NULL)
          {
             D_Error("Pool: No free memory");
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
          D_String("Pool: First init is OK");
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
        D_String("Memory: Ckecking system pool...");
        D_Check;
        D_String("OK");

        return  FALSE;
}