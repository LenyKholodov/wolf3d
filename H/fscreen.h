#ifndef __FLAT_SCREEN_H__
#define __FLAT_SCREEN_H__

#include <screen.h>

template <class T> class FlatScreen:public Screen
{
protected:
	T* mData;
public:
	FlatScreen(void):Screen()			{ mData=NULL; }
	FlatScreen(int w,int h,void* ptr)			{ mData=NULL; open(w,h,ptr); }
	BOOL open(int w,int h,void* ptr);
	T* GetPtr(void)	    				{ return mData; }

	virtual void close(void);
	virtual ~FlatScreen(void)			{ close(); }
// диагностика и информация
	virtual BOOL IsValid(void) const;
	virtual int  GetType(void)    const;
/// Графические примитивы /////////////////////////////////
    	virtual void _PutPixel(int x,int y,long color);
  	virtual long _GetPixel(int x,int y);
  	virtual void _vline(int x,int y,int h,long color);
  	virtual void _line(int x0,int y0,int x1,int y1,long color);
/// Работа с текстом /////////////////////////////////////
    	virtual void _outchar(int x,int y,int ch,long color,const tBinFont* font,const cRect* rect);
  	virtual void _outcharf(int x,int y,int ch,const tImage* image,const tBinFont* font,const cRect* rect);
/// Графические примитивы ////////////////////////////////
    	virtual void _PutImage(int x,int y,const tImage* image,const cRect* rect);
  	virtual void _GetImage(int x,int y,int w,int h,tImage* image,const cRect* rect);
};

class fScreen8bit:public FlatScreen<uchar>
{
protected:
	BOOL mPaletteExist;	
public:
	BOOL	PaletteExist(void) const { return mPaletteExist; }
  	virtual void _hline(int x,int y,int w,long color);
  	virtual void _bar(int left,int top,int right,int bottom,long color);
/// Графические примитивы ////////////////////////////////
  	virtual void _PutSprite(int x,int y,const tSprite* image,const cRect* rect);
/// Палитра //////////////////////////////////////////////
//    	virtual void _PutPalette(int index,int count,const RGBStruct* palette);
//  	virtual void _PutPalette(int index,int count,const RGBQUAD* palette);
//  	virtual void _GetPalette(int index,int count,RGBStruct* palette);
//  	virtual void _GetPalette(int index,int count,RGBQUAD* palette);
// other
   	virtual long rgb(uchar red,uchar green,uchar blue);
  	virtual void rgb(long color,uchar* red,uchar* green,uchar* blue);
};

class fScreen16bit:public FlatScreen<uint16>
{
public:
/// Графические примитивы /////////////////////////////////
  	virtual void _hline(int x,int y,int w,long color);
  	virtual void _bar(int left,int top,int right,int bottom,long color);
/// Графические примитивы ////////////////////////////////
  	virtual void _PutSprite(int x,int y,const tSprite* image,const cRect* rect);
};

class fScreen32bit:public FlatScreen<ulong>
{
public:
/// Графические примитивы /////////////////////////////////
  	virtual void _hline(int x,int y,int w,long color);
  	virtual void _bar(int left,int top,int right,int bottom,long color);
/// Графические примитивы ////////////////////////////////
  	virtual void _PutSprite(int x,int y,const tSprite* image,const cRect* rect);
// other
   	virtual long rgb(uchar red,uchar green,uchar blue);
  	virtual void rgb(long color,uchar* red,uchar* green,uchar* blue);
};
  
#endif