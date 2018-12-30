#include "stack.h"

void    ObjectStack::Push (StackBlock* object)
{
  *up++ =  object;
}

StackBlock*  ObjectStack::Pop ()
{
#ifndef NDEBUG
  if (IsEmpty())
     return NULL;
#endif
  return *--up;
}

ObjectStack::ObjectStack (int max)
{
  up = down = new StackBlock* [max];
}

ObjectStack::~ObjectStack()
{
  delete down;
}
