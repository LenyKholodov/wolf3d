#include <iostream.h>
#include <dpmi32gw.h>
#include <fscreen.h>
#include <vesalo.h>
#include <time.h>

class vScreen:public fScreen16bit
{
	VESAModeInfoStruct mi;
public:
	vScreen(void):fScreen16bit() {}
	virtual void close(void);
	virtual ~vScreen(void);
	BOOL open(int mode);
};

BOOL vScreen::open(int mode)
{
	uint16 rseg;
	void __far* mi_rm=DPMIDOSAlloc(sizeof(mi),&rseg);
	if(mi_rm==NULL)
		return FALSE;
	VESALOModeInfo(MakeDword(rseg,0),mode);
	_fmemcpy(&mi,mi_rm,sizeof(mi));
	DPMIDOSFree(FP_SEG(mi_rm));
	mData=(uint16*)DPMILinAddr(mi.PhysBasePtr,4*1024*1024);
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
	if(IsValid())
	{
		fScreen16bit::close();
		cout<<"close"<<endl;
		::SetMode(3);
	}
}

vScreen::~vScreen(void)
{
	close();
}

vScreen screen;

const int mode=VESA1024x768x16LFB;

void main(void)
{
   int GetMaxX,GetMaxY;
// init graphics
   if(!screen.open(mode))
   {
	cout<<"Can't open mode"<<hex<<mode<<endl;
	return;
   }
// init data
   GetMaxX=screen.getmaxx();
   GetMaxY=screen.getmaxy();
   for(int i=0;i<GetMaxY;i++)
   	   screen.hline(0,i,GetMaxX+1,i*250);
   screen.rectangle(99,99,GetMaxX-99,GetMaxY-99,15);
   screen.SetViewPort(100,100,GetMaxX-100,GetMaxY-100);
   tPoint x[4];
// run
   clock_t stime,etime;
   stime=clock();
   for(int frames=0;!kbhit();frames++)
   {
   	for(int i=0;i<3;i++)
	{
		x[i].x=random(GetMaxX);
		x[i].y=random(GetMaxY);
	}
//	screen.triangle(x,random(65536));
	int color=random(65536);
	screen.circle(x[0].x,x[0].y,random(200),color);
	screen.fill(x[0].x,x[0].y,frames,color);
   }
// finish   
   etime=clock();
   getch();
   screen.close();
   cout<<"FPS:"<<double(frames*CLOCKS_PER_SEC)/(etime-stime)<<endl;
}
