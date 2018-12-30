#include <tables.h>
#include <iostream.h>

Angle   __ATAN [0xFFFF];

void    InitTables();
Angle   ATan(Fixed);

void    main()
{
  InitTables();

  Angle x = ANGLE_90/3;

  cout<<ATan(tan(x))<<endl;
}

Angle   ATan(Fixed _tan)
{
  return __ATAN[_tan>>7];
}

void    InitTables()
{
  for (Fixed _tan = 0;_tan<tan(ANGLE_90-ANGLE_1);_tan+=128)
      __ATAN[_tan>>7] = atan(_tan);
}
