#include <sbinfile.h>
#include <malloc.h>
#include <mem.h>
#include <picture.h>
#include <iostream.h>
#include <dpmi32gw.h>
#include <screen.h>
#include <vbscreen.h>
#include <vesalo.h>

vbScreen8bit screen;

const int mode=VESA1024x768x8;

RGBStruct palette[256];

void main(void)
{
        cPicture picture;
        if(!picture.load("d:\\resource\\bitmaps\\atoll1.bmp"))
                return;
        long i=0,j,i2=0,j2=10;
        if(!screen.open(mode))
        {
                cout<<"Can't open mode"<<hex<<mode<<endl;
                return;
        }

        ShiftPalette(picture.GetPaletteSize(),(RGBStruct*)picture.GetPalette(),-2);
        screen.PutPalette(0,picture.GetPaletteSize(),(RGBStruct*)picture.GetPalette());
        tImage* data=picture.GetImage();
        screen.SetViewPort(50,50,screen.width()-50,screen.height()-50);
        for(j=15;!kbhit();)
        {
                i+=j;
                i2+=j2;
                if(i>screen.width() || i<0)
                        j=-j;
                if(i2>screen.height() || i2<0)
                        j2=-j2;
                screen.PutImage(i,i2,data);
        }
        getch();
        screen.close();
        SetMode(3);
}