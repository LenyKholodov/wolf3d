#include <conio.h>
#include <vbscreen.h>

BOOL vbScreen16bit::IsValid() const
{
 return TRUE;
}

vbScreen16bit screen;
int mode = VESA800x600x16;

void main()
{
 screen.open(mode);
 tBinFont* font=BFload("block.fon");
 if (NULL==font)
  return;
 screen.outtext(100,100,"Hello world!",0xFFFF,font);
 getch();
 screen.close();
}