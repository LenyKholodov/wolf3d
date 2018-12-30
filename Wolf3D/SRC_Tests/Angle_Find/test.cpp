#include <tables.h>
#include <iostream.h>

void    main ()
{
  cout<<fixed2float (div (float2fixed(3600),float2fixed (3.0) ))<<endl;
  cout<<fixed2float (div (float2fixed(0.36),float2fixed (3.0) ))<<endl;
}