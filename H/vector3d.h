//Взято из книги Е.В Шикина и А.В Борескова
//"Компьютерная графика. Полигональные модели"

#ifndef __VECTOR_3D__
#define __VECTOR_3D__

#include <math.h>
#include <types.h>
#include <iostream.h>
#include <fixed.h>

template <class Type>
class Vector3D
{
  friend class Vector2D;
  protected:
  public:
          Type         x,y,z;
          Vector3D()    {}
          Vector3D(Type px,Type py,Type pz=0): x(px), y(py), z(pz) {}
          Vector3D(const Vector3D& v): x(v.x), y(v.y), z(v.z) {}
          Vector3D& operator = (const Vector3D& v)
          {
            x=v.x;
            y=v.y;
            z=v.z;
            return *this;
          }
          Vector3D  operator + () const { return *this; }
          Vector3D  operator  - () const { return Vector3D<Type>(-x,-y,-z); }
          Vector3D& operator += (const Vector3D& v)
          {
            x+=v.x;
            y+=v.y;
            z+=v.z;
            return *this;
          }
          Vector3D& operator -= (const Vector3D& v)
          {
            x-=v.x;
            y-=v.y;
            z-=v.z;
            return *this;
          }
          Vector3D& operator *= (const Vector3D& v)
          {
            x*=v.x;
            y*=v.y;
            z*=v.z;
            return *this;
          }
          Vector3D& operator *= (Type f)
          {
            x*=f;
            y*=f;
            z*=f;
            return *this;
          }
          Vector3D& operator /= (const Vector3D& v)
          {
            x/=v.x;
            y/=v.y;
            z/=v.z;
            return *this;
          }
          Vector3D& operator /= (Type f)
          {
            x/=f;
            y/=f;
            z/=f;
            return *this;
          }
          Type& operator [] (int index) { return *(index+&x);}
          BOOL   operator == (const Vector3D& v) const
          {
            return x==v.x && y==v.y && z==v.z;
          }
          BOOL   operator != (const Vector3D& v) const
          {
            return x!=v.x || y!=v.y || z!=v.z;
          }
          BOOL   operator < (const Vector3D& v) const
          {
            return (x<v.x) || ((x==v.x) && (y<v.y));
          }
          BOOL   operator > (const Vector3D& v) const
          {
            return (x>v.x) || ((x==v.x) && (y>v.y));
          }
          Type length () const { return (Type)sqrt(x*x+y*y+z*z); }
          friend Vector3D operator + (const Vector3D&,const Vector3D&);
          friend Vector3D operator - (const Vector3D&,const Vector3D&);
          friend Vector3D operator * (const Vector3D&,const Vector3D&);
          friend Vector3D operator * (Type f,const Vector3D& u)
          {
            return Vector3D<Type> (u.x*f,u.y*f,u.z*f);
          }
          friend Vector3D operator * (const Vector3D& u,Type f)
          {
            return Vector3D(u.x*f,u.y*f,u.z*f);
          }
          friend Vector3D operator / (const Vector3D&,const Vector3D&);
          friend Vector3D operator / (const Vector3D&,Type);
          friend Type    operator & (const Vector3D&,const Vector3D&);
          friend Vector3D operator ^ (const Vector3D&,const Vector3D&);
          friend ostream& operator<<(ostream&,const Vector3D&);
          friend istream& operator>>(istream&,Vector3D&);
};

#include <vector2d.h>

typedef Vector3D<double>        DoubleVector3D;
typedef Vector3D<Fixed16>       Fixed16Vector3D;
typedef Vector3D<Fixed8>        Fixed8Vector3D;

template <class Type>
inline ostream& operator<<(ostream& os,const Vector3D<Type>& v)
{
                                os<<"Vector3D: x="<<v.x<<" y="<<v.y<<" z="<<v.z;
                                return os;
}

template <class Type>
inline istream& operator>>(istream& is,Vector3D<Type>& v)
{
                                is>>v.x;
                                is>>v.y;
        is>>v.z;
        return is;
}

template <class Type>
inline Vector3D<Type> operator + (const Vector3D<Type>& u,const Vector3D<Type>& v)
{
  return Vector3D<Type> (u.x+v.x,u.y+v.y,u.z+v.z);
}

template <class Type>
inline  Vector3D<Type>  operator  - (const Vector3D<Type>& u,const Vector3D<Type>& v)
{
  return Vector3D<Type> (u.x-v.x,u.y-v.y,u.z-v.z);
}

template <class Type>
inline  Vector3D<Type>  operator  * (const Vector3D<Type>& u,const Vector3D<Type>& v)
{
  return Vector3D<Type> (u.x*v.x,u.y*v.y,u.z*v.z);
}

//template <class Type>
//inline  Vector3D<Type>  operator  * (Vector3D<Type>& u,Type f)
//{
//  return Vector3D<Type>(u.x*f,u.y*f,u.z*f);
//}

//template <class Type>
//inline  Vector3D<Type>  operator  * (Type f,const Vector3D<Type>& u)
//{
//  return Vector3D<Type> (u.x*f,u.y*f,u.z*f);
//}

template <class Type>
inline  Vector3D<Type>  operator  / (const Vector3D<Type>& u,const Vector3D<Type>& v)
{
        return Vector3D<Type> (u.x/v.x,u.y/v.y,u.z/v.z);
}

template <class Type>
inline  Vector3D<Type>  operator  / (const Vector3D<Type>& u,Type f)
{
  return Vector3D<Type> (u.x/f,u.y/f,u.z/f);
}

template <class Type>
inline  Type  operator  & (const Vector3D<Type>& u,const Vector3D<Type>& v)
{
  return u.x*v.x+u.y*v.y+u.z*v.z;
}

template <class Type>
inline  Vector3D<Type>  operator  ^ (const Vector3D<Type>& u,const Vector3D<Type>& v)
{
  return Vector3D<Type>(u.y*v.z-u.z*v.y,u.z*v.x-u.x*v.z,u.x*v.y-u.y*v.x);
}

#endif