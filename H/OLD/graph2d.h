#ifndef __GRAPH2D_H__
#define __GRAPH2D_H__

#include <picture.h>
#include <vesalo.h>
#include <vgalo.h>
#include <dpmi32gw.h>

tBinFont* LoadBF(BinFile*);
BOOL      SaveBF(BinFile*,const tBinFont*);
tBinFont* LoadBIOSBF(int height);		// available 8/14/16

RGBPalette* loadRGBPal(BinFile*);
BOOL 	    saveRGBPal(BinFile*,const RGBPalette*);

class VESAdevice
{
protected:
      int LastOperationStatus,
	  Flags,
	  ChangeMode;
      VESAInfoStruct Info;
      int OldMode;
#ifdef __386__
      void far* DosMemoryBufferPtr;
      uint 	 DosMemoryBufferRealSeg;
      int	 DosMemoryBufferSize;
#endif
      int CheckVideoModeList(void) const;
      int ReadDosVar(const char* var="VESA");
      int SaveAdapter(void);
      int RestoreAdapter(void);
      int GetModeInfo(VESAModeInfoStruct* ptr,int mode);
public:	
      enum { 	ENABLE_2X		=0x0001,
	        ENABLE_DOS_VAR		=0x0002,
		ENABLE_LFB		=0x0004,
		ENABLE_INIT		=0x0008,
		ENABLE_RESTORE		=0x0010,
		ENABLE_DIRECT_CALL     	=0x0020,
		BUG_MODE_LIST		=0x0040,
		DEFAULT			=ENABLE_2X|ENABLE_DOS_VAR|ENABLE_LFB|ENABLE_INIT|ENABLE_DIRECT_CALL
	   };
       enum {   OK=0,
       	    	TOO_FEW_DOS_MEMORY,
		NOT_DETECTED,
		DEVICE_LOCKED,
		MODE_UNAVAILABLE
	    }; 
      VESAdevice(int mode=DEFAULT)	{ 	Flags=0; open(mode);	}
      int open(int mode=DEFAULT);
      int close(void);
      VESAInfoStruct* info(void){ return Flags?&Info:NULL; }
      int version(void) const 	{ return (Flags&ENABLE_2X)?Info.Version:0x100; }
      const CHPTR OemStr(void) const
      	    {	  return Flags?STRING_PTR(Info.OemStringPtr):NULL; }
      int TotalMemory(void) const
      	    {	  return Flags?Info.TotalVideoMem*64:0;	}
      ostream& adapterInfo(ostream&);
      ostream& modeInfo(ostream&,int);
      ostream& about(ostream& os);
      ~VESA(void)		{	close();	}
////////////////////////////////////////////////////////////////////////////
      Screen* SetMode(
};

/////////////////////////////////////////////////////////////////////////////
// Иерархия работы с экранами
// Примечание 1: в непалитровых режимах функции палитры игнорируются!
// Примечание 2: 
//     функции с префиксом _ не проверяют коррекность области отсечения
//     и корректность работы вообще
// Примечание 3: палитра - она либо есть(256 цветов), либо нет.

//////// Экран - протокласс 
class Screen
{
protected:
	int mWidth,mHeight,mBpp;
	cRect mClipRect;
public:
	BOOL open(int w,int h,int bpp);
	virtual void close(void);

	Screen(void) 			{ Screen::close(); }
	Screen(int w,int h,int bpp)     { open(w,h,bpp);   }

	virtual ~Screen(void)		{ close();	   }
//// Работа с характеристиками экрана //////////////////////
     	virtual BOOL IsValid(void) const;
	int width(void)  const { return mWidth;  }
	int height(void) const { return mHeight; }
	int bpp(void)	 const { return mBpp;	 }
	void GetViewPort(cRect* cr) const { *cr=mClipRect; }
	void SetViewPort(cRect* cr);
	void SetViewPort(int x0,int y0,int x1,int y1);	
/// Графические примитивы /////////////////////////////////
	virtual void _PutPixel(int x,int y,long color);
	virtual long _GetPixel(int x,int y);
	virtual void _vline(int x,int y,int h,long color);
	virtual void _hline(int x,int y,int w,long color);
	virtual void _line(int x0,int y0,int x1,int y1,long color);
	virtual void _bar(int left,int top,int right,int bottom,long color);

	void PutPixel(int x,int y,long color)  
	     		  { if(mClipRect.in(x,y)) _PutPixel(x,y,color); }
	long GetPixel(int x,int y)  
	     		  { return mClipRect.in(x,y)?_GetPixel(x,y):0; }
	void vline(int x,int y,int h,long color);
	void hline(int x,int y,int w,long color);
	void line(int x0,int y0,int x1,int y1,long color);
	void bar(int left,int top,int right,int bottom,long color);
	void rectangle(int left,int top,int right,int bottom,long color);
/// Работа с текстом /////////////////////////////////////
	virtual void _outchar(int x,int y,int char,long color,const tBinFont* font,const cRect* rect);
	virtual void _outcharf(int x,int y,int char,const tImage* image,const tBinFont* font,const cRect* rect);

	void outtext(int x,int y,const char* s,long color,const tBinFont* font);
	void outtextf(int x,int y,const char* s,const tImage* image,const tBinFont* font);
	void outtext(int x,int y,const char* s,const tSpriteFont* font);
/// Графические примитивы ////////////////////////////////
    	virtual void _PutImage(int x,int y,const tImage* image,const cRect* rect);
	virtual void _GetImage(int x,int y,int w,int h,tImage* image,const cRect* rect);
	virtual void _PutSprite(nt x,int y,const tSprite* image,const cRect* rect);

	void PutImage(int x,int y,const tImage* image);
	void GetImage(int x,int y,int w,int h,tImage* image);
	void PutSprite(int x,int y,const tSprite* image);
/// Палитра //////////////////////////////////////////////
	virtual void PutPalette(int index,int count,const RGBStruct* palette);
	virtual void PutPalette(int index,int count,const RGBQUAD* palette);
	virtual void GetPalette(int index,int count,RGBStruct* palette);
	virtual void GetPalette(int index,int count,RGBQUAD* palette);

	void PutPalette(int index,const RGBStruct* palette);
	void PutPalette(int index,const RGBQUAD* palette);
	void PutPalette(int index,int red,int green,int blue);
	void PutPalette(const RGBStruct* palette);
	void PutPalette(const RGBQUAD* palette);

	void GetPalette(int index,RGBStruct* palette);
	void GetPalette(int index,RGBQUAD* palette);
	void GetPalette(RGBStruct* palette);
	void GetPalette(RGBQUAD* palette);
};

class aScreen: public Screen
{
protected:
	int mX,mY;
	Screen* mOwner;
public:
	BOOL open(int w,int h,Screen* owner);
	virtual void close(void);

	aScreen(void) 			{ aScreen::close(); }
	aScreen(int w,int h,Screen* o)  { open(w,h,o);	    }

	virtual ~aScreen(void)		{ close();	   }
//// Работа с характеристиками экрана //////////////////////
     	virtual BOOL IsValid(void) const;
/// Графические примитивы /////////////////////////////////
	virtual void _PutPixel(int x,int y,long color);
	virtual long _GetPixel(int x,int y);
	virtual void _vline(int x,int y,int h,long color);
	virtual void _hline(int x,int y,int w,long color);
	virtual void _line(int x0,int y0,int x1,int y1,long color);
	virtual void _bar(int left,int top,int right,int bottom,long color);
/// Работа с текстом /////////////////////////////////////
	virtual void _outchar(int x,int y,int char,long color,const tBinFont* font,const cRect* rect);
	virtual void _outcharf(int x,int y,int char,const tImage* image,const tBinFont* font,const cRect* rect);
/// Графические примитивы ////////////////////////////////
    	virtual void _PutImage(int x,int y,const tImage* image,const cRect* rect);
	virtual void _GetImage(int x,int y,int w,int h,tImage* image,const cRect* rect);
	virtual void _PutSprite(nt x,int y,const tSprite* image,const cRect* rect);
/// Палитра //////////////////////////////////////////////
	virtual void PutPalette(int index,int count,const RGBStruct* palette);
	virtual void PutPalette(int index,int count,const RGBQUAD* palette);
	virtual void GetPalette(int index,int count,RGBStruct* palette);
	virtual void GetPalette(int index,int count,RGBQUAD* palette);	
};

template <class T> class FlatScreen:public Screen
{
protected:
	T* data;
public:
	FlatScreen(void)				{ data=NULL; }
	FlatScreen(int w,int h,T* ptr)			{ data=NULL; open(w,h,ptr); }
	BOOL open(int w,int h,T* ptr);
	T* GetPtr(void)	    				{ return data; }

	virtual void close(void);
// диагностика и информация
	virtual BOOL IsValid(void) const;
};

class fScreen8bit:public FlatScreen<uchar>
{
protected:
	BOOL mPaletteExist;	
public:
	BOOL	PaletteExist(void) const { return mPaletteExist; }
/// Графические примитивы /////////////////////////////////
	virtual void _PutPixel(int x,int y,long color);
	virtual long _GetPixel(int x,int y);
	virtual void _vline(int x,int y,int h,long color);
	virtual void _hline(int x,int y,int w,long color);
	virtual void _line(int x0,int y0,int x1,int y1,long color);
	virtual void _bar(int left,int top,int right,int bottom,long color);
/// Работа с текстом /////////////////////////////////////
	virtual void _outchar(int x,int y,int char,long color,const tBinFont* font,const cRect* rect);
	virtual void _outcharf(int x,int y,int char,const tImage* image,const tBinFont* font,const cRect* rect);
/// Графические примитивы ////////////////////////////////
    	virtual void _PutImage(int x,int y,const tImage* image,const cRect* rect);
	virtual void _GetImage(int x,int y,int w,int h,tImage* image,const cRect* rect);
	virtual void _PutSprite(nt x,int y,const tSprite* image,const cRect* rect);
/// Палитра //////////////////////////////////////////////
	virtual void PutPalette(int index,int count,const RGBStruct* palette);
	virtual void PutPalette(int index,int count,const RGBQUAD* palette);
	virtual void GetPalette(int index,int count,RGBStruct* palette);
	virtual void GetPalette(int index,int count,RGBQUAD* palette);	
};

class fScreen16bit:public FlatScreen<uint16>
{
public:
/// Графические примитивы /////////////////////////////////
	virtual void _PutPixel(int x,int y,long color);
	virtual long _GetPixel(int x,int y);
	virtual void _vline(int x,int y,int h,long color);
	virtual void _hline(int x,int y,int w,long color);
	virtual void _line(int x0,int y0,int x1,int y1,long color);
	virtual void _bar(int left,int top,int right,int bottom,long color);
/// Работа с текстом /////////////////////////////////////
	virtual void _outchar(int x,int y,int char,long color,const tBinFont* font,const cRect* rect);
	virtual void _outcharf(int x,int y,int char,const tImage* image,const tBinFont* font,const cRect* rect);
/// Графические примитивы ////////////////////////////////
    	virtual void _PutImage(int x,int y,const tImage* image,const cRect* rect);
	virtual void _GetImage(int x,int y,int w,int h,tImage* image,const cRect* rect);
	virtual void _PutSprite(nt x,int y,const tSprite* image,const cRect* rect);
};

class fScreen24bit:public FlatScreen<RGBStruct>
{
public:
/// Графические примитивы /////////////////////////////////
	virtual void _PutPixel(int x,int y,long color);
	virtual long _GetPixel(int x,int y);
	virtual void _vline(int x,int y,int h,long color);
	virtual void _hline(int x,int y,int w,long color);
	virtual void _line(int x0,int y0,int x1,int y1,long color);
	virtual void _bar(int left,int top,int right,int bottom,long color);
/// Работа с текстом /////////////////////////////////////
	virtual void _outchar(int x,int y,int char,long color,const tBinFont* font,const cRect* rect);
	virtual void _outcharf(int x,int y,int char,const tImage* image,const tBinFont* font,const cRect* rect);
/// Графические примитивы ////////////////////////////////
    	virtual void _PutImage(int x,int y,const tImage* image,const cRect* rect);
	virtual void _GetImage(int x,int y,int w,int h,tImage* image,const cRect* rect);
	virtual void _PutSprite(nt x,int y,const tSprite* image,const cRect* rect);
};

class fScreen32bit:public FlatScreen<ulong>
{
public:
/// Графические примитивы /////////////////////////////////
	virtual void _PutPixel(int x,int y,long color);
	virtual long _GetPixel(int x,int y);
	virtual void _vline(int x,int y,int h,long color);
	virtual void _hline(int x,int y,int w,long color);
	virtual void _line(int x0,int y0,int x1,int y1,long color);
	virtual void _bar(int left,int top,int right,int bottom,long color);
/// Работа с текстом /////////////////////////////////////
	virtual void _outchar(int x,int y,int char,long color,const tBinFont* font,const cRect* rect);
	virtual void _outcharf(int x,int y,int char,const tImage* image,const tBinFont* font,const cRect* rect);
/// Графические примитивы ////////////////////////////////
    	virtual void _PutImage(int x,int y,const tImage* image,const cRect* rect);
	virtual void _GetImage(int x,int y,int w,int h,tImage* image,const cRect* rect);
	virtual void _PutSprite(nt x,int y,const tSprite* image,const cRect* rect);
};

class memScreen8bit:public fScreen8bit
{
protected:
	tPicture *mPicture;
public:
	memScreen8bit(int w,int h,BOOL palexist=TRUE);
	memScreen8bit(tPicture*);
	virtual void close(void);	

	BOOL open(int w,int h,BOOL palexist=TRUE);
	BOOL open(tPicture*);

	tPicture*  GetPicture(void) const	{ return mPicture; }
	RGBStruct* GetPalette(void);
	tImage*	   GetImage(void);
};

class memScreen16bit:public fScreen16bit
{
protected:
	tPicture *mPicture;
public:
	memScreen16bit(int w,int h);
	memScreen16bit(tPicture*);
	virtual void close(void);	

	BOOL open(int w,int h);
	BOOL open(tPicture*);

	tPicture*  GetPicture(void) const	{ return mPicture; }
	tImage*	   GetImage(void);
};

class memScreen24bit:public fScreen24bit
{
protected:
	tPicture *mPicture;
public:
	memScreen24bit(int w,int h);
	memScreen24bit(tPicture*);
	virtual void close(void);	

	BOOL open(int w,int h);
	BOOL open(tPicture*);

	tPicture*  GetPicture(void) const	{ return mPicture; }
	tImage*	   GetImage(void);
};

class memScreen32bit:public fScreen32bit
{
protected:
	tPicture *mPicture;
public:
	memScreen32bit(int w,int h);
	memScreen32bit(tPicture*);
	virtual void close(void);	

	BOOL open(int w,int h);
	BOOL open(tPicture*);

	tPicture*  GetPicture(void) const	{ return mPicture; }
	tImage*	   GetImage(void);
};

/////////////////// VGA ////////////////////////////////////////
class vga13hScreen:public fScreen8bit
{
	int mOldMode,mOldVPage,mOldSize;
	void* mOldBuffer;
public:
	vga13hScreen(void);
	BOOL open(BOOL save=FALSE);
	virtual void close(void);	
/// Палитра //////////////////////////////////////////////
	virtual void PutPalette(int index,int count,const RGBStruct* palette);
	virtual void PutPalette(int index,int count,const RGBQUAD* palette);
	virtual void GetPalette(int index,int count,RGBStruct* palette);
	virtual void GetPalette(int index,int count,RGBQUAD* palette);
};


/////////////////// VESA ///////////////////////////////////////////////

#endif