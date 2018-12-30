#ifndef __POOL__
#define __POOL__

#include <types.h>

#define MEM_BLOCK_ID    0xFA

struct memblock_t
{
             friend class        Pool;
             uchar               id;
             Pool*               owner;
             memblock_t*         next;
             uchar               reserv[6];
             uchar               data[1];
};

///////////////////////////////////////////////////////////////////////////
////Менеджер памти осуществляющий работу с фиксированными блоками
///////////////////////////////////////////////////////////////////////////
class Pool
{
  private:
             memblock_t*         pool;
             memblock_t*         firstFree;
             long                cl_size,pool_size;
  protected:
             BOOL       InitPool();
  public:
          Pool(long pool_sz,long cl_sz);
          virtual ~Pool();
          memblock_t*    Alloc();
          void           Free(memblock_t* mb);
          BOOL           CheckHeap ();
};

///////////////////////////////////////////////////////////////////////////
////Менеджер памяти не удаляющий блоки
///////////////////////////////////////////////////////////////////////////
class   BuildPool
{
  public:
          BuildPool(long size) { last = pool = new uchar[size]; }
          virtual ~BuildPool() { delete pool; }

          void*         Alloc(long size)  {
                                  void* tmp = last;
                                  last = (uchar*)last+size;
                                  return tmp;
                        }
          void          Free(void*)       { }
  protected:
  private:
           void*        pool;
           void*        last;
};

#endif