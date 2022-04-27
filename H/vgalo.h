#ifndef __VGA_LO_H__
#define __VGA_LO_H__

#include <grtypes.h>

#define VGA_PALETTE_MASK  	   0x3C6
#define VGA_PALETTE_REGISTER_RD    0x3C7
#define VGA_PALETTE_REGISTER_WR    0x3C8
#define VGA_PALETTE_DATA       	   0x3C9
#define VGA_RETRACE_REG	       	   0x3DA

void SetMode(int mode);
int GetMode(void);
BOOL FindVGA(void);

// Ожидание хода обратного луча.
#ifdef __BORLANDC__
       #define WaitForVsync() while(!(inp(VGA_RETRACE_REG)&0x08))
#else
       inline void WaitForVsync(void) { while(!(inp(VGA_RETRACE_REG)&0x08)); }
#endif

void SetPalette(int index,long rgb);
void SetPalette(int index,int r,int g,int b);
long GetPalette(int index);

void SetPalette(int index,const RGBStruct* color); 
void GetPalette(int index,RGBStruct* color);
void SetPalette(int index,int count,const RGBStruct* color);
void GetPalette(int index,int count,RGBStruct* color);

inline void SetPalette(const RGBStruct* color)       { SetPalette(0,256,color); }
inline void GetPalette(RGBStruct* color)       	     { GetPalette(0,256,color); }

inline void SetPalette(const RGBPalette* c)   { SetPalette(c->start,c->count,c->palette); }
inline void GetPalette(RGBPalette* c)         { GetPalette(c->start,c->count,c->palette); }

#endif
