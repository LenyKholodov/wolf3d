#include <ascreen.h>

tAllocFnPtr aScreen::m_alloc_fn=malloc;
tFreeFnPtr  aScreen::m_free_fn=free;

aScreen::~aScreen(void)
{
  close();  
}

void aScreen::CorrectCW(void)
{
  Screen::CorrectCW();
  if(IsValid())
    mClipRect&=cRect(-mX,-mY,mScr->getmaxx()-mX,mScr->getmaxy()-mY);
}

BOOL aScreen::open(int x0,int y0,int w,int h,Screen* ptr)
{
	if(ScrScreen(ptr))
	{
		ScrX(x0);
		ScrY(y0);
		return Screen::open(w,h,mScr->bpp());
	}
	return FALSE;
}

void aScreen::close(void)
{
	mScr=NULL;
	Screen::close();
}

BOOL aScreen::IsValid(void) const
{
	return Screen::IsValid() && !IsBad(mScr);
}

int aScreen::GetType(void) const
{
	return ID_ASCREEN;
}

BOOL aScreen::ScrScreen(Screen* scr)
{
	if(!IsBad(scr))
	{
		mScr=scr;
		return TRUE;
	}
	return FALSE;
}

void aScreen::_PutPixel(int x,int y,long color)
{
	mScr->_PutPixel(mX+x,mY+y,color);
}

long aScreen::_GetPixel(int x,int y)
{
	return mScr->_GetPixel(mX+x,mY+y);
}

void aScreen::_vline(int x,int y,int h,long color)
{
	mScr->_vline(x+mX,y+mY,h,color);
}

void aScreen::_hline(int x,int y,int w,long color)
{
	mScr->_hline(x+mX,y+mY,w,color);
}

void aScreen::_line(int x0,int y0,int x1,int y1,long color)
{
	mScr->_line(x0+mX,y0+mY,x1+mX,y1+mY,color);
}

void aScreen::_bar(int left,int top,int right,int bottom,long color)
{
	mScr->_bar(left+mX,top+mY,right+mX,bottom+mY,color);
}

void aScreen::_outchar(int x,int y,int ch,long color,const tBinFont* font,const cRect* rect)
{
	cRect cr(rect->left+mX,
	      	 rect->top+mY,
		 min(mScr->getmaxx(),rect->right+mX),
		 min(mScr->getmaxy(),rect->bottom+mY));
	if(!cr.empty())
		mScr->_outchar(x+mX,y+mY,ch,color,font,&cr);
}

void aScreen::_outcharf(int x,int y,int ch,const tImage* image,const tBinFont* font,const cRect* rect)
{
	cRect cr(rect->left+mX,
	      	 rect->top+mY,
		 min(mScr->getmaxx(),rect->right+mX),
		 min(mScr->getmaxy(),rect->bottom+mY));
	if(!cr.empty())
		mScr->_outcharf(x+mX,y+mY,ch,image,font,&cr);
}

void aScreen::_PutImage(int x,int y,const tImage* image,const cRect* rect)
{
	cRect cr(rect->left+mX,
	      	 rect->top+mY,
		 min(mScr->getmaxx(),rect->right+mX),
		 min(mScr->getmaxy(),rect->bottom+mY));
	if(!cr.empty())
		mScr->_PutImage(x+mX,y+mY,image,&cr);
}

void aScreen::_GetImage(int x,int y,int w,int h,tImage* image,const cRect* rect)
{
	cRect cr(rect->left+mX,
	      	 rect->top+mY,
		 min(mScr->getmaxx(),rect->right+mX),
		 min(mScr->getmaxy(),rect->bottom+mY));
	if(!cr.empty())
		mScr->_GetImage(x+mX,y+mY,w,h,image,&cr);
}

void aScreen::_PutSprite(int x,int y,const tSprite* image,const cRect* rect)
{
	cRect cr(rect->left+mX,
	      	 rect->top+mY,
		 min(mScr->getmaxx(),rect->right+mX),
		 min(mScr->getmaxy(),rect->bottom+mY));
	if(!cr.empty())
		mScr->_PutSprite(x+mX,y+mY,image,&cr);
}

void aScreen::_PutPalette(int index,int count,const RGBStruct* palette)
{
	if(IsValid())
		mScr->_PutPalette(index,count,palette);
}

void aScreen::_PutPalette(int index,int count,const RGBQUAD* palette)
{
	if(IsValid())
		mScr->_PutPalette(index,count,palette);
}

void aScreen::_GetPalette(int index,int count,RGBStruct* palette)
{
	if(IsValid())
		mScr->_GetPalette(index,count,palette);
}

void aScreen::_GetPalette(int index,int count,RGBQUAD* palette)
{
	if(IsValid())
		mScr->_GetPalette(index,count,palette);
}

long aScreen::rgb(uchar red,uchar green,uchar blue)
{
	return IsValid()?mScr->rgb(red,green,blue):0;
}

void aScreen::rgb(long color,uchar* red,uchar* green,uchar* blue)
{
	if(IsValid())
		mScr->rgb(color,red,green,blue);
}
