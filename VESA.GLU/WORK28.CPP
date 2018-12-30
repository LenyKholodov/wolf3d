#include <iostream.h>
#include <dpmi32gw.h>
#include <vbscreen.h>
#include <screen.h>
#include <vesalo.h>

vbScreen8bit screen;

typedef struct
{
        int x,y,w,h,color;
} rectData;

#define RectCount        5
#define Speed            20

const int scale=2;

void RectShow(int x,int y,int w,int h,int color)
{
        screen.rectangle((x-w)/scale,(y-h)/scale,(x+w)/scale,(y+h)/scale,color);
}

rectData   rects[RectCount];

const int mode=VESA1024x768x8;

void main(void)
{
   int maxx,maxy,i,GetMaxX,GetMaxY;
// init graphics
   if(!screen.open(mode))
   {
        cout<<"Can't open mode"<<hex<<mode<<endl;
        return;
   }
// init data
   maxx=((GetMaxX=screen.getmaxx())+1)*scale;
   maxy=((GetMaxY=screen.getmaxy())+1)*scale;
   screen.rectangle(99,99,GetMaxX-99,GetMaxY-99,15);
   screen.SetViewPort(100,100,GetMaxX-100,GetMaxY-100);
   for(i=0;i<RectCount;i++)
   {
        rects[i].x=random(maxx);
        rects[i].y=random(maxy);
        rects[i].w=random(1000)+1;
        rects[i].h=random(1000)+1;
        rects[i].color=1+random(15);
   }
// run
   for(int mode=0;!kbhit();)
   {
     for(i=0;i<RectCount;i++)
     {
        RectShow(rects[i].x,rects[i].y,rects[i].w--,rects[i].h--,0);
        if(rects[i].w==0 || rects[i].h==0)
        {
                rects[i].x=random(maxx);
                rects[i].y=random(maxy);
                rects[i].w=random(1000)+1;
                rects[i].h=random(1000)+1;
                rects[i].color=1+random(15);
        }
        RectShow(rects[i].x,rects[i].y,rects[i].w,rects[i].h,rects[i].color);
     }
   }
// finish
   getch();
   screen.close();
}
