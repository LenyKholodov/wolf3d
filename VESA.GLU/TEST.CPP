#include <iostream.h>
#include <dpmi32gw.h>
#include <ascreen.h>
#include <vbscreen.h>

vbScreen8bit screen;

const int mode=VESA800x600x8;

void main(void)
{
        if(!screen.open(mode))
        {
                cout<<"Can't open mode"<<hex<<mode<<endl;
                return;
        }
        aScreen scr;
        scr.open(100,100,screen.width()-200,screen.height()-200,&screen);
        screen.rectangle(99,99,screen.width()-99,screen.height()-99,12);
        screen.rectangle(97,97,screen.width()-97,screen.height()-97,10);
        for(int i=0;i<screen.width();i++)
        {
                scr.PutPixel(i,300,i);
                scr.line(0,0,i,screen.getmaxy(),i);
        }
        getch();
        screen.close();
        SetMode(3);
}