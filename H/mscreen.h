#ifndef __MEM_SCREEN_H__
#define __MEM_SCREEN_H__

#include <fscreen.h>
#include <picture.h>

class mScreenSub
{
protected:	
	BOOL 	  mOwner;
	tPicture *mPicture;
	cRect	  mRefreshRect;
	Screen*	  mRefreshScr;
	int	  mRX,mRY;
	void	  sclose(void);
	void	  srefresh(void) { if(mRefreshScr!=NULL && mRefreshScr->IsValid()) _refresh(); }
public:
  	void GetRefreshRect(cRect* cr) const;
  	void SetRefreshRect(int l,int t,int r,int b);
  	void SetRefreshRect(const cRect* cr) { SetRefreshRect(cr->left,cr->top,cr->right,cr->bottom); }

	int  RefreshX(void) const { return mRX; }
	int  RefreshY(void) const { return mRY; }

	int  RefreshX(int);
	int  RefreshY(int);

	void SetRefreshPoint(int x,int y) { RefreshX(x); RefreshY(y); }
	Screen* RefreshScreen(void) 	  { return mRefreshScr; }
	BOOL 	  RefreshScreen(Screen*);
	void _refresh(void) 
	     	{ mRefreshScr->_PutImage(mRX,mRY,&mPicture->image,&mRefreshRect); }

	tPicture*  GetPicture(void)		{ return mPicture; }
	tPicture*  freeze(void)		  	{ mOwner=FALSE; return mPicture; }
};

class mScreen8bit:public fScreen8bit, public mScreenSub
{
public:
	void refresh(void)			{ if(IsValid()) srefresh(); }
	mScreen8bit(void):fScreen8bit() 	{ mOwner=FALSE; close(); }
	mScreen8bit(int w,int h,BOOL px=TRUE)	{ mOwner=FALSE; open(w,h,px); }
	mScreen8bit(tPicture* p)		{ mOwner=FALSE; open(p); }
	virtual void close(void);
	virtual ~mScreen8bit(void);
  	virtual int  GetType(void)    const;
	BOOL open(int w,int h,BOOL palexist=TRUE);
	BOOL open(tPicture*);
	RGBStruct* GetPalette(void) 		{ return IsValid()?mPicture->GetPalette():NULL; }
/// Палитра //////////////////////////////////////////////
        virtual void _PutPalette(int index,int count,const RGBStruct* palette);
        virtual void _PutPalette(int index,int count,const RGBQUAD* palette);
        virtual void _GetPalette(int index,int count,RGBStruct* palette);
        virtual void _GetPalette(int index,int count,RGBQUAD* palette);
};

class mScreen16bit:public fScreen16bit, public mScreenSub
{
public:
	void refresh(void)			{ if(IsValid()) srefresh(); }
	mScreen16bit(void):fScreen16bit()	{ mOwner=FALSE; close(); }
	mScreen16bit(int w,int h)		{ mOwner=FALSE; open(w,h); }
	mScreen16bit(tPicture* p)		{ mOwner=FALSE; open(p); }
	virtual void close(void);
	virtual ~mScreen16bit(void);
  	virtual int  GetType(void)    const;
	BOOL open(int w,int h);
	BOOL open(tPicture*);
// other
   	virtual long rgb(uchar red,uchar green,uchar blue);
  	virtual void rgb(long color,uchar* red,uchar* green,uchar* blue);
};

class mScreen32bit:public fScreen32bit, public mScreenSub
{
public:
	void refresh(void)			{ if(IsValid()) srefresh(); }
	mScreen32bit(void):fScreen32bit()	{ mOwner=FALSE; close(); }
	mScreen32bit(int w,int h)		{ mOwner=FALSE; open(w,h); }
	mScreen32bit(tPicture* p)		{ mOwner=FALSE; open(p); }
	virtual void close(void);
	virtual ~mScreen32bit(void);
  	virtual int  GetType(void)    const;
	BOOL open(int w,int h);
	BOOL open(tPicture*);
};


Screen* GimmeMirror(Screen*);

#endif
