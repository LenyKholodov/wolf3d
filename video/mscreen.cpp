#include <debug.h>
#include <mscreen.h>

template <class Ts> void mScreen<Ts>::sclose(void)
{
	if(mOwner && mPicture!=NULL)
		  __free_fn(mPicture);
	mPicture=NULL;
	mOwner=FALSE;
	mRX=mRY=0; 
	mRefreshRect.setup(0,0,0,0);
	mRefreshScr=NULL;
}

template <class Ts> void mScreen<Ts>::GetRefreshRect(cRect* cr) const
{
  *cr=mRefreshRect;
}

template <class Ts> void mScreen<Ts>::SetRefreshRect(int l,int t,int r,int b)
{
  mRefreshRect.setup(max(0,l),max(0,t),r,b);  
  if(mRefreshScr!=NULL)
    mReallyRefreshRect.setup(max(0,mRefreshRect.left+mRX),
			     max(0,mRefreshRect.top+mRY),
			     min(mRefreshRect.right+mRX,mRefreshScr->GetMaxX()),
			     min(mRefreshRect.bottom+mRY,mRefreshScr->GetMaxY()));
  debug("mRefreshRect="<<mRefreshRect<<endl
        <<"mReallyRefreshRect="<<mReallyRefreshRect<<endl);
}

template <class Ts> int mScreen<Ts>::RefreshX(void) const
{
  return mRX;
}

template <class Ts> int mScreen<Ts>::RefreshY(void) const
{
  return mRY;
}

template <class Ts> void mScreen<Ts>::SetRefreshPoint(int x,int y)
{
  mRX=x;
  mRY=y;
  if(mRefreshScr!=NULL)
    mReallyRefreshRect.setup(max(0,mRefreshRect.left+mRX),
			     max(0,mRefreshRect.top+mRY),
			     min(mRefreshRect.right+mRX,mRefreshScr->GetMaxX()),
			     min(mRefreshRect.bottom+mRY,mRefreshScr->GetMaxY()));
}

template <class Ts> Screen* mScreen<Ts>::RefreshScreen(void)
{
  return mRefreshScr;
}

template <class Ts> BOOL mScreen<Ts>::RefreshScreen(Screen* scr)
{
  debug("Calling into RefreshScreen:"<<endl);
  if(IsBad(scr))
  {
     debug("mScreen::RefreshScreen: bad refresh screen"<<endl);
     return FALSE;
  }
  debug("\tbpp="<<scr->bpp()<<'('<<int(mPicture->bpp)<<')'<<endl);
  if(scr->bpp()==mPicture->bpp)
  {    
    debug("All correct: screen assigned"<<endl);
    mRefreshScr=scr;
    mReallyRefreshRect.setup(max(0,mRefreshRect.left+mRX),
                           max(0,mRefreshRect.top+mRY),
			   min(mRefreshRect.right+mRX,mRefreshScr->GetMaxX()),
			   min(mRefreshRect.bottom+mRY,mRefreshScr->GetMaxY()));
    debug("mRefreshRect="<<mRefreshRect<<endl
        <<"mReallyRefreshRect="<<mReallyRefreshRect<<endl
	<<"Rpoint=("<<mRX<<','<<mRY<<')'<<endl);

    return TRUE;
  }
  return FALSE;
}

template <class Ts> void mScreen<Ts>::refresh(void)
{
  if(IsValid()) 
    srefresh();
}

template <class Ts> tPicture* mScreen<Ts>::GetPicture(void)
{
  return mPicture;
}

template <class Ts> tPicture* mScreen<Ts>::freeze(void)
{
  mOwner=FALSE; 
  return mPicture;
}

template <class Ts> mScreen<Ts>::~mScreen(void)
{
  close(); 
}

template <class Ts> int mScreen<Ts>::GetType(void) const
{
  return ID_MSCREEN;
}

template <class Ts> void mScreen<Ts>::close(void)
{
  sclose();
  Ts::close();	
}


///////////////////////////////////////////////////////////////////////////

BOOL mScreen8bit::open(int w,int h,BOOL palexist)
{	
  close();
  mPicture=tPicture::create(0,0,w,h,8,palexist?256:0,__alloc_fn);
  mPaletteExist=palexist;
  if(mPicture!=NULL)
  {
    mRefreshRect.setup(0,0,w-1,h-1);
    mOwner=TRUE;
    return mScreen::open(w,h,mPicture->image.data);
  }
  return FALSE;
}

BOOL mScreen8bit::open(tPicture* p)
{	
  close();
  if(p==NULL || p->bpp!=8)
    return FALSE;
  mPicture=p;
  mPaletteExist=p->palSize!=0;
  mOwner=FALSE;
  mRefreshRect.setup(0,0,p->image.width-1,p->image.height-1);
  return mScreen<fScreen8bit>::open(p->image.width,p->image.height,mPicture->image.data);
}


void mScreen8bit::_PutPalette(int index,int count,const RGBStruct* palette)
{
  RGBStruct* pal=GetPalette();
  if(pal!=NULL)
    if((count=min(mPicture->palSize-index,count))>0)
      memcpy((pal+index),palette,count*sizeof(RGBStruct));
}

void mScreen8bit::_PutPalette(int index,int count,const RGBQUAD* palette)
{
  RGBStruct* pal=GetPalette();
  if(count+index>mPicture->palSize)
    count=mPicture->palSize-index;
  if(pal!=NULL)
    for(;count--;palette++)
    {
      pal[index].red=palette->red;
      pal[index].green=palette->green;
      pal[index].blue=palette->blue;
      index++;
    }
}

void mScreen8bit::_GetPalette(int index,int count,RGBStruct* palette)
{
  RGBStruct* pal=GetPalette();
  if(pal!=NULL)
    if((count=min(mPicture->palSize-index,count))>0)
      memcpy(palette,(pal+index),count*sizeof(RGBStruct));
}

void mScreen8bit::_GetPalette(int index,int count,RGBQUAD* palette)
{
  RGBStruct* pal=GetPalette();
  if(count+index>mPicture->palSize)
    count=mPicture->palSize-index;
  if(pal!=NULL)
    for(;count--;palette++)
    {
      palette->red=pal[index].red;
      palette->green=pal[index].green;
      palette->blue=pal[index].blue;
      index++;
    }
}

BOOL mScreen16bit::open(int w,int h)
{	
  close();
  if((mPicture=tPicture::create(0,0,w,h,16,0,__alloc_fn))!=NULL)
  {
    mRefreshRect.setup(0,0,w-1,h-1);
    mOwner=TRUE;
    return mScreen<fScreen16bit>::open(w,h,(uint16*)mPicture->image.data);
  }
  return FALSE;
}

long mScreen16bit::rgb(uchar red,uchar green,uchar blue)
{
  if(!IsValid())
    return 0;
  return (mPicture->bpp==15)?
     ((red>>3)<<10)|((green>>3)<<5)|(blue>>3): //15bit:5:5:5
     ((red>>3)<<11)|((green>>2)<<5)|(blue>>3); //16bit:5:6:5
}

void mScreen16bit::rgb(long color,uchar* red,uchar* green,uchar* blue)
{
  if(IsValid())
  {
    *blue=color<<3;
    if(mPicture->bpp==15)
    {
       *green=(color>>2)&0xF8;
       *red=(color>>7)&0xF8;
    }
    else // 16 bit
    {
       *green=(color>>3)&0xFC;
       *red=(color>>8)&0xF8;
    }
  }
}

BOOL mScreen32bit::open(int w,int h)
{	
  close();
  if((mPicture=tPicture::create(0,0,w,h,32,0,__alloc_fn))!=NULL)
  {
    mRefreshRect.setup(0,0,w-1,h-1);
    mOwner=TRUE;
    return mScreen<fScreen32bit>::open(w,h,(ulong*)mPicture->image.data);
  }
  return FALSE;
}


Screen* GimmeMirror(Screen* scr,tAllocFnPtr __a,tFreeFnPtr __f)
{
  if(IsBad(scr))
  {
    debug("Function GimmeMirror: bad scr\n");
  }
  debug("Function GimmeMirror: scr.bpp="<<scr->bpp()<<endl);
  Screen* mscr;
  switch(scr->bpp())
  {
    case 8:  mscr=new mScreen8bit(scr->width(),scr->height(),__a,__f); break;
    case 15: 
    case 16: mscr=new mScreen16bit(scr->width(),scr->height(),__a,__f); break;
    case 32: mscr=new mScreen32bit(scr->width(),scr->height(),__a,__f); break;
    default: mscr=NULL;
  }
  if(IsBad(mscr))
  {
    debug("GimmeMirror: screen can't create\n");
    delete mscr;
    return NULL;
  }
  mscr->RefreshScreen(scr);
  return mscr;
}
