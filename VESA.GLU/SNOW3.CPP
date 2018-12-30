#include <vbscreen.h>
#include <vgalo.h>
#include <picture.h>

RGBStruct palette[256];

#define SNOW_COUNT   100
#define SNOW_PLANES  4

tPoint snow[SNOW_PLANES][SNOW_COUNT];

void main(void)
{
        int mode=VESA800x600x8;
        vbScreen8bit screen;
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