#ifndef __MEMORY_SIZE_TEST__
#define __MEMORY_SIZE_TEST__

#include <malloc.h>

static  int     maxSize = 0;
static  int     curSize = 0;

void*   operator new (size_t bytes)
{
  if ((curSize += bytes)>maxSize)
     maxSize = curSize;

  long*  x = (long*)malloc (bytes+sizeof(long));
  *x       = bytes;

  return x+1;
}

void    operator delete (void* x)
{
  long* del = (((long*)x)-1);
  curSize  -= *del;
  free (del);
}

#endif