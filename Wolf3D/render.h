#ifndef __RENDER__
#define __RENDER__

#include <fscreen.h>

#include "tables.h"
#include "map.h"
#include "palette.h"
#include "light.h"
#include "textures.h"
#include "sprite.h"
#include "defs.h"

        //Colors

const int DOOR_COLOR    = Palette::rgb(0,255,0);
const int WALL_V_COLOR  = Palette::rgb(0,0,255);
const int WALL_H_COLOR  = Palette::rgb(255,255,0);
const int STAGE_COLOR   = Palette::rgb(0,255,255);
const int CEILING_COLOR = Palette::rgb(10,10,10);
const int FLOOR_COLOR   = Palette::rgb(32,32,63);
const int SKY_COLOR     = Palette::rgb(0,64,255);

class   Render;

/////////////////////////////////////////////////////////////////////////////
////Пареметры для отрисовки текстур
/////////////////////////////////////////////////////////////////////////////
#pragma pack(1)
struct  TextParam
{
        Fixed           dist;           //Расстояние от игрока до стены
        uint            index;          //Индекс стены на карте
        Side            side;           //Сторона света
        uint            id;             //Идентификатор текстуры
        uint            txtOffs;        //Смещение от начала стены
        uint            x;              //Экранное смещение
};
#pragma pack(0)

/////////////////////////////////////////////////////////////////////////////
////Абстактный класс графического движка
/////////////////////////////////////////////////////////////////////////////
class   Engine
{
  public:
                                Engine   () {}
                                Engine   (fScreen8bit*,EngineParam* = NULL) {}
          virtual               ~Engine  () {}

          virtual void          Reset     ()                = 0;
          virtual BOOL          SetParam(int,double)        = 0;
          virtual double        GetParam(int)               = 0;


          virtual void          DrawView (Fixed,Fixed,Angle)= 0;
};

/////////////////////////////////////////////////////////////////////////////
////Отсекатель лучей
/////////////////////////////////////////////////////////////////////////////
class   RayCast: public Engine
{
  protected:
           friend class         Render;

           Screen*              screen;         //frame screen
           Fixed  *             zbuffer;        //z-buffer
           SpriteSystem*        spriteSystem;   //list of visible sprites
           int                  screenWidth,    //Clip view width
                                screenHeight;   //Clip view height
           Angle*               rayAngle;       //Ray angles table

           Fixed                out_of_map;     //Out of map ray casting flag
           Map*                 map;            //Stage map

           int                  *h1,            //Wall height begin
                                *h2;            //Wall height end
           int                  *txtOffs;       //Textures offset
           Fixed*               dyTable;        //For drawing VText

           int                  shift;          //Shift for wall height index
           int                  wallMask;       //Mask for table bound (debug)
           int                  wallFeel;       //Feel for draw wall
           Angle                detailAngle;

           double               viewAngle;      //View angle (default = 60 deg)
           double               focus,          //Focus (default = 0.5)
                                wallHeight,     //Wall height (default = 3.0)
                                detailQ,        //detail angle
                                textHeight;     //height of textures

           int                  textShift;      //Для отрисовки текстуры

           BOOL                 heightFlag,     //Против двойных расчётов
                                widthFlag;

  static   TextParam            textureParam;   //Описание текстуры для скорости

  protected:
    virtual  void               DrawLine    ()    = 0;
    static   int                GetShift    (int width);

  public:
          enum Params{
               VIEW_ANGLE,
               FOCUS,
               WALL_HEIGHT,
               WALL_FEEL,
               DETAIL_ANGLE,
               MAX_SPRITES,
               TEXT_WIDTH,
               TEXT_HEIGHT
          };

          RayCast(Screen* video,BOOL autoreset = TRUE);
          RayCast(int w,int h,BOOL autoreset = TRUE);
          virtual ~RayCast();

/////////////////////////////////////////////////////////////////////////////
/////Установки
/////////////////////////////////////////////////////////////////////////////
  virtual void          SetMap(Map* m) { map = m; }
  virtual void          SetCache (SpriteCache* x)
                        { if (spriteSystem) spriteSystem->SetCache(x); }
  virtual void          SetLightMap (LightMap*) {}
  virtual void          SetTexture(void*) = 0;
  virtual BOOL          SetParam(int,double value);
  virtual double        GetParam(int);

  virtual void          Reset();

/////////////////////////////////////////////////////////////////////////////
/////Трассировка луча
/////////////////////////////////////////////////////////////////////////////
          void          DrawView(Fixed x,Fixed y,Angle angle);
  inline  Fixed  *      GetZBuffer() { return zbuffer; }
          void          ClearZBuffer(Fixed   value);
};

/////////////////////////////////////////////////////////////////////////////
/////Простейший рендер стен сцены
/////////////////////////////////////////////////////////////////////////////
class   SimpleStageRender: public RayCast
{
  public:
                        SimpleStageRender (fScreen8bit*,BOOL autoreset = TRUE);

  virtual void          SetTexture(void*) {}

  protected:
             uchar*     frame;  //frame buffer
  protected:
    virtual  void       DrawLine();

};

/////////////////////////////////////////////////////////////////////////////
/////Класс для отрисовки стен в уровне с текстурами типа Mip-Map
/////////////////////////////////////////////////////////////////////////////
class   StageRender: public RayCast
{
  public:
                        StageRender (Screen*,BOOL autoreset = TRUE);
     virtual            ~StageRender();

/////////////////////////////////////////////////////////////////////////////
/////Установки
/////////////////////////////////////////////////////////////////////////////
     virtual   void     Reset();
     virtual   void     SetTexture(void*);

               MMT*     GetTexture(int);

  protected:
           int*                 scaleTable;     //Table fo mip-map
           TextCache*           wtf;            //Textures list

           MMT*                 curTexture;
           int                  curIndex;

           BOOL                 textFlag;
};

/////////////////////////////////////////////////////////////////////////////
/////Класс пригодный для отрисовки стен в уровне (8-bit)
/////////////////////////////////////////////////////////////////////////////
class   RayCast8bit: public StageRender
{
  public:
                        RayCast8bit (fScreen8bit*,BOOL autoreset = TRUE);
  protected:
   virtual void         DrawLine    ();

                        RayCast8bit (Screen*);
  protected:
    uchar*              frame;          //frame buffer
};

/////////////////////////////////////////////////////////////////////////////
/////Класс пригодный для отрисовки стен в уровне с простейшим освещением
/////////////////////////////////////////////////////////////////////////////
class   RayCastShadow: public RayCast8bit
{
  public:
                        RayCastShadow(fScreen8bit*,BOOL autoreset = TRUE);
  protected:
   virtual void  DrawLine();
};

/////////////////////////////////////////////////////////////////////////////
/////Освещение типа Light-Map
/////////////////////////////////////////////////////////////////////////////
class   RayCastLightMap: public RayCast8bit
{
  public:
          RayCastLightMap(fScreen8bit*,BOOL autoreset = TRUE);
          virtual ~RayCastLightMap();

/////////////////////////////////////////////////////////////////////////////
/////Установки
/////////////////////////////////////////////////////////////////////////////
    virtual  void      SetLightMap (LightMap* x) { lightMap = x; }

  protected:
   virtual void  DrawLine();
  protected:
          LightMap*     lightMap;       //light map
};

/////////////////////////////////////////////////////////////////////////////
/////Класс пригодный для отрисовки стен в уровне (16-bit)
/////////////////////////////////////////////////////////////////////////////
class   RayCast16bit: public StageRender
{
  public:
                        RayCast16bit (fScreen16bit*,BOOL autoreset = TRUE);
  protected:
   virtual void         DrawLine    ();

                        RayCast16bit (Screen*);
  protected:
    uint16*             frame;          //frame buffer
};

/////////////////////////////////////////////////////////////////////////////
/////Базовый класс для всех планов
///////////////////////////////////////////////////////////////////////////////
class   Plane: public Engine
{
  public:
                                Plane   (fScreen8bit*,BOOL autoreset = TRUE);

          virtual void          SetTexture (MipMapText*);
          virtual void          SetColor   (int) {}

/////////////////////////////////////////////////////////////////////////////
/////Установки
/////////////////////////////////////////////////////////////////////////////
          enum Params
          {
            H   = 0,
            D0,
            C,
            DELTA,
            FIRST_LINE,                 //for floor
            LAST_LINE,                  //for sky
            VIEW_ANGLE,
            WALL_HEIGHT
          };

  protected:
         fScreen8bit* screen;
         uchar*       frame;
         MipMapText*  mmt;         //Текстура

         int          screenWidth,  //Ширина экрана
                      screenHeight; //Высота экрана
         ulong        height;      //Высота

         float        viewAngle;    //Угол обзора
};

/////////////////////////////////////////////////////////////////////////////
/////Класс простейшего рендеринга планов
/////////////////////////////////////////////////////////////////////////////
class   SimplePlane: public Plane
{
  public:
                    SimplePlane   (fScreen8bit*,BOOL autoreset = TRUE);

  virtual  void      DrawView(Fixed,Fixed,Angle);
  virtual  BOOL      SetParam(int,double value);
  virtual  double    GetParam(int);
  virtual  void      Reset();

  virtual  void      SetColor   (int col) { color = col; }

  protected:
             int        color;

};

/////////////////////////////////////////////////////////////////////////////
/////Класс отрисовывающий пол (использует пирамиду текстур)
/////128x128x1
/////////////////////////////////////////////////////////////////////////////
class   Floor: public Plane
{
  protected:
           friend class         Render;

           int          halfWidth,    //Половина высоты экрана для прорисовки
                        quartWidth;   //Четверть высоты экрана для прорисовки

           Fixed  *     distTable;   //Таблица расстояний
           uint*        scaleTable;
           Fixed        *CSin,       //Таблица коэффициентов
                        *CCos;

           double       h,            //Параметры
                        d0,
                        delta,
                        c,
                        wallHeight;

           BOOL         angleFlag,
                        heightFlag;

  protected:
  public:

          Floor(fScreen8bit* screen,BOOL autoreset = TRUE);
          virtual ~Floor();

 virtual  void          SetTexture(MipMapText* text);
 virtual  BOOL          SetParam(int,double value);
 virtual  double        GetParam(int);
 virtual  void          Reset();

/////////////////////////////////////////////////////////////////////////////
/////Рисование
/////////////////////////////////////////////////////////////////////////////

 virtual  void          DrawView(Fixed   x,Fixed   y,Angle angle);
};

class   FloorShadow: public Floor
{
  public:
                              FloorShadow(fScreen8bit*,BOOL autoreset = TRUE);
                              virtual ~FloorShadow();

   virtual      void          DrawView(Fixed x,Fixed y,Angle angle);
   virtual      void          Reset();
   virtual      BOOL          SetParam(int,double value);

  protected:
                uchar*        lightTable;
};

/////////////////////////////////////////////////////////////////////////////
/////Потолок
/////////////////////////////////////////////////////////////////////////////
class   FloorUp:public Floor
{
  public:
                FloorUp (fScreen8bit*,BOOL autoreset = TRUE);

   virtual      BOOL          SetParam(int,double value);
   virtual      double        GetParam(int param);
   virtual      void          Reset();
};

class   FloorUpShadow:public FloorShadow
{
  public:
                FloorUpShadow(fScreen8bit*,BOOL autoreset = TRUE);

   virtual      BOOL          SetParam(int param,double x);
   virtual      double        GetParam(int param);
   virtual      void          Reset();

};

/////////////////////////////////////////////////////////////////////////////
/////Небо
/////////////////////////////////////////////////////////////////////////////
class   Sky:public Plane
{
  public:
                Sky (fScreen8bit*,BOOL autoreset = TRUE);

   virtual      BOOL          SetParam(int,double value);
   virtual      double        GetParam(int param);
   virtual      void          Reset();

   virtual      void          DrawView   (Fixed,Fixed,Angle);

  protected:

   Fixed        ystep, xstep;
};

/////////////////////////////////////////////////////////////////////////////
/////Производит рендеринг сцены
/////////////////////////////////////////////////////////////////////////////
class   Render: public Engine
{
  private:
           Screen*        screen;  //Экран на который строится изображение
//           Screen*        frame_screen;
           Palette*       game_palette;  //Глобальную игровая палитра
           int            renderType;  //Конфигурация

           Plane*         sky;       //Потолок
           Plane*         floor;     //Пол
           RayCast*       stage;     //Стены

  protected:
  public:
          Render (Screen*,int type = DEFAULT_RENDER,BOOL autoreset = TRUE);
          virtual ~Render();


          enum BufId{
               FRAME_BUFFER     = 1,
               SECOND_BUFFER    = 2,
               Z_BUFFER         = 4
          };

          enum Params{
               VIEW_ANGLE = 1,
               WALL_HEIGHT,
               FOCUS
          };

/////////////////////////////////////////////////////////////////////////////
/////Работа с движками
/////////////////////////////////////////////////////////////////////////////
          RayCast*      StagePtr() const { return stage; }
          Plane*        FloorPtr() const { return floor; }
          Plane*        SkyPtr()   const { return sky; }
          Screen*       FrameScreen() { return screen; }

/////////////////////////////////////////////////////////////////////////////
/////Установки (выполняются без вызова Reset)
/////////////////////////////////////////////////////////////////////////////
  virtual BOOL          SetParam(int,double  value);
  virtual double        GetParam(int id);
  virtual void          Reset();

/////////////////////////////////////////////////////////////////////////////
/////Работа с видео
/////////////////////////////////////////////////////////////////////////////
//          void          SwapBuffers() { frame_screen->refresh(); }
          void          ClearBuffer(int id,int    value);
          void          ClearBuffer(int id,Fixed  value);
          void          NormPalette();
          void          Close();
          BOOL          Open(Screen*,int type = DEFAULT_RENDER,BOOL autoreset = TRUE);

          int           RenderType () const { return renderType; }

/////////////////////////////////////////////////////////////////////////////
/////FPS
/////////////////////////////////////////////////////////////////////////////
          int           GetFPS(Fixed,Fixed,Angle);

/////////////////////////////////////////////////////////////////////////////
/////Отрисовка
/////////////////////////////////////////////////////////////////////////////
  virtual void          DrawView(Fixed,Fixed,Angle);
};

#endif