#include <mp.h>

template <class T>
static int mPtr<T>::current=0;

template <class T>
inline mPtr<T>::mPtr():object(new T) { current++; }

template <class T>
inline mPtr<T>::mPtr(const mPtr<T>& p):object(new T((*p.object)))
       { current++; }

template <class T>
mPtr<T>::~mPtr() { delete object; current--; }

template <class T>
inline mPtr<T>& mPtr<T>::operator = (const mPtr<T>& p)
{
  if (this !=&p)
  {
    delete object;
    object = new T(*(p.object));
  }
  return *this;
}

template <class T>
inline mPtr<T>::operator -> () const
{
  return object;
}

template <class T>
inline const MPRO<T>::operator -> () const
{
  return object;
}