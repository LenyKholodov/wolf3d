#ifndef __QUEUE__
#define __QUEUE__

#include <types.h>
#include <memory\memory.h>

template <class T>
class Queue
{
  protected:
     static  Pool*   pool;
     static  Queue  *first, *last;
     T       data;
     Queue*  next;
  public:
             Queue() {}
             Queue(T d) { data = d;}
             virtual ~Queue() {}
     BOOL    Insert(T el);
     T       GetData();
     BOOL    IsEmpty() { return first==last; }
     void    Clear();
     void*   operator new (size_t bytes);
     void    operator delete (void* element);
};

template <class T>
void     Queue<T>::Clear()
{
   for (Queue<T>* tmp = first->next;tmp;tmp = tmp->next)
       delete tmp;
   (last=first)->next = first;
}

template <class T>
void*   Queue<T>::operator new(size_t bytes)
{
  return (void*)(pool->Alloc()->data);
}

template <class T>
void    Queue<T>::operator delete(void* el)
{
  pool->Free((memblock_t*)((uchar*)el-15)); //!!
}

template <class T>
BOOL    Queue<T>::Insert(T el)
{
  if ((last->next = new Queue(el)) == NULL)
     return FALSE;
  last       = last->next;
  return     TRUE;
}

template <class T>
T       Queue<T>::GetData()
{
  if (last==first)
  {
     T empty;
     return empty;
  }
  
  T el  = first->data;
  delete first;
  first      = first->next;
  return  el;
}

#endif