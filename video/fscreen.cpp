#include <fscreen.h>

/////////////////////////////////////////////////////////////////////////////
// TEMPLATE
/////////////////////////////////////////////////////////////////////////////

template <class T>FlatScreen<T>::~FlatScreen(void)
{
  close(); 
}

template <class T> BOOL FlatScreen<T>::open(int w,int h,void* ptr)
{
	mData=(T*)ptr;
	Screen::open(w,h,sizeof(T)*8);
	return TRUE;
}

template <class T> void FlatScreen<T>::close(void)
{
	mData=NULL;
	Screen::close();
}

template <class T> BOOL FlatScreen<T>::IsValid(void) const
{
	return Screen::IsValid() && mData!=NULL;
}

template <class T> int FlatScreen<T>::GetType(void) const
{
	return ID_FSCREEN;
}

template <class T> long FlatScreen<T>::_GetPixel(int x,int y)
{
	return mData[x+y*mWidth];
}

template <class T> void FlatScreen<T>::_PutPixel(int x,int y,long color)
{
	mData[x+y*mWidth]=color;
}

template <class T> void FlatScreen<T>::_vline(int x,int y,int h,long color)
{
	for(T* d=mData+x+y*mWidth;h--;d+=mWidth)
		   *d=color;
}

template <class T> void FlatScreen<T>::_line(int xs,int ys,int xe,int ye,long color)
{
// rem: xs<=xe
   	T* ptr;
	*(ptr=mData+xs+ys*mWidth)=color;
   	int dx=xe-xs;
	int dy=abs(ye-ys);
	if(dy<=dx)
	{
		int d,d1,d2=(dy-dx)<<1;
		d=(d1=dy<<1)-dx;
		if(ye>ys)
			for(;dx--;*++ptr=color)
			{
				if(d>0)
				{
					d+=d2;
					ptr+=mWidth;
				}
				else
					d+=d1;
			}
		else
			for(;dx--;*++ptr=color)
			{
				if(d>0)
				{
					d+=d2;
					ptr-=mWidth;
				}
				else
					d+=d1;
			}
	}
	else
	{
		int d,d1,d2=(dx-dy)<<1;
		d=(d1=dx<<1)-dy;
		if(ye>ys)
			for(;dy--;*(ptr+=mWidth)=color)
			{
				if(d>0)
				{
					d+=d2;
					ptr++;
				}
				else
					d+=d1;
			}
		else
			for(;dy--;*(ptr-=mWidth)=color)
			{
				if(d>0)
				{
					d+=d2;
					ptr++;
				}
				else
					d+=d1;
			}
	}
}

/// Работа с текстом /////////////////////////////////////
template <class T> void FlatScreen<T>::_outchar(int x,int y,int ch,long color,const tBinFont* font,const cRect* rect)
{
	int h=font->height,fwb=font->widthb;
	const uchar* ptr=font->chardata(ch);
	if(ptr==NULL)
		return;
	int w=*ptr++;
	if(y<rect->top)
	{
		if((h-=rect->top-y)<=0)
			return;
		ptr+=fwb*(rect->top-y);
		y=rect->top;
	}
	if(y+h>rect->bottom+1)     
		h=rect->bottom-y+1;
	if(x+w>rect->right+1)
		w=rect->right-x+1;
	int dx;
	uchar smask=128;
	if(x<rect->left)
	{
		dx=rect->left-x;
		x=rect->left;
		ptr+=dx/8;
		smask>>=dx&7;
	}
	else
		dx=0;
	T* ScrData=mData+y*mWidth;
	for(int i=0;i++<h;ptr+=fwb,ScrData+=mWidth)
	{
		const uchar* cptr=ptr;
		uchar mask=smask,data=*cptr++;
		int xl=x;
		for(int j=dx;j++<w;mask>>=1,xl++)
		{
			if(mask==0)
			{
				data=*cptr++;
				mask=128;
			}
			if(mask&data)
				ScrData[xl]=color;
		}
	}
}

template <class T> void FlatScreen<T>::_outcharf(int x,int y,int ch,const tImage* image,const tBinFont* font,const cRect* rect)
{
	int h=font->height,fwb=font->widthb;
	const uchar* ptr=font->chardata(ch);
	if(ptr==NULL)
		return;
	int w=*ptr++;
	if(y<rect->top)
	{
		if((h-=rect->top-y)<=0)
			return;
		ptr+=fwb*(rect->top-y);
		y=rect->top;
	}
	if(y+h>rect->bottom+1)     
		h=rect->bottom-y+1;
	if(x+w>rect->right+1)
		w=rect->right-x+1;
	int dx;
	uchar smask=128;
	if(x<rect->left)
	{
		dx=rect->left-x;
		x=rect->left;
		ptr+=dx/8;
		smask>>=dx&7;
	}
	else
		dx=0;
	T* ScrData=mData+y*mWidth;
	const T* pData=((const T*)image->data)+image->width*y;
	for(int i=0;i++<h;ptr+=fwb,ScrData+=mWidth,pData+=image->width)
	{
		const uchar* cptr=ptr;
		uchar mask=smask,data=*cptr++;
		int xl=x;
		for(int j=dx;j++<w;mask>>=1,xl++)
		{
			if(mask==0)
			{
				data=*cptr++;
				mask=128;
			}
			if(mask&data)
				ScrData[xl]=pData[xl];
		}
	}	
}

/// Графические примитивы ////////////////////////////////
template <class T> void FlatScreen<T>::_PutImage(int x,int y,const tImage* image,const cRect* rect)
{
	long w=image->width,
	     h=image->height,
	     rw=w;
	const T* src=(const T*)image->data;
	// vertical truncate
	if(rect->bottom<y+h)
		if((h=rect->bottom-y+1)<=0)
			return;
	if(rect->top>y)
	{
		int d=rect->top-y;
		if((h-=d)<=0)
			return;
		src+=w*d;		
		y=rect->top;
	}
        T* dest=mData+x+y*mWidth;
	if(rect->right<x+w)
		if((rw=rect->right-x+1)<=0)
			return;
	if(x<rect->left)
	{
		int d=rect->left-x;
		src+=d;
		dest+=d;
		if((rw-=d)<0)
			return;
	}
	for(rw*=sizeof(T);h--;src+=w)
	{
		memcpy(dest,src,rw);
		dest+=mWidth;
	}
}

template <class T> void FlatScreen<T>::_GetImage(int x,int y,int w,int h,tImage* image,const cRect* rect)
{
	long rw=w;
	image->width=w;
	image->height=h;
	T* dest=(T*)image->data;
	// vertical truncate
	if(rect->bottom<y+h)
		if((h=rect->bottom-y+1)<=0)
			return;
	if(rect->top>y)
	{
		int d=rect->top-y;
		if((h-=d)<=0)
			return;
		dest+=w*d;		
		y=rect->top;
	}
        const T* src=mData+x+y*mWidth;
	if(rect->right<x+w)
		if((rw=rect->right-x+1)<=0)
			return;
	if(x<rect->left)
	{
		int d=rect->left-x;
		src+=d;
		dest+=d;
		if((rw-=d)<0)
			return;
	}
	for(rw*=sizeof(T);h--;src+=mWidth)
	{
		memcpy(dest,src,rw);
		dest+=w;
	}
}


tAllocFnPtr fScreen8bit::m_alloc_fn=malloc;
tFreeFnPtr  fScreen8bit::m_free_fn=free;

tAllocFnPtr fScreen16bit::m_alloc_fn=malloc;
tFreeFnPtr  fScreen16bit::m_free_fn=free;

tAllocFnPtr fScreen32bit::m_alloc_fn=malloc;
tFreeFnPtr  fScreen32bit::m_free_fn=free;


/////////////////////////////////////////////////////////////////////////////
// Flat 8bit
/////////////////////////////////////////////////////////////////////////////
void fScreen8bit::_hline(int x,int y,int w,long color)
{
	memset(mData+x+y*mWidth,color,w);
}

void fScreen8bit::_bar(int xs,int ys,int xe,int ye,long color)
{
	int l;
	uchar* ptr=mData+xs+ys*mWidth;
	if((l=xe-xs+1)==mWidth)
		memset(ptr,color,mWidth*(ye-ys+1));
	else	
		for(;ys++<=ye;ptr+=mWidth)
			memset(ptr,color,l);
}

void fScreen8bit::_PutSprite(int x,int y,const tSprite* image,const cRect* rect)
{
	if((x-=image->x)>rect->right)
		return;		
	y-=image->y;
	const uchar* data=image->data;
// 	Структура строки:          
//	int16 len;
//		  (тип цепи:2bit,длина цепи):слово
//		       прозрачность, 
//		       копирование
//		       z-цвет,
	int h=image->height;
        if(y<rect->top)
	{
	  if((h-=rect->top-y)<=0)
            return;
	  for(;y<rect->top;y++)
	    data+=*(const uint16*)data;
	}
	if(y+h>rect->bottom)
		if((h=rect->bottom-y+1)<=0)
			return;
	int w;
	if(x+(w=(uint)image->width)<rect->left)
		return;
	if(x+w>rect->right)		  
		if((w=rect->right-x+1)<=0)
			return;
	uchar* ScrData=mData+y*mWidth;
       	for(;h>0;h--,ScrData+=mWidth)
       	{
       		const uchar* ldata=data+2;
       		uint16 len;
       		int j=0;
       		for(int xl=x;xl<rect->left;)
       		{
       			len=*(uint16*)ldata;
       			ldata+=2;
       			if(len<0x4000) // empty space
       			{
       				j+=len;
       				xl+=len;
       				continue;
       			}				
       			if(len<0x8000) // copy line mode
       			{	
       				j+=(len&=0x3FFF);
				int dvx=rect->left-xl;
       				if(len>=dvx)
       				{       
					xl+=len;
					memcpy(ScrData+rect->left,ldata+=dvx,len-=dvx);
					ldata+=len;
					// len=0;
       					break;				
       				}
       				ldata+=len;
       				xl+=len;
       				continue;
       			}
       			// len&0xC000==0x8000 z-line
       			uchar zpixel=*ldata++;
       			j+=(len&=0x3FFF);
			int dvx=rect->left-xl;
       			if(len>=dvx)
       			{       
       				len-=dvx;
       				ldata+=dvx;
       				for(xl=rect->left;len--;xl++,ldata++)
       					if(zpixel!=*ldata)
       						ScrData[xl]=*ldata;	
       				break;				
       			}
       			ldata+=len;
       			xl+=len;
       		}
       		for(;j<w;)
       		{
       			len=*(const uint16*)ldata;
       			ldata+=2;
       			if(len<0x4000) // empty space
       			{
       				j+=len;
       				xl+=len;
       				continue;
       			}
       			if(len<0x8000) // copy line
       			{					
       				if((len&=0x3FFF)+j>w)
       					len=w-j;
				j+=len;
				memcpy(ScrData+xl,ldata,len);
				xl+=len;
				ldata+=len;
       				continue;
       			}
       			// len&0xC000==0x8000 z-line
       			uchar zpixel=*ldata++;
       			if((len&=0x3FFF)+j>w)
       				len=w-j;
       			for(j+=len;len--;xl++,ldata++)
       				if(zpixel!=*ldata)
					ScrData[xl]=*ldata;
       		}
       		len=*(const uint16*)data;
       		data+=len;
       	}
}

// other
long fScreen8bit::rgb(uchar red,uchar green,uchar blue)
{
#define COLORLEN(c,r,g,b)	(sqr(long(c.red)-long(r))+sqr(long(c.green)-long(g))+sqr(long(c.blue)-long(b)))
	RGBStruct rgb;
	int imin=0;
	GetPalette(0,&rgb);
	long lmin=COLORLEN(rgb,red,green,blue),l;
	for(int i=1;i<256;i++)
	{
		GetPalette(i,&rgb);
		if((l=COLORLEN(rgb,red,green,blue))<lmin)
		{
			lmin=l;
			imin=i;
		}
	}
#undef COLORLEN
	return imin;
}

void fScreen8bit::rgb(long color,uchar* red,uchar* green,uchar* blue)
{
	RGBStruct rgb;
	GetPalette(color,&rgb);
	*red=rgb.red;
	*green=rgb.green;
	*blue=rgb.blue;
}

/////////////////////////////////////////////////////////////////////////////
// Flat 16bit
/////////////////////////////////////////////////////////////////////////////
void fScreen16bit::_hline(int x,int y,int w,long color)
{
	memsetw(mData+x+y*mWidth,color,w);
}

void fScreen16bit::_bar(int xs,int ys,int xe,int ye,long color)
{
	int l;
	uint16* ptr=mData+xs+ys*mWidth;
	if((l=xe-xs+1)==mWidth)
		memsetw(ptr,color,mWidth*(ye-ys+1));
	else	
		for(;ys++<=ye;ptr+=mWidth)
			memsetw(ptr,color,l);
}

void fScreen16bit::_PutSprite(int x,int y,const tSprite* image,const cRect* rect)
{
	if((x-=image->x)>rect->right)
		return;		
	y-=image->y;
	const uchar* data=image->data;
// 	Структура строки:          
//	int16 len;
//		  (тип цепи:2bit,длина цепи):слово
//		       прозрачность, 
//		       копирование
//		       z-цвет,
	int h=image->height;
        if(y<rect->top)
	{
	  if((h-=rect->top-y)<=0)
            return;
	  for(;y<rect->top;y++)
	    data+=*(const uint16*)data;
	}
	if(y+h>rect->bottom)
		if((h=rect->bottom-y+1)<=0)
			return;
	int w;
	if(x+(w=(uint)image->width)<rect->left)
		return;
	if(x+w>rect->right)		  
		if((w=rect->right-x+1)<=0)
			return;
	uint16* ScrData=mData+mWidth*y;
      	for(;h>0;h--,ScrData+=mWidth)
      	{
      		const uint16* ldata=(const uint16*)(data+2);
      		uint16 len;
      		int j=0;
      		for(int xl=x;xl<rect->left;)
      		{
      			len=*ldata;
      			ldata++;
      			if(len<0x4000) // empty space
      			{
      				j+=len;
      				xl+=len;
      				continue;
      			}				
      			if(len<0x8000) // copy line mode
      			{	
      				j+=(len&=0x3FFF);
				int dxl=rect->left-xl;
				xl+=len;
      				if(len>=dxl)
      				{       
      					len-=dxl;
      					ldata+=dxl;
					memcpy(ScrData+rect->left,ldata,len*2);
					ldata+=len;
      					break;				
      				}
      				ldata+=len;
      				continue;
      			}
      			// len&0xC000==0x8000 z-line
      			uint16 zpixel=*ldata++;
      			j+=(len&=0x3FFF);
      			if(len+xl>=rect->left)
      			{       
      				len-=rect->left-xl;
      				ldata+=rect->left-xl;
      				for(xl=rect->left;len--;xl++,ldata++)
      					if(zpixel!=*ldata)
      						ScrData[xl]=*ldata;	
      				break;				
      			}
      			ldata+=len;
      			xl+=len;
      		}
      		for(;j<w;)
      		{
      			len=*ldata;
      			ldata++;
      			if(len<0x4000) // empty space
      			{
      				j+=len;
      				xl+=len;
      				continue;
      			}
      			if(len<0x8000) // copy line
      			{					
      				if((len&=0x3FFF)+j>w)
      					len=w-j;
				memcpy(ScrData+xl,ldata,2*len);
				j+=len;
      				xl+=len;
				ldata+=len;
      				continue;
      			}
      			// len&0xC000==0x8000 z-line
      			uint16 zpixel=*ldata++;
      			if((len&=0x3FFF)+j>w)
      				len=w-j;
      			for(j+=len;len--;xl++,ldata++)
      				if(zpixel!=*ldata)
					ScrData[xl]=*ldata;
      		}
      		len=*(uint16*)data;
      		data+=len;
      		y++;
      	}
}

/////////////////////////////////////////////////////////////////////////////
// Flat 32bit
/////////////////////////////////////////////////////////////////////////////
void fScreen32bit::_hline(int x,int y,int w,long color)
{
	memsetd(mData+x+y*mWidth,color,w);
}

void fScreen32bit::_bar(int xs,int ys,int xe,int ye,long color)
{
	int l;
	uint32* ptr=mData+xs+ys*mWidth;
	if((l=xe-xs+1)==mWidth)
		memsetd(ptr,color,mWidth*(ye-ys+1));
	else	
		for(;ys++<=ye;ptr+=mWidth)
			memsetd(ptr,color,l);
}

void fScreen32bit::_PutSprite(int x,int y,const tSprite* image,const cRect* rect)
{
	if((x-=image->x)>rect->right)
		return;		
	y-=image->y;
	const uchar* data=image->data;
// 	Структура строки:          
//	int16 len;
//		  (тип цепи:2bit,длина цепи):слово
//		       прозрачность, 
//		       копирование
//		       z-цвет,
	int h=image->height;
        if(y<rect->top)
	{
	  if((h-=rect->top-y)<=0)
            return;
	  for(;y<rect->top;y++)
	    data+=*(const uint16*)data;
	}

	if(y+h>rect->bottom)
		if((h=rect->bottom-y+1)<=0)
			return;
	int w;
	if(x+(w=(uint)image->width)<rect->left)
		return;
	if(x+w>rect->right)		  
		if((w=rect->right-x+1)<=0)
			return;
       	for(ulong* ScrData=mData+y*mWidth;h-->0;ScrData+=mWidth)
       	{
       		const ulong* ldata=(const ulong*)(data+2);
       		uint16 len;
       		int j=0;
       		for(int xl=x;xl<rect->left;)
       		{
       			len=*(const uint16*)ldata;
       			ldata=(const ulong*)(((const uint16*)ldata)+1);
       			if(len<0x4000) // empty space
       			{
       				j+=len;
       				xl+=len;
       				continue;
       			}				
       			if(len<0x8000) // copy line mode
       			{	
       				j+=(len&=0x3FFF);
       				int dxl=rect->left-xl;
                                xl+=len;
       				if(len>=dxl)
       				{       
       					len-=dxl;
       					ldata+=dxl;
       					memcpy(ScrData+rect->left,ldata,len*4);
       					ldata+=len;
       					break;				
       				}
       				ldata+=len;
       				continue;
       			}
       			// len&0xC000==0x8000 z-line
       			long zpixel=*ldata++;
       			j+=(len&=0x3FFF);
       			int dxl=rect->left-xl;
       			if(len>=dxl)
       			{       
       				len-=dxl;
       				ldata+=dxl;
       				for(xl=rect->left;len--;xl++,ldata++)
       					if(zpixel!=*ldata)
       					        ScrData[xl]=*ldata;	
       				break;				
       			}
       			ldata+=len;
       			xl+=len;
       		}
       		for(;j<w;)
       		{
       			len=*ldata;
       			ldata++;
       			if(len<0x4000) // empty space
       			{
       				j+=len;
       				xl+=len;
       				continue;
       			}
       			if(len<0x8000) // copy line
       			{					
       				if((len&=0x3FFF)+j>w)
       					len=w-j;
       				memcpy(ScrData+xl,ldata,len*4);
				j+=len;
				ldata+=len;
				xl+=len;
       				continue;
       			}
       			// len&0xC000==0x8000 z-line
       			long zpixel=*ldata++;
       			if((len&=0x3FFF)+j>w)
       				len=w-j;
       			for(j+=len;len--;xl++,ldata++)
       				if(zpixel!=*ldata)
       					ScrData[xl]=*ldata;
       		}
       		len=*(uint16*)data;
       		data+=len;
       	}
}

long fScreen32bit::rgb(uchar red,uchar green,uchar blue)
{
	return ulong(blue)|(ulong(green)<<8)|(ulong(red)<<16);
}

void fScreen32bit::rgb(long color,uchar* red,uchar* green,uchar* blue)
{
	*red=color>>16;
	*green=color>>8;
	*blue=color;
}
