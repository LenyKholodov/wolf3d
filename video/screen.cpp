#include <screen.h>

tAllocFnPtr Screen::m_alloc_fn=malloc;
tFreeFnPtr  Screen::m_free_fn=free;

Screen::~Screen(void)
{
  close();
}

void Screen::CorrectCW(void)
{
  if(mClipRect.left<0)
    mClipRect.left=0;
  if(mClipRect.right>=mWidth)
    mClipRect.right=mWidth-1;
  if(mClipRect.top<0)
    mClipRect.top=0;
  if(mClipRect.bottom>=mHeight)
    mClipRect.bottom=mHeight-1;
}

BOOL Screen::open(int w,int h,int bpp)
{
  mWidth=w;
  mHeight=h;
  mBpp=bpp;
  mClipRect.setup(0,0,w-1,h-1);
  return TRUE;
}

void Screen::close(void)
{
  mBpp=mWidth=mHeight=0;
  CorrectCW();
}

int Screen::GetType(void) const
{
  return ID_SCREEN;
}

BOOL Screen::IsValid(void) const
{
  return !mClipRect.empty();
}

void Screen::_PutPixel(int,int,long)
{
}

long Screen::_GetPixel(int,int)
{
  return 0;
}

void Screen::_vline(int x,int y,int h,long color)
{
  for (int ye=y+h;y<ye;_PutPixel(x,y++,color));
}

void Screen::_hline(int x,int y,int w,long color)
{
 for(int xe=x+w;x<xe;_PutPixel(x++,y,color));
}

void Screen::_line(int xs,int ys,int xe,int ye,long color)
{
// rem: xs<=xe
  _PutPixel(xs,ys,color);
  int dx=xe-xs;
  int dy=abs(ye-ys);
  if(dy<=dx)
  {
    int d,d1,d2=(dy-dx)<<1;
    d=(d1=dy<<1)-dx;
    if(ye>ys)
      for(;dx--;_PutPixel(++xs,ys,color))
      {
        if(d>0)
        {
          d+=d2;
          ys++;
        }
        else
          d+=d1;
      }
    else
      for(;dx--;_PutPixel(++xs,ys,color))
      {
              if(d>0)
        {
          d+=d2;
          ys--;
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
      for(;dy--;_PutPixel(xs,++ys,color))
      {
        if(d>0)
        {
          d+=d2;
          xs++;
        }
        else
          d+=d1;
      }
    else
      for(;dy--;_PutPixel(xs,--ys,color))
      {
        if(d>0)
        {
          d+=d2;
          xs++;
        }
        else
          d+=d1;
      }
  }
}

void Screen::_bar(int left,int top,int right,int bottom,long color)
{
  for(int w=right-left+1;top<=bottom;_hline(left,top++,w,color));
}

void Screen::vline(int x,int y,int h,long color)
{
        if(!mClipRect.inX(x))
                    return;
        if(h<0)
                    y-=h=-h;
        if(y+h>mClipRect.bottom+1)
                    h=mClipRect.bottom+1-y;
        if(y<mClipRect.top)
        {
                    h+=y-mClipRect.top;
                y=mClipRect.top;
        }
        if(h>0)
                _vline(x,y,h,color);
}

void Screen::hline(int x,int y,int w,long color)
{
          if(!mClipRect.inY(y))
                return;
        if(w<0)
                x-=w=-w;
        if(x+w>mClipRect.right+1)
                w=mClipRect.right+1-x;
        if(x<mClipRect.left)
        {
                w+=x-mClipRect.left;
                x=mClipRect.left;
        }
        if(w>0)
                _hline(x,y,w,color);
}

void Screen::line(int xs,int ys,int xe,int ye,long color)
{
        if(xs>xe)
        {
                swap(xs,xe);
                swap(ys,ye);
        }
        if(xs>mClipRect.right || xe<mClipRect.left)
                return;
        if(xs==xe)
        {
                vline(xs,ys,ye-ys+1,color);
                return;
        }
        if(xs<mClipRect.left)
        {
                ys+=(long)(ye-ys)*(mClipRect.left-xs)/(xe-xs);
                if((xs=mClipRect.left)==xe)
                {
                        vline(xs,ys,ye-ys+1,color);
                        return;
                }
        }
        if(xe>mClipRect.right)
        {
                ye+=(long)(ye-ys)*(mClipRect.right-xe)/(xe-xs);
                if((xe=mClipRect.right)==xs)
                {
                        vline(xs,ys,ye-ys+1,color);
                        return;
                }

        }
        if(ys==ye)
        {
                hline(xs,ys,xe-xs+1,color);
                return;
        }
        if(ys<mClipRect.top)
        {
                if(ye<mClipRect.top)
                        return;
                xs+=(long)(xe-xs)*(mClipRect.top-ys)/(ye-ys);
                if(xs>xe)
                        return;
                if((ys=mClipRect.top)==ye)
                {
                        _hline(xs,ys,xe-xs+1,color);
                        return;
                }
        }
        else
                if(ys>mClipRect.bottom)
                {
                        if(ye>mClipRect.bottom)
                                return;
                        xs+=(long)(xe-xs)*(mClipRect.bottom-ys)/(ye-ys);
                        if(xs>xe)
                                return;
                        if((ys=mClipRect.bottom)==ye)
                        {
                                _hline(xs,ys,xe-xs+1,color);
                                return;
                        }
                }
        if(ye<mClipRect.top)
        {
                xe+=(long)(xe-xs)*(mClipRect.top-ye)/(ye-ys);
                if((ye=mClipRect.top)==ys)
                {
                        _hline(xs,ys,xe-xs+1,color);
                        return;
                }
        }
        else
                if(ye>mClipRect.bottom)
                {
                        xe+=(long)(xe-xs)*(mClipRect.bottom-ye)/(ye-ys);
                        if((ye=mClipRect.bottom)==ys)
                        {
                                _hline(xs,ys,xe-xs+1,color);
                                return;
                        }
                }
        _line(xs,ys,xe,ye,color);
}

void Screen::bar(int xs,int ys,int xe,int ye,long color)
{
        if(xs>xe)
                swap(xs,xe);
        if(ys>ye)
                swap(ys,ye);
        if(xs<mClipRect.left)
                xs=mClipRect.left;
        if(xe>mClipRect.right)
                xe=mClipRect.right;
        if(ys<mClipRect.top)
                ys=mClipRect.top;
        if(ye>mClipRect.bottom)
                ye=mClipRect.bottom;
        if(xs>xe || ys>ye)
                 return;
        _bar(xs,ys,xe,ye,color);
}

void Screen::rectangle(int xs,int ys,int xe,int ye,long color)
{
        if(xs>xe)
                swap(xs,xe);
        if(ys>ye)
                swap(ys,ye);
        hline(xs,ys,xe-xs+1,color);
        hline(xs,ye,xe-xs+1,color);
        vline(xs,ys+1,ye-ys-1,color);
        vline(xe,ys+1,ye-ys-1,color);
}


/// Работа с текстом /////////////////////////////////////

void Screen::_outchar(int x,int y,int ch,long color,
                                       const tBinFont* font,const cRect* rect)
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
        for(int i=0;i++<h;ptr+=fwb,y++)
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
                                _PutPixel(xl,y,color);
                }
        }
}

void Screen::_outcharf(int x,int y,int ch,const tImage* image,
                                const tBinFont* font,const cRect* rect)
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
        int bpp=mBpp>>3;// div 8
        int   dby=image->width*bpp;
        const uchar* idata=image->data+dby*y;
        for(int i=0;i++<h;ptr+=fwb,y++,idata+=dby)
        {
                const uchar* cptr=ptr;
                uchar mask=smask,data=*cptr++;
                int xl=x,xb=x*bpp;
                for(int j=dx;j++<w;mask>>=1,xl++,xb+=bpp)
                {
                        if(mask==0)
                        {
                                data=*cptr++;
                                mask=128;
                        }
                        if(mask&data)
                                _PutPixel(xl,y,*(long*)(idata+xb));
                }
        }
}

void Screen::outtext(int x,int y,const char* s,long color,const tBinFont* font)
{
  if(!IsValid() || y>mClipRect.bottom || y+font->height<=mClipRect.top)
    return;
  for(uchar c;(c=*s++)!=0 && x<=mClipRect.right;x+=font->width(c))
    _outchar(x,y,c,color,font,&mClipRect);
}

void Screen::outtextf(int x,int y,const char* s,const tImage* image,
                                                             const tBinFont* font)
{
  if(!IsValid() || y>mClipRect.bottom || y+font->height<=mClipRect.top)
    return;
  for(uchar c;(c=*s++)!=0 && x<=mClipRect.right;x+=font->width(c))
    _outcharf(x,y,c,image,font,&mClipRect);
}

void Screen::outtext(int x,int y,const char* s,tSpriteFont* font)
{
        if(!IsValid() || y>mClipRect.bottom)
                return;
        for(;*s;s++)
        {
                if(x>mClipRect.right)
                        break;
                tSprite* spr=(*font)[*s];
                if(spr!=NULL)
                {
                        _PutSprite(x,y,spr,&mClipRect);
                        x+=spr->width;
                }
        }
}

/// Графические примитивы ////////////////////////////////
void Screen::_PutImage(int x,int y,const tImage* image,const cRect* rect)
{
        int bpp=mBpp>>3, // Bytes per pixel
            wb=image->width*bpp;
        const uchar* data=image->data;
        int h=image->height,w=image->width;
        if(y<rect->top)
        {
                if((h-=rect->top-y)<=0)
                        return;
                data+=wb*(rect->top-y);
                y=rect->top;
        }
        if(y+h>rect->bottom+1)
                h=rect->bottom-y+1;
        if(x+w>rect->right+1)
                if((w=rect->right-x+1)<=0)
                        return;
        int i,j,stx;
        if(x<rect->left)
                data+=(stx=rect->left-x)*bpp;
        else
                stx=0;
        for(i=0;i++<h;y++,data+=wb)
        {
                const uchar* ldata=data;
                for(int xl=x+(j=stx);j<w;j++,ldata+=bpp)
                        _PutPixel(xl++,y,*((long*)ldata));
        }
}

void Screen::_GetImage(int x,int y,int w,int h,tImage* image,const cRect* rect)
{
        image->width=w;
        image->height=h;
        if(x>rect->right || y>rect->bottom)
                return;
        switch(mBpp)
        {
                case 8:
                {
                        uchar* data=image->data;
                        if(y<rect->top)
                        {
                                if((h-=rect->top-y)<=0)
                                        return;
                                data+=(rect->top-y)*w;
                                y=rect->top;
                        }
                        if(y+h>rect->bottom)
                                h=rect->bottom-y+1;
                        if(x+w>rect->right)
                                w=rect->right-x+1;
                        int stx=x<rect->left?rect->left-x:0;
                        for(int i=0;i<h;i++,y++,data+=w)
                                for(int j=stx;j<w;j++)
                                        data[j]=_GetPixel(x+j,y);
                }
                break;

                case 15:
                case 16:

                {
                        uint16* data=(uint16*)image->data;
                        if(y<rect->top)
                        {
                                if((h-=rect->top-y)<=0)
                                        return;
                                data+=(rect->top-y)*w;
                                y=rect->top;
                        }
                        if(y+h>rect->bottom)
                                h=rect->bottom-y+1;
                        if(x+w>rect->right)
                                w=rect->right-x+1;
                        int stx=x<rect->left?rect->left-x:0;
                        for(int i=0;i<h;i++,y++,data+=w)
                                for(int j=stx;j<w;j++)
                                        data[j]=_GetPixel(x+j,y);
                }
                break;

                case 24:
                {
                        RGBStruct* data=(RGBStruct*)image->data;
                        if(y<rect->top)
                        {
                                if((h-=rect->top-y)<=0)
                                        return;
                                data+=(rect->top-y)*w;
                                y=rect->top;
                        }
                        if(y+h>rect->bottom)
                                h=rect->bottom-y+1;
                        if(x+w>rect->right)
                                w=rect->right-x+1;
                        int stx=x<rect->left?rect->left-x:0;
                        for(int i=0;i<h;i++,y++,data+=w)
                                for(int j=stx;j<w;j++)
                                {
                                        long l=_GetPixel(x+j,y);
                                        data[j].red=lobyte(l);
                                        data[j].green=hibyte(l);
                                        data[j].blue=hiword(l);
                                }
                }
                break;

                case 32:
                {
                        ulong* data=(ulong*)image->data;
                        if(y<rect->top)
                        {
                                if((h-=rect->top-y)<=0)
                                        return;
                                data+=(rect->top-y)*w;
                                y=rect->top;
                        }
                        if(y+h>rect->bottom)
                                h=rect->bottom-y+1;
                        if(x+w>rect->right)
                                w=rect->right-x+1;
                        int stx=x<rect->left?rect->left-x:0;
                        for(int i=0;i<h;i++,y++,data+=w)
                                for(int j=stx;j<w;j++)
                                        data[j]=_GetPixel(x+j,y);
                }
                break;
        }
}

void Screen::_PutSprite(int x,int y,const tSprite* image,const cRect* rect)
{
        if((x-=image->x)>rect->right)
                return;
        y-=image->y;
        const uchar* data=image->data;
//         Структура строки:
//        int16 len;
//                  (тип цепи:2bit,длина цепи):слово
//                       прозрачность,
//                       копирование
//                       z-цвет,
        for(int h=image->height;y<rect->top;y++)
        {
                uint16 len=*(uint16*)data;
                data+=len;
                if(--h<=0)
                        return;
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
        switch(mBpp)
        {
                case 8:
                        for(;h>0;h--)
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
                                                if(len+xl>=rect->left)
                                                {
                                                        len-=rect->left-xl;
                                                        ldata+=rect->left-xl;
                                                        for(xl=rect->left;len--;xl++,ldata++)
                                                                _PutPixel(xl,y,*ldata);
                                                        break;
                                                }
                                                ldata+=len;
                                                xl+=len;
                                                continue;
                                        }
                                        // len&0xC000==0x8000 z-line
                                        uchar zpixel=*ldata++;
                                        j+=(len&=0x3FFF);
                                        if(len+xl>=rect->left)
                                        {
                                                len-=rect->left-xl;
                                                ldata+=rect->left-xl;
                                                for(xl=rect->left;len--;xl++,ldata++)
                                                        if(zpixel!=*ldata)
                                                                _PutPixel(xl,y,*ldata);
                                                break;
                                        }
                                        ldata+=len;
                                        xl+=len;
                                }
                                for(;j<w;)
                                {
                                        len=*(uint16*)ldata;
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
                                                for(j+=len;len--;xl++,ldata++)
                                                        _PutPixel(xl,y,*ldata);
                                                continue;
                                        }
                                        // len&0xC000==0x8000 z-line
                                        uchar zpixel=*ldata++;
                                        if((len&=0x3FFF)+j>w)
                                                len=w-j;
                                        for(j+=len;len--;xl++,ldata++)
                                                if(zpixel!=*ldata)
                                                        _PutPixel(xl,y,*ldata);
                                }
                                len=*(uint16*)data;
                                data+=len;
                                y++;
                        }
                        break;
                case 15:
                case 16:
                        for(;h>0;h--)
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
                                                if(len+xl>=rect->left)
                                                {
                                                        len-=rect->left-xl;
                                                        ldata+=rect->left-xl;
                                                        for(xl=rect->left;len--;xl++,ldata++)
                                                                _PutPixel(xl,y,*ldata);
                                                        break;
                                                }
                                                ldata+=len;
                                                xl+=len;
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
                                                                _PutPixel(xl,y,*ldata);
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
                                                for(j+=len;len--;xl++,ldata++)
                                                        _PutPixel(xl,y,*ldata);
                                                continue;
                                        }
                                        // len&0xC000==0x8000 z-line
                                        uint16 zpixel=*ldata++;
                                        if((len&=0x3FFF)+j>w)
                                                len=w-j;
                                        for(j+=len;len--;xl++,ldata++)
                                                if(zpixel!=*ldata)
                                                        _PutPixel(xl,y,*ldata);
                                }
                                len=*(uint16*)data;
                                data+=len;
                                y++;
                        }
                        break;
                case 24:
                        for(;h>0;h--)
                        {
                                const RGBStruct* ldata=(const RGBStruct*)(data+2);
                                uint16 len;
                                int j=0;
                                for(int xl=x;xl<rect->left;)
                                {
                                        len=*(const uint16*)ldata;
                                        ldata=(const RGBStruct*)(((const uint16*)ldata)+1);
                                        if(len<0x4000) // empty space
                                        {
                                                j+=len;
                                                xl+=len;
                                                continue;
                                        }
                                        if(len<0x8000) // copy line mode
                                        {
                                                j+=(len&=0x3FFF);
                                                if(len+xl>=rect->left)
                                                {
                                                        len-=rect->left-xl;
                                                        ldata+=rect->left-xl;
                                                        for(xl=rect->left;len--;xl++,ldata++)
                                                                _PutPixel(xl,y,*(const long*)ldata);
                                                        break;
                                                }
                                                ldata+=len;
                                                xl+=len;
                                                continue;
                                        }
                                        // len&0xC000==0x8000 z-line
                                        long zpixel=(*(const long*)ldata++);
                                        j+=(len&=0x3FFF);
                                        if(len+xl>=rect->left)
                                        {
                                                len-=rect->left-xl;
                                                ldata+=rect->left-xl;
                                                for(xl=rect->left;len--;xl++,ldata++)
                                                        if(((*(const long*)ldata)^zpixel)&0x00FFFFFF)
                                                                _PutPixel(xl,y,*(const long*)ldata);
                                                break;
                                        }
                                        ldata+=len;
                                        xl+=len;
                                }
                                for(;j<w;)
                                {
                                        len=*(const uint16*)ldata;
                                        ldata=(const RGBStruct*)(((const uint16*)ldata)+1);
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
                                                for(j+=len;len--;xl++,ldata++)
                                                        _PutPixel(xl,y,*(const long*)ldata);
                                                continue;
                                        }
                                        // len&0xC000==0x8000 z-line
                                        long zpixel=(*(const long*)ldata++);
                                        if((len&=0x3FFF)+j>w)
                                                len=w-j;
                                        for(j+=len;len--;xl++,ldata++)
                                                if((zpixel^(*(const long*)ldata))&0x00FFFFFF)
                                                        _PutPixel(xl,y,*(const long*)ldata);
                                }
                                len=*(uint16*)data;
                                data+=len;
                                y++;
                        }
                        break;
                case 32:
                        for(;h>0;h--)
                        {
                                const long* ldata=(const long*)(data+2);
                                uint16 len;
                                int j=0;
                                for(int xl=x;xl<rect->left;)
                                {
                                        len=*(const uint16*)ldata;
                                        ldata=(const long*)(((const uint16*)ldata)+1);
                                        if(len<0x4000) // empty space
                                        {
                                                j+=len;
                                                xl+=len;
                                                continue;
                                        }
                                        if(len<0x8000) // copy line mode
                                        {
                                                j+=(len&=0x3FFF);
                                                if(len+xl>=rect->left)
                                                {
                                                        len-=rect->left-xl;
                                                        ldata+=rect->left-xl;
                                                        for(xl=rect->left;len--;xl++,ldata++)
                                                                _PutPixel(xl,y,*ldata);
                                                        break;
                                                }
                                                ldata+=len;
                                                xl+=len;
                                                continue;
                                        }
                                        // len&0xC000==0x8000 z-line
                                        long zpixel=*ldata++;
                                        j+=(len&=0x3FFF);
                                        if(len+xl>=rect->left)
                                        {
                                                len-=rect->left-xl;
                                                ldata+=rect->left-xl;
                                                for(xl=rect->left;len--;xl++,ldata++)
                                                        if(zpixel!=*ldata)
                                                                _PutPixel(xl,y,*ldata);
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
                                                for(j+=len;len--;xl++,ldata++)
                                                        _PutPixel(xl,y,*ldata);
                                                continue;
                                        }
                                        // len&0xC000==0x8000 z-line
                                        long zpixel=*ldata++;
                                        if((len&=0x3FFF)+j>w)
                                                len=w-j;
                                        for(j+=len;len--;xl++,ldata++)
                                                if(zpixel!=*ldata)
                                                        _PutPixel(xl,y,*ldata);
                                }
                                len=*(uint16*)data;
                                data+=len;
                                y++;
                        }
                        break;
        }
}


/// Палитра //////////////////////////////////////////////
void Screen::PutPalette(int index,int red,int green,int blue)
{
  RGBStruct x={red,green,blue};
  _PutPalette(index,1,&x);
}


void Screen::_PutPalette(int,int,const RGBStruct*)
{
}

void Screen::_PutPalette(int,int,const RGBQUAD*)
{
}

void Screen::_GetPalette(int,int,RGBStruct*)
{
}

void Screen::_GetPalette(int,int,RGBQUAD*)
{
}

long Screen::rgb(uchar,uchar,uchar)
{
        return 0;
}

void Screen::rgb(long,uchar*,uchar*,uchar*)
{
}

////////////////////////////////////////////////////////////////////////

void Screen::circle(int xc,int yc,int r,long color)
{
        int y=r,
            d=1-r,
            delta1=3,
            delta2=-2*r+5;
        for(int x=0;x<=y;x++)
        {
                 PutPixel(x+xc,y+yc,color);
                PutPixel(-x+xc,y+yc,color);
                 PutPixel(x+xc,-y+yc,color);
                 PutPixel(-x+xc,-y+yc,color);
                 PutPixel(y+xc,x+yc,color);
                 PutPixel(-y+xc,x+yc,color);
                 PutPixel(y+xc,-x+yc,color);
                 PutPixel(-y+xc,-x+yc,color);
                  if(d<0)
                  {
                           d+=delta1;
                           delta1+=2;
                           delta2+=2;
                  }
                  else
                  {
                           d+=delta2;
                           delta1+=2;
                           delta2+=4;
                           y--;
                  }
         }
}

void Screen::ellipse(int x,int y,int a,int b,long color)
{
  int lx=0,ly=b;
  long aa=long(a)*long(a), aa2=2*aa, bb=long(b)*long(b) ,bb2=bb*2;
  long d=bb-aa*long(b)+aa/4, dx=0, dy=aa2*long(b);
  PutPixel(x,y-ly,color);
  PutPixel(x,y+ly,color);
  PutPixel(x-a,y,color);
  PutPixel(x+a,y,color);
  while (dx<dy)
 {
  if (d>0)
  {
   --ly;
   dy-=aa2;
   d-=dy;
  }
  ++lx;
  dx+=bb2;
  d+=bb + dx;
  PutPixel(x+lx,y+ly,color);
  PutPixel(x-lx,y+ly,color);
  PutPixel(x+lx,y-ly,color);
  PutPixel(x-lx,y-ly,color);
 }
 d+=((3*(aa-bb)/2-(dx+dy))/2);
 while (ly > 0)
 {
  if (d < 0)
  {
   ++lx;
   dx+=bb2;
   d+=bb+dx;
  }
  --ly;
  dy-=aa2;
  d+=aa-dy;
  PutPixel(x+lx,y+ly,color);
  PutPixel(x-lx,y+ly,color);
  PutPixel(x+lx,y-ly,color);
  PutPixel(x-lx,y-ly,color);
 }
}

inline long fraction(int a,int b) { return (a<<16)/b; }
inline long frac0(int a,int b) { return b?fraction(a,b):0; }

void Screen::triangle(const tPoint* p,long color)
{
  int iMax=0,iMin=0,iMid=0,i;
  if(p[1].y<p[iMin].y)
    iMin=1;
  else
    if(p[1].y>p[iMax].y)
      iMax=1;
    if(p[2].y<p[iMin].y)
      iMin=2;
    else
      if(p[2].y>p[iMax].y)
        iMax=2;
  iMid=3-iMin-iMax;
  long dx1,dx2,x1,x2;
  x1=x2=p[iMin].x<<16;
  int ym=p[iMid].y;

  dx1=frac0(p[iMax].x-p[iMin].x,p[iMax].y-p[iMin].y);
  dx2=frac0(p[iMin].x-p[iMid].x,p[iMin].y-p[iMid].y);
  for(i=p[iMin].y;i<=ym;i++)
  {
     hline(x1>>16,i,((x2-x1)>>16)+1,color);
     x1+=dx1;
     x2+=dx2;
  }
  int dyxn;
  if((dyxn=p[iMax].y-p[iMid].y)!=0)
  {
    dx2=fraction(p[iMax].x-p[iMid].x,dyxn);
    for(ym=p[iMax].y;i<=ym;i++)
    {
      x1+=dx1;
      x2+=dx2;
      hline(x1>>16,i,((x2-x1)>>16)+1,color);
    }
  }
}

void Screen::fill(int x,int y,long fillColor,long color)
{
        if(!mClipRect.in(x,y))
                return;
        mFillColor=fillColor;
        mColor          =color;
        LineFill(x,y,1,x,x);
}

int Screen::LineFill(int x,int y,int dir,int PrevXL,int PrevXR)
{
        long c;
        for(int xl=x;--xl>=mClipRect.left;)
                 if((c=_GetPixel(xl,y))==mColor || c==mFillColor)
                         break;
        for(int xr=x;++xr<=mClipRect.right;)
                 if((c=_GetPixel(xr,y))==mColor || c==mFillColor)
                         break;
        --xr;
         ++xl;
        _hline(xl,y,xr-xl+1,mFillColor);
        if(mClipRect.inY(y+dir))
                  for(x=xl;x<=xr;x++)
                           if((c=_GetPixel(x,y+dir))!=mColor && c!=mFillColor)
                                         x=LineFill(x,y+dir,dir,xl,xr);
         if(mClipRect.inY(y-dir))
        {
                  for(x=xl;x<=PrevXL;x++)
                           if((c=_GetPixel(x,y-dir))!=mColor && c!=mFillColor)
                                      x=LineFill(x,y-dir,-dir,xl,xr);
                  for (x=PrevXR;x<=xr;x++)
                            if((c=_GetPixel(x,y-dir))!=mColor && c!=mFillColor)
                                     x=LineFill(x,y-dir,-dir,xl,xr);
        }
         return xl;
}

void Screen::polyline(int count,const tPoint* p,long color)
{
        for(int i=1;i<count;i++)
                line(p[i-1].x,p[i-1].y,p[i].x,p[i].y,color);
}

void Screen::polygone(int n,const tPoint* p,long color)
{
  int yMin,yMax,topPointIndex,i=1,pTop;
  for(pTop=yMin=yMax=p[topPointIndex=0].y;i<n;i++)
  {
    int piy=p[i].y;
    if(piy<pTop)
      pTop=p[topPointIndex=i].y;
    else
      if(piy>yMax)
        yMax=piy;
  }
  if((yMin=pTop)==yMax)
  {
    int xMin,xMax;
    xMin=xMax=p[0].x;
    for(i=1;i<n;i++)
    {
      int pix=p[i].x;
      if(pix<xMin)
        xMin=pix;
      else
        if(pix>xMax)
          xMax=pix;
    }
    hline(xMin,yMin,xMax-xMin+1,color);
    return;
  }
  int i1Next,i2Next;
  for(int i1=topPointIndex;;)
  {
    if(p[i1Next=i1<1?n-1:i1-1].y<p[i1].y)
      return;
    else
      if(p[i1Next].y==p[i1].y)
        i1=i1Next;
      else
        break;
  }
  for(int i2=topPointIndex;;)
  {
    if(p[i2Next=(i2+1)%n].y<p[i2].y)
      return;
    else
      if(p[i2Next].y==p[i2].y)
        i2=i2Next;
      else
        break;
  }
  int y1=p[i1Next].y,y2=p[i2Next].y;
  long x1=p[i1].x<<16,
       x2=p[i2].x<<16,
       dx1=fraction(p[i1Next].x-p[i1].x,y1-p[i1].y),
       dx2=fraction(p[i2Next].x-p[i2].x,y2-p[i2].y);
  for(int y=yMin;y<=yMax;)
  {
    hline(x1>>16,y,((x2-x1)>>16)+1,color);
    x1+=dx1;
    x2+=dx2;
    y++;
    if(y==y1)
    {
      i1=i1Next;
      if(--i1Next<0)
        i1Next=n-1;
      if(p[i1].y==(y1=p[i1Next].y))
        break;
      dx1=fraction(p[i1Next].x-p[i1].x,y1-p[i1].y);
    }
    if(y==y2)
    {
      i2=i2Next;
      if(p[i2].y==(y2=p[i2Next=(i2Next+1)%n].y))
        break;
      dx2=fraction(p[i2Next].x-p[i2].x,y2-p[i2].y);
    }
  }
}

void Screen::line(int xs,int ys,int xe,int ye,long color,long lineStyle)
{
        if(xs>xe)
        {
                swap(xs,xe);
                swap(ys,ye);
        }
        if(xs>mClipRect.right || xe<mClipRect.left)
                return;
        if(xs<mClipRect.left)
        {
                if(xs==xe)
                        return;
                ys+=(long)(ye-ys)*(mClipRect.left-xs)/(xe-xs);
                xs=mClipRect.left;
        }
        if(xe>mClipRect.right)
        {
                if(xs==xe)
                        return;
                ye+=(long)(ye-ys)*(mClipRect.right-xe)/(xe-xs);
                xe=mClipRect.right;
        }
        if(ys<mClipRect.top)
        {
                if(ye<mClipRect.top)
                        return;
                xs+=(long)(xe-xs)*(mClipRect.top-ys)/(ye-ys);
                if(xs>xe)
                        return;
                ys=mClipRect.top;
        }
        else
                if(ys>mClipRect.bottom)
                {
                        if(ye>mClipRect.bottom)
                                return;
                        xs+=(long)(xe-xs)*(mClipRect.bottom-ys)/(ye-ys);
                        if(xs>xe)
                                return;
                        ys=mClipRect.bottom;
                }
        if(ye<mClipRect.top)
        {
                xe+=(long)(xe-xs)*(mClipRect.top-ye)/(ye-ys);
                ye=mClipRect.top;
        }
        else
                if(ye>mClipRect.bottom)
                {
                        xe+=(long)(xe-xs)*(mClipRect.bottom-ye)/(ye-ys);
                        ye=mClipRect.bottom;
                }
#define MASK_PIXEL(x,y)        if(lineStyle&(1L<<(step++&0x1F))) _PutPixel(x,y,color)
// rem: xs<=xe
           int step=0;
          MASK_PIXEL(xs,ys);
          int dx=xe-xs;
          int dy=abs(ye-ys);
          if(dy<=dx)
          {
                    int d,d1,d2=(dy-dx)<<1;
                    d=(d1=dy<<1)-dx;
                    if(ye>ys)
                              for(;dx--;++xs)
                              {
                                      MASK_PIXEL(xs,ys);
                                if(d>0)
                                {
                                          d+=d2;
                                          ys++;
                                }
                                else
                                          d+=d1;
                              }
                    else
                              for(;dx--;++xs)
                              {
                                      MASK_PIXEL(xs,ys);
                                      if(d>0)
                                {
                                          d+=d2;
                                          ys--;
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
                              for(;dy--;++ys)
                              {
                                      MASK_PIXEL(xs,ys);
                                if(d>0)
                                {
                                          d+=d2;
                                          xs++;
                                }
                                else
                                          d+=d1;
                              }
                    else
                              for(;dy--;--ys)
                              {
                                      MASK_PIXEL(xs,ys);
                                if(d>0)
                                {
                                          d+=d2;
                                          xs++;
                                }
                                else
                                          d+=d1;
                              }
          }
#undef MASK_PIXEL
}

//////////////////////////////////////////////////////////////////////////
void Screen::GetRefreshRect(cRect* cr) const
{
}

void Screen::SetRefreshRect(int l,int t,int r,int b)
{
}

int Screen::RefreshX(void) const
{
  return 0;
}

int Screen::RefreshY(void) const
{
  return 0;
}

void Screen::SetRefreshPoint(int x,int y)
{
}

Screen* Screen::RefreshScreen(void)
{
  return NULL;
}

BOOL Screen::RefreshScreen(Screen*)
{
  return FALSE;
}

void Screen::refresh(void)
{
}

tPicture* Screen::GetPicture(void)
{
  return NULL;
}

tPicture* Screen::freeze(void)
{
  return NULL;
}

//////////////////////////////////////////////////////////////////////////

void Screen::_outchar(int x0,int y0,int ch,long color,const tBGIFont* font)
{
  const uint16* vec=font->getdata(ch);
  uint op=1;
  int x=x0,y=y0,x1,y1;
  for(;op!=0;vec++)
  {
    if (!(op=*vec&0x8080)) break;
    int xx=*vec & 0x7F;
    int yy=(*vec>>8) & 0x7F;
    if (yy>63)
      yy-=128;
    x1=x0+xx;
    y1=y0-yy;
    if (op==0x8080)
       line(x,y,x1,y1,color);
       x=x1;
       y=y1;
  }
}

void Screen::outtext(int x,int y,const char* s,long color,const tBGIFont* font)
{
        if(IsValid())
                  for(;*s && x<=mClipRect.right;x+=font->width(*s),s++)
                                  if(font->in(*s))
                                    _outchar(x,y,*s,color,font);
}