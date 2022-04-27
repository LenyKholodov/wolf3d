//Взято из книги Е.В Шикина и А.В Борескова
//"Компьютерная графика. Полигональные модели"

#ifndef __MATRIX__
#define __MATRIX__

#include <vector3d.h>
#include <fixed.h>
#include <mem.h>

template <class Type>
class Matrix
{
  protected:
          Type x[4][4];
  public:

          Matrix () {}
          Matrix (Type);
          Matrix (const Matrix& m) {memcpy(x,m.x,16*sizeof(Type));}
          Matrix& operator += (const Matrix&);
          Matrix& operator -= (const Matrix&);
          Matrix& operator *= (const Matrix&);
          Matrix& operator *= ( Type );
          Matrix& operator /= ( Type );

          Type* operator [] (int i) { return x[i]; }//&x[i][0]
          void            invert();
          void            transpose();

          friend Matrix operator + (const Matrix&,const Matrix&);
          friend Matrix operator - (const Matrix&,const Matrix&);
          friend Matrix operator * (const Matrix&,const Matrix&);
          friend Matrix operator * (const Matrix&,Type);
          friend Matrix operator * (Type,const Matrix&);
          friend Vector3D<Type> operator * (const Matrix&,const Vector3D<Type>&);
          friend Vector3D<Type> operator * (const Vector3D<Type>&,const Matrix&);

          static Matrix<Type>  translate(const Vector3D<Type>&);
          static Matrix<Type>  scale (const Vector3D<Type>&);
          static Matrix<Type>  rotateX ( Type );
          static Matrix<Type>  rotateY ( Type );
          static Matrix<Type>  rotateZ ( Type );
          static Matrix<Type>  rotate  (const Vector3D<Type>&,Type);
          static Matrix<Type>  mirrorX ();
          static Matrix<Type>  mirrorY ();
          static Matrix<Type>  mirrorZ ();
};

typedef Matrix<double>          DoubleMatrix;
typedef Matrix<Fixed16>         Fixed16Matrix;
typedef Matrix<Fixed8>          Fixed8Matrix;

#endif