#include <vga.h>

tAllocFnPtr vga13hScreen::m_alloc_fn=malloc;
tFreeFnPtr  vga13hScreen::m_free_fn=free;

BOOL vga13hScreen::open(BOOL/*save*/)
{
	if(IsValid())
		return TRUE;
	if(FindVGA())
	{
		mOldMode=GetMode();
		SetMode(0x13);
#ifdef __386__
		fScreen8bit::open(320,200,(uchar*)0xA0000);
#else
		fScreen8bit::open(320,200,(uchar*)0xA0000000);
#endif
		return TRUE;
	}
	return FALSE;		
}

int vga13hScreen::GetType(void) const
{
	return ID_VGASCREEN;
}

void vga13hScreen::close(void)
{
	if(IsValid())
	{
		SetMode(mOldMode);
		fScreen8bit::close();
	}	
}

void vga13hScreen::_PutPalette(int index,int count,const RGBStruct* palette)
{
	::SetPalette(index,count,palette);
}

void vga13hScreen::_PutPalette(int index,int count,const RGBQUAD* palette)
{
	for(;count--;index++)
		::SetPalette(index,((long*)palette)[index]);
}

void vga13hScreen::_GetPalette(int index,int count,RGBStruct* palette)
{
	::GetPalette(index,count,palette);
}

void vga13hScreen::_GetPalette(int index,int count,RGBQUAD* palette)
{
	for(;count--;index++)
		((long*)palette)[index]=::GetPalette(index);
}

