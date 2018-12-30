#include <iostream.h>

void    Ha (int* x) { cout<<*x<<endl;}

void    main()
{
  int   x = 10;
  int&  y = x;

  Ha(&y);
}