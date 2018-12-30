#include <tables.h>
#include <iostream.h>

#include <math.h>
#include <time.h>

void    main()
{
  cout<<"TEST ANGLE\tTAN\tANGLE\tDELTA"<<endl;
  for (Angle angle = 0;angle<ANGLE_90-ANGLE_1;angle+=ANGLE_1)
  {
      cout<<angle2int(angle)<<"\t";
      cout<<fixed2float(tan(angle))<<"\t";
      cout<<angle2int(atan(tan(angle)))<<"\t";
      cout<<angle2int(float2angle(atan(fixed2float(tan(angle)))));
      cout<<endl;
  }
  clock_t start = clock();
  Angle x;
  for (Fixed i=0;i<0xFFFFF;i++)
      x = atan(i);
  clock_t end   = clock();
  cout<<"FPS: "<<(i+1)/((end-start)*CLK_TCK)<<endl;
}