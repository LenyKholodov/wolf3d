#include <memory/system.h>

#include <dpmi32gw.h>

/////////////////////implementation of class SystemSpace/////////////////////

inline BOOL       SystemSpace::Belong  ( void* ) const { return TRUE; }

inline size_t     SystemSpace::size    () const
{
  FreeMemInfo   meminfo;
  if (!DPMIFreeMemInfo(&meminfo))
     return -1;

  return  meminfo.TotPhysPgs*4096;
}

inline size_t     SystemSpace::free    () const
{
  FreeMemInfo   meminfo;
  if (!DPMIFreeMemInfo(&meminfo))
     return -1;

  return  meminfo.TotFreePgs*4096;
}

inline size_t     SystemSpace::maxfree () const
{
  FreeMemInfo   meminfo;
  if (!DPMIFreeMemInfo(&meminfo))
     return -1;

  return  meminfo.AvailBytes;
}

inline BOOL    SystemSpace::IsValid () const { return !_heapchk(); }

SystemSpace::~SystemSpace ()
{
#ifndef NDEBUG
  if (count()>0)
     dout<<"Pool: Not all objects were deleted!"<<endl;
#endif
}

inline void*      SystemSpace::_realloc ( void* data,size_t bytes)
{
  return realloc (data,bytes);
}

inline void*      SystemSpace::_alloc ( size_t bytes)    { return malloc (bytes); }
inline void       SystemSpace::_free  ( void*  data)     { ::free(data);          }