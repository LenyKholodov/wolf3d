#ifndef __FIXED_H__
#define __FIXED_H__

#include <misc.h>

#define GRADUS (0x10000/360)
#define RAD    (0x10000/FL_ANGLE_180)
#define RAD_2  (RAD/2)

typedef long    Fixed;
typedef ulong   uFixed;
typedef uint16  Angle;

const double    FL_PI        =   3.14159265358979323846;
const double    FL_RAD       =   180.0/FL_PI;
const double    FL_GRADUS    =   FL_PI/180.0;
const double    FL_ANGLE_90  =   FL_PI/2.0;
const double    FL_ANGLE_180 =   FL_PI;
const double    FL_ANGLE_270 =   FL_ANGLE_90*1.5;
const double    FL_ANGLE_360 =   2*FL_PI;

#define fixed2int(x) ((x)>>16)
#define int2fixed(x) ((x)<<16)
#define fixed2float(x) ((float)((x)/65536.0))
#define float2fixed(x) ((Fixed)((x)*65536.9))
#define float2angle(x) ((Angle)((x)*RAD_2))
#define int2angle(x)   ((Angle)(GRADUS*(x)))
#define angle2int(x)   ((int)((x)/GRADUS))
#define angle2float(x) ((float)((x)/RAD_2))

const Fixed     FX_NULL      =    0;
const Fixed     FX_ONE       =    0xFFFF;
const Fixed     FX_HALF      =    float2fixed(0.5);
const Fixed     FX_VERY_LOW  =    1;

const Fixed     FX_PI        =   float2fixed(FL_PI);
const Fixed     FX_RAD       =   float2fixed(180.0/FL_PI);
const Fixed     FX_GRADUS    =   float2fixed(FL_PI/180.0);

const Angle     ANGLE_0      =   0;
const Angle     ANGLE_1      =   float2angle(FL_PI/180.0);
const Angle     ANGLE_10     =   float2angle(FL_PI/18.0);
const Angle     ANGLE_30     =   float2angle(FL_PI/6.0);
const Angle     ANGLE_45     =   float2angle(FL_PI/4.0);
const Angle     ANGLE_60     =   float2angle(FL_PI/3.0);
const Angle     ANGLE_90     =   0x4000;
const Angle     ANGLE_120    =   ANGLE_60*2;
const Angle     ANGLE_180    =   0x8000;
const Angle     ANGLE_270    =   0xC000;
const Angle     ANGLE_360    =   0xFFFF; //0x10000

Fixed   mul     (Fixed,Fixed);
Fixed   div     (Fixed,Fixed);
Fixed   abs     (Fixed x);
Fixed   frac    (Fixed x);

#endif
