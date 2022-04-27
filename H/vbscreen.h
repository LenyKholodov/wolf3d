#ifndef __BANK_SCREEN__
#define __BANK_SCREEN__

#include <screen.h>
#include <vesalo.h>
#include <vgalo.h>

class vbScreen8bit:public Screen
{
 VESAModeInfoStruct mi;
 int curBank;
public:
 vbScreen8bit(void):Screen() {}
 virtual void close(void);
 virtual ~vbScreen8bit(void);
 virtual BOOL IsValid(void) const ;
 BOOL open(int mode);
 virtual void _bar(int left,int top,int right,int bottom,long color);
 virtual void _line(int x1,int y1,int x2,int y2,long color);
 virtual void _hline(int xs,int y,int width,long color);
 virtual void _vline(int xs,int ys,int height,long color);
 virtual void _PutPixel(int x,int y,long color);
 virtual long _GetPixel(int x,int y);
 virtual void _PutImage(int x,int y,const tImage* image,const cRect* rect);
 virtual void _GetImage(int x,int y,int w,int h,tImage* image,const cRect* rect);

 virtual long rgb(char red,char green,char blue) ;
 virtual void rgb(long color,char* red,char* green,char* blue) ;

 virtual void _outchar(int x,int y,int ch,long color,
                                       const tBinFont* font,const cRect* rect);

 virtual void _PutPalette(int index,int count,const RGBStruct* palette);
 virtual void _PutPalette(int index,int count,const RGBQUAD* palette);
 virtual void _GetPalette(int index,int count,RGBStruct* palette);
 virtual void _GetPalette(int index,int count,RGBQUAD* palette);
};

class vbScreen16bit:public Screen
{
 VESAModeInfoStruct mi;
 int curBank;
 int bpl;
public:
 vbScreen16bit(void):Screen() {}
 virtual void close(void);
 virtual ~vbScreen16bit(void);
 virtual BOOL IsValid(void) const ;
 BOOL open(int mode);
 virtual void _PutPixel(int x,int y,long color);
 virtual long _GetPixel(int x,int y);
 virtual long rgb(char red,char green,char blue) const
         { return (blue>>3)+((green>>2)<<5)+((red>>3)<<11);}
 virtual void rgb(long color,char* red,char* green,char* blue) const
         { *red=(color>>11)<<3; *green=(color>>5)<<2; *blue=color>>3; }
 virtual void _bar(int left,int top,int right,int bottom,long color);
 virtual void _line(int x1,int y1,int x2,int y2,long color);
 virtual void _hline(int xs,int y,int width,long color);
 virtual void _vline(int xs,int ys,int height,long color);

 virtual void _PutImage(int x,int y,const tImage* image,const cRect* rect);
 virtual void _GetImage(int x,int y,int w,int h,tImage* image,const cRect* rect);

 virtual void _outchar(int x,int y,int ch,long color,
                                       const tBinFont* font,const cRect* rect);
};

class vbScreen24bit:public Screen
{
 VESAModeInfoStruct mi;
 int curBank;
 int bpl;
public:
 vbScreen24bit(void):Screen() {}
 virtual void close(void);
 virtual ~vbScreen24bit(void);
 virtual BOOL IsValid(void) const ;
 BOOL open(int mode);
 virtual void _PutPixel(int x,int y,long color);
 virtual long _GetPixel(int x,int y);
 virtual long rgb(char red,char green,char blue) const
         { return blue+(green<<8)+(red<<16);}
 virtual void rgb(long color,char* red,char* green,char* blue) const
         { *red=color>>16; *green=color>>8; *blue=color; }
 virtual void _bar(int left,int top,int right,int bottom,long color);
 virtual void _line(int x1,int y1,int x2,int y2,long color);
 virtual void _hline(int xs,int y,int width,long color);
 virtual void _vline(int xs,int ys,int height,long color);

 virtual void _PutImage(int x,int y,const tImage* image,const cRect* rect);
 virtual void _GetImage(int x,int y,int w,int h,tImage* image,const cRect* rect);

 virtual void _outchar(int x,int y,int ch,long color,
                                       const tBinFont* font,const cRect* rect);
};

#endif