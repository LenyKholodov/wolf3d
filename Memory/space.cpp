#include <memory/space.h>

//////////////////////implementation of class Space//////////////////////////

Space::Space (Pool* pool,size_t length): Pool(pool)
{
#ifndef POOL_EXCEPTIONS
  mPool = owner()->Alloc(mSize=length);
#else
  if (owner() == NULL)
     throw WRONG_POOL;

  mPool = owner()->Alloc(mSize=length);

  if (!IsValid())
     throw WRONG_SPACE;
#endif
}

Space::Space (Pool* pool): Pool (pool)
{
#ifndef POOL_EXCEPTIONS
  mPool = owner()->Alloc(mSize=owner()->maxfree());
#else
  if (owner() == NULL)
     throw WRONG_POOL;

  mPool = owner()->Alloc(mSize=owner()->maxfree());

  if (!IsValid())
     throw WRONG_SPACE;
#endif
}

Space::Space(void* buf,size_t length)
    :Pool(), mSize(length), mPool(buf)
{
#ifdef POOL_EXCEPTIONS
  if (mPool == NULL)
     throw WRONG_PTR;
#endif
}

Space::~Space()
{
  if (owner())
     owner()->Free(mPool);
}

inline BOOL    Space::IsValid () const { return mPool!=NULL; }

BOOL    Space::Belong (void* data) const
{
  return mPool<=data && data<(uchar*)mPool+mSize;
}

void*   Space::_realloc(void* data,size_t bytes) { return NULL; }
void    Space::_free  (void*)                    {}
void*   Space::_alloc (size_t)                   { return NULL; }

inline size_t  Space::size() const { return mSize; }
inline size_t  Space::free() const { return 0;     }
inline size_t  Space::maxfree() const { return free();}
inline void*   Space::getbuf()  const { return mPool; }