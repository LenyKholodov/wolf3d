#include <tables.h>
#include "table.dat"

const Fixed* _sin    = (Fixed*)__sin;
const Fixed* _tan    = (Fixed*)__tan;
const Fixed* _invSin = (Fixed*)__invSin;

Fixed  sin(Angle angle)  {  return _sin[angle>>3]; }
Fixed  cos(Angle angle)  {  return sin(ANGLE_90-angle);      }
Fixed  isin(Angle angle)  {  return _invSin[angle]; }
Fixed  icos(Angle angle)  {  return isin(ANGLE_90-angle);      }
Fixed  tan(Angle angle)  {  return _tan[angle];    }
Fixed  ctan(Angle angle) {  return tan(ANGLE_90-angle);      }

Fixed   mul     (Fixed x,Fixed x1) { return imulshr16(x,x1); }
//Fixed   div     (Fixed x,Fixed x1) { return Fixed((x/x1)<<16); }
//Fixed   div     (Fixed x,Fixed x1) { return Fixed((x<<8)/((x1>>8)+1)); }
Fixed   abs     (Fixed x) { return x>0 ? x : -x; }
Fixed   frac    (Fixed x) { return x & 0xFFFF; }
Fixed   div     (Fixed x,Fixed x1)
{
  return  ((x/x1)<<16) + (frac(x)<<8)/(x1>>8);
}

Fixed   arsign  (Fixed x)
{
  if      (x==0) return 0;
  else if (x<0)  return -FX_ONE;
  else           return FX_ONE;
}

#include <math.h>

Angle  atan(Fixed x)
{
  return float2angle(atan(fixed2float(x)));
}