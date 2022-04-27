#ifndef __PICTURE_H__
#define __PICTURE_H__

#include <grtypes.h>
#include <BinFile.h>

enum PICTURE_FILE_ERROR { PFE_OK=0,PFE_FILE,PFE_FORMAT,PFE_BPP };

#pragma pack(1);

typedef struct
{
	uint16 bfType;
	ulong  bfSize,
	       Reserved,
	       bfOffBits;
} BitmapFileHeader;

typedef struct
{
	ulong 	biSize;
	long  	biWidth,
	        biHeight;
	uint16  biPlanes,
	      	biBitCount;
	ulong 	biCompression,
	      	biSizeImage;
	long  	biXPelsPerMeter,
	      	biYPelsPerMeter;
	ulong 	biClrUsed,
	      	biClrImportant;
} BitmapInfoHeader;

typedef struct
{
	uchar manufacturer,	// =10
	      version,		// work with 5 only (ver 3.0)
	      encoding,		// =1
	      BitsPerPixel;	// work with 8 only
	int16 xmin,ymin,
	      xmax,ymax;
	int16 hRes,
	      vRes;
	char  ega_palette[48];
	char  reserved;
	uchar CountColorPlanes;//work with 1 only
	int16 BytesPerLine,
	      PaletteType;
	char  padding[58];
} PCXHeader;

class tPicture
{
public:
	uchar  bpp;
	ulong  palSize;
	long   x,y;
	tImage image;
	static ulong size(long w,long h,int bpp,int PalCount);
	static ulong size(const BitmapInfoHeader* bih);
	static ulong size(const PCXHeader* bih);

	static tPicture* create(long x0,long y0,long w,long h,int bpp,int PalCount=0);
	static tPicture* create(const BitmapInfoHeader* bih);
	static tPicture* create(const PCXHeader* bih);

	void init(long x0,long y0,long w,long h,int Bpp,int PalCount)
	     	       { bpp=Bpp; palSize=PalCount; image.width=w; image.height=h; x=x0; y=y0; }
	void init(const BitmapInfoHeader* bih)
	     	       { init(0,0,bih->biWidth,bih->biHeight,bih->biBitCount,bih->biClrUsed==0?256:bih->biClrUsed); }
        void init(const PCXHeader* bih)
	     	       { init(bih->xmin,bih->ymin,bih->xmax-bih->xmin+1,bih->ymax-bih->ymin+1,8,256); }

	ulong size(void) const
	      { return size(image.width,image.height,bpp,palSize); }

	RGBStruct* GetPalette(void) 
		   { return (bpp==8 && palSize)?(RGBStruct*)(image.data+ulong(image.width)*ulong(image.height)):NULL; }
};

#pragma pack()

int BMPWrite(BinFile& wr,const tPicture* image);
int BMPWrite(BinFile& wr,const tImage* image,int bpp,int palCount=0,const RGBQUAD* pal=NULL);

int BMPReadHeader(BinFile& rd,BitmapInfoHeader* bih);

int BMPReadBody(BinFile& rd,tPicture* image);
int BMPReadBody(BinFile& rd,BitmapInfoHeader* bih,tImage* image,RGBQPalette* pal=NULL);

int PCXWrite(BinFile& wr,const tPicture* image);

int PCXReadHeader(BinFile& rd,PCXHeader* bih);
int PCXReadBody(BinFile& rd,tPicture* image);
int PCXReadBody(BinFile& rd,PCXHeader* bih,tImage* image,RGBStruct* pal=NULL);

class cPicture
{
protected:
	tPicture* src;
	tPicture** picture;
public:
	cPicture(void) 		 { src=NULL; picture=NULL; }
	cPicture(long x,long y,long width,long height,int bpp,int PalCount)
		      	     	 { src=NULL; init(x,y,width,height,bpp,PalCount); }

	void Free(void)          { if(src) { free(src); src=NULL; } picture=NULL; }

	tPicture* GetPicture(void) { return picture!=NULL?*picture:NULL; }
	tImage*   GetImage(void)   { return IsValid()?&((*picture)->image):NULL; }
	void*	  GetData(void)	   { return IsValid()?(*picture)->image.data:0; }

	int	  bpp(void)            const { return IsValid()?(*picture)->bpp:0; }

	long	   GetPaletteSize(void) const
		   		    { return bpp()==8?(*picture)->palSize:0; }
	RGBStruct* GetPalette(void) { return IsValid()?(*picture)->GetPalette():NULL; }

	BOOL PCXload(const char*);
	BOOL BMPload(const char*);
	BOOL PCXload(BinFile&);
	BOOL BMPload(BinFile&);

	BOOL load(const char* name)  { if(BMPload(name)) return TRUE; 
	     		      	       return PCXload(name); }
	BOOL load(BinFile& rd)	     { if(BMPload(rd)) return TRUE;
	     		   	       rd.reset(); return PCXload(rd); }

	BOOL PCXsave(const char*);
	BOOL BMPsave(const char*);
	BOOL PCXsave(BinFile& wr) { return IsValid()?PCXWrite(wr,*picture)==PFE_OK:FALSE; }
	BOOL BMPsave(BinFile& wr) { return IsValid()?BMPWrite(wr,*picture)==PFE_OK:FALSE; }

	BOOL init(long x,long y,long width,long height,int bpp,int PalCount);
	void init(tPicture** ptr)    { Free(); picture=ptr; }

	BOOL IsValid(void) const { return picture!=NULL && (*picture)!=NULL; }

	long x(void)	   const { return IsValid()?(*picture)->x:0; }
	long y(void)	   const { return IsValid()?(*picture)->y:0; }
	void x(long x)	   	 { if(IsValid()) (*picture)->x=x; }
	void y(long y)	   	 { if(IsValid()) (*picture)->y=y; }
	long width(void)   const { return IsValid()?(*picture)->image.width:0; }
	long height(void)  const { return IsValid()?(*picture)->image.height:0; }

	~cPicture(void)    	 { Free(); }
	void swapRB(void);
};

#endif