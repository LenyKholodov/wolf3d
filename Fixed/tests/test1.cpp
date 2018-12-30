#include <tables.h>
#include <iostream.h>

void    main()
{
  cout<<hex<<endl;
  for (Angle x=0;x<ANGLE_90;x+=ANGLE_1)
      cout<<tan(x)<<endl;
}