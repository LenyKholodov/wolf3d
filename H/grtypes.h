#ifndef __GR_TYPES_H__
#define __GR_TYPES_H__

#include <misc.h>
#include <binfile.h>


enum { STD_COLOR=1,
       BPP_8bit=1,
       HI_COLOR=2,
       BPP_16bit=2,
       TRUE_COLOR=3,
       BPP_24bit=3,
       TRUE32_COLOR=4,
       BPP_32bit=4};


#pragma pack(1)

class cRect
{
public:
        int left,top,right,bottom;
        cRect(void)        {}
        cRect(int l,int t,int r,int b) { setup(l,t,r,b); }
        BOOL inX(int x)      const { return x>=left && x<=right; }
        BOOL inY(int y)      const { return y>=top && y<=bottom; }
        BOOL in(int x,int y) const { return inX(x) && inY(y);    }
        BOOL empty(void)     const { return left>right || top>bottom; }
        cRect& operator=(const cRect& r) { left=r.left; right=r.right; top=r.top; bottom=r.bottom; return *this; }
        cRect& operator&=(const cRect& r){ if(left<r.left)    left=r.left;
                                                     if(right>r.right)  right=r.right;
                                           if(top<r.top)      top=r.top;
                                           if(bottom>r.bottom)bottom=r.bottom;
                                           return *this;
                                         }
        void setup(int l,int t,int r,int b) { left=l; right=r; top=t; bottom=b; }
        cRect& add(int x,int y)                { left+=x; right+=x; top+=y; bottom+=y; return *this; }
        int width(void)  const { return right-left+1; }
        int height(void) const { return bottom-top+1; }
        void dump(ostream& os) const { os<<'('<<left<<','<<top<<")-("
                                                          <<right<<','<<bottom<<")\n"; }
};

typedef struct
{
        int x,y;
} tPoint;


///////////////////////////////////////////////////////////////////////////
// Palette
///////////////////////////////////////////////////////////////////////////

typedef struct
{
        uchar blue;
        uchar green;
        uchar red;
        uchar reserved;
} RGBQUAD;

typedef union _COLOR32
{
        ulong all;
        RGBQUAD rgb;
        uchar r[4];
} COLOR32;

typedef struct
{
        uchar red, green, blue;
} RGBStruct;

typedef RGBStruct RGB_color, *RGB_color_ptr;

void ShiftPalette(int count,RGBStruct* ptr,int offset);

inline ulong RGB(uchar r,uchar g,uchar b) {  return (ulong(r)<<16)|(ulong(g)<<8)|ulong(b); }

void RGB2RGBQ(int count,const RGBStruct* rgb,RGBQUAD* q);
void QRGB2RGB(int count,const RGBQUAD* q,RGBStruct* rgb);

class RGBPalette
{
public:
        uint16 start,count;
        RGBStruct palette[1];

        static RGBPalette* load(BinFile*);
        static RGBPalette* load(const char* name);
        static RGBPalette* create(int s,int c);

        static ulong size(int n)
                    { return sizeof(RGBPalette)+sizeof(RGBStruct)*(n-1); }

        void shift(int offset) { ShiftPalette(count,palette,offset); }
        ulong size(void) const { return RGBPalette::size(count); }

        BOOL save(BinFile*) const;
        BOOL save(const char*) const;

};

typedef struct
{
        uint16 start,count;
        RGBQUAD palette[1];
} RGBQPalette;

///////////////////////////////////////////////////////////////////////////
// Image
///////////////////////////////////////////////////////////////////////////

class tImage
{
public:
        int16  width,height;
        uchar  data[1];

        static ulong size(int w,int h,int bpp)
                    { return sizeof(tImage)-1+ulong(w)*ulong(h)*ulong(bpp); }

        ulong  size(int bpp) const  { return size(width,height,bpp); }
        void   shift(uchar value)
                    { for(ulong i=ulong(width)*ulong(height);i--;data[i]+=value); }
};

class tImageList
{
public:
        uint16 count;
        uint16 bpp,palsize;
        uchar data[1];
//        long IndexList[count];
//        RGBStruct palette[palsize];
//        tImage image[0];

        static tImageList* load(BinFile*);
        static tImageList* load(const char* name);
        static tImageList* create(int c,int bitpp,tImage** imgs,RGBPalette* p=NULL);
        static tImageList* create(int c,int bitpp,cRect* imgs,int psize=0);

        tImage* GetImage(int i)
                { return (i>=0 && i<count)?(tImage*)(data+((long*)data)[i]):NULL; }

        tImage* operator[](int i) { return GetImage(i); }

        RGBPalette* GetPalette(void)
                { return palsize?(RGBPalette*)(data+count*sizeof(long)):NULL; }
        void shift(int value);

        ulong  size(void);

        BOOL save(BinFile*);
        BOOL save(const char*);
};

///////////////////////////////////////////////////////////////////////////
// Sprite
///////////////////////////////////////////////////////////////////////////

class tSprite
{
public:
        int16 x,y,width,height;
        uchar data[1];
//         Структура строки:
//        int16 len;
//                  (тип цепи:2bit,длина цепи):слово
//                       прозрачность,
//                       копирование
//                       z-цвет,
        ulong  size(void) const;
        void   shift(uchar value); // корректная работа только для режима 8bit!
};

class tSpriteList
{
public:
        uint16 count;
        int16 bpp,palsize;
        uchar data[1];
//        long IndexList[count];
//        RGBStruct palette[palsize];
//        tSprite image[0];
        static tSpriteList* load(BinFile*);
        static tSpriteList* load(const char* name);
        static tSpriteList* create(int c,int bitpp,tSprite** imgs,RGBPalette* p=NULL);

        void reindex(void);

        tSprite* GetImage(int i)
                { return (i>=0 && i<count)?(tSprite*)(data+((long*)data)[i]):NULL; }

        tSprite* operator[](int i) { return GetImage(i); }

        RGBPalette* GetPalette(void)
                { return palsize?(RGBPalette*)(data+count*sizeof(long)):NULL; }
        void shift(int value);

        ulong  size(void);

        BOOL save(BinFile*);
        BOOL save(const char*);
};

///////////////////////////////////////////////////////////////////////////
// Font
///////////////////////////////////////////////////////////////////////////

class tBinFont
{
public:
        uchar start,end;
        uchar widthb,height;
        uchar data[1];

        static tBinFont* load(BinFile*);
        static tBinFont* load(const char* name);
        static tBinFont* create(int st,int e,int w,int h);

        static ulong size(int st,int e,int w,int h)
               { return sizeof(tBinFont)-1+(e-st+1)*(1+h*((w+7)>>3)); }

        ulong size(void) const
              { return tBinFont::size(start,end,widthb*8,height); }


        int charsize(void) const { return widthb*height+1; }
        const uchar* chardata(int i) const
               { return (i>=start && i<=end)?data+charsize()*(i-start):NULL; }

        uchar* operator[](int i)
               { return (i>=start && i<=end)?data+charsize()*(i-start):NULL; }

        int width(const char*) const;
        int width(int i) const
                  { return (i>=start && i<=end)?data[charsize()*(i-start)]:0;}

        BOOL save(BinFile*);
        BOOL save(const char* name);
};

class tSpriteFont
{
public:
        long start;
        tSpriteList sprites;

        static tSpriteFont* load(BinFile*);
        static tSpriteFont* load(const char* name);
        static tSpriteFont* create(int st,tSpriteList* imgs);

        ulong size(void)      { return sizeof(long)+sprites.size(); }

        tSprite* operator[](int i)
               { return i>=start?sprites.GetImage(i-start):NULL; }

        int width(const char*);
        int width(int i);

        BOOL save(BinFile*);
        BOOL save(const char* name);
};

struct tCHRName
{
  uint16        HeaderOffset;
  uchar         FontName[4];
  uint16        FontSize;
  uchar         FontVersion[3];
};

class tBGIFont
{
public:
        uint16  CharsCount;
        uchar   Reserv1;
        uchar   FirstChar;
        uint16  DataOffset;
        uchar   FillFlag;
        uchar   UpperMargin;
        uchar   Reserv2;
        uchar   LowerMargin;
        uchar   Reserv3[5];
        uint16  Offsets[256];
        uchar   Widths[256];
        uchar   data[1];

        static tBGIFont* load(BinFile*);
        static tBGIFont* load(const char* name);

        ulong size(void) const { return CharsCount*256; }//Приблизительно

        BOOL    in(int i) const
                { return i>=FirstChar && i<FirstChar+CharsCount; }
        uint16* getdata(int i) const
          { return in(i) ? (uint16*)(data+Offsets[i-FirstChar]):NULL; }
        uint16* operator[](int i)
          { return getdata(i); }

        int width(const char*) const;
        int width(int i) const;
        int height(int i='A') const;

        BOOL save(BinFile*);
        BOOL save(const char* name);
};

#pragma pack();

char* ColorInfo(int bits);

#endif
