//Взято из книги Е.В Шикина и А.В Борескова
//"Компьютерная графика. Полигональные модели"

#ifndef __MATRIX_3D__
#define __MATRIX_3D__

#include <vector3d.h>

class Matrix3D
{
  public:
          float x[3][3];

          Matrix3D () {}
          Matrix3D (float);
          Matrix3D (const Matrix3D&);
          Matrix3D& operator = (const Matrix3D&);
          Matrix3D& operator = ( float );
          Matrix3D& operator += (const Matrix3D&);
          Matrix3D& operator -= (const Matrix3D&);
          Matrix3D& operator *= (const Matrix3D&);
          Matrix3D& operator *= ( float );
          Matrix3D& operator /= ( float );

          float* operator [] (int i) { return &x[i][0]; }
          void            invert();
          void            transpose();

          static Matrix3D      scale (const Vector3D&);
          static Matrix3D      rotateX ( float );
          static Matrix3D      rotateY ( float );
          static Matrix3D      rotateZ ( float );
          static Matrix3D      mirrorX ();
          static Matrix3D      mirrorY ();
          static Matrix3D      mirrorZ ();
          static Matrix3D      rotate (const Vector3D& v,float angle);

          friend Matrix3D operator + (const Matrix3D&,const Matrix3D&);
          friend Matrix3D operator - (const Matrix3D&,const Matrix3D&);
          friend Matrix3D operator * (const Matrix3D&,const Matrix3D&);
          friend Matrix3D operator * (const Matrix3D&,float);
          friend Matrix3D operator * (float,const Matrix3D&);
          friend Vector3D operator * (const Matrix3D&,const Vector3D);
};

Matrix3D      scale (const Vector3D&);
Matrix3D      rotateX ( float );
Matrix3D      rotateY ( float );
Matrix3D      rotateZ ( float );
Matrix3D      rotate  (const Vector3D& v,float angle);
Matrix3D      mirrorX ( float );
Matrix3D      mirrorY ( float );
Matrix3D      mirrorZ ( float );

#endif