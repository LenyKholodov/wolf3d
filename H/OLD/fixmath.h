//Е.В. Шикин, А.В. Боресков - "Компьтерная графика. Полигональные модели"
//с. 453-456
// Fixmath.h
#ifndef _FIXED_MATH
#define _FIXED_MATH

//interface

#define ANGLE_90  16384U
#define ANGLE_180 32768U
#define ANGLE_270 49152U
#define MAX_FIXED 0x7FFFFFFF
#define ONE       0x10000

typedef unsigned short Angle;
typedef long           Fixed;

//implementation
#include <math.h>

Fixed *sinTable;
Fixed *cosTable;
Fixed *tanTable;
Fixed *ctgTable;
Fixed *invSinTable;
Fixed *invCosTable;

inline int fraction2Fixed(int a,int b)
{
 return (long)((((long) a)<<16)/(long) b);
}

inline int fixed2Int( Fixed x)
{
 return (int)(x>>16);
}

inline float fixed2Float(Fixed x)
{
 return ((float)x)/65536.0;
}

inline Fixed frac (Fixed x)
{
 return x & 0xFFFF;
}

inline Fixed float2Fixed (float x)
{
 return (long)(65536.0*x);
}

inline Fixed int2Fixed(int x)
{
 return ((long) x)<<16;
}

inline Fixed fxabs(Fixed x)
{
 return x>0 ? x:-x;
}

inline Fixed fxsin(Angle angle)
{
 return sinTable[angle>>3];
}

inline Fixed fxcos(Angle angle)
{
 return cosTable[angle>>3];
}

inline Fixed fxtan(Angle angle)
{
 return tanTable[angle>>3];
}

inline Fixed fxctg(Angle angle)
{
 return ctgTable[angle>>3];
}

inline Fixed fxInvSin(Angle angle)
{
 return invSinTable[angle>>3];
}

inline Fixed fxInvCos(Angle angle)
{
 return invCosTable[angle>>3];
}

inline Angle rad2Angle(float angle)
{
 return (Angle)(32768.0*angle/M_PI);
}

inline  float angle2Rad(Angle angle)
{
 return ((float)angle)*M_PI/32768.0;
}

void InitTables()
{
 sinTable=new Fixed[8192];
 cosTable=new Fixed[8192];
 invSinTable=new Fixed[8192];
 invCosTable=new Fixed[8192];
 tanTable=new Fixed[8192];
 ctgTable=new Fixed[8192];

 for (int i=0;i<8192;i++)
 {
  float x=i*2*M_PI/(8192.0);
  float sx=sin(x);
  float cx=cos(x);
  float tx=tan(x);

  sinTable[i]=float2Fixed(sx);
  cosTable[i]=float2Fixed(cx);
  tanTable[i]=float2Fixed(tx);

  if (tx>0.0001 || tx<-0.0001)
   ctgTable[i]=(long)(65536.0/tx);
  else
   ctgTable[i]=(tx>0 ? 65536.0*10000.0 : -65536.0*10000.0);

  if (sx>0.0001 || sx<-0.0001)
   invSinTable[i]=(long)(65536.0/sx);
  else
   invSinTable[i]=(sx>0 ? 65536.0*10000.0 : -65536.0*10000.0);

  if (cx>0.0001 || cx<-0.0001)
   invCosTable[i]=(long)(65536.0/cx);
  else
   invCosTable[i]=(cx>0 ? 65536.0*10000.0 : -65536.0*10000.0);
 }
}

void DoneTables()
{
 delete sinTable,cosTable,tanTable,ctgTable,invSinTable,invCosTable;
}

#endif
