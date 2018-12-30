#include <iostream.h>
#include <malloc.h>

void*   operator new (size_t bytes)
{
  cout<<"!"<<endl;
  return malloc (bytes);
}

void    main()
{
  char* x =  new char [1000];

  cout<<(void*)x<<endl;
}