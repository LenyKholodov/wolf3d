#include <math.h>
#include <tables.h>
#include <iostream.h>

const   float   PI      = 3.1415926;
const   float   A1      = PI/180;

void    main()
{
  cout<<"TEST ANGLE\tTAN\tANGLE\tDELTA"<<endl;
  for (double angle = 0;angle<PI/2;angle+=A1)
  {
    cout<<angle2int(float2angle(angle))<<"\t";
    cout<<tan(angle)<<"\t";
    cout<<angle2int(float2angle(atan(tan(angle))))<<"\t";
//    cout<<angle2int(angle-atan(tan(angle)));
    cout<<endl;
  }
}