#include <memory/build.h>

///////////////////////implementation of class BuildPool/////////////////////

BuildPool::BuildPool  ( Pool* pool,size_t length)
          : Space(pool,length), mPos((uchar*)getbuf()) { }
BuildPool::BuildPool  ( Pool* pool   )
          : Space(pool), mPos((uchar*)getbuf()) {  }
BuildPool::BuildPool  ( void* buf,size_t length)
          : Space(buf,length), mPos((uchar*)getbuf()) { }

void*   BuildPool::_alloc (size_t bytes)
{
  if (mPos+bytes>(uchar*)getbuf()+size())
     return NULL;

  mPos  += bytes;

  return mPos-bytes;
}

BOOL    BuildPool::Belong (void* data) const
{
  return getbuf()<=data && data<mPos;
}

size_t  BuildPool::free() const
{
  return size()-(size_t)(mPos-(uchar*)getbuf());
}

size_t         BuildPool::maxfree() const { return free(); }
inline void*   BuildPool::getpos() const { return mPos; }
