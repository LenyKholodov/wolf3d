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
////��६���� ��� ���ᮢ�� ⥪����
/////////////////////////////////////////////////////////////////////////////
#pragma pack(1)
struct  TextParam
{
        Fixed           dist;           //�����ﭨ� �� ��ப� �� �⥭�
        uint            index;          //������ �⥭� �� ����
        Side            side;           //��஭� ᢥ�
        uint            id;             //�����䨪��� ⥪�����
        uint            txtOffs;        //���饭�� �� ��砫� �⥭�
        uint            x;              //��࠭��� ᬥ饭��
};
#pragma pack(0)

/////////////////////////////////////////////////////////////////////////////
////���⠪�� ����� ����᪮�� ������
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
////��ᥪ�⥫� ��祩
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

           int                  textShift;      //��� ���ᮢ�� ⥪�����

           BOOL                 heightFlag,     //��⨢ ������� ����⮢
                                widthFlag;

  static   TextParam            textureParam;   //���ᠭ�� ⥪����� ��� ᪮���

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
/////��⠭����
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
/////����஢�� ���
/////////////////////////////////////////////////////////////////////////////
          void          DrawView(Fixed x,Fixed y,Angle angle);
  inline  Fixed  *      GetZBuffer() { return zbuffer; }
          void          ClearZBuffer(Fixed   value);
};

/////////////////////////////////////////////////////////////////////////////
/////���⥩訩 ७��� �⥭ �業�
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
/////����� ��� ���ᮢ�� �⥭ � �஢�� � ⥪���ࠬ� ⨯� Mip-Map
/////////////////////////////////////////////////////////////////////////////
class   StageRender: public RayCast
{
  public:
                        StageRender (Screen*,BOOL autoreset = TRUE);
     virtual            ~StageRender();

/////////////////////////////////////////////////////////////////////////////
/////��⠭����
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
/////����� �ਣ���� ��� ���ᮢ�� �⥭ � �஢�� (8-bit)
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
/////����� �ਣ���� ��� ���ᮢ�� �⥭ � �஢�� � ���⥩訬 �ᢥ饭���
/////////////////////////////////////////////////////////////////////////////
class   RayCastShadow: public RayCast8bit
{
  public:
                        RayCastShadow(fScreen8bit*,BOOL autoreset = TRUE);
  protected:
   virtual void  DrawLine();
};

/////////////////////////////////////////////////////////////////////////////
/////�ᢥ饭�� ⨯� Light-Map
/////////////////////////////////////////////////////////////////////////////
class   RayCastLightMap: public RayCast8bit
{
  public:
          RayCastLightMap(fScreen8bit*,BOOL autoreset = TRUE);
          virtual ~RayCastLightMap();

/////////////////////////////////////////////////////////////////////////////
/////��⠭����
/////////////////////////////////////////////////////////////////////////////
    virtual  void      SetLightMap (LightMap* x) { lightMap = x; }

  protected:
   virtual void  DrawLine();
  protected:
          LightMap*     lightMap;       //light map
};

/////////////////////////////////////////////////////////////////////////////
/////����� �ਣ���� ��� ���ᮢ�� �⥭ � �஢�� (16-bit)
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
/////������ ����� ��� ��� ������
///////////////////////////////////////////////////////////////////////////////
class   Plane: public Engine
{
  public:
                                Plane   (fScreen8bit*,BOOL autoreset = TRUE);

          virtual void          SetTexture (MipMapText*);
          virtual void          SetColor   (int) {}

/////////////////////////////////////////////////////////////////////////////
/////��⠭����
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
         MipMapText*  mmt;         //�������

         int          screenWidth,  //��ਭ� �࠭�
                      screenHeight; //���� �࠭�
         ulong        height;      //����

         float        viewAngle;    //���� �����
};

/////////////////////////////////////////////////////////////////////////////
/////����� ���⥩襣� ७��ਭ�� ������
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
/////����� ���ᮢ뢠�騩 ��� (�ᯮ���� ��ࠬ��� ⥪����)
/////128x128x1
/////////////////////////////////////////////////////////////////////////////
class   Floor: public Plane
{
  protected:
           friend class         Render;

           int          halfWidth,    //�������� ����� �࠭� ��� ���ᮢ��
                        quartWidth;   //��⢥��� ����� �࠭� ��� ���ᮢ��

           Fixed  *     distTable;   //������ ����ﭨ�
           uint*        scaleTable;
           Fixed        *CSin,       //������ �����樥�⮢
                        *CCos;

           double       h,            //��ࠬ����
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
/////��ᮢ����
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
/////��⮫��
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
/////����
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
/////�ந������ ७��ਭ� �業�
/////////////////////////////////////////////////////////////////////////////
class   Render: public Engine
{
  private:
           Screen*        screen;  //��࠭ �� ����� ��ந��� ����ࠦ����
//           Screen*        frame_screen;
           Palette*       game_palette;  //��������� ��஢�� ������
           int            renderType;  //���䨣����

           Plane*         sky;       //��⮫��
           Plane*         floor;     //���
           RayCast*       stage;     //�⥭�

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
/////����� � ��������
/////////////////////////////////////////////////////////////////////////////
          RayCast*      StagePtr() const { return stage; }
          Plane*        FloorPtr() const { return floor; }
          Plane*        SkyPtr()   const { return sky; }
          Screen*       FrameScreen() { return screen; }

/////////////////////////////////////////////////////////////////////////////
/////��⠭���� (�믮������� ��� �맮�� Reset)
/////////////////////////////////////////////////////////////////////////////
  virtual BOOL          SetParam(int,double  value);
  virtual double        GetParam(int id);
  virtual void          Reset();

/////////////////////////////////////////////////////////////////////////////
/////����� � �����
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
/////���ᮢ��
/////////////////////////////////////////////////////////////////////////////
  virtual void          DrawView(Fixed,Fixed,Angle);
};

#endif