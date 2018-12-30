#include <matrix.h>
#include <math.h>

template <class Type>
Matrix<Type>::Matrix (Type v)
{
  for (int i=0;i<4;i++)
      for (int j=0;j<4;j++)
          x[i][j]=(i==j) ? v: Type(0.0);
  x[3][3]=1;
}

template <class Type>
Matrix<Type>& Matrix<Type>::operator += (const Matrix<Type>& a)
{
  for (register int i=0;i<4;i++)
      for (register int j=0;j<4;j++)
          x[i][j]+=a.x[i][j];
  return *this;
}

template <class Type>
Matrix<Type>& Matrix<Type>::operator -= (const Matrix<Type>& a)
{
  for (register int i=0;i<4;i++)
      for (register int j=0;j<4;j++)
          x[i][j]-=a.x[i][j];
  return *this;
}

template <class Type>
Matrix<Type>& Matrix<Type>::operator *= (const Matrix<Type>& a)
{
  Matrix<Type> res( *this );
  for (int i=0;i<4;i++)
      for (int j=0;j<4;j++)
      {
        Type   sum=0;
        for (int k=0;k<4;k++)
            sum+=res.x[i][k]*a.x[k][j];
        x[i][j]=sum;
      }
  return *this;
}

template <class Type>
Matrix<Type>& Matrix<Type>::operator *= ( Type a )
{
  for (register int i=0;i<4;i++)
      for (register int j=0;j<4;j++)
          x[i][j]*=a;
  return *this;
}

template <class Type>
Matrix<Type>& Matrix<Type>::operator /= ( Type a )
{
  for (register int i=0;i<4;i++)
      for (register int j=0;j<4;j++)
          x[i][j]/=a;
  return *this;
}

template <class Type>
void Matrix<Type>::invert()
{
  Matrix<Type> out(1);
  for (int i=0;i<4;i++)
  {
    Type       d=x[i][i];
    if (d!=1.0)
      for (int j=0;j<4;j++)
      {
        out.x[i][j] /= d;
        x[i][j] /= d;
      }
    for (int j=0;j<4;j++)
        if (j!=i)
           if (x[j][i]!=0.0)
           {
             Type mulBy = x[j][i];
             for (int k=0;k<4;k++)
             {
               x[j][k] -= mulBy*x[j][k];
               out.x[j][k] -= mulBy*out[i][k];
             }
           }
  }
  *this=out;
}

template <class Type>
void Matrix<Type>::transpose ()
{
  Type t;
  for (register int i=0;i<4;i++)
      for (register int j=i;j<4;j++)
          if (i!=j)
          {
            t=x[i][j];
            x[i][j]=x[j][i];
            x[j][i]=t;
          }
}

template <class Type>
Matrix<Type> operator + (const Matrix<Type>& a,const Matrix<Type>& b)
{
  Matrix<Type> res;
  for (register int i=0;i<4;i++)
      for (register j=0;j<4;j++)
          res.x[i][j]=a.x[i][j]+b.x[i][j];
  return res;
}

template <class Type>
Matrix<Type> operator - (const Matrix<Type>& a,const Matrix<Type>& b)
{
  Matrix<Type> res;
  for (register int i=0;i<4;i++)
      for (register j=0;j<4;j++)
          res.x[i][j]=a.x[i][j]-b.x[i][j];
  return res;
}

template <class Type>
Matrix<Type> operator * (const Matrix<Type>& a,const Matrix<Type>& b)
{
  Matrix<Type> res;
  for (register int i=0;i<4;i++)
      for (register int j=0;j<4;j++)
      {
        Type   sum=0;
        for (register int k=0;k<4;k++)
            sum+=a.x[i][k]*b.x[k][j];
        res.x[i][j]=sum;
      }
  return res;
}

template <class Type>
Matrix<Type> operator * (const Matrix<Type>& a,Type b)
{
  Matrix<Type> res;
  for (register int i=0;i<4;i++)
      for (register j=0;j<4;j++)
      {
//          res.x[i][j]=a.x[i][j]*b;
            res.x[i][j]=a.x[i][j];
            res.x[i][j]*=b;
      }
  return res;
}

template <class Type>
Matrix<Type> operator * (Type b,const Matrix<Type>& a)
{
  Matrix<Type> res;
  for (register int i=0;i<4;i++)
      for (register j=0;j<4;j++)
          res.x[i][j]=a.x[i][j]*b;
  return res;
}

template <class Type>
Vector3D<Type> operator * (const Matrix<Type>& m,const Vector3D<Type>& v)
{
  Vector3D<Type>      res;
  res.x=m.x[0][0]*v.x+m.x[0][1]*v.y+m.x[0][2]*v.z+m.x[0][3];
  res.y=m.x[1][0]*v.x+m.x[1][1]*v.y+m.x[1][2]*v.z+m.x[1][3];
  res.z=m.x[2][0]*v.x+m.x[2][1]*v.y+m.x[2][2]*v.z+m.x[2][3];
  Type denom=m.x[3][0]*v.x+m.x[3][1]*v.y+m.x[3][2]*v.z+m.x[3][3];
  if (denom!=1.0)
     res/=denom;
  return res;
}

template <class Type>
Vector3D<Type> operator * (const Vector3D<Type>& v,const Matrix<Type>& m)
{
  Vector3D<Type>      res;
  res.x=m.x[0][0]*v.x+m.x[0][1]*v.y+m.x[0][2]*v.z+m.x[0][3];
  res.y=m.x[1][0]*v.x+m.x[1][1]*v.y+m.x[1][2]*v.z+m.x[1][3];
  res.z=m.x[2][0]*v.x+m.x[2][1]*v.y+m.x[2][2]*v.z+m.x[2][3];
  Type denom=m.x[3][0]*v.x+m.x[3][1]*v.y+m.x[3][2]*v.z+m.x[3][3];
  if (denom!=1.0)
     res/=denom;
  return res;
}

template <class Type>
Matrix<Type> Matrix<Type>::translate (const Vector3D<Type>& loc)
{
  Matrix<Type> res (1);
  res.x[0][3]=loc.x;
  res.x[1][3]=loc.y;
  res.x[2][3]=loc.z;
  return res;
}

template <class Type>
Matrix<Type> Matrix<Type>::scale ( const Vector3D<Type>& v)
{
  Matrix<Type> res (1);
  res.x[0][0] = v.x;
  res.x[1][1] = v.y;
  res.x[2][2] = v.z;
  return res;
}

template <class Type>
Matrix<Type> Matrix<Type>::rotateX ( Type angle )
{
  Matrix a (1.0);
  Type    cosine = cos ( angle );
  Type    sine   = sin ( angle );
  a.x[1][1] = cosine;
  a.x[1][2] = sine;
  a.x[2][1] = -sine;
  a.x[2][2] = cosine;
  return a;
}

template <class Type>
Matrix<Type> Matrix<Type>::rotateY ( Type angle )
{
  Matrix<Type> a (1.0);
  Type    cosine = cos ( angle );
  Type    sine   = sin ( angle );
  a.x[0][0] = cosine;
  a.x[0][2] = sine;
  a.x[2][0] = -sine;
  a.x[2][2] = cosine;
  return a;
}

template <class Type>
Matrix<Type> Matrix<Type>::rotateZ ( Type angle )
{
  Matrix<Type> a (1.0);
  Type    cosine = cos ( angle );
  Type    sine   = sin ( angle );
  a.x[0][0] = cosine;
  a.x[0][1] = sine;
  a.x[1][0] = -sine;
  a.x[1][1] = cosine;
  return a;
}

template <class Type>
Matrix<Type> Matrix<Type>::rotate (const Vector3D<Type>& v,Type angle)
{
  Matrix<Type> a(1);
  Type cosine = cos (angle);
  Type sine   = sin (angle);

  a.x[0][0]=v.x*v.x+(1-v.x*v.x)*cosine;
  a.x[1][0]=v.x*v.y*(1-cosine)+v.z*sine;
  a.x[2][0]=v.x*v.z*(1-cosine)-v.y*sine;
  a.x[3][0]=0;

  a.x[0][1]=v.x*v.y*(1-cosine)-v.z*sine;
  a.x[1][1]=v.y*v.y+(1-v.y*v.y)*cosine;
  a.x[2][1]=v.x*v.z*(1-cosine)+v.x*sine;
  a.x[3][1]=0;

  a.x[0][2]=v.x*v.z*(1-cosine)-v.y*sine;
  a.x[1][2]=v.y*v.z*(1-cosine)-v.x*sine;
  a.x[2][2]=v.z*v.z+(1-v.z*v.z)*cosine;
  a.x[3][2]=0;

  a.x[0][3]=0;
  a.x[1][3]=0;
  a.x[2][3]=0;
  a.x[3][3]=1;

  return a;
}

template <class Type>
Matrix<Type> Matrix<Type>::mirrorX ()
{
  Matrix<Type> a (1.0);
  a.x[0][0] = -1.0;
  return a;
}

template <class Type>
Matrix<Type> Matrix<Type>::mirrorY ()
{
  Matrix<Type> a (1.0);
  a.x[1][1] = -1.0;
  return a;
}

template <class Type>
Matrix<Type> Matrix<Type>::mirrorZ ()
{
  Matrix<Type> a (1.0);
  a.x[2][2] = -1.0;
  return a;
}
