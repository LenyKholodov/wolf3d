#include <grtypes.h>
#include <sbinfile.h>
#include <cbinfile.h>

///////////////////////////////////////////////////////////////////////////
// Palette
///////////////////////////////////////////////////////////////////////////

RGBPalette* RGBPalette::load(BinFile* f,tAllocFnPtr __alloc_fn)
{
  if(IsBad(f))
    return NULL;
  int start=f->gets(),count=f->gets();
  RGBPalette* p=create(start,count,__alloc_fn);
  if(p!=NULL)
    f->read(p->palette,sizeof(RGBStruct)*count);
  return p;
}

RGBPalette* RGBPalette::load(const char* name,tAllocFnPtr __alloc_fn)
{
  BinFile* f=new stdBinFile(name,BinFile::OPEN_RO);
  RGBPalette* ptr=load(f,__alloc_fn);
  delete f;
  return ptr;
}

RGBPalette* RGBPalette::create(int s,int c,tAllocFnPtr __alloc_fn)
{
  RGBPalette* p;
  if((p=(RGBPalette*)__alloc_fn(RGBPalette::size(c)))==NULL)
    return NULL;
  p->start=s;
  p->count=c;
  return p;
}

BOOL RGBPalette::save(const char* name) const
{
  BinFile* f=new stdBinFile(name,BinFile::OPEN_CR);
  BOOL result=FALSE;
  if(!IsBad(f))
    result=save(f);
  delete f;
  return result;
}

///////////////////////////////////////////////////////////////////////////
// Image
///////////////////////////////////////////////////////////////////////////

ulong tImage::size(int w,int h,int bpp)
{
  switch(bpp)
  {
    case BPP_8bit:  // STD_COLOR
            return sizeof(tImage)-1+w*h;
    case BPP_15bit: // HI_COLOR
    case BPP_16bit:
                return sizeof(tImage)-1+2*w*h;
    case BPP_24bit: // TRUE_COLOR
                return sizeof(tImage)-1+3*w*h;
    case BPP_32bit: //TRUE32_COLOR
                return sizeof(tImage)-1+4*w*h;
  }
  debug("tImage::size("<<w<<","<<h<<","<<bpp<<")- error bpp\n");
  return NULL;
}

#ifdef __BORLANDC__
void   tImage::shift(uchar value)
{
  for(ulong i=ulong(width)*ulong(height);i--;data[i]+=value);
}
#endif


const long ImageListId=tag2long("ILST"); // ImageLiST

tImageList* tImageList::load(BinFile* f,tAllocFnPtr __alloc_fn)
{
  if(IsBad(f))
    return NULL;
  ImageListHeader header;
  if(f->read(&header,sizeof(header))!=sizeof(header))
    return NULL;
  if(header.Id!=ImageListId)
  {
    debug("tImageList::load - invalid ImageList tag\n");
    return NULL;
  }
  tImageList* ilist=(tImageList*)__alloc_fn(f->size()+sizeof(long)*(header.count-1));
  if(ilist!=NULL)
  {
    ilist->count=header.count;
    ilist->bpp=header.bpp;
    ilist->palsize=header.palsize;
    // read data
    long* index=(long*)ilist->data;
    f->read(index+header.count,f->size()-sizeof(header));
    // create index
    index[0]=sizeof(long)*header.count+
                        (ilist->palsize?ilist->GetPalette()->size():0);
    for(int i=1;i<ilist->count;i++)
      index[i]=index[i-1]+ilist->GetImage(i-1)->size(ilist->bpp);
  }
  return ilist;
}

tImageList* tImageList::load(const char* name,tAllocFnPtr __alloc_fn)
{
  BinFile* f=new stdBinFile(name,BinFile::OPEN_RO);
  tImageList* ptr=load(f,__alloc_fn);
  delete f;
  return ptr;
}

tImageList* tImageList::create(int c,int bitpp,tImage** imgs,RGBPalette* p,tAllocFnPtr __alloc_fn)
{
// calculate size and allocate mem
  if(bitpp!=8 && bitpp!=15 && bitpp!=16 && bitpp!=24 && bitpp!=32)
    return NULL;
  ulong size=sizeof(tImageList)-1+sizeof(long)*c;
  if(p!=NULL)
    size+=p->size();
  for(int i=0;i<c;i++)
    size+=imgs[i]->size(bitpp);
  tImageList* il;
  if((il=(tImageList*)__alloc_fn(size))==NULL)
    return NULL;
  il->bpp=bitpp;
  il->count=c;
  long* index=(long*)il->data;
  index[0]=sizeof(long)*c;
  if(p!=NULL)
  {
    il->palsize=p->count;
    memcpy(il->GetPalette(),p,p->size());
    index[0]+=p->size();
  }
  else
    il->palsize=0;
  memcpy(il->GetImage(0),imgs[0],imgs[0]->size(bitpp));
  for(i=1;i<c;i++)
  {
    index[i]=index[i-1]+imgs[i-1]->size(bitpp);
    memcpy(il->GetImage(i),imgs[i],imgs[i]->size(bitpp));
  }
  return il;
}

tImageList* tImageList::create(int c,int bitpp,cRect* imgs,int psize,tAllocFnPtr __alloc_fn)
{
  ulong size=sizeof(tImageList)-1+sizeof(long)*c;
  switch(bitpp)
  {
    case 8: if(psize) size+=RGBPalette::size(psize);
    case 15:
    case 16:
    case 24:
    case 32:break;
    default: return NULL;
  }

  for(int i=0;i<c;i++)
    size+=tImage::size(imgs[i].width(),imgs[i].height(),bitpp);
  tImageList* il;
  if((il=(tImageList*)__alloc_fn(size))==NULL)
    return NULL;
  il->bpp=bitpp;
  il->count=c;
  long* index=(long*)il->data;
  index[0]=sizeof(long)*c;
  if(psize && bitpp==8)
  {
    il->palsize=psize;
    RGBPalette* p=il->GetPalette();
    p->count=psize;
    p->start=0;
    index[0]+=p->size();
  }
  else
    il->palsize=0;
  il->GetImage(0)->width=imgs[0].width();
  il->GetImage(0)->height=imgs[0].height();
  for(i=1;i<c;i++)
  {
    index[i]=index[i-1]+il->GetImage(i-1)->size(bitpp);
    il->GetImage(i)->width=imgs[i].width();
    il->GetImage(i)->height=imgs[i].height();
  }
  return il;
}

void tImageList::shift(int value)
{
  if(bpp==8)
  {
    RGBPalette* p=GetPalette();
    if(p!=NULL)
      p->start+=value;
    for(int i=0;i<count;i++)
      GetImage(i)->shift(value);
  }
}

ulong  tImageList::size(void)
{
  ulong sz=sizeof(tImageList)-1;
  for(int i=0;i<count;i++)
    sz+=GetImage(i)->size(bpp);
  RGBPalette* p=GetPalette();
  return (p!=NULL)?sz+p->size():sz;
}

BOOL tImageList::save(BinFile* f)
{
  if(IsBad(f))
    return FALSE;
  long sz=sizeof(tImageList)-1,isz=count*sizeof(long);
  f->putl(ImageListId);                // id
  f->write(this,sz);                     // header
  sz=size()-sz-isz;                          // skip index
  return f->write(data+isz,sz)==sz;
}

BOOL tImageList::save(const char* name)
{
  BinFile* f=new stdBinFile(name,BinFile::OPEN_CR);
  BOOL result=save(f);
  delete f;
  return result;
}

///////////////////////////////////////////////////////////////////////////
// Sprite
///////////////////////////////////////////////////////////////////////////

ulong tSprite::size(void) const
{
  const uchar* ptr=data;
  for(int i=0;i<height;i++)
    ptr+=*((uint16*)ptr);
  return ptr-((const uchar*)this);
}

void tSprite::shift(uchar value) // корректная работа только для режима 8bit!
{
  uchar* ptr=data;
  for(int i=0;i<height;i++)
  {
    ulong len=*((uint16*)ptr);
    for(int i=2;i<len;)
    {
      uint16 l=*((uint16*)(ptr+i));
      i+=2;
      if(l>=0x4000) // непрозрачность
      {
        if(l<0x8000) // добавить z-пиксел
          l++;
        for(l&=0x3FFF;l--;ptr[i++]+=value);
      }
    }
    ptr+=len;
  }
}

const long SpriteListId=tag2long("SLST"); // SpriteLiST

void tSpriteList::reindex(void)
{
  debug("tSpriteList::reindex running"<<endl);
  long* index=(long*)data;
  debug("tSpriteList::reindex: count="<<count<<" palsize="<<palsize<<endl);
  debug("                      ReallyPalsize="<<(palsize?GetPalette()->size():0)<<endl);
  index[0]=sizeof(long)*count+(palsize?GetPalette()->size():0);
  debug("Index #0:"<<index[0]<<endl);
  for(int i=1;i<count;i++)
  {
    debug("index[i-1]="<<index[i-1]<<" really:"<<((long*)data)[i-1]<<" ptr="
                                       <<(tImage*)(data+((long*)data)[i-1])<<endl);
    index[i]=index[i-1]+GetImage(i-1)->size();
    debug("Index #"<<i<<':'<<index[i]<<endl);
  }
}

tSpriteList* tSpriteList::load(BinFile* f,tAllocFnPtr __alloc_fn)
{
  debug("tSpriteList::load starting..."<<endl);
  if(IsBad(f))
    return NULL;
  ImageListHeader header;
  if(f->read(&header,sizeof(header))!=sizeof(header))
    return NULL;
  if(header.Id!=SpriteListId)
    return NULL;
  debug("tSpriteList::load alloc memory for list calling"<<endl);
  debug("tSpriteList::load Count="<<header.count<<endl);
  tSpriteList* ilist=(tSpriteList*)__alloc_fn(f->size());//+sizeof(long)*(header.count-1));
  debug("tSpriteList::load alloc memory for list completed"<<endl);
  if(ilist!=NULL)
  {
    ilist->count=long(header.count);
    ilist->palsize=header.palsize;
    f->read(ilist->data,f->size()-sizeof(header));
  }
  
  return ilist;
}

tSpriteList* tSpriteList::load(const char* name,tAllocFnPtr __alloc_fn)
{
  BinFile* f=new stdBinFile(name,BinFile::OPEN_RO);
  debug("tSpriteList::load(const char*,tAllocFnPtr)...."<<endl);
  tSpriteList* ptr=tSpriteList::load(f,__alloc_fn);
  delete f;
  return ptr;
}

tSpriteList* tSpriteList::create(int c,int bitpp,tSprite** imgs,RGBPalette* p,tAllocFnPtr __alloc_fn)
{
  // calculate size and allocate mem

  ulong size=sizeof(tSpriteList)-1+sizeof(long)*c;
  if(p!=NULL)
    size+=p->size();
  for(int i=0;i<c;i++)
    size+=imgs[i]->size();
  tSpriteList* il;
  if((il=(tSpriteList*)__alloc_fn(size))!=NULL)
  {
    il->bpp=bitpp;
    il->count=c;
    long* index=(long*)il->data;
    index[0]=sizeof(long)*c;
    if(p!=NULL)
    {
      il->palsize=p->count;
      memcpy(il->GetPalette(),p,p->size());
      index[0]+=p->size();
    }
    else
      il->palsize=0;
    memcpy(il->GetImage(0),imgs[0],imgs[0]->size());
    for(i=1;i<c;i++)
    {
      index[i]=index[i-1]+imgs[i-1]->size();
      memcpy(il->GetImage(i),imgs[i],imgs[i]->size());
    }
  }
  return il;
}

void tSpriteList::shift(int value)
{
  if(bpp==8)
  {
    RGBPalette* p=GetPalette();
    if(p!=NULL)
      p->start+=value;
    for(int i=0;i<count;i++)
      GetImage(i)->shift(value);
  }
}

ulong  tSpriteList::size(void)
{
  ulong sz=sizeof(tSpriteList)-1+count*sizeof(ulong);
  for(int i=0;i<count;i++)
    sz+=GetImage(i)->size();
  RGBPalette* p=GetPalette();
  return (p!=NULL)?sz+p->size():sz;
}

BOOL tSpriteList::save(BinFile* f)
{
  if(IsBad(f))
    return FALSE;
  long sz = size();
  
  f->putl(SpriteListId);                // id
  return f->write(this,sz)==sz;
}

BOOL tSpriteList::save(const char* name)
{
  BinFile* f=new stdBinFile(name,BinFile::OPEN_CR);
  BOOL result=save(f);
  delete f;
  return result;
}

//////////////////////////////////////////////////////////////////////

void tzSprite::shift(uchar value)
{
  uchar* ptr=data;
  for(int i=width*height+1;i--;ptr++)
    *ptr+=value;
}

//////////////////////////////////////////////////////////////////////

void RGB2RGBQ(int count,const RGBStruct* rgb,RGBQUAD* q)
{
        for(int i=0;i<count;i++)
        {
                q[i].red  =rgb[i].red;
                q[i].green=rgb[i].green;
                q[i].blue =rgb[i].blue;
                q[i].reserved=0;
        }
}

void QRGB2RGB(int count,const RGBQUAD* q,RGBStruct* rgb)
{
        for(int i=0;i<count;i++)
        {
                rgb[i].red  =q[i].red;
                rgb[i].green=q[i].green;
                rgb[i].blue =q[i].blue;
        }
}

void ShiftPalette(int count,RGBStruct* ptr,int offset)
{
        if(offset>0)
                while(count-->0)
                {
                        ptr[count].red  <<=offset;
                        ptr[count].green<<=offset;
                        ptr[count].blue <<=offset;
                }
        else
        {
                offset=-offset;
                while(count-->0)
                {
                        ptr[count].red  >>=offset;
                        ptr[count].green>>=offset;
                        ptr[count].blue >>=offset;
                }
        }
}

const long BinFontId=tag2long("BFNT"); // BinFoNT

#pragma pack(1)
typedef struct
{
  long Id;
  uchar start,end,widthb,height;
} BinFontHeader;
#pragma pack()

tBinFont* tBinFont::load(BinFile* f,tAllocFnPtr __alloc_fn)
{
  BinFontHeader header;
  if(IsBad(f))
    return NULL;
  if(f->read(&header,sizeof(header))!=sizeof(header))
    return NULL;
  if(header.Id!=BinFontId)
    return NULL;
  tBinFont* ptr;
  ulong fdsize=(ulong(header.widthb)*ulong(header.height)+1)
                          *(long(header.end)-long(header.start)+1);
  if((ptr=create(header.start,header.end,header.widthb*8,header.height,__alloc_fn))==NULL)
    return NULL;
  f->read(ptr->data,fdsize);
  return ptr;
}

tBinFont* tBinFont::load(const char* name,tAllocFnPtr __alloc_fn)
{
  BinFile* f=new stdBinFile(name,BinFile::OPEN_RO);
  tBinFont* ptr=load(f,__alloc_fn);
  delete f;
  return ptr;
}

tBinFont* tBinFont::create(int st,int e,int w,int h,tAllocFnPtr __alloc_fn)
{
  tBinFont* ptr;
  if((ptr=(tBinFont*)__alloc_fn(tBinFont::size(st,e,w,h)))==NULL)
    return NULL;
  ptr->start =st;
  ptr->end   =e;
  ptr->widthb=(w+7)>>3;
  ptr->height=h;
  return ptr;
}

int tBinFont::width(const char* s) const
{
  for(int w=0;*s;s++)
    w+=width(*s);
  return w;
}

BOOL tBinFont::save(BinFile* f)
{
  if(IsBad(f))
    return FALSE;
  f->putl(BinFontId);
  ulong sz=size();
  return f->write(this,sz)==sz;
}

BOOL tBinFont::save(const char* name)
{
  BinFile* f=new stdBinFile(name,BinFile::OPEN_CR);
  BOOL result=IsBad(f)?FALSE:save(f);
  delete f;
  return result;
}

const long SpriteFontId=tag2long("SFNT"); // SpriteFoNT
#pragma pack(1)
typedef struct
{
  long Id;
  long start;
  uint16 count,bpp,palsize;
} SpriteFontHeader;
#pragma pack()

tSpriteFont* tSpriteFont::load(BinFile* f,tAllocFnPtr __alloc_fn)
{
  if(IsBad(f))
    return NULL;
  SpriteFontHeader header;
  if(f->read(&header,sizeof(header))!=sizeof(header))
    return NULL;
  if(header.Id!=SpriteFontId)
    return NULL;
  tSpriteFont* font=(tSpriteFont*)__alloc_fn(f->size()+sizeof(long)*(header.count-1));
  if(font!=NULL)
  {
    font->sprites.count=header.count;
    font->sprites.bpp=header.bpp;
    font->sprites.palsize=header.palsize;
        // read data
    f->read(font->sprites.data+sizeof(long)*ulong(header.count),f->size()-sizeof(header));
        // create index
    font->sprites.reindex();
  }
  return font;
}

tSpriteFont* tSpriteFont::load(const char* name,tAllocFnPtr __alloc_fn)
{
  BinFile* f=new stdBinFile(name,BinFile::OPEN_RO);
  tSpriteFont* result=IsBad(f)?NULL:load(f,__alloc_fn);
  delete f;
  return result;
}

tSpriteFont* tSpriteFont::create(int st,tSpriteList* imgs,tAllocFnPtr __alloc_fn)
{
  tSpriteFont* font;
  if((font=(tSpriteFont*)__alloc_fn(sizeof(long)+imgs->size()))!=NULL)
  {
    font->start=st;
    memcpy(&font->sprites,imgs,imgs->size());
  }
  return font;
}

int tSpriteFont::width(const char* s)
{
  for(int l=0;*s;s++)
    l+=width(*s);
  return l;
}

int tSpriteFont::width(int i)
{
  tSprite* ptr=sprites.GetImage(i-start);
  return ptr!=NULL?ptr->width:0;
}

BOOL tSpriteFont::save(BinFile* f)
{
  if(IsBad(f))
    return FALSE;
  long sz=sizeof(tSpriteFont)-1,isz=sprites.count*sizeof(long);
  f->putl(SpriteFontId);                // id
  f->write(this,sz);                     // header
  sz=size()-sz-isz;                          // skip index
  return f->write(sprites.data+isz,sz)==sz;
}

BOOL tSpriteFont::save(const char* name)
{
  BinFile* f=new stdBinFile(name,BinFile::OPEN_CR);
  BOOL result=save(f);
  delete f;
  return result;
}

char* ColorInfo(int bits)
{
        switch(bits)
        {
                case 4: return "16";
                case 8: return "256";
                case 15:return "32K(HiColor)";
                case 16:return "64K(HiColor)";
                case 24:return "16M(TrueColor)";
                case 32:return "2G(TrueColor)";
        }
        return "???";
}
