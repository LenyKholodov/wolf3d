#ifndef __GAME_SCREEN_INTERFACE__
#define __GAME_SCREEN_INTERFACE__

#include <screen.h>
#include "sprlist.h"

///////////////////////Элементы экранного интерфейса/////////////////////////

int     TransformCoord (int x_proc,int width);

enum Justify{
     TOP = 0,
     BOTTOM,
     LEFT = 0,
     RIGHT,
     CENTER
};

/////////////////////////////////////////////////////////////////////////////
////Абстрактный класс элемента экранного интерфейса
/////////////////////////////////////////////////////////////////////////////
class  FaceItem
{
  public:
                FaceItem (int _x,int _y,Justify __x = LEFT,Justify __y = TOP)
                         : x(_x), y(_y), xJust(__x), yJust (__y) {}
      virtual   ~FaceItem () {}

      virtual   void    Draw (Screen*) const = 0;
      virtual   void    Send (int)           = 0;
      virtual   void    Send (const char*)   = 0;

      virtual   void    Reload (int resIndex)= 0;
                void    MoveTo (int _x,int _y){ x=_x; y=_y;}

                int     getx   () const { return x;}
                int     gety   () const { return y;}

  protected:
     int        x,y;
     Justify    xJust,yJust;
};

/////////////////////////////////////////////////////////////////////////////
////Эелемент использующий кэширование
/////////////////////////////////////////////////////////////////////////////
class  FaceCache: public FaceItem
{
  public:
                FaceCache (int _x,int _y,Cache* _cache,int res_index,Justify __x = LEFT,Justify __y = TOP)
                         : FaceItem(_x,_y,__x,__y), cache(_cache), index(res_index) {}

      virtual   void    Draw (Screen*) const = 0;
      virtual   void    Send (int)           = 0;
      virtual   void    Send (const char*)   = 0;

      virtual   void    Reload (int resIndex) { index = resIndex; }

                void    SetRes (int x) { index = x; }
                int     GetRes () const { return index; }

  protected:
     Cache*     cache;
     int        index;
};

/////////////////////////////////////////////////////////////////////////////
/////Элемент для вывода текста
/////////////////////////////////////////////////////////////////////////////
class   FaceBinFont: public FaceItem
{
  public:
                FaceBinFont (int,int,tBinFont*,Justify=LEFT,Justify=TOP);

      virtual   void    Draw (Screen*) const      ;
      virtual   void    Send (int)         ;
      virtual   void    Send (const char*) ;

      virtual   void    Reload (int x) { color = x; }

  protected:
     tBinFont*          font;
     int                color;
     char               message_string [128];
};

/////////////////////////////////////////////////////////////////////////////
/////Элемент для вывода текста
/////////////////////////////////////////////////////////////////////////////
class   FaceBGIFont: public FaceItem
{
  public:
                FaceBGIFont (int,int,tBGIFont*,Justify=LEFT,Justify=TOP);

      virtual   void    Draw (Screen*) const      ;
      virtual   void    Send (int)         ;
      virtual   void    Send (const char*) ;

      virtual   void    Reload (int) {}

  protected:
     tBGIFont*          font;
     char               message_string [128];
};

/////////////////////////////////////////////////////////////////////////////
/////Элемент для вывода спрайта
/////////////////////////////////////////////////////////////////////////////
class   FaceSprite: public FaceCache
{
  public:
                        FaceSprite (int,int,SpriteListCache*,int,Justify=LEFT,Justify=TOP);

      virtual   void    Draw (Screen*) const      ;
      virtual   void    Send (int)         ;
      virtual   void    Send (const char*)  {}

  protected:
     int                pos;
};

/////////////////////////////////////////////////////////////////////////////
/////Плоскость для накладки интерфейсов
/////////////////////////////////////////////////////////////////////////////

struct  FaceNode
{
  FaceItem*     item;
  FaceNode*     next;
};

class   ScreenFace
{
  public:
                ScreenFace (Screen*);
     virtual    ~ScreenFace ();

        FaceNode*       AddFace (FaceItem*);
        void            Send    (FaceNode*,int);
        void            Send    (FaceNode*,const char*);
        void            DelFace (FaceNode*);

        void            Draw ();

  protected:
             Screen*    screen;
             FaceNode*  list;
};


#endif