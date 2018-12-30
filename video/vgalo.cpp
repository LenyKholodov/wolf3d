#include <vgalo.h>
#ifdef __WATCOMC__
#include <i86.h>
#else
#include <dos.h>
#endif

void SetMode(int mode)
{
	union REGS regs;
#ifdef __386__
        regs.w.ax=lobyte(mode);
        int386(0x10,&regs,&regs);
#else
        regs.x.ax=lobyte(mode);
        int86(0x10,&regs,&regs);
#endif
}

int GetMode(void)
{
	union REGS regs;
        regs.h.ah=0x0F;
#ifdef __386__
        int386(0x10,&regs,&regs);
#else
        int86(0x10,&regs,&regs);
#endif
	return regs.h.al;
}

BOOL FindVGA(void)
{
	union REGS regs;
#ifdef __386__
        regs.w.ax=0x1A00;
        int386(0x10,&regs,&regs);
#else
        regs.x.ax=0x1A00;
        int86(0x10,&regs,&regs);
#endif
	return regs.h.al==0x1A;
}

void SetPalette(int index,long rgb) 
{  
	outp(VGA_PALETTE_MASK,0xFF);
	outp(VGA_PALETTE_REGISTER_WR,index);
	outp(VGA_PALETTE_DATA,rgb);
	outp(VGA_PALETTE_DATA,rgb>>8);
	outp(VGA_PALETTE_DATA,rgb>>16);
}

void SetPalette(int index,int r,int g,int b)
{  
	outp(VGA_PALETTE_MASK,0xFF);
	outp(VGA_PALETTE_REGISTER_WR,index);
	outp(VGA_PALETTE_DATA,r);
	outp(VGA_PALETTE_DATA,g);
	outp(VGA_PALETTE_DATA,b);
}

long GetPalette(int index)     	    
{ 
	COLOR32 rgb;
	outp(VGA_PALETTE_MASK,0xFF);
	outp(VGA_PALETTE_REGISTER_RD,index);
        insb(VGA_PALETTE_DATA,(void*)&rgb,3);
	return rgb.all&0xFFFFFF;
}


void SetPalette(int index,const RGBStruct* color)
{
	outp(VGA_PALETTE_MASK,0xFF);
	outp(VGA_PALETTE_REGISTER_WR,index);
	outp(VGA_PALETTE_DATA,color->red);
	outp(VGA_PALETTE_DATA,color->green);
	outp(VGA_PALETTE_DATA,color->blue);
}

void GetPalette(int index,RGBStruct* color)
{
	outp(VGA_PALETTE_MASK,0xFF);
	outp(VGA_PALETTE_REGISTER_RD,index);
	color->red  =inp(VGA_PALETTE_DATA);
	color->green=inp(VGA_PALETTE_DATA);
	color->blue =inp(VGA_PALETTE_DATA);
}

void SetPalette(int index,int count,const RGBStruct* color)
{
	outp(VGA_PALETTE_MASK,0xFF);
	outp(VGA_PALETTE_REGISTER_WR,index);
        outsb(VGA_PALETTE_DATA,(void*)color,sizeof(RGBStruct)*count);
}

void GetPalette(int index,int count,RGBStruct* color)
{
	outp(VGA_PALETTE_MASK,0xFF);
	outp(VGA_PALETTE_REGISTER_RD,index);
        insb(VGA_PALETTE_DATA,(void*)color,sizeof(RGBStruct)*count);
}


