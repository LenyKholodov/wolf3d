#ifndef __CLEVER_PTR__
#define __CLEVER_PTR__

#include <types.h>

/////////////////////////////////////////////////////////////////////////////
//Класс "Умный указатель" представляет класс совместимый с Object
/////////////////////////////////////////////////////////////////////////////
template <class T>
class Ptr
{
        protected:
                                         T*    pointer;
        public:
           Ptr():pointer(NULL) { }
           Ptr(T* p):pointer(p) { }
           operator T*() { return pointer;}
           operator = (T* p) { pointer=p; }
           T* operator -> () { return pointer; }
};

/////////////////////////////////////////////////////////////////////////////
//Класс кеш-указателя. Если объект недоступен - загружает его с диска
/////////////////////////////////////////////////////////////////////////////
template <class T>
class cachePtr:public Ptr<T>
{
  protected:
             void       ReadObject() { }
  public:
          operator T*()
          {
                   if (pointer==NULL) pointer = this->ReadObject();
                   return pointer;
          }
          T* operator -> ()
          {
                   if (pointer==NULL) pointer = this->ReadObject();
                   return pointer;
          }
};

/////////////////////////////////////////////////////////////////////////////
////Класс указатель. Усли объекта нет - создаёт его
/////////////////////////////////////////////////////////////////////////////
template <class T>
class _Ptr:public Ptr<T>
{
  protected:
  public:
          T* operator -> () {
               if (pointer==NULL)
                  return new Ptr<T>;
               return pointer;
          }
};

#endif