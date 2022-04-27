#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <grtypes.h>

const int ID_SCREEN=0,
          ID_ASCREEN=1,
          ID_FSCREEN=2,
          ID_VGASCREEN=3,
          ID_MSCREEN=4,
          ID_VBSCREEN=5,
          ID_VFSCREEN=6;

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

  virtual void CorrectCW(void);
public:
  BOOL open(int w,int h,int bpp);
  virtual void close(void);

  Screen(void)                         { Screen::close(); }
  Screen(int w,int h,int bpp)     { open(w,h,bpp);   }

  virtual ~Screen(void)                { close();           }
//// Работа с характеристиками экрана //////////////////////
  virtual BOOL IsValid(void)     const;
  int                width(void)             const { return mWidth;  }
  int                height(void)            const { return mHeight; }
  int                bpp(void)               const { return mBpp;         }
  void                GetViewPort(cRect* cr) const { *cr=mClipRect; }
  const cRect& GetViewPort(void) const { return mClipRect; }
  void                SetViewPort(const cRect* w)     { mClipRect=*w; CorrectCW(); }
  void                SetViewPort(const cRect& w)     { mClipRect=w; CorrectCW(); }
  void                SetViewPort(int l,int t,int r,int b) { mClipRect.setup(l,t,r,b); CorrectCW(); }

  virtual int  GetType(void)    const;
  int                GetMaxX(void)    const   { return mWidth-1; }
  int                GetMaxY(void)    const   { return mHeight-1; }
  int                getmaxx(void)    const   { return mWidth-1; }
  int                getmaxy(void)    const   { return mHeight-1; }
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
  void rectangle(int xs,int ys,int xe,int ye,long color);
/// Работа с текстом /////////////////////////////////////
  virtual void _outchar(int x,int y,int,long color,const tBinFont* font,const cRect* rect);
  virtual void _outchar(int x,int y,int,long color,const tBGIFont* font);
  virtual void _outcharf(int x,int y,int,const tImage* image,const tBinFont* font,const cRect* rect);

  void outtext(int x,int y,const char* s,long color,const tBinFont* font);
  void outtext(int x,int y,const char* s,long color,const tBGIFont* font);
  void outtextf(int x,int y,const char* s,const tImage* image,const tBinFont* font);
  void outtext(int x,int y,const char* s,tSpriteFont* font);
/// Графические примитивы ////////////////////////////////
  virtual void _PutImage(int x,int y,const tImage* image,const cRect* rect);
  virtual void _GetImage(int x,int y,int w,int h,tImage* image,const cRect* rect);
  virtual void _PutSprite(int x,int y,const tSprite* image,const cRect* rect);

  void PutImage(int x,int y,const tImage* image)
       { if(IsValid()) _PutImage(x,y,image,&mClipRect); }

  void GetImage(int x,int y,int w,int h,tImage* image)
       { if(IsValid()) _GetImage(x,y,w,h,image,&mClipRect); }
  void PutSprite(int x,int y,const tSprite* image)
       { if(IsValid()) _PutSprite(x,y,image,&mClipRect); }
/// Палитра //////////////////////////////////////////////
  virtual void _PutPalette(int index,int count,const RGBStruct* palette);
  virtual void _PutPalette(int index,int count,const RGBQUAD* palette);
  virtual void _GetPalette(int index,int count,RGBStruct* palette);
  virtual void _GetPalette(int index,int count,RGBQUAD* palette);

  void PutPalette(int index,int count,const RGBStruct* palette)
       { _PutPalette(index,count,palette); }
  void PutPalette(int index,int count,const RGBQUAD* palette)
       { _PutPalette(index,count,palette); }
  void GetPalette(int index,int count,RGBStruct* palette)
       { _GetPalette(index,count,palette); }
  void GetPalette(int index,int count,RGBQUAD* palette)
       { _GetPalette(index,count,palette); }

  void PutPalette(int index,const RGBStruct* palette)
        { _PutPalette(index,1,palette); }
  void PutPalette(int index,const RGBQUAD* palette)
        { _PutPalette(index,1,palette); }
  void PutPalette(int index,int red,int green,int blue)
        { RGBStruct x={red,green,blue};  _PutPalette(index,1,&x); }
  void PutPalette(const RGBStruct* palette)
        { _PutPalette(0,256,palette); }
  void PutPalette(const RGBQUAD* palette)
        { _PutPalette(0,256,palette); }

  void GetPalette(int index,RGBStruct* palette)
        { _GetPalette(index,1,palette); }
  void GetPalette(int index,RGBQUAD* palette)
        { _GetPalette(index,1,palette); }
  void GetPalette(RGBStruct* palette)
        { _GetPalette(0,256,palette); }
  void GetPalette(RGBQUAD* palette)
        { _GetPalette(0,256,palette); }
// other
  void cls(long fill=0)
                         { if(IsValid()) _bar(mClipRect.left,mClipRect.top,
                                            mClipRect.right,mClipRect.bottom,fill); }
  virtual long rgb(uchar red,uchar green,uchar blue);
  virtual void rgb(long color,uchar* red,uchar* green,uchar* blue);

  long RGBColor(uchar red,uchar green,uchar blue) { return rgb(red,green,blue); }

  void line(int xs,int ys,int xe,int ye,long color,long lineStyle);

  void polyline(int count,const tPoint* p,long color);

  void circle(int xc,int yc,int r,long color);
  void ellipse(int xc,int yc,int a,int b,long color);
  void triangle(const tPoint* p,long color);
  void polygone(int count,const tPoint* p,long color);
  void triangle(int x1,int y1,int x2,int y2,int x3,int y3,long color)
                                     { tPoint p[3];
                            p[0].x=x1; p[0].y=y1;
                            p[1].x=x2; p[1].y=y2;
                             p[2].x=x3; p[2].y=y3;
                            triangle(p,color); }
  void fill(int x,int y,long fillColor,long color);
protected:
        int mColor,mFillColor;
        int LineFill(int x,int y,int dir,int PrevXL,int PrevXR);
};

#endif
