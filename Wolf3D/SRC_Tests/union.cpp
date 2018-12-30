#include <iostream.h>
#include <types.h>

union   PACK
{
  long  a;
  struct X
  {
    ulong      b : 16;
    ulong      c : 8;
  }x;
}_PACK;

PACK    x;

void    input() { cin>>x.a;}
void    output(int d) { cout<<d<<endl; }
int     recalc() { return x.x.c; }

void    main()
{
  PACK  x;
  cin>>x.a;
  cout<<x.x.c<<endl;
}