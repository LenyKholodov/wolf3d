BOOL    msScreen::InRect(int x,int y) const
{
  return msRect.in(x,y);
}

BOOL    msScreen::InRect(cRect rect) const
{
  return InRect(rect.left,rect.top) || InRect(rect.left,rect.bottom) ||
         InRect(rect.right,rect.bottom) || InRect(rect.right,rect.top) ||
         rect.in(msRect.left,msRect.top) || rect.in(msRect.left,msRect.bottom) ||
         rect.in(msRect.right,msRect.bottom) || rect.in(msRect.right,msRect.top);
}

BOOL    msScreen::InRect(int left,int top,int right,int bottom) const
{
  return InRect(left,top,right,bottom);
}


BOOL msScreen::SetCursor(tSprite* sprite)
{
  if (sprite == NULL)
     return FALSE;

  if (store)
     free(store);

  if (icon)
     free(icon);

//  HideMouse();
  iconWidth   = sprite->width;
  iconHeight  = sprite->height;
  icon        = sprite;

  cout <<"Set icon: width = "<<iconWidth<<" height = "<<iconHeight<<endl;

  store = (tImage*)malloc(iconWidth*iconHeight*mScr->bpp()+sizeof(tImage)-1);

//    msRect = cRect(0,0,iconWidth,iconHeight);

  return TRUE;
}

BOOL    msScreen::SetCursor(BinFile* file)
{
  tSpriteList*      sl = tSpriteList::load(file);
  if (sl == NULL)
     return FALSE;
//  ShiftPalette
  mScr->PutPalette((RGBStruct*)sl->GetPalette());
  return SetCursor(sl->GetImage(0));
}

BOOL    msScreen::SetCursor(const char* name)
{
  tSpriteList*      sl = tSpriteList::load(name);
  if (sl == NULL)
     return FALSE;
//  ShiftPalette
  mScr->PutPalette((RGBStruct*)sl->GetPalette());
  return SetCursor(sl->GetImage(0));
}

void   _loadds   msScreen::_hidemouse()
{
//  mScr->PutImage(oldX,oldY,store);              //Hide mouse cursor
}

void   _loadds   msScreen::_showmouse(int x,int y)
{
//  mScr->GetImage(oldX=x,oldY=y,iconWidth,iconHeight,store);
  mScr->PutSprite(x,y,icon);
}

//#pragma off (check_stack)
void _loadds far msScreen::handler (int max, int mcx, int mdx)
{
#pragma aux handler parm [EAX] [ECX] [EDX]
//  if (!visible)
//     return;

//  mScr->PutImage(oldX,oldY,store);              //Hide mouse cursor

//  mScr->GetImage(oldX=mcx,oldY=mdx,iconWidth,iconHeight,store);
  mScr->PutSprite(mcx,mdx,icon);
}
//#pragma on (check_stack)

int        msScreen::GetX() const       { return  mouse->GetScanX(); }
int        msScreen::GetY() const       { return  mouse->GetScanY(); }
RGBStruct* msScreen::GetPalette() const {  return palette; }

void    msScreen::MoveTo(int x,int y)
{
  HideMouse();
//  mouse->MoveTo(x+scrRect.left,y+scrRect.top);
  ShowMouse();
}

void    msScreen::ShowMouse()
{
  if (!visible)
  {
    _showmouse(GetX(),GetY());
    visible = TRUE;
  }
}
void    msScreen::HideMouse()
{
  if (visible)
  {
    _hidemouse();
    visible = FALSE;
  }
}

void    msScreen::ShowMouse(BOOL state)
{
  state ? ShowMouse() : HideMouse();
}

BOOL    msScreen::open(int x0,int y0,int w,int h,Screen* dest)
{
  mScr = dest;
//  SetBoundRect(x0,y0,x0+w,y0+h);
  mouse->SetHandler((Mouse::Event)2,FP_SEG(handler),FP_OFF(handler));
//  HideMouse();
  return TRUE;
}

void    msScreen::SetBoundRect(cRect rect)
{
  HideMouse();
  mouse->SetBoundRect(rect.left,rect.top,rect.right,rect.bottom);
//  scrRect=rect;
  MoveTo((rect.right-rect.left)/2,(rect.bottom-rect.top)/2);
}

void    msScreen::SetBoundRect(int x1,int y1,int x2,int y2)
{
  SetBoundRect(cRect(x1,y1,x2,y2));
}

msScreen::msScreen()
{
  if (count++ == 0)
  {
     icon  = (tSprite*)malloc(100);
     store = (tImage*)malloc(100);
     mouse = new Mouse();
  }
}

msScreen::msScreen(int x0,int y0,int width,int height,Screen* dest)
{
  if (count++ == 0)
  {
    icon  = (tSprite*)malloc(100);
    store = (tImage*)malloc(100);
    mouse = new Mouse();
  }
  visible = FALSE;
  open (x0,y0,width,height,dest);
}

msScreen::~msScreen()
{
  if (--count == 0)
  {
    free(store);
    free(icon);
    delete mouse;
  }
}

/////////////////////////////////////////////////////////////////////////////
////Класс реализующий поддержку мыши в SVGA-режимах
/////////////////////////////////////////////////////////////////////////////
class msScreen
{
  public:
          msScreen();
          msScreen(int x0,int y0,int width,int height,Screen* dest);
          virtual ~msScreen();

          BOOL          open(int x0,int y0,int width,int height,Screen* dest);
          void          close();

          BOOL          SetCursor(tSprite*   spr);
          BOOL          SetCursor(BinFile*   file);
          BOOL          SetCursor(const char* path);

          RGBStruct*    GetPalette() const;

          void          ShowMouse();
          void          ShowMouse(BOOL state);
          void          HideMouse();

          int           GetX() const;
          int           GetY() const;
          void          MoveTo(int x,int y);
          void          SetBoundRect(cRect rect);
          void          SetBoundRect(int left,int top,int right,int bottom);

          Mouse*        MousePtr() const { return mouse; }

  private:
   static  tSprite   * icon;         //Mouse cursor image
   static  tImage    * store;        //Store of drawing
   static  RGBStruct * palette;      //Cursor palette
   static  Screen    * mScr;

   static  BOOL       visible;
   static  int        count;

   static  int        oldX,oldY;
   static  int        iconWidth,
                      iconHeight;

   static  Mouse*     mouse;

  protected:
     static  void _loadds far handler (int max, int mcx, int mdx);
     static  void             _hidemouse();
     static  void             _showmouse(int x,int y);
};

Mouse*          msScreen::mouse   = NULL;
tSprite*        msScreen::icon    = NULL;
tImage*         msScreen::store   = NULL;
RGBStruct*      msScreen::palette = NULL;
Screen*         msScreen::mScr    = NULL;
BOOL            msScreen::visible = FALSE;
int             msScreen::count   = 0;
int             msScreen::oldX    = 0,
                msScreen::oldY    = 0;
int             msScreen::iconWidth =0,
                msScreen::iconHeight =0;