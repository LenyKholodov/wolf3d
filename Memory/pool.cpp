#include <memory\space.h>

#include <debug\debug.h>

Debug   debug;

////////////////////////implementation of class Pool/////////////////////////

void*   Pool::Alloc (size_t bytes)
{
  void* x = _alloc (bytes);

  if ( x = _alloc(bytes))     Grab ();
  else                     return NULL;

  return test;
}

void    Pool::Free  (void* data)
{
  if (Belong(data))
  {
     _free (data);
     Release();
  }
  else
       dout<<"Pool: Wrong pointer delete"<<endl;
}

void*   Pool::Realloc (void* data,size_t ptr)
{
  if (Belong(data))     return _realloc (data,ptr);
  else
  {
    dout<<"Pool: Wrong pointer resize"<<endl;
    return data;
  }
}

inline int Pool::count () const { return  mCount; }

inline void    Pool::Grab ()    { ++mCount; }
inline void    Pool::Release () { --mCount; }

Pool::Pool(Pool* pool):mOwner(pool), mCount(0) {}

inline  Pool*   Pool::owner() const { return mOwner; }


