#include <memory/memory.h>

void    main()
{
  Pool* pool = new Pool(1000,1000);
  while (pool->Alloc());
  delete pool;
}