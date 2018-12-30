#ifndef __TEXTURES__
#define __TEXTURES__

#include <memory/memory.h>
#include "cache.h"

#include <grtypes.h>
#include <binfile.h>

#define   TEXT_SIZE       0xFFFF

//const int TEXT_WIDTH   =  128;
//const int TEXT_HEIGHT  =  128;

class  Texture;
class  MipMapText;

////////////////////////////////////////////////////////////////////////////
////Make-fucntions for all classes
////////////////////////////////////////////////////////////////////////////
Texture*        makeTexture (tImage*,int bytesPP=1);
Texture*        makeTexture (void*,int w,int h,int bytesPP = 1);
MipMapText*     makeMMT     (uchar**,int count,int w,int h);
MipMapText*     makeMMT     (Texture*,RGBStruct*,int count=1);
MipMapText*     makeMMT     (BinFile*);
MipMapText*     makeMMT     (const char*);

////////////////////////////////////////////////////////////////////////////
////Описание заголовка файла MMT
////////////////////////////////////////////////////////////////////////////
struct  MMTHeader
{
  uchar         id[4];          //"MMT "
  ulong         text_size;      //Size of MMT
  ulong         reserv[2];
};

/////////////////////////////////////////////////////////////////////////////
////Просто текстура
/////////////////////////////////////////////////////////////////////////////
class   Texture
{
  public:
          uint           width,         //Ширина текстуры
                         height;        //Высота текстуры
          uchar          data[1];       //Данные текстуры

                         Texture  (tImage*,int bytesPP=1);
                         Texture  (void*,int,int,int bytesPP=1);

             ulong       size (int bytesPP=1) const;
    static   ulong       size (int w,int h,int bytesPP=1);

             void*       operator new (size_t x) { return ::operator new (x); }
             void*       operator new (size_t,size_t x) { return ::operator new (x); }
             void*       operator new (size_t,void* dest) { return dest; }


             tImage*     GetImage (int bytesPP=1)  const;
                         operator uchar*  ()        { return data; }
};

/////////////////////////////////////////////////////////////////////////////
////Класс пирамида фильтрованных текстур
/////////////////////////////////////////////////////////////////////////////
////Строится из одной текстуры
/////////////////////////////////////////////////////////////////////////////
class   MipMapText
{
  protected:
          uchar          mBpp;         //Кол-во байт на пиксель
          uint           mCount;       //Кол-во текстур
          uchar          mData[1];     //Данные текстуры (включая смещения)

  public:
          MipMapText (Texture*,RGBStruct*,int count=1);
          MipMapText (uchar** list,int count,int w,int h,int bytesPP=1);
          MipMapText (BinFile* file)                      { Load (file); }
          MipMapText (const char* name)                   { Load (name); }

          Texture*      GetTexture  (uint num) const;
          tImage*       GetImage    (uint num) const { return GetTexture(num)->GetImage(mBpp); }

          uint          width(uint num)  const     { return GetTexture(num)->width; }
          uint          height(uint num) const     { return GetTexture(num)->height; }
          uint          width()  const             { return width(0); }
          uint          height() const             { return height(0);}

          uchar         bpp()    const             { return mBpp; }
          uint          count()  const             { return mCount; }

          uint          size ()   const;
   static uint          size (Texture*,int bpp=1,int count=1);
   static uint          size (int w,int h,int bpp=1,int count=1);

/////////////////////////////////////////////////////////////////////////////
////Работа с памятью
/////////////////////////////////////////////////////////////////////////////
          void*         operator new    (size_t x)   { return ::operator new (x); }
          void*         operator new    (size_t,size_t x) { return ::operator new(x) ;}
          void*         operator new    (size_t bytes,void* dest) { return dest; }

/////////////////////////////////////////////////////////////////////////////
////Загрузка и сохранение
/////////////////////////////////////////////////////////////////////////////
          BOOL          Load(BinFile* file);
          BOOL          Load(const char* name);
          void          Save(BinFile* file);
          void          Save(const char* name);
};

typedef MipMapText      MMT;

/////////////////////////////////////////////////////////////////////////////
///Класс кэш текстур
/////////////////////////////////////////////////////////////////////////////
class   TextCache: public NameCache
{
  public:
                  TextCache  (char* names[],
                              long id_num,
                              long cache_sz,
                              long cl_sz);
                  TextCache  (BinFile* file,long cache_sz=1);
                  TextCache  (const char* name,long cache_sz=1);

  protected:
          virtual void          LoadData(long id,CacheList* dest);
};

#endif