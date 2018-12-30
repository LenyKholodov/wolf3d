#include "textures.h"

#include <sbinfile.h>
#include <cbinfile.h>

#include <mem.h>
#include <malloc.h>

//#define NDEBUG
#include <debug\debug.h>

//////////////////Make-functions for all classes ////////////////////////////
Texture*        makeTexture (tImage* image,int bpp)
{
  return        new (Texture::size(image->width,image->height,bpp))
                Texture (image,bpp);
}

Texture*        makeTexture (void* x,int w,int h,int bpp)
{
  return        new (Texture::size(w,h,bpp))
                Texture (x,w,h,bpp);
}

MipMapText*     makeMMT     (Texture* x,RGBStruct* pal,int count)
{
  return        new (MMT::size(x,1,count))
                MMT (x,pal,count);
}

MipMapText*     makeMMT     (uchar** list,int count,int w,int h,int bytesPP)
{
  return        new (w*h*count*bytesPP)    //Довольно приблизительно
                MMT (list,count,w,h,bytesPP);
}

MipMapText*     makeMMT     (BinFile* file)
{
  MMTHeader header;

  file->reset();
  file->read(&header,sizeof(MMTHeader));

  if (memcmp(&header.id,"MMT ",4))
  {
     dout<<"MMT: Wrong file format"<<endl;
     return NULL;
  }

  return        new (header.text_size)
                MMT (file);

}

MipMapText*     makeMMT     (const char* name)
{
  stdBinFile    file;

  if (!file.open(name,BinFile::OPEN_RO))
  {
    dout<<"MMT: File not found '"<<name<<"'"<<endl;
    return NULL;
  }

  MMT* x = makeMMT(&file);

  file.close();

  return x;
}

//////////////////implementation of class Texture////////////////////////////

tImage*     Texture::GetImage (int bpp) const
{
  tImage* image = (tImage*)malloc(width*height*bpp+sizeof(tImage)-sizeof(uchar));
  if (image == NULL)     return image;

  image->width  = width;
  image->height = height;

  memcpy (image->data,data,width*height*bpp);

  return image;
}

ulong       Texture::size (int bpp) const
{
  return sizeof(Texture)-sizeof(uchar)+bpp*width*height;
}

ulong       Texture::size (int w,int h,int bpp)
{
  return sizeof(Texture)-sizeof(uchar)+bpp*w*h;
}

Texture::Texture  (tImage* image,int bpp)
        : width(image->width), height(image->height)
{
  memcpy(data,image->data,width*height*bpp);
}

Texture::Texture  (void* x,int w,int h,int bpp)
        : width (w), height(h)
{
  memcpy (data,x,w*h*bpp);
}

/////////////////implementation of class MipMapTexture///////////////////////


BOOL    MMT::Load (BinFile* file)
{
  MMTHeader header;

  file->reset();
  file->read(&header,sizeof(MMTHeader));

  if (memcmp(&header.id,"MMT ",4))
  {
     dout<<"MMT: Wrong file format"<<endl;
     return FALSE;
  }

  file->read(&mBpp,header.text_size);

  return TRUE;
}

void    MMT::Save (BinFile* file)
{
  MMTHeader     header;

  memcpy(&header.id,"MMT ",4);
  memset(&header.reserv,0,sizeof(ulong)*2);

  header.text_size  = size();   //??

  file->reset();
  file->write(&header,sizeof(MMTHeader));
  file->write(&mBpp,header.text_size);
}

BOOL MMT::Load(const char* path)
{
  stdBinFile f;
  if (!f.open(path,BinFile::OPEN_RO))
  {
     dout<<"MMT: File not found "<<path<<endl;
     return FALSE;
  }
  BOOL status=Load(&f);
  f.close();
  return status;
}

void MMT::Save(const char* path)
{
  stdBinFile f;
  f.open(path,BinFile::OPEN_CR);
  Save(&f);
  f.close();
}

Texture*      MMT::GetTexture  (uint num) const
{
  ulong* offset = (ulong*)mData;

  return (Texture*)(mData + offset[num]);
}


uint          MMT::size ()   const
{
  int   sz = sizeof(MMT)-sizeof(uchar)+sizeof(ulong)*mCount;

  for (int i=0;i<mCount;sz+=GetTexture(i)->size(mBpp),i++);

  return sz;
}

uint          MMT::size (Texture* x,int bpp,int count)
{
  return size (x->width,x->height,bpp,count);
}

uint          MMT::size (int width,int height,int bpp,int count)
{
  int sz     = sizeof(MMT) - sizeof(uchar);

  for (int i=0;i<count;sz+=Texture::size(width,height,bpp),width>>=1,height>>=1,i++);

  return sz;
}

MMT::MipMapText (Texture* x,RGBStruct* pal,int count)
    :mBpp(1),mCount(count)
{
  ////!!!!!!
}

MMT::MipMapText (uchar** list,int c,int w,int h,int bpp)
    :mCount(c), mBpp(bpp)
{
  ulong*        offset = (ulong*)mData;
  ulong         pos    = c * sizeof(ulong);

  for (int i=0;i<mCount;i++,w>>=1,h>>=1)
  {
    offset [i] = pos;
    new (mData+pos) Texture (list[i],w,h,bpp);
    pos += Texture::size(w,h,bpp);
  }
}


////////////////////implementation of class TextCache////////////////////////

void   TextCache::LoadData(long id,CacheList* dest)
{
  new (dest->data) MMT(GetName(id));
}

TextCache::TextCache(char* names[],long id_num,long cache_sz,long cl_sz)
          :NameCache(names,id_num,id_num,cache_sz,cl_sz)
{   }

TextCache::TextCache(BinFile* file,long cache_sz)
          :NameCache(file,cache_sz)
{   }

TextCache::TextCache(const char* name,long cache_sz)
          :NameCache(name,cache_sz)
{   }



