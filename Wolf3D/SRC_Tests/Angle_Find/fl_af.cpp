#include <math.h>
#include <iostream.h>

float    FindAngle (float x1,float y1,float x2,float y2);

void    main()
{
  float         x1 = 0,
                y1 = 0;

  for (float ang = 0;ang<6.28;ang+=0.1)
         cout<<"Angle "<<ang<<" find angle "
             <<FindAngle (x1,y1,cos(ang),sin(ang))<<endl;

}

float    FindAngle (float x1,float y1,float x2,float y2)
{
  float angle = atan( fabs( (y2-y1)/(x2-x1+0.0001) ) );

  if (x1>x2) angle  = 3.14 - angle;
  if (y1>y2) angle  = 6.28 - angle;

  return angle;
}
