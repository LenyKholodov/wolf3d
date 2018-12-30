#include <vector3d.h>
#include <types.h>

#include <iostream.h>

DoubleVector3D          v1(3,1,0);
DoubleVector3D          v2(-1,-3,0);

BOOL    IsFront (DoubleVector3D,DoubleVector3D);

void    main()
{
  if (IsFront(v1,v2))
    cout<<"Front is: TRUE"<<endl;
  else
    cout<<"Front is: FALSE"<<endl;
}

BOOL    IsFront (DoubleVector3D v1,DoubleVector3D v2)
{
  return (v1 & v2)<0;   //optimize
}
