#ifndef __HANDLER_PTR__
#define __HANDLER_PTR__

#include <memory\mp.h>
#include <memory\cp.h>

//Класс "Дескриптор". Использует умные указатели для доступа к ведущим

template <class T>
class hPtr
{
  private:
           mPtr<T>&     ptr;
  public:
          hPtr(): ptr(*(new mPtr<T>)) {}
          hPtr(mPtr<T>& mp): ptr(mp) {}
          mPtr<T>& operator -> () const { return ptr; }
};

#endif