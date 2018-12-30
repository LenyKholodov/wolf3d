#include <iostream.h>

struct X
{
  int     t [100];
};

struct Y: public X
{
  int     t1 [1000];
};

void    Print (X* x) { cout <<sizeof(*x)<<endl; }

void    main()
{
  X     x;
  Y     y;

  Print (&x);
  Print (&y);

  cout<<"Size of short "<<sizeof(short)<<endl;
}