#ifndef __VGA_H__
#define __VGA_H__

#include <fscreen.h>
#include <vgalo.h>

/////////////////// VGA ////////////////////////////////////////
class vga13hScreen:public fScreen8bit
{
	int mOldMode,mOldVPage,mOldSize;
	void* mOldBuffer;
public:
	vga13hScreen(void):fScreen8bit()	{}
	BOOL open(BOOL save=FALSE);
	virtual void close(void);	
  	virtual int  GetType(void)    const;
/// Палитра //////////////////////////////////////////////
	virtual void _PutPalette(int index,int count,const RGBStruct* palette);
	virtual void _PutPalette(int index,int count,const RGBQUAD* palette);
	virtual void _GetPalette(int index,int count,RGBStruct* palette);
	virtual void _GetPalette(int index,int count,RGBQUAD* palette);
};

#endif