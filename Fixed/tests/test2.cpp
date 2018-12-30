#include <iostream.h>
#include <tables.h>

Fixed   _div     (Fixed x,Fixed x1) { return Fixed((x/x1)<<16); }

void    main()
{
  Fixed x = int2fixed(-12344);
  Fixed y = int2fixed(-1221);
  Fixed z = _div(x,y);

  cout<<fixed2int(z)<<endl;
}