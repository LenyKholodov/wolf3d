#include <sbinfile.h>
#include <malloc.h>
#include <mem.h>
#include <picture.h>
#include <iostream.h>
#include <dpmi32gw.h>
#include <screen.h>
#include <vesalo.h>

class vScreen:public Screen
{
	uchar* LFB;
	VESAModeInfoStruct mi;
public:
	vScreen(void):Screen() {}
	virtual void close(void);
	virtual ~vScreen(void);
	BOOL open(int mode);

	virtual void _PutPixel(int x,int y,long color);
  	virtual long _GetPixel(int x,int y);

	virtual void PutPalette(int index,int count,const RGBStruct* palette);
	virtual void PutPalette(int index,int count,const RGBQUAD* palette);
	virtual void GetPalette(int index,int count,RGBStruct* palette);
	virtual void GetPalette(int index,int count,RGBQUAD* palette);
};

BOOL vScreen::open(int mode)
{
/*	uint16 rseg;
	void __far* mi_rm=DPMIDOSAlloc(sizeof(mi),&rseg);
	if(mi_rm!=NULL)
		putch('\007');
	VESALOModeInfo(MakeDword(rseg,0),mode);
	_fmemcpy(&mi,mi_rm,sizeof(mi));
	DPMIDOSFree(FP_SEG(mi_rm));
	LFB=(uchar*)DPMILinAddr(mi.PhysBasePtr,4*1024*1024);*/
	if(!VESALOSetMode(mode))
		return FALSE;
	mWidth=mi.XResolution;
	mHeight=mi.YResolution;
	mBpp=mi.BitsPerPixel;
	mClipRect.setup(0,0,mWidth-1,mHeight-1);
	return TRUE;
}

void vScreen::close(void)
{
//	LFB=NULL;
	::SetMode(3);
	Screen::close();
}

vScreen::~vScreen(void)
{
	close();
}

void vScreen::_PutPixel(int x,int y,long color)
{
 long adr=long(y)*mWidth+x;
 VESALOSetBank(adr>>16,adr>>16);
 *((char *)(0xA0000+unsigned(adr)))=color;
//	LFB[x+y*mWidth]=color;
}

long vScreen::_GetPixel(int x,int y)
{
 long adr=long(y)*mWidth+x;
 VESALOSetBank(adr>>16,adr>>16);
 return *((char *)(0xA0000+unsigned(adr)));
//	return LFB[x+y*mWidth];
}

void vScreen::PutPalette(int index,int count,const RGBStruct* palette)
{
	::SetPalette(index,count,palette);
}

void vScreen::PutPalette(int index,int count,const RGBQUAD* palette)
{
}

void vScreen::GetPalette(int index,int count,RGBStruct* palette)
{
	::GetPalette(index,count,palette);
}

void vScreen::GetPalette(int index,int count,RGBQUAD* palette)
{
}

vScreen screen;

const int mode=VESA640x480x8LFB;

RGBStruct palette[256];

#define SNOW_COUNT   1000
#define SNOW_PLANES  4

tPoint snow[SNOW_PLANES][SNOW_COUNT];

void main(void)
{
	cPicture picture;
	if(!picture.load("snow.bmp"))
		return;
	long i,j;
	if(!screen.open(mode))
	{
		cout<<"Can't open mode"<<hex<<mode<<endl;
		return;
	}

	ShiftPalette(picture.GetPaletteSize(),(RGBStruct*)picture.GetPalette(),-2);
	screen.PutPalette(0,picture.GetPaletteSize(),(RGBStruct*)picture.GetPalette());
	for(i=0;i<SNOW_PLANES;i++)
		for(j=0;j<SNOW_COUNT;j++)
		{
			snow[i][j].x=random(screen.width());
			snow[i][j].y=random(screen.height());
		}
	uchar* data=(uchar*)picture.GetData();
	while(!kbhit())
	{
		tPoint* c=snow[0];
		for(i=1;i<=SNOW_PLANES;i++)
			for(long j=0;j<SNOW_COUNT;j++)
			{
				screen.PutPixel(c->x,c->y,data[c->x+c->y*640]);
				if((c->y+=i)>screen.height())
					c->y=0;
				c->x+=i-(random(2*i));
				c->x%=screen.width();
				screen.PutPixel(c->x,c->y,i*16-1);
				c++;
			}
		WaitForVsync();
	}
	getch();
	screen.close();
}