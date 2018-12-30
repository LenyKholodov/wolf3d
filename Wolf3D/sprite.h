#ifndef __SPRITE__
#define __SPRITE__

#include <binfile.h>
#include <fscreen.h>

#include "fixed.h"
#include "map.h"
#include "cache.h"

class   SimpleSprite;
class   Sprite3D;
class   GameSprite3D;
class   SpriteCache;

/////////////////////////////////////////////////////////////////////////////
////Параметры для создания спрайтов из утилит
/////////////////////////////////////////////////////////////////////////////
const   int       MAX_SPRITE_FACES   = 512;
//const   int       SPRITE_SIZE        = 500000;//256*256*1*8*4;
const   int       MAX_SPRITES        = 256;
extern  int       SPRITE_SIZE;

/////////////////////////////////////////////////////////////////////////////
////Make-functions для создания всех типов объектов с плавающим размером
/////////////////////////////////////////////////////////////////////////////

SimpleSprite*  makeSimpleSprite ( tImage*,int z=0,int bpp=1 );

Sprite3D*      makeSprite3D     ( tImageList*,int z=0 );
Sprite3D*      makeSprite3D     ( SimpleSprite**,int num,int bpp=1);

GameSprite3D*  makeGameSprite3D ( Sprite3D**,int num);
GameSprite3D*  makeGameSprite3D ( BinFile*);
GameSprite3D*  makeGameSprite3D ( const char*);

/////////////////////////////////////////////////////////////////////////////
////Заголовок файла для сохранения спрайтов
/////////////////////////////////////////////////////////////////////////////
struct  WSFHeader
{
  uchar         id[4];          //"WSF "
  uint          num;            //Колличество фаз
  ulong         total_size;     //Общий размер всего игрового спрайта
  uchar         reserv[8];      //Зарезервировано
};

/////////////////////////////////////////////////////////////////////////////
////Плоскость спрайта
/////////////////////////////////////////////////////////////////////////////
class  SimpleSprite
{
  public:
    int           width;        //Ширина
    int           height;       //Высота
    int           z_color;      //Цвет прозрачности
    uchar         data[1];      //Данные

                  SimpleSprite (tImage*,int z=0);
                  SimpleSprite (BinFile*,long size);

           long   size   (int bpp) const;
    static long   size   (int w,int h,int bpp);

    ulong*        offs   ()            const   { return (ulong*)data; }
    uchar*        operator [] (int i)  const   { return (uchar*)(data+offs()[i]); }

    void*         operator           new        (size_t,size_t);
    void*         operator           new        (size_t,void* x) { return x; }
};

/////////////////////////////////////////////////////////////////////////////
////Фаза движения 3D спрайта (n-направлений)
/////////////////////////////////////////////////////////////////////////////
class  Sprite3D
{
  public:
    int           num;                  //кол-во направлений обзора
    int           bpp;                  //Bytes Per Pixel
    uchar         data[1];              //Данные

                  Sprite3D  (tImageList*,int z=0);
                  Sprite3D  (SimpleSprite**,int,int bpp=1);
                  Sprite3D  (BinFile*,long);

             long  size ()       const;
    static   long  size (tImageList*);
    static   long  size (SimpleSprite**,int num,int bpp=1);

    ulong*        offs ()       const { return (ulong*)data; }
    SimpleSprite* operator [] (int i)  const
               { return i<num ? (SimpleSprite*)(data+offs()[i]) :
                        (num==1  ? (SimpleSprite*)(data+offs()[0]) :
                                   (SimpleSprite*)(data+offs()[i & 1]));}

    void*         operator           new        (size_t,size_t);
    void*         operator           new        (size_t,void* x) { return x; }
};

/////////////////////////////////////////////////////////////////////////////
////Игровой спрайт имеющий несколько фаз движения
/////////////////////////////////////////////////////////////////////////////
class  GameSprite3D
{
  friend GameSprite3D* makeGameSprite3D (BinFile*);
  public:
    int           num;                  //кол-во фаз движения
    uchar         data[1];              //Данные

                  GameSprite3D (Sprite3D** array,int num);
                  GameSprite3D (BinFile*,long);

           long   size ()   const;
    static long   size (Sprite3D**,int num);

    ulong*        offs ()   const  { return (ulong*)data; }
    Sprite3D*     operator [] (int i)  const   { return (Sprite3D*)(data+offs()[i]); }

    void*         operator           new        (size_t,size_t);
    void*         operator           new        (size_t,void* x) { return x; }

/////////////////////////////////////////////////////////////////////////////
////Сохранение существующего спрайта в файл
/////////////////////////////////////////////////////////////////////////////
          void    Save (BinFile*);
          void    Save (const char*);
};

////////////////////////////////////////////////////////////////////////////
////Структура необходимая для отрисовки видимого спрайта
////////////////////////////////////////////////////////////////////////////
struct  VisSprite
{
        Fixed           dist;
        int             id, face, angle;
        int             xstart;
        Block*          block;

        void*           operator new (size_t,void* x) { return x; }
        void*           operator new (size_t x)       { return ::operator new(x); }
};

////////////////////////////////////////////////////////////////////////////
////Класс собирающий и сортирующий видимые спрайты
////////////////////////////////////////////////////////////////////////////
class   SpriteList
{
  public:
                SpriteList (int max_count = 0xFF);
    virtual     ~SpriteList();

/////////////////////////////////////////////////////////////////////////////
////Система сравнения и сортировки спрайтов
/////////////////////////////////////////////////////////////////////////////
    static int          CmpSprites  (const void*,const void*);
           void         SortSprites ();

           void         Reset () { count = -1; }

           VisSprite**  GetList()  const  { return node;  }
           int          GetCount() const  { return count + 1; } //-1 protection
           int          GetMaxCount() const  { return maxCount; }
           void         SetMaxCount(int count);

/////////////////////////////////////////////////////////////////////////////
////Система встаки, взятия спрайто из списка
/////////////////////////////////////////////////////////////////////////////
           BOOL         PutSprite (VisSprite*);
           VisSprite*   GetSprite ();
           BOOL         IsEmpty   ()        const   { return count>0;    }

  protected:
                VisSprite**     node;   //array of visible sprites
                int             count,  //count of visible sprites
                                maxCount;
};

/////////////////////////////////////////////////////////////////////////////
////Класс - менеджер для работы со спрайтами
/////////////////////////////////////////////////////////////////////////////
////Параметр del указывает должен ли класс удалять члены при собств. удалении
/////////////////////////////////////////////////////////////////////////////
class  RenderSprite;
class  SpriteCache;
class  SpriteSystem
{
        public:
                        SpriteSystem (RenderSprite*,SpriteList*,BOOL del = TRUE);
              virtual   ~SpriteSystem ();

           void         Reset    (Fixed viewx,Fixed viewy,Angle viewAngle);
           void         Reset    ();
           void         DrawView ();

/////////////////////////////////////////////////////////////////////////////
////Преобразование к делегирующим системам
/////////////////////////////////////////////////////////////////////////////
           RenderSprite*         RenderPtr () const { return render; }
           SpriteList*           ListPtr   () const { return list;   }

           operator RenderSprite* ()          const { return RenderPtr(); }
           operator SpriteList*   ()          const { return ListPtr();   }

           void                  SetCache (SpriteCache* x);

/////////////////////////////////////////////////////////////////////////////
////Проeктирование спрайта на кaртинную плоскость
/////////////////////////////////////////////////////////////////////////////
           VisSprite*            ProjectSprite (Fixed  spritex,
                                                Fixed  spritey,
                                                Block* = NULL) const;

           void                  ProcessSprite (Fixed,Fixed,Angle,Block* = NULL);


        protected:
                RenderSprite*   render;
                SpriteList*     list;
                VisSprite*      sprites;

                BOOL            deleteAccess;

                Fixed           vangleFx;
                Fixed           viewsin,
                                viewcos;
                Fixed           viewx,
                                viewy;
                Fixed           proj_x;
};

/////////////////////////////////////////////////////////////////////////////
////Класс для отрисовки списка видимых спрайтов (back-to-front)
/////////////////////////////////////////////////////////////////////////////
////Параметры: фрейм - экран и z-buffer
/////////////////////////////////////////////////////////////////////////////
class   RenderSprite
{
  public:
                RenderSprite (Screen* dest,SpriteCache*,Fixed* zbuf);
        virtual ~RenderSprite();

/////////////////////////////////////////////////////////////////////////////
////Отрисовка картинной плоскости
/////////////////////////////////////////////////////////////////////////////
                void    DrawSprites (SpriteList* list);
                void    DrawView    (SpriteList* list)
                        { DrawSprites (list); }

/////////////////////////////////////////////////////////////////////////////
////Параметры
/////////////////////////////////////////////////////////////////////////////
                enum Param {
                     FOCUS = 0,
                     VIEW_ANGLE,
                     WALL_HEIGHT
                };

        virtual void    SetParam(Param,double);
        virtual double  GetParam(Param);
        virtual void    Reset();

                void    SetCache (SpriteCache* x) { cache = x; }

/////////////////////////////////////////////////////////////////////////////
////Преобразования к членам
/////////////////////////////////////////////////////////////////////////////
                Screen* ScreenPtr () const { return screen; }
                Fixed*  ZBufferPtr() const { return zbuffer; }

  protected:
      virtual   void    DrawSprite  (VisSprite&) = 0;

  protected:
        Fixed*          zbuffer;
        Screen*         screen;
        SpriteCache*    cache;

        int             screenWidth,
                        screenHeight;

        Fixed*          xscale;         //Дробный шаг по оси X
        Fixed*          yscale;         //Дробный шаг по оси Y

        uint            distShift;
        uint            distMask;

        float           focus;          //focus distance
        float           viewAngle;      //view angle
        float           wallHeight;     //coef for wall scale
};

class   RenderSprite8bit: public RenderSprite
{
  public:
          RenderSprite8bit (fScreen8bit* scr,SpriteCache*,Fixed*,int = -1);

  protected:
          RenderSprite8bit(Screen* scr,SpriteCache* _cache,Fixed* z):RenderSprite(scr,_cache,z) { }

      virtual   void    DrawSprite  (VisSprite&) ;
      virtual   void    Reset       ();

  protected:
        uchar*          frame;     //frame buffer
        int             centerY;

};

/////////////////////////////////////////////////////////////////////////////
////Класс кэш спрайтов
/////////////////////////////////////////////////////////////////////////////
////Внимание: размер спрайтов фиксированный
/////////////////////////////////////////////////////////////////////////////
class   SpriteCache: public NameCache
{
  friend class    SpriteSystem;
  public:
                  SpriteCache  (char* names[],
                              long id_num,
                              long cache_sz,
                              long cl_sz);
                  SpriteCache  (BinFile* file,long cache_sz=1);
                  SpriteCache  (const char* name,long cache_sz=1);
    virtual       ~SpriteCache () { Close (); }

          virtual void          Close       ();

                  Sprite3D*     GetData (int id,int face);
                  void    *     GetData (int id) { return Cache::GetData(id); }

  protected:
                  int*        idOffs;   //смещения отн. начала таблицы
                  int*        index;    //номер спрайта

  protected:
          virtual void          LoadData    (long id,CacheList* dest);
                  int           Research (const char*);
                  void          Recalc();

};

#endif