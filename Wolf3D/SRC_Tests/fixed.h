#ifndef __FIXED_H__
#define __FIXED_H__

#include <misc.h>
#include <iostream.h>

const double           FL_PI        =   3.14159265358979323846;
const double           FL_RAD       =   180.0/FL_PI;
const double           FL_GRADUS    =   FL_PI/180.0;
const double           FL_ANGLE_90  =   FL_PI/2.0;
const double           FL_ANGLE_180 =   FL_PI;
const double           FL_ANGLE_270 =   FL_ANGLE_90*1.5;
const double           FL_ANGLE_360 =   2*FL_PI;

class Fixed8
{
protected:
        long data;
public:
        Fixed8(void)                          {}
        Fixed8(int x)                        { data=long(x)<<8; }
        Fixed8(double x)                       { data=x*256; }
        Fixed8(const Fixed8& x)                { data=x.data; }
        Fixed8(int a,int b)                  { fraction(a,b); }

        void fraction(int a,int b)        { data=a; data<<=8; data/=b; }
        void frac0(int a,int b)                  { if(b) { data=a; data<<=8; data/=b; } else data=0; }

        operator int(void)    const { return data>>8; }
        operator double(void) const { return data/256.0f; }
        operator float (void) const { return data/256.0f; }

        Fixed8& operator+=(const Fixed8& x) { data+=x.data; return *this; }
        Fixed8& operator+=(int x)      { data+=long(x)<<8; return *this; }
        Fixed8& operator+=(double x)   { data+=x*256.0; return *this; }

        Fixed8& operator-=(const Fixed8& x) { data-=x.data; return *this; }
        Fixed8& operator-=(int x)    { data-=long(x)<<8; return *this; }
        Fixed8& operator-=(double x)  { data-=x*256.0; return *this; }

        Fixed8& operator*=(const Fixed8& x) { data=imulshr8(data,x.data); return *this; }
        Fixed8& operator*=(int x)      { data*=x; return *this; }
        Fixed8& operator*=(double x)   { data*=x; return *this; }

        Fixed8& operator/=(const Fixed8& x)  { data<<=8; data/=x.data; return *this; }
        Fixed8& operator/=(int x)      { data/=x; return *this; }
        Fixed8& operator/=(double x)   { data/=x; return *this; }

        Fixed8 operator+(const Fixed8& x) const  { Fixed8 a=*this; a+=x; return a; };
        Fixed8 operator+(int x)    const   { Fixed8 a=*this; a+=x; return a; };
        Fixed8 operator+(double x)   const { Fixed8 a=*this; a+=x; return a; };

        Fixed8 operator-(const Fixed8& x) const  { Fixed8 a=*this; a-=x; return a; };
        Fixed8 operator-(int x)    const          { Fixed8 a=*this; a-=x; return a; };
        Fixed8 operator-(double x)   const { Fixed8 a=*this; a-=x; return a; };

        Fixed8 operator*(const Fixed8& x) const  { Fixed8 a=*this; a*=x; return a; };
        Fixed8 operator*(int x)    const          { Fixed8 a=*this; a*=x; return a; };
        Fixed8 operator*(double x)   const { Fixed8 a=*this; a*=x; return a; };

        Fixed8 operator/(const Fixed8& x) const { Fixed8 a=*this; a/=x; return a; };
        Fixed8 operator/(int x)      const { Fixed8 a=*this; a/=x; return a; };
        Fixed8 operator/(double x)   const { Fixed8 a=*this; a/=x; return a; };

        Fixed8& operator=(const Fixed8& x)  { data=x.data; return *this; }
        Fixed8& operator=(int x)              { data=long(x)<<16; return *this; }
        Fixed8& operator=(double x)              { data=x*256.0; return *this; }

        friend ostream& operator<<(ostream&,const Fixed8&);
        friend istream& operator>>(istream&,Fixed8&);

        Fixed8 operator-(void) const { Fixed8 a; a.data=-data; return a; }
        friend class Fixed16;
};

inline ostream& operator<<(ostream& os,const Fixed8& x)
{
        os<<double(x);
        return os;
}

inline istream& operator>>(istream& is,Fixed8& x)
{
        double a;
        is>>a;
        x=a;
        return is;
}

class Fixed16
{
protected:
        long data;
public:
        Fixed16(void)                          {}
        Fixed16(int x)                        { data=long(x)<<16; }
        Fixed16(double x)               { data=x*65536; }
        Fixed16(const Fixed16& x)        { data=x.data; }
        Fixed16(const Fixed8& x)        { data=x.data<<8; }
        Fixed16(int a,int b)                  { fraction(a,b); }
        long        GetData() const           { return data; }
        void        SetData(long d)            { data = d;}

        void fraction(int a,int b)        { data=a; data<<=16; data/=b; }
        void frac0(int a,int b)                  { if(b) { data=a; data<<=16; data/=b; } else data=0; }

        operator int   (void)    const { return data>>16; }
        operator double(void) const { return data/65536.0; }
        operator Fixed8(void) const { Fixed8 a; a.data=data>>8; return a; }

        Fixed16& operator+=(const Fixed16& x) { data+=x.data; return *this; }
        Fixed16& operator+=(const Fixed8& x)  { data+=x.data<<8; return *this; }
        Fixed16& operator+=(int x)      { data+=long(x)<<16; return *this; }
        Fixed16& operator+=(double x)   { data+=x*65536.0; return *this; }

        Fixed16& operator-=(const Fixed16& x) { data-=x.data; return *this; }
        Fixed16& operator-=(const Fixed8& x)  { data-=x.data<<8; return *this; }
        Fixed16& operator-=(int x)    { data-=long(x)<<16; return *this; }
        Fixed16& operator-=(double x)  { data-=x*65536.0; return *this; }

        Fixed16& operator*=(const Fixed16& x) { data=imulshr16(data,x.data); return *this; }
        Fixed16& operator*=(const Fixed8& x)  { data=imulshr8(data,x.data); return *this; }
        Fixed16& operator*=(int x)      { data*=x; return *this; }
        Fixed16& operator*=(double x)   { data*=x; return *this; }

        Fixed16& operator/=(const Fixed16& x) { data<<=8; data/=(x.data>>8); return *this; }
        Fixed16& operator/=(const Fixed8& x)  { data<<=8; data/=x.data; return *this; }
        Fixed16& operator/=(int x)    { data/=x; return *this; }
        Fixed16& operator/=(double x)  { data/=x; return *this; }

        Fixed16 operator+(const Fixed16& x) const { Fixed16 a=*this; a+=x; return a; };
        Fixed16 operator+(const Fixed8& x) const { Fixed16 a=*this; a+=x; return a; };
        Fixed16 operator+(int x)    const { Fixed16 a=*this; a+=x; return a; };
        Fixed16 operator+(double x)   const { Fixed16 a=*this; a+=x; return a; };

        Fixed16 operator-(const Fixed8& x) const { Fixed16 a=*this; a-=x; return a; };
        Fixed16 operator-(const Fixed16& x) const { Fixed16 a=*this; a-=x; return a; };
        Fixed16 operator-(int x)    const { Fixed16 a=*this; a-=x; return a; };
        Fixed16 operator-(double x)   const { Fixed16 a=*this; a-=x; return a; };

        Fixed16 operator*(const Fixed8& x) const { Fixed16 a=*this; a*=x; return a; };
        Fixed16 operator*(const Fixed16& x) const { Fixed16 a=*this; a*=x; return a; };
        Fixed16 operator*(int x)    const { Fixed16 a=*this; a*=x; return a; };
        Fixed16 operator*(double x)   const { Fixed16 a=*this; a*=x; return a; };

        Fixed16 operator/(const Fixed8& x) const { Fixed16 a=*this; a/=x; return a; };
        Fixed16 operator/(const Fixed16& x) const { Fixed16 a=*this; a/=x; return a; };
        Fixed16 operator/(int x)      const { Fixed16 a=*this; a/=x; return a; };
        Fixed16 operator/(double x)   const { Fixed16 a=*this; a/=x; return a; };

        Fixed16& operator=(const Fixed8& x)    { data=x.data<<8; return *this; };
        Fixed16& operator=(const Fixed16& x)  { data=x.data; return *this; }
        Fixed16& operator=(const int& x)      { data=long(x)<<16; return *this; }
        Fixed16& operator=(const double& x)   { data=x*65536.0; return *this; }

        BOOL operator == (const Fixed16& f) const { return f.data == data; }
        BOOL operator > (const Fixed16& f) const { return data>f.data; }
        BOOL operator < (const Fixed16& f) const { return data<f.data; }
        BOOL operator >= (const Fixed16& f) const { return data>=f.data; }
        BOOL operator <= (const Fixed16& f) const { return data<=f.data; }
        BOOL operator != (const Fixed16& f) const { return data!=f.data; }

        BOOL operator == (const int& f) const { return f == (int)*this; }
        BOOL operator > (int f) const { return (int)*this>f; }
        BOOL operator < (int f) const { return (int)*this<f; }
        BOOL operator >= (int f) const { return (int)*this>=f; }
        BOOL operator <= (int f) const { return (int)*this<=f; }
        BOOL operator != (int f) const { return (int)*this!=f; }

        BOOL operator == (double f) const { return f == (double)*this; }
        BOOL operator > (double f) const { return (double)*this>f; }
        BOOL operator < (double f) const { return (double)*this<f; }
        BOOL operator >= (double f) const { return (double)*this>=f; }
        BOOL operator <= (double f) const { return (double)*this<=f; }
        BOOL operator != (double f) const { return (double)*this!=f; }

        Fixed16& operator >>= (int mask) { data>>=mask; return *this;}
        Fixed16& operator <<= (int mask) { data<<=mask; return *this;}

        Fixed16  operator >> (int mask) { Fixed16 a=*this; a>>=mask; return a;}
        Fixed16  operator << (int mask) { Fixed16 a=*this; a<<=mask; return a;}

        Fixed16& operator &= (long mask) { data&=mask; return *this;}
//        Fixed16  operator &  (long mask) { Fixed16 a=*this; a&=mask; return a; }

        Fixed16 operator ! () const { Fixed16 a; a.data=!data; return a;}
        friend ostream& operator<<(ostream&,const Fixed16&);
        friend istream& operator>>(istream&,Fixed16&);

        Fixed16 operator-(void) const { Fixed16 a; a.data=-data; return a; }
};

inline ostream& operator<<(ostream& os,const Fixed16& x)
{
        os<<double(x);
        return os;
}

inline istream& operator>>(istream& is,Fixed16& x)
{
        double a;
        is>>a;
        x=a;
        return is;
}

class Angle
{
  private:
           uint16       angle;
           #define GRADUS (0x10000/360)
           #define RAD    (0x10000/FL_ANGLE_180)
           #define RAD_2  (RAD/2)
  public:
        Angle(void)                          {}
        Angle(int ang)                      { angle = GRADUS*ang; }
        Angle(double x)               { angle=x*RAD_2; }
        Angle(const Fixed16& x)        { angle=(double)x*RAD_2; }
        Angle(const Fixed8& x)        { angle = (double)x*RAD_2; }
        uint16        GetData() const           { return angle; }
        void          SetData(uint16 d)            { angle = d;}

        operator int   (void)    const { return angle/GRADUS; }
        operator double(void) const { return angle/RAD_2; }
        operator Fixed8(void) const { return angle/RAD_2; }
        operator Fixed16(void) const { return angle/RAD_2; }

        Angle& operator+=(const Angle& x) { angle+=x.angle; return *this; }
        Angle& operator+=(const Fixed16& x) { angle+=(Angle)x*RAD_2; return *this; }
        Angle& operator+=(const Fixed8& x)  { angle+=(Angle)x*RAD_2; return *this; }
        Angle& operator+=(int x)      { angle+=x*GRADUS; return *this; }
        Angle& operator+=(double x)   { angle+=x*RAD_2; return *this; }

        Angle& operator-=(const Angle& x) { angle-=x.angle; return *this; }
        Angle& operator-=(const Fixed16& x) { angle-=(Angle)x*RAD_2; return *this; }
        Angle& operator-=(const Fixed8& x)  { angle-=(Angle)x*RAD_2; return *this; }
        Angle& operator-=(int x)      { angle-=x*GRADUS; return *this; }
        Angle& operator-=(double x)   { angle-=x*RAD_2; return *this; }

        Angle& operator*=(const Angle& x) { angle*=x.angle; return *this; }
        Angle& operator*=(const Fixed16& x) { angle*=(double)x; return *this; }
        Angle& operator*=(const Fixed8& x)  { angle*=(double)x; return *this; }
        Angle& operator*=(int x)      { angle*=x; return *this; }
        Angle& operator*=(double x)   { angle*=x; return *this; }

        Angle& operator/=(const Angle& x) { angle/=x.angle; return *this; }
        Angle& operator/=(const Fixed16& x) { angle/=(double)x; return *this; }
        Angle& operator/=(const Fixed8& x)  { angle/=(double)x; return *this; }
        Angle& operator/=(int x)      { angle/=x; return *this; }
        Angle& operator/=(double x)   { angle/=x; return *this; }

        Angle operator+(const Angle& x) const { Angle a=*this; a+=x; return a; };
        Angle operator+(const Fixed16& x) const { Angle a=*this; a+=x; return a; };
        Angle operator+(const Fixed8& x) const { Angle a=*this; a+=x; return a; };
        Angle operator+(int x)    const { Angle a=*this; a+=x; return a; };
        Angle operator+(double x)   const { Angle a=*this; a+=x; return a; };

        Angle operator-(const Angle& x) const { Angle a=*this; a-=x; return a; };
        Angle operator-(const Fixed16& x) const { Angle a=*this; a-=x; return a; };
        Angle operator-(const Fixed8& x) const { Angle a=*this; a-=x; return a; };
        Angle operator-(int x)    const { Angle a=*this; a-=x; return a; };
        Angle operator-(double x)   const { Angle a=*this; a-=x; return a; };

        Angle& operator=(const Angle& x)    { angle=x.angle; return *this; };
        Angle& operator=(const Fixed8& x)    { angle=(double)x*RAD_2; return *this; };
        Angle& operator=(const Fixed16& x)  { angle=(double)x*RAD_2; return *this; }
        Angle& operator=(const int& x)      { angle=x*GRADUS; return *this; }
        Angle& operator=(const double& x)   { angle=x*RAD_2; return *this; }

        BOOL operator == (const Angle& f) const { return f.angle == angle; }
        BOOL operator > (const Angle& f) const { return angle>f.angle; }
        BOOL operator < (const Angle& f) const { return angle<f.angle; }
        BOOL operator >= (const Angle& f) const { return angle>=f.angle; }
        BOOL operator <= (const Angle& f) const { return angle<=f.angle; }
        BOOL operator != (const Angle& f) const { return angle!=f.angle; }

        BOOL operator == (const int& f) const { return f == (int)*this; }
        BOOL operator > (int f) const { return (int)*this>f; }
        BOOL operator < (int f) const { return (int)*this<f; }
        BOOL operator >= (int f) const { return (int)*this>=f; }
        BOOL operator <= (int f) const { return (int)*this<=f; }
        BOOL operator != (int f) const { return (int)*this!=f; }

        BOOL operator == (double f) const { return f == (double)*this; }
        BOOL operator > (double f) const { return (double)*this>f; }
        BOOL operator < (double f) const { return (double)*this<f; }
        BOOL operator >= (double f) const { return (double)*this>=f; }
        BOOL operator <= (double f) const { return (double)*this<=f; }
        BOOL operator != (double f) const { return (double)*this!=f; }

        friend ostream& operator<<(ostream&,const Angle&);
        friend istream& operator>>(istream&,Angle&);
};


inline ostream& operator<<(ostream& os,const Angle& x)
{
        os<<double(x);
        return os;
}

inline istream& operator>>(istream& is,Angle& x)
{
        double a;
        is>>a;
        x=a;
        return is;
}

const Fixed16   FX_NULL       =    0;
const Fixed16   FX_ONE        =    1;
const Fixed16   FX_HALF       =    0.5;
const Fixed16   FX_VERY_LOW   =    0.0001;

const Fixed16           FX_PI        =   FL_PI;
const Fixed16           FX_RAD       =   180.0/FL_PI;
const Fixed16           FX_GRADUS    =   FL_PI/180.0;
const Fixed16           FX_ANGLE_90  =   FL_PI/2.0;
const Fixed16           FX_ANGLE_180 =   FL_PI;
const Fixed16           FX_ANGLE_270 =   FL_PI*1.5;
const Fixed16           FX_ANGLE_360 =   2.0*FL_PI;

const Angle             ANGLE_0      =   0.0;
const Angle             ANGLE_1      =   FL_PI/180;
const Angle             ANGLE_10     =   FL_PI/18.0;
const Angle             ANGLE_30     =   FL_PI/6.0;
const Angle             ANGLE_45     =   FL_PI/4.0;
const Angle             ANGLE_60     =   FL_PI/3.0;
const Angle             ANGLE_90     =   FL_PI/2.0;
const Angle             ANGLE_120    =   ANGLE_60*2.0;
const Angle             ANGLE_180    =   FL_PI;
const Angle             ANGLE_270    =   ANGLE_180*1.5;
const Angle             ANGLE_360    =   2.0*FL_PI;

#endif
