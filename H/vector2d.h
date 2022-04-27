#ifndef  __VECTOR_2D__
#define  __VECTOR_2D__

#include <vector3d.h>

template <class Type>
class Vector2D:public Vector3D<Type>
{
  friend class Vector3D;
  protected:
          Vector2D(Type px,Type py,Type pz) { }
          Vector2D(const Vector3D& v) { }
  public:
          Vector2D(const Vector2D& v) { }
          Vector2D(Type px,Type py) { x=px; y=py; }
          Vector2D& operator = (const Vector3D& v)
          {
            x=v.x;
            y=v.y;
            z=0;
            return *this;
          }
          Vector2D& operator = (const Vector2D& v)
          {
            x=v.x;
            y=v.y;
            z=0;
            return *this;
          }
          operator Vector3D() { return Vector3D(x,y,0); }
};

typedef Vector2D<double>        DoubleVector2D;
typedef Vector2D<Fixed16>       Fixed16Vector2D;
typedef Vector2D<Fixed8>        Fixed8Vector2D;

#endif
