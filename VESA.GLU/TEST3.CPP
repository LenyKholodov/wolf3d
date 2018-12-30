#include <vbscreen.h>
#include <screen.h> 
#include <bios.h> 
#include <dpmi32gw.h>

void main()
{
 vbScreen8bit scr;
 scr.open    (VESA800x600x8);
 int width=scr.width()-1,height=scr.height()-1;
 while (!_bios_keybrd(1))
  scr.line(random(width),random(height),random(width),random(height),
    random(256));
 getch();
 while (!_bios_keybrd(1))
  scr.rectangle(random(width),random(height),random(width),random(height),
    random(256));
 getch();
 while (!_bios_keybrd(1))
  scr.bar(random(width),random(height),random(width),random(height),
    random(256));
 getch();
 while (!_bios_keybrd(1))
  scr.circle(random(width),random(height),random(100),random(256));
 getch();
 while (!_bios_keybrd(1))
 {
  long color=random(256);
   int x=random(width),y=random(height);
  scr.ellipse(x,y,random(100)+1,random(100)+1,color);
  scr.fill(x,y,random(256),color);
 }
 tImage *image=(tImage*)malloc(800*600+100);
 scr.GetImage(0,0,800,600,image);
 getch();
 scr.cls(0);
 scr.PutImage(0,0,image);
 getch();
 scr.close();
 SetMode(3);
}
