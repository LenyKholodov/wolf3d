#ifndef __A_SCREEN_H__
#define __A_SCREEN_H__

#include <screen.h>

class aScreen: public Screen
{
protected:
	Screen* mScr;
	int mX,mY;
  	virtual void CorrectCW(void);
public:
	aScreen(void)					{ mScr=NULL; mX=mY=0; }
	aScreen(int x0,int y0,int w,int h,Screen* ptr)	{ mScr=NULL; open(x0,y0,w,h,ptr); }
	BOOL open(int x0,int y0,int w,int h,Screen* ptr);
	virtual void close(void);
	virtual ~aScreen(void)				{ close();  }
//// Работа с характеристиками экрана //////////////////////
     	virtual BOOL IsValid(void)    const;
	virtual int  GetType(void)    const;
	Screen* ScrScreen(void)      { return mScr; }
	BOOL	ScrScreen(Screen* scr);
	int	ScrX(void) const  { return mX; }
	int	ScrY(void) const  { return mY; }
	int	ScrX(int x)	  { return mX=x; }
	int	ScrY(int y)	  { return mY=y; }
/// Графические примитивы /////////////////////////////////
    	virtual void _PutPixel(int x,int y,long color);
  	virtual long _GetPixel(int x,int y);
  	virtual void _vline(int x,int y,int h,long color);
  	virtual void _hline(int x,int y,int w,long color);
  	virtual void _line(int x0,int y0,int x1,int y1,long color);
  	virtual void _bar(int left,int top,int right,int bottom,long color);
/// Работа с текстом /////////////////////////////////////
    	virtual void _outchar(int x,int y,int ch,long color,const tBinFont* font,const cRect* rect);
  	virtual void _outcharf(int x,int y,int ch,const tImage* image,const tBinFont* font,const cRect* rect);
/// Графические примитивы ////////////////////////////////
    	virtual void _PutImage(int x,int y,const tImage* image,const cRect* rect);
  	virtual void _GetImage(int x,int y,int w,int h,tImage* image,const cRect* rect);
  	virtual void _PutSprite(int x,int y,const tSprite* image,const cRect* rect);
/// Палитра //////////////////////////////////////////////
    	virtual void _PutPalette(int index,int count,const RGBStruct* palette);
  	virtual void _PutPalette(int index,int count,const RGBQUAD* palette);
  	virtual void _GetPalette(int index,int count,RGBStruct* palette);
  	virtual void _GetPalette(int index,int count,RGBQUAD* palette);
// other
   	virtual long rgb(uchar red,uchar green,uchar blue);
  	virtual void rgb(long color,uchar* red,uchar* green,uchar* blue);
};

#endif
