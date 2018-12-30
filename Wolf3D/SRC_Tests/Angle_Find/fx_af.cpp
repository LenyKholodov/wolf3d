#include <tables.h>
#include <iostream.h>

Angle    FindAngle (Fixed x1,Fixed y1,Fixed x2,Fixed y2);

void    main()
{
  Fixed         x1 = 0,
                y1 = 0;

  for (Angle ang = 0;ang<ANGLE_360-ANGLE_1;ang+=ANGLE_1)
         cout<<"Angle "<<angle2float(ang)<<" find angle "
             <<angle2float(FindAngle (x1,y1,cos(ang),sin(ang)))<<endl;
}

Angle    FindAngle (Fixed x1,Fixed y1,Fixed x2,Fixed y2)
{
  Angle angle = atan( abs( div(y2-y1,x2-x1+512) ) );

  if (x1>x2) angle  = ANGLE_180 - angle;
  if (y1>y2) angle  = - angle;

  return angle;
}
