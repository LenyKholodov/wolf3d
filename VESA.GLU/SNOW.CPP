#include <sbinfile.h>
#include <dpmi32gw.h>
#include<malloc.h>
#include<mem.h>
#include <vesalo.h>
#include <picture.h>

RGBStruct palette[256];

void DownPalette(void)
{
        RGBStruct p[256];
        GetPalette(p);
        for(long i=0;i<32;i++)
        {
                for(long j=0;j<256;j++)
                {
                        if(p[j].red>2) p[j].red-=2; else p[j].red=0;
                        if(p[j].green>2) p[j].green-=2; else p[j].green=0;
                        if(p[j].blue>2) p[j].blue-=2; else p[j].blue=0;
                }
                WaitForVsync();
                SetPalette(p);
        }
}

uchar* LFB;

#define SNOW_COUNT   100
#define SNOW_PLANES  4

tPoint snow[SNOW_PLANES][SNOW_COUNT];

const long SCR_WIDTH=640,
          SCR_HEIGHT=480,
          SCR_H=SCR_HEIGHT,
          SCR_W=SCR_WIDTH;

inline void PutPixel(long x,long y,long color)
{
        LFB[x+long(y)*SCR_WIDTH]=color;
}

VESAModeInfoStruct mi;

void main(void)
{
        long oldmode=GetMode();
        cPicture picture;
        if(!picture.load("snow.bmp"))
                return;
        long i,j;
        DownPalette();
//        VESAInfoStruct vi;
//        if(!VESALOinfo(&vi))
//                return;
        uint16 rseg;
        void __far* mi_rm=DPMIDOSAlloc(sizeof(mi),&rseg);
        if(mi_rm!=NULL)
                putch('\007');
        VESALOModeInfo(MakeDword(rseg,0),VESA640x480x8LFB);
        _fmemcpy(&mi,mi_rm,sizeof(mi));
        DPMIDOSFree(FP_SEG(mi_rm));
        LFB=(uchar*)DPMILinAddr(mi.PhysBasePtr,4*1024*1024);
        if(!VESALOSetMode(VESA640x480x8LFB))
                return;
        putch('\007');
        ShiftPalette(picture.GetPaletteSize(),(RGBStruct*)picture.GetPalette(),-2);
        SetPalette(0,picture.GetPaletteSize(),(RGBStruct*)picture.GetPalette());
        for(i=0;i<SNOW_PLANES;i++)
                for(j=0;j<SNOW_COUNT;j++)
                {
                        snow[i][j].x=random(SCR_W);
                        snow[i][j].y=random(SCR_H);
                }
        uchar* data=(uchar*)picture.GetData();
        while(!kbhit())
        {
                tPoint* c=snow[0];
                for(i=1;i<=SNOW_PLANES;i++)
                        for(long j=0;j<SNOW_COUNT;j++)
                        {
                                PutPixel(c->x,c->y,data[c->x+c->y*640]);
                                if((c->y+=i)>SCR_H)
                                        c->y=0;
                                c->x+=i-(random(2*i));
                                c->x%=SCR_W;
                                PutPixel(c->x,c->y,i*16-1);
                                c++;
                        }
                WaitForVsync();
        }
        getch();
        DownPalette();
        SetMode(oldmode);
}