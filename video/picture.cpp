#include <picture.h>
#include <sbinfile.h>
#include <cbinfile.h>

#include <debug/debug.h>

ulong tPicture::size(long w,long h,int bpp,int PalCount)
{
        switch(bpp)
        {
//                case 1: return sizeof(tPicture)-1+((w*h+7)/8)+PalCount*sizeof(RGBStruct);
//                case 4: return sizeof(tPicture)-1+((w*h+1)/2)+PalCount*sizeof(RGBStruct);
                case 8:  return sizeof(tPicture)-1+w*h+PalCount*sizeof(RGBStruct);
                case 15:
                case 16: return sizeof(tPicture)-1+w*h*2;
                case 24: return sizeof(tPicture)-1+w*h*3;
                case 32: return sizeof(tPicture)-1+w*h*4;
        }
        return 0;
}

ulong tPicture::size(const BitmapInfoHeader* bih)
{
        return size(bih->biWidth,bih->biHeight,
                                  bih->biBitCount,
                           bih->biClrUsed==0?256:bih->biClrUsed);
}

ulong tPicture::size(const PCXHeader* bih)
{
        return size(bih->xmax-bih->xmin+1,bih->ymax-bih->ymin+1,8,256);
}

tPicture* tPicture::create(long x0,long y0,long w,long h,int bpp,int PalCount,tAllocFnPtr __a)
{
  tPicture* ptr=(tPicture*)__a(size(w,h,bpp,PalCount));
  if(ptr!=NULL)
    ptr->init(x0,y0,w,h,bpp,PalCount);
  return ptr;
}

tPicture* tPicture::create(const BitmapInfoHeader* bih,tAllocFnPtr __a)
{
  tPicture* ptr=(tPicture*)__a(size(bih));
  if(ptr!=NULL)
    ptr->init(bih);
  return ptr;
}

tPicture* tPicture::create(const PCXHeader* bih,tAllocFnPtr __a)
{
  tPicture* ptr=(tPicture*)__a(size(bih));
  if(ptr!=NULL)
    ptr->init(bih);
  return ptr;
}


int BMPWrite(BinFile* wr,const tPicture* image)
{
  if(IsBad(wr) || !wr->canwrite())
                return PFE_FILE;
  BitmapFileHeader bfh;
  BitmapInfoHeader bih;
  // Initialise file header
  bfh.bfType=0x4D42;        // 'BM' signature
  bfh.Reserved=0;                // Always Zero
  bfh.bfOffBits=sizeof(BitmapFileHeader)+sizeof(BitmapInfoHeader)+image->palSize*sizeof(RGBQUAD); //bitimage offset
  ulong BytesPerLine,
        width =image->image.width,
        height=image->image.height;
  int Appendix;
  switch(image->bpp)
  {
    case 8:  Appendix=(4-width)&0x00000003;
                 BytesPerLine=width;
             break;
    case 15:
    case 16: Appendix=width&1?2:0;
             BytesPerLine=width*2;
             break;
    case 24: Appendix=(4-width*3)&0x00000003;
             BytesPerLine=width*3;
             break;
    case 32: Appendix=0;
             BytesPerLine=width*4;
             break;
    default: return PFE_BPP;
  }
  bfh.bfSize=(BytesPerLine+Appendix)*height+bfh.bfOffBits; // file's size
  bih.biSize=sizeof(BitmapInfoHeader);
  bih.biWidth=width;
  bih.biHeight=height;
  bih.biPlanes=1;
  bih.biBitCount=image->bpp;
  bih.biCompression=0;
  bih.biSizeImage=(BytesPerLine+Appendix)*height;
  bih.biXPelsPerMeter=32000;
  bih.biYPelsPerMeter=32000;
  bih.biClrUsed=image->palSize==256?0:image->palSize;
  bih.biClrImportant=0;
  wr->write(&bfh,sizeof(bfh));
  wr->write(&bih,sizeof(bih));
  const uchar* data=image->image.data+BytesPerLine*height;
  if(image->palSize)
  {
    RGBStruct* ptr=(RGBStruct*)data;
    for(int i=image->palSize;i-->0;ptr++)
    {
      wr->putc(ptr->blue);
      wr->putc(ptr->green);
      wr->putc(ptr->red);
      wr->putc(0);
    }
  }
  while(height-->0)
  {
    wr->write(data-=BytesPerLine,BytesPerLine);
    switch(Appendix)
    {
      case 3: wr->putc(0);
      case 2: wr->putc(0);
      case 1: wr->putc(0);
    }
  }
  return PFE_OK;
}

int BMPWrite(BinFile* wr,const tImage* image,int bpp,int palCount,const RGBQUAD* pal)
{
  if(IsBad(wr) || !wr->canwrite())
    return PFE_FILE;
  BitmapFileHeader bfh;
  BitmapInfoHeader bih;
  // Initialise file header
  bfh.bfType=0x4D42;        // 'BM' signature
  bfh.Reserved=0;                // Always Zero
  bfh.bfOffBits=sizeof(BitmapFileHeader)+sizeof(BitmapInfoHeader)+palCount*sizeof(RGBQUAD); //bitimage offset
  ulong BytesPerLine,
        height=image->height;
  int Appendix;
  switch(bpp)
  {
        case 8:  Appendix=(4-image->width)&0x00000003;
                      BytesPerLine=image->width; break;
        case 15:
        case 16: Appendix=(image->width&1)?2:0;
                 BytesPerLine=image->width*2; break;
        case 24: Appendix=(4-(image->width*3))&0x00000003;
                 BytesPerLine=image->width*3; break;
        case 32: Appendix=0;
                 BytesPerLine=image->width*4; break;
        default: return PFE_BPP;
  }
  bfh.bfSize=(BytesPerLine+Appendix)*height+bfh.bfOffBits; // file's size
  bih.biSize=sizeof(BitmapInfoHeader);
  bih.biWidth=image->width;
  bih.biHeight=height;
  bih.biPlanes=1;
  bih.biBitCount=bpp;
  bih.biCompression=0;
  bih.biSizeImage=(BytesPerLine+Appendix)*height;
  bih.biXPelsPerMeter=32000;
  bih.biYPelsPerMeter=32000;
  bih.biClrUsed=palCount==256?0:palCount;
  bih.biClrImportant=0;
  wr->write(&bfh,sizeof(bfh));
  wr->write(&bih,sizeof(bih));
  if(palCount)
    wr->write(pal,sizeof(RGBQUAD)*palCount);
  // picture write
  while(height-->0)
  {
    wr->write(image->data+height*BytesPerLine,BytesPerLine);
    switch(Appendix)
    {
      case 3: wr->putc(0);
      case 2: wr->putc(0);
      case 1: wr->putc(0);
    }
  }
  return PFE_OK;
}

int BMPReadHeader(BinFile* rd,BitmapInfoHeader* bih)
{
  if(IsBad(rd) || !rd->canread() || rd->size()<sizeof(BitmapInfoHeader)+sizeof(BitmapFileHeader))
    return PFE_FILE;
  BitmapFileHeader bfh;
  rd->reset();
  if(rd->read(&bfh,sizeof(bfh))!=sizeof(bfh))
    return PFE_FILE;
  if(bfh.bfType!=0x4D42)                // 'BM' signature
    return PFE_FORMAT;
  if(rd->read(bih,sizeof(BitmapInfoHeader))!=sizeof(BitmapInfoHeader))
    return PFE_FILE;
  if(bih->biPlanes!=1 || bih->biCompression!=0)
    return PFE_FORMAT;
  switch(bih->biBitCount)
  {
//    case 1:  break;
//    case 4:  break;
    case 8:  break;
    case 15: break;
    case 16: break;
    case 24: break;
    case 32: break;
    default:  return PFE_FORMAT;
  }
  return PFE_OK;
}

int BMPReadBody(BinFile* rd,tPicture* image)
{
  ulong BytesPerLine,
        height=image->image.height,
        width=image->image.width;
  int Appendix;
  switch(image->bpp)
  {
    case 8:  Appendix=(4-width)&0x00000003;
                    BytesPerLine=width;
             break;
    case 15:
    case 16: Appendix=(width&1)?2:0;
                         BytesPerLine=width*2; break;
                case 24: Appendix=(4-width*3)&0x00000003;
                         BytesPerLine=width*3; break;
                case 32: Appendix=0;
                         BytesPerLine=width*4; break;
                default: return PFE_BPP;
  }
  uchar* data=image->image.data+BytesPerLine*height;
  if(image->palSize!=0)
  {
    RGBStruct* ptr=(RGBStruct*)data;
    for(int i=image->palSize;i-->0;ptr++)
    {
      ptr->blue =rd->getc();
      ptr->green=rd->getc();
      ptr->red  =rd->getc();
      rd->getc();
    }
  }
  while(height-->0)
  {
    rd->read(data-=BytesPerLine,BytesPerLine);
    switch(Appendix)
    {
      case 3: rd->getc();
      case 2: rd->getc();
      case 1: rd->getc();
    }
  }
  return PFE_OK;
}

int BMPReadBody(BinFile* rd,BitmapInfoHeader* bih,tImage* image,RGBQPalette* pal)
{
  ulong BytesPerLine,
        height=image->height=bih->biHeight,
        width =image->width =bih->biWidth;
  int Appendix,palcount=0;
        switch(bih->biBitCount)
        {
                case 8:  Appendix=(4-width)&0x00000003;
                              palcount=bih->biClrUsed;
                         if(palcount==0)
                                 palcount=256;
                              BytesPerLine=width; break;
                case 15:
                case 16: Appendix=(width&1)?2:0;
                         BytesPerLine=width*2; break;
                case 24: Appendix=(4-width*3)&0x00000003;
                         BytesPerLine=width*3; break;
                case 32: Appendix=0;
                         BytesPerLine=width*4; break;
                default: return PFE_BPP;
        }
  uchar* data=image->data+BytesPerLine*height;
  if(pal!=NULL && palcount>0)
    rd->read(pal->palette,(pal->count=palcount)*sizeof(RGBQUAD));
  while(height-->0)
  {
    rd->read(data-=BytesPerLine,BytesPerLine);
    switch(Appendix)
    {
      case 3: rd->getc();
      case 2: rd->getc();
      case 1: rd->getc();
    }
  }
  return PFE_OK;
}

int PCXWrite(BinFile* wr,const tPicture* image)
{
  PCXHeader pcxh;
  if(image->bpp!=8)
    return PFE_FORMAT;
  if(IsBad(wr) || !wr->canwrite())
    return PFE_FILE;
  ulong width=image->image.width,
        height=image->image.height;
  memset(&pcxh,0,sizeof(PCXHeader));
  pcxh.manufacturer=10;
  pcxh.version         =5;
  pcxh.encoding         =1;
  pcxh.BitsPerPixel=8;
  pcxh.xmin         =image->x;
  pcxh.ymin         =image->y;
  pcxh.xmax         =image->x+width-1;
  pcxh.ymax         =image->y+height-1;
  pcxh.CountColorPlanes=1;
  pcxh.BytesPerLine=(width+1)&0xFFFE; // выравнивание на гр. слова
  wr->write(&pcxh,sizeof(PCXHeader));

  const uchar* ptr=image->image.data;
  ulong size=width*height,count;
  for(ulong i=0;i<size;i+=count)
  {
    char cp=*ptr;
    for(count=1;i+count<size && ptr[count]==cp && count<63;count++);
    if(count>1 || (cp&0xC0)==0xC0)
      wr->putc(count|0xC0);
    wr->putc(cp);
    ptr+=count;
  }
  wr->putc(12);
  uint psize=image->palSize;
  if(psize==0)
    for(i=0;i<256;i++)
      wr->putc(i).putc(i).putc(i);
  else
  {
    wr->write(ptr,psize*sizeof(RGBStruct));
    for(i=psize;i<256;i++)
      wr->putc(0).putc(0).putc(0);
  }
  return PFE_OK;
}

int PCXReadHeader(BinFile* rd,PCXHeader* bih)
{
  if(IsBad(rd) || !rd->canread() || rd->size()<sizeof(PCXHeader))
    return PFE_FILE;
  rd->reset();
  if(rd->read(bih,sizeof(PCXHeader))!=sizeof(PCXHeader))
    return PFE_FILE;
  if(bih->manufacturer!=10 ||
     bih->version!=5          ||
     bih->encoding!=1          ||
     bih->CountColorPlanes!=1 ||
     bih->BitsPerPixel!=8)
    return PFE_FORMAT;
  return PFE_OK;
}

int PCXReadBody(BinFile* rd,tPicture* image)
{
  if(image->bpp!=8)
    return PFE_BPP;
  uchar* data=image->image.data;
  for(ulong size=image->image.height*image->image.width;size;)
  {
    ulong c=rd->getc();
    if((c&0xC0)==0xC0)
    {
      if((c&=0x3F)>size)
        c=size;
      if(c)
        memset(data,rd->getc(),c);
      data+=c;
      size-=c;
    }
    else
    {
      *data++=c;
      size--;
    }
  }
  if(image->palSize!=0)
  {
    int PalFlag=rd->getc();
    RGBStruct* ptr=(RGBStruct*)data;
    rd->read(data,image->palSize*sizeof(RGBStruct));
    if(PalFlag==10)
      for(int i=image->palSize;i-->0;ptr++)
      {
        ptr->red  <<=2;
        ptr->green<<=2;
        ptr->blue <<=2;
      }
  }
  return PFE_OK;
}

int PCXReadBody(BinFile* rd,PCXHeader* bih,tImage* image,RGBStruct* pal)
{
  if(bih->BitsPerPixel!=8)
    return PFE_BPP;
        uchar* data=image->data;
        for(ulong size=image->height*image->width;size;)
        {
                ulong c=rd->getc();
                if((c&0xC0)==0xC0)
                {
                        if((c&=0x3F)>size)
                                c=size;
                        if(c)
                                memset(data,rd->getc(),c);
                        data+=c;
                        size-=c;
                }
                else
                {
                        *data++=c;
                        size--;
                }
        }
        if(pal!=NULL)
        {
                int PalFlag=rd->getc();
                RGBStruct* ptr=(RGBStruct*)data;
                rd->read(data,256*sizeof(RGBStruct));
                if(PalFlag==10)
                        for(int i=256;i-->0;ptr++)
                        {
                                ptr->red  <<=2;
                                ptr->green<<=2;
                                ptr->blue <<=2;
                        }
        }
        return PFE_OK;
}

tAllocFnPtr cPicture::m_alloc_fn=malloc;
tFreeFnPtr  cPicture::m_free_fn=free;


BOOL cPicture::PCXload(const char* name)
{
  BinFile* f=new stdBinFile(name,BinFile::OPEN_RO);
  BOOL result=PCXload(f);
  delete f;
  return result;
}

BOOL cPicture::BMPload(const char* name)
{
  BinFile* f=new stdBinFile(name,BinFile::OPEN_RO);
  BOOL result=BMPload(f);
  delete f;
  return result;
}

BOOL cPicture::PCXload(BinFile* rd)
{
  Free();
  if(IsBad(rd))
    return FALSE;
  PCXHeader bih;
  if(PCXReadHeader(rd,&bih)!=PFE_OK)
    return FALSE;
  if((src=tPicture::create(&bih,__alloc_fn))==NULL)
    return FALSE;
  if(PCXReadBody(rd,src)!=PFE_OK)
  {
    Free();
    return FALSE;
  }
  picture=&src;
  return TRUE;
}

BOOL cPicture::BMPload(BinFile* rd)
{
  Free();
  if(IsBad(rd))
    return FALSE;
  BitmapInfoHeader bih;
  if(BMPReadHeader(rd,&bih)!=PFE_OK)
    return FALSE;
  if((src=tPicture::create(&bih,__alloc_fn))==NULL)
    return FALSE;
  if(BMPReadBody(rd,src)!=PFE_OK)
  {
    Free();
    return FALSE;
  }
  picture=&src;
  return TRUE;
}

BOOL cPicture::PCXsave(const char* name)
{
  if(!IsValid())
    return FALSE;
  BinFile* f=new stdBinFile(name,BinFile::OPEN_CR);
  BOOL result=PCXsave(f);
  delete f;
  return result;
}

BOOL cPicture::BMPsave(const char* name)
{
  if(!IsValid())
    return FALSE;
  BinFile* f=new stdBinFile(name,BinFile::OPEN_CR);
  BOOL result=BMPsave(f);
  delete f;
  return result;
}

BOOL cPicture::init(long x,long y,long width,long height,int bpp,int PalCount)
{
  Free();
  if((src=tPicture::create(x,y,width,height,bpp,PalCount,__alloc_fn))==NULL)
    return FALSE;
  picture=&src;
  return TRUE;
}

void cPicture::swapRB(void)
{
        switch(bpp())
        {
                case 8:  {
                              RGBStruct* data=GetPalette();
                         for(ulong i=GetPaletteSize();i--;data++)
                                  swap(data->red,data->blue);
                         }
                         break;
                case 24: {
                                RGBStruct* data=(RGBStruct*)GetData();
                                for(ulong i=width()*height();i--;data++)
                                          swap(data->red,data->blue);
                         }
                         break;
                case 32: {
                                RGBQUAD* data=(RGBQUAD*)GetData();
                                for(ulong i=width()*height();i--;data++)
                                          swap(data->red,data->blue);
                         }
                         break;
        }
}



cPicture::~cPicture(void)
{
  Free();
}