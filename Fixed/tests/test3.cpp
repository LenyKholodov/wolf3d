#include <tables.h>
#include <debug/debug.h>

void    main()
{
  Angle angle = 0;

  dout<<"Angle "<<angle<<endl;

  dout<<"Sin "<<fixed2float(sin(angle))<<" Cos "<<fixed2float(cos(angle))<<endl;
}