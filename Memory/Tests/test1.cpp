#include <memory\memory.h>
#include <malloc.h>

void    main()
{
  Pool* pool = new Pool(1024,sizeof(int));
  memblock_t* test1 = pool->Alloc();
  memblock_t* test2 = pool->Alloc();
  memblock_t* test3 = pool->Alloc();
  pool->Free(test2);
  test2 = pool->Alloc();
  pool->CheckHeap();
  _heapchk();
}