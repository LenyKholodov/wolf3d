#ifndef  __VIDEO_PRO_H__
#define __VIDEO_PRO_H__
#include <iostream.h>
#include <string.h>
#include <misc.h>

typedef struct
{
	int x,y,BytesPerPixel;
	uchar* data;
} ImageStruct;

class PointStruct
{
public:
	int x,y;
	friend ostream& operator<<(ostream&,PointStruct const&);
};

inline ostream& operator<<(ostream& os,PointStruct const& p)
		{ return os<<'('<<p.x<<','<<p.y<<')'; }

class Image: public ImageStruct
{
public:
//	int x,y,BytesPerPixel;
//	uchar* data;
};

class RectStruct
{
public:
	int left,top,right,bottom;
	RectStruct(void)				  {}
	RectStruct(int l,int t,int r,int b)
		       :left(l),top(t),right(r),bottom(b) {}
	RectStruct(RectStruct& r)
		       { left=r.left;top=r.top;right=r.right;bottom=r.bottom; }	
	RectStruct& operator=(RectStruct& r)
	           { left=r.left;top=r.top;right=r.right;bottom=r.bottom; return *this; }	
	RectStruct& operator&=(RectStruct& r)
		   { left=max(left,r.left); top=max(top,r.top);
		     right=min(right,r.right); bottom=min(bottom,r.bottom); 
		     return *this; }
	friend ostream& operator<<(ostream&,RectStruct const&);
};

inline ostream& operator<<(ostream& os,RectStruct const& p)
		{ return os<<'('<<p.left<<','<<p.top<<','<<p.right<<','<<p.bottom<<')'; }
		                                        

class Font;

class ScreenPro;

class Font
{
protected:
public:
	virtual int OutText(int x,int y,const char* text,int color,ScreenPro& scr);
};

/***********************************************************************
* Протокласс для экрана. Содержит длину, ширину, ограничения по высоте,*
* ширине, прототипы рисования линий, дуг, прямоугольников и пр. лабуды *
***********************************************************************/
const int SCREEN_PRO_ID=0x100;

class ScreenPro
{
protected:
	int width,		// ширина
	    height,		// высота
	    type,		// тип экрана
	    BytesPerPixel;	// размер точки
	RectStruct window;  	// окно отсечения
public:
	int GetWidth(void)     const   { return width; }
	int GetHeight(void)    const   { return height; }
	int GetMaxX(void)      const   { return width-1; }
	int GetMaxY(void)      const   { return height-1; }
	int GetType(void)      const   { return type; }
	int GetPixelSize(void) const   { return BytesPerPixel; }
	ScreenPro(void);
	ScreenPro(int w,int h,int t);
	~ScreenPro(void)    		  {  }
	void GetViewPort(RectStruct* w)    const  { memcpy((void*)w,(void*)&window,sizeof(RectStruct)); }
	void SetViewPort(RectStruct const*);
	void SetViewPort(int l,int t,int r,int b);
	int IsGood(void) const        { return IsValid(); }
	int IsBad(void) const         { return !IsValid(); }
	int cls(int fill=0) 	      { return bar(0,0,width,height,fill); }

	virtual int PutPixel(int x,int y,int color);
	virtual int GetPixel(int x,int y);

	virtual int line(int xs,int ys,int xe,int ye,int color);
	virtual int lineXOR(int xs,int ys,int xe,int ye,int color);
	virtual int lineOR(int xs,int ys,int xe,int ye,int color);
	virtual int lineAND(int xs,int ys,int xe,int ye,int color);

	virtual int lineh(int x,int y,int w,int color);
	virtual int linehXOR(int x,int y,int w,int color);
	virtual int linehOR(int x,int y,int w,int color);
	virtual int linehAND(int x,int y,int w,int color);

	virtual int linev(int x,int y,int h,int color);
	virtual int linevXOR(int x,int y,int h,int color);
	virtual int linevOR(int x,int y,int h,int color);
	virtual int linevAND(int x,int y,int h,int color);

	virtual int rectangle(int x,int y,int w,int h,int color);
	virtual int rectangleXOR(int x,int y,int w,int h,int color);
	virtual int rectangleOR(int x,int y,int w,int h,int color);
	virtual int rectangleAND(int x,int y,int w,int h,int color);

	virtual int bar(int x,int y,int w,int h,int color);
	virtual int barXOR(int x,int y,int w,int h,int color);
	virtual int barOR(int x,int y,int w,int h,int color);
	virtual int barAND(int x,int y,int w,int h,int color);

	virtual int IsValid(void) const;

	virtual int GetImage(int x,int y,int w,int h,uchar* image);
	virtual int PutImage(int x,int y,int w,int h,uchar* image);
	virtual int PutImage(int x,int y,int w,int h,uchar* image,RectStruct*);
	virtual int PutSprite(int x,int y,int w,int h,uchar* image,int empty=0);

	virtual int PutImage(int x,int y,Image& iml);
	virtual int PutImage(int x,int y,Image& iml,RectStruct*);
	virtual int PutSprite(int x,int y,Image& iml);

	int OutText(int x,int y,const char* text,Font& fnt,int color=0)
	    { return fnt.OutText(x,y,text,color,*this); }

	virtual ostream& about(ostream&) const;

	friend ostream& operator<<(ostream&,ScreenPro&);
};

inline ostream& operator<<(ostream& os,ScreenPro const& scr)
       		{ return scr.about(os); }

/****************************************************************************
*    Прототип для экранов с линейной структурой памяти                      *
****************************************************************************/
const int SCREEN_FLAT_ID=SCREEN_PRO_ID+1;

class ScreenFlat: public ScreenPro
{
protected:
	uchar __far* data;
public:
	ScreenFlat(void):ScreenPro(0,0,SCREEN_FLAT_ID)		{ data=NULL; }
	ScreenFlat(int w,int h,uchar* ptr,int t=SCREEN_FLAT_ID);
	~ScreenFlat(void)    		  			{  }
	virtual int IsValid(void) const;
	virtual uchar __far* lfb(void);
	virtual ostream& about(ostream&) const;
};

/****************************************************************************
*    			   Прототип для экранов в памяти                    *
****************************************************************************/
const int SCREEN_MEM_ID=SCREEN_FLAT_ID+1;

class ScreenMem:public ScreenFlat
{
protected:
	uchar* rptr;
	RectStruct rwin;
	int  setup(int w,int h,int bpp)	      { return setup(w,h,bpp,w*h*bpp); }
	int  setup(int w,int h,int bpp,int size);
	void free(void);
public:	
	int rx,ry;
	ScreenPro* rscr;
	ScreenMem(void);
	ScreenMem(int w,int h,int bpp,int t=SCREEN_MEM_ID);
	ScreenMem(int w,int h,int bpp,int t,long size);
	~ScreenMem(void)    	  	  		{ free(); }
	virtual ostream& about(ostream&) const;
	int refresh(void)		             { return rscr==NULL?0:rscr->PutImage(rx,ry,width,height,rptr,&rwin); }
	void GetRefreshPort(RectStruct* w)    const  { memcpy((void*)w,(void*)&rwin,sizeof(RectStruct)); }
	void SetRefreshPort(RectStruct const*);
	void SetRefreshPort(int l,int t,int r,int b);
};

/*class Palette;

class Screen8bit;
class Screen16bit;
class Screen24bit;
class Screen32bit;

class ScreenMemPal:ScreenMem,ScreenPal,Screen8bit;
class ScreenMem16bit:ScreenMem,Screen16bit;
class ScreenMem24bit:ScreenMem,Screen24bit;
class ScreenMem32bit:ScreenMem,Screen32bit;

class ScreenVESA;
class ScreenVESALinear:ScreenVESA,ScreenFlat;
class ScreenVESABanked:ScreenVESA;
class ScreenVESANBankedDirectCall:ScreenVESABanked;

class ScreenVESALinearPal:ScreenVESALinear,Screen8bit,ScreenPal;
class ScreenVESALinear16bit:ScreenVESALinear,Screen16bit;
class ScreenVESALinear24bit:ScreenVESALinear,Screen24bit;
class ScreenVESALinear32bit:ScreenVESALinear,Screen32bit;

class ScreenVESABankedPal:ScreenVESABanked,Screen8bit,ScreenPal;
class ScreenVESABanked16bit:ScreenVESABanked,Screen16bit;
class ScreenVESABanked24bit:ScreenVESABanked,Screen24bit;
class ScreenVESABanked32bit:ScreenVESABanked,Screen32bit;

class ScreenVESABankedDCPal:ScreenVESABankedDirectCall,Screen8bit,ScreenPal;
class ScreenVESABankedDC16bit:ScreenVESABankedDirectCall,Screen16bit;
class ScreenVESABankedDC24bit:ScreenVESABankedDirectCall,Screen24bit;
class ScreenVESABankedDC32bit:ScreenVESABankedDirectCall,Screen32bit;

class Sprite:Image;
class ImageList:ScreenPal;

class SpriteFont:ImageList,Font;
class BFont:Font;
  */
#endif