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
////��ࠬ���� ��� ᮧ����� �ࠩ⮢ �� �⨫��
/////////////////////////////////////////////////////////////////////////////
const   int       MAX_SPRITE_FACES   = 512;
//const   int       SPRITE_SIZE        = 500000;//256*256*1*8*4;
const   int       MAX_SPRITES        = 256;
extern  int       SPRITE_SIZE;

/////////////////////////////////////////////////////////////////////////////
////Make-functions ��� ᮧ����� ��� ⨯�� ��ꥪ⮢ � ������騬 ࠧ��஬
/////////////////////////////////////////////////////////////////////////////

SimpleSprite*  makeSimpleSprite ( tImage*,int z=0,int bpp=1 );

Sprite3D*      makeSprite3D     ( tImageList*,int z=0 );
Sprite3D*      makeSprite3D     ( SimpleSprite**,int num,int bpp=1);

GameSprite3D*  makeGameSprite3D ( Sprite3D**,int num);
GameSprite3D*  makeGameSprite3D ( BinFile*);
GameSprite3D*  makeGameSprite3D ( const char*);

/////////////////////////////////////////////////////////////////////////////
////��������� 䠩�� ��� ��࠭���� �ࠩ⮢
/////////////////////////////////////////////////////////////////////////////
struct  WSFHeader
{
  uchar         id[4];          //"WSF "
  uint          num;            //�������⢮ 䠧
  ulong         total_size;     //��騩 ࠧ��� �ᥣ� ��஢��� �ࠩ�
  uchar         reserv[8];      //��१�ࢨ஢���
};

/////////////////////////////////////////////////////////////////////////////
////���᪮��� �ࠩ�
/////////////////////////////////////////////////////////////////////////////
class  SimpleSprite
{
  public:
    int           width;        //��ਭ�
    int           height;       //����
    int           z_color;      //���� �஧�筮��
    uchar         data[1];      //�����

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
////���� �������� 3D �ࠩ� (n-���ࠢ�����)
/////////////////////////////////////////////////////////////////////////////
class  Sprite3D
{
  public:
    int           num;                  //���-�� ���ࠢ����� �����
    int           bpp;                  //Bytes Per Pixel
    uchar         data[1];              //�����

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
////��஢�� �ࠩ� ����騩 ��᪮�쪮 䠧 ��������
/////////////////////////////////////////////////////////////////////////////
class  GameSprite3D
{
  friend GameSprite3D* makeGameSprite3D (BinFile*);
  public:
    int           num;                  //���-�� 䠧 ��������
    uchar         data[1];              //�����

                  GameSprite3D (Sprite3D** array,int num);
                  GameSprite3D (BinFile*,long);

           long   size ()   const;
    static long   size (Sprite3D**,int num);

    ulong*        offs ()   const  { return (ulong*)data; }
    Sprite3D*     operator [] (int i)  const   { return (Sprite3D*)(data+offs()[i]); }

    void*         operator           new        (size_t,size_t);
    void*         operator           new        (size_t,void* x) { return x; }

/////////////////////////////////////////////////////////////////////////////
////���࠭���� �������饣� �ࠩ� � 䠩�
/////////////////////////////////////////////////////////////////////////////
          void    Save (BinFile*);
          void    Save (const char*);
};

////////////////////////////////////////////////////////////////////////////
////������� ����室���� ��� ���ᮢ�� �������� �ࠩ�
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
////����� ᮡ���騩 � ������騩 ������ �ࠩ��
////////////////////////////////////////////////////////////////////////////
class   SpriteList
{
  public:
                SpriteList (int max_count = 0xFF);
    virtual     ~SpriteList();

/////////////////////////////////////////////////////////////////////////////
////���⥬� �ࠢ����� � ���஢�� �ࠩ⮢
/////////////////////////////////////////////////////////////////////////////
    static int          CmpSprites  (const void*,const void*);
           void         SortSprites ();

           void         Reset () { count = -1; }

           VisSprite**  GetList()  const  { return node;  }
           int          GetCount() const  { return count + 1; } //-1 protection
           int          GetMaxCount() const  { return maxCount; }
           void         SetMaxCount(int count);

/////////////////////////////////////////////////////////////////////////////
////���⥬� ��⠪�, ����� �ࠩ� �� ᯨ᪠
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
////����� - �������� ��� ࠡ��� � �ࠩ⠬�
/////////////////////////////////////////////////////////////////////////////
////��ࠬ��� del 㪠�뢠�� ������ �� ����� 㤠���� 童�� �� ᮡ��. 㤠�����
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
////�८�ࠧ������ � ���������騬 ��⥬��
/////////////////////////////////////////////////////////////////////////////
           RenderSprite*         RenderPtr () const { return render; }
           SpriteList*           ListPtr   () const { return list;   }

           operator RenderSprite* ()          const { return RenderPtr(); }
           operator SpriteList*   ()          const { return ListPtr();   }

           void                  SetCache (SpriteCache* x);

/////////////////////////////////////////////////////////////////////////////
////��e��஢���� �ࠩ� �� �a�⨭��� ���᪮���
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
////����� ��� ���ᮢ�� ᯨ᪠ ������� �ࠩ⮢ (back-to-front)
/////////////////////////////////////////////////////////////////////////////
////��ࠬ����: �३� - �࠭ � z-buffer
/////////////////////////////////////////////////////////////////////////////
class   RenderSprite
{
  public:
                RenderSprite (Screen* dest,SpriteCache*,Fixed* zbuf);
        virtual ~RenderSprite();

/////////////////////////////////////////////////////////////////////////////
////���ᮢ�� ���⨭��� ���᪮��
/////////////////////////////////////////////////////////////////////////////
                void    DrawSprites (SpriteList* list);
                void    DrawView    (SpriteList* list)
                        { DrawSprites (list); }

/////////////////////////////////////////////////////////////////////////////
////��ࠬ����
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
////�८�ࠧ������ � 童���
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

        Fixed*          xscale;         //�஡�� 蠣 �� �� X
        Fixed*          yscale;         //�஡�� 蠣 �� �� Y

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
////����� ��� �ࠩ⮢
/////////////////////////////////////////////////////////////////////////////
////��������: ࠧ��� �ࠩ⮢ 䨪�஢����
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
                  int*        idOffs;   //ᬥ饭�� ��. ��砫� ⠡����
                  int*        index;    //����� �ࠩ�

  protected:
          virtual void          LoadData    (long id,CacheList* dest);
                  int           Research (const char*);
                  void          Recalc();

};

#endif