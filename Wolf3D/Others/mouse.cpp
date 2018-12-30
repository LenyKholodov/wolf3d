#include <mouse.h>

#include <i86.h>
#include <dos.h>
#include <grtypes.h>

#include "debug.h"
#include <stdio.h>
#include <malloc.h>

static REGS    regs;
static SREGS   sregs;

//////////////////////Работа с интерфейсом 0x33/////////////////////////////

int Mouse::IsMouse()
{
  regs.w.ax  = INIT;

  int386(MouseInt,&regs,&regs);

  return regs.w.ax;
}

Mouse::Mouse()
{
  if (!IsMouse())
     D_Error("Mouse: Mouse not present")
  else
      ms_handler = new MouseMessage();
}

Mouse::~Mouse() { delete ms_handler; Reset(); }

void Mouse::HideMouse()
{
  regs.w.ax  = HIDE_MOUSE;

  int386(MouseInt,&regs,&regs);
}

void Mouse::ShowMouse()
{
  regs.w.ax   = SHOW_MOUSE;

  int386(MouseInt,&regs,&regs);
}

BOOL Mouse::IsDblClick(int dblclick)
{
        //Пока не нажата LEFT_BUTTON
  for (int k=0;k!=dblclick && !IsPress(LEFT_BUTTON);k++)
    delay(1);
  if (k==dblclick)
     return FALSE;

        //Пока нажата LEFT_BUTTON
  for(;k!=dblclick && IsPress(LEFT_BUTTON);k++)
    delay(1);
  if (k==dblclick)
     return FALSE;

        //Пока не нажата LEFT_BUTTON
  for (;k!=dblclick && !IsPress(LEFT_BUTTON);k++)
    delay(1);
  if (k==dblclick)
     return FALSE;

  return TRUE;
}

void Mouse::SetBoundRect(int left,int top,int right,int bottom)
{
  regs.w.ax  = MOVE_X;
  regs.w.cx  = left;
  regs.w.dx  = right;

  int386(MouseInt,&regs,&regs);

  regs.w.ax  = MOVE_Y;
  regs.w.cx  = top;
  regs.w.dx  = bottom;

  int386(MouseInt,&regs,&regs);
}

void Mouse::SetSpeed(int xs,int ys)
{
  regs.w.ax = SET_SPEED;
  regs.w.cx = xs;
  regs.w.dx = ys;

  int386(MouseInt,&regs,&regs);
}

void Mouse::SetHardText(int first,int last)
{
  regs.w.ax = TEXT_PARAM;
  regs.w.bx = HARD_TEXT_CURSOR;
  regs.w.cx = first;
  regs.w.dx = last;

  int386(MouseInt,&regs,&regs);
}

void Mouse::SetSoftText(int scr,int cursor)
{
  regs.w.ax = TEXT_PARAM;
  regs.w.bx = SOFT_TEXT_CURSOR;
  regs.w.cx = scr;
  regs.w.dx = cursor;

  int386(MouseInt,&regs,&regs);
}

void Mouse::MoveTo(int x,int y)
{
  regs.w.ax = MOVE_TO;
  regs.w.cx = x;
  regs.w.dx = y;

  int386(MouseInt,&regs,&regs);
}

void Mouse::GetPos(int& x,int& y)
{
  regs.w.ax = GET_STATE;

  int386(MouseInt,&regs,&regs);

  x = regs.w.cx;
  y = regs.w.dx;
}

Mouse::Buttons Mouse::GetButton()
{
  regs.w.ax = GET_STATE;

  int386(MouseInt,&regs,&regs);

  return *(Buttons*)&regs.w.bx;
}

BOOL Mouse::IsPress(Mouse::Button button)
{
  Buttons but = GetButton();

  if (button == LEFT_BUTTON)   return but.left;
  if (button == RIGHT_BUTTON)  return but.right;
  if (button == CENTER_BUTTON) return but.center;
  if (button == ANY_BUTTON)    return *(BOOL*)&but;

  return FALSE;
}

void Mouse::SetDoubleSpeed(int speed)
{
  regs.w.ax = DOUBLE_SPEED_BOUND;
  regs.w.dx = speed;

  int386(MouseInt,&regs,&regs);
}

void Mouse::Reset()
{
  if (!IsMouse())
     D_Error("Mouse: Mouse not present");
}

void Mouse::GetSpeed(int& xs,int& ys,int& dbl)
{
  regs.w.ax = GET_SPEED;

  int386(MouseInt,&regs,&regs);

  xs  = regs.w.bx;
  ys  = regs.w.cx;
  dbl = regs.w.dx;
}

int Mouse::GetScanX()
{
  int x,y;
  GetPos(x,y);
  return x;
}

int Mouse::GetScanY()
{
  int x,y;
  GetPos(x,y);
  return y;
}

void Mouse::SetHandler(Mouse::Event mask,void far* handler)
{
  regs.w.ax      = USER_HANDLER;
  regs.w.cx      = mask;
  regs.x.edx     = FP_OFF( handler );
  sregs.es       = FP_SEG( handler );

  int386x( MouseInt, &regs, &regs, &sregs );
}

/////////////Реализация иерархии оболочек для обр. сообщений////////////////

        //Две заглушки для особо одарённых
void   _loadds far GlobalHandler(int,int,Mouse::Buttons) { }
void   _loadds far GlobalPress(int,int,int,Mouse::Buttons) {}


        //Следующие две процедуры получают управление
        //при перемещении или нажатии (0xC)
#pragma off (check_stack)
#pragma aux _move_handler parm        [ECX] [EDX] [EBX] //bl
#pragma aux _press_handler parm       [EAX] [ECX] [EDX] [EBX]//bl

void   _loadds far _move_handler   (int x,int y,Mouse::Buttons but)
{
  MouseMove::user_handler(x,y,but);
}

void   _loadds far _press_handler   (int ev,int x,int y,Mouse::Buttons but)
{
  MousePress::user_handler(ev,x,y,but);
}

#pragma on (check_stack)

        //Обработчики классов MouseMove & MousePress

static  void  (far* MouseMove::user_handler)(int,int,Mouse::Buttons)  = GlobalHandler;
static  void  (far* MousePress::user_handler)(int,int,int,Mouse::Buttons) = GlobalPress;

      //Реализация MouseHandler - базовый класс для всех обработчиков
      //сообщений мыши

MouseHandler::MouseHandler():event(Mouse::NO_EVENT) {}

MouseHandler::MouseHandler(Mouse::Event e,void far* user)
{
  if (user == NULL)
     user = GlobalHandler;

  Mouse::SetHandler(event=e,user);
}

MouseHandler::~MouseHandler()
{
  Mouse::SetHandler(event,GlobalHandler);
}

        //Реализация класса     MouseMove

MouseMove::MouseMove(void (far* user)(int,int,Mouse::Buttons))
                        : MouseHandler(Mouse::MOVE,_move_handler)
{
  user_handler = user;
}

        //Реализация класса     MousePress

MousePress::MousePress(Mouse::Event e,void (far* user)(int,int,int,Mouse::Buttons))
          :MouseHandler(e,_press_handler)
{
  user_handler = user;
}

      //Реализация класса отвечающего за работу с глобальной очередью событий

MousePress*                        MouseMessage::handler  = NULL;
ButtonDownMessage*                 MouseMessage::ms       = NULL;
int                                MouseMessage::count    = 0;

void   far _loadds
MouseMessage::_handler (
                               int ev,
                               int x,
                               int y,
                               Mouse::Buttons but)
{
  ms->x      = x;
  ms->y      = y;
  ms->state  = but;
  ms->id     = ev;

  if (ev == Mouse::MOVE)
     message_queue.PutMessage(ms,sizeof(MouseMoveMessage));
  else
  {
     ms->id &= 0xFE;
     message_queue.PutMessage(ms,sizeof(ButtonDownMessage));
  }
}

MouseMessage::MouseMessage()
{
  if (count++ == 0)
  {
    handler   = new MousePress ((Mouse::Event)0xFF,
              (void (far*)(int,int,int,Mouse::Buttons))_handler);
    ms        = new ButtonDownMessage();
  }
}

MouseMessage::~MouseMessage()
{
  if (--count == 0)
  {
    delete handler;
    delete ms;
  }
}

/////////////////////////////Класс мышка для VESA режимов////////////////////

void    MouseScreen::SetCursor(tSprite* spr)
{
  if (!spr)
     return;

  HideMouse();

  if (cursor)
     free (cursor);

  if (store)
     free (store);

  cursor = (tSprite*)malloc(spr->size());        //!!!

  memcpy(cursor,spr,spr->size());

//  store  = (tImage*)malloc(spr->width*spr->height*dest->bpp()
//                           +sizeof(tImage)-1);
  store = (tImage*)malloc(100*100+100);

  store->width  = width  = spr->width;
  store->height = height = spr->height;
}

void    MouseScreen::SetCursor(BinFile* file)
{
  tSpriteList* sl = tSpriteList::load(file);
  if (sl == NULL)
     return ;

  if (sl->GetPalette())
  {
    //  ShiftPalette
    owner->PutPalette((RGBStruct*)sl->GetPalette());
  }

  SetCursor(sl->GetImage(0));

  free(sl);

  return;
}

void    MouseScreen::SetCursor(const char* name)
{
  tSpriteList* sl = tSpriteList::load(name);
  if (sl == NULL)
     return ;

  if (sl->GetPalette())
  {
    //  ShiftPalette
    owner->PutPalette((RGBStruct*)sl->GetPalette());
  }

  SetCursor(sl->GetImage(0));

  free(sl);

  return;
}

void    MouseScreen::_showmouse(int x,int y)
{
  owner->GetImage  (oldX=x,oldY=y,x+width,y+height,store);
  owner->PutSprite (x,y,cursor);
}

void    MouseScreen::_hidemouse()
{
  owner->PutImage(oldX,oldY,store);
}

void    MouseScreen::ShowMouse()
{
  if (!visible)
     _showmouse(Mouse::GetScanX(),Mouse::GetScanY());
  visible = TRUE;
}

void    MouseScreen::HideMouse()
{
  if ( visible )
     _hidemouse();
  visible = FALSE;
}

void    MouseScreen::ShowMouse(MouseScreen::State state)
{
  switch ( state )
  {
    case SHOW: ShowMouse(); break;
    case HIDE: HideMouse();
  }
}

void    MouseScreen::MoveTo(int x,int y)
{
  if ( visible )
     _hidemouse();
  _showmouse(x,y);
}

void    MouseScreen::open(Screen* screen) //!!
{
  HideMouse();

  owner   = screen;
  visible = FALSE;
}

MouseScreen::MouseScreen(Screen* screen)
        : owner(screen), visible (FALSE), cursor (NULL), store (NULL)
 {  }

MouseScreen::MouseScreen(Screen* screen,BinFile* file)
        : owner(screen), visible (FALSE), cursor (NULL), store (NULL)
{
  SetCursor(file);
}

MouseScreen::MouseScreen(Screen* screen,const char* name)
        : owner(screen), visible (FALSE), cursor (NULL), store (NULL)
{
  SetCursor(name);
}

MouseScreen::~MouseScreen()
{
  HideMouse();
  free(cursor);
  free(store);
}