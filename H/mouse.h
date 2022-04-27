#include <types.h>

#include <screen.h>
#include <binfile.h>
#include <message.h>

////////////////////////////////////////////////////////////////////////////
////����� �⢥��騩 �� �������� �����প� ��誨
////////////////////////////////////////////////////////////////////////////
class   MouseMessage;
class   Mouse
{
  public:
       enum {
            MouseInt    = 0x33,

            INIT        = 0,
            SHOW_MOUSE,
            HIDE_MOUSE,
            GET_STATE,
            MOVE_TO,
            MOVE_X = 7,
            MOVE_Y,
            TEXT_PARAM = 10,
            USER_HANDLER = 12,
            DOUBLE_SPEED_BOUND = 19,
            SET_SPEED = 15,
            GET_SPEED = 27,
            SOFT_TEXT_CURSOR = 0,
            HARD_TEXT_CURSOR = 1
            };

       enum Button       {
              LEFT_BUTTON       = 1,
              RIGHT_BUTTON      = 2,
              CENTER_BUTTON     = 4,
              ANY_BUTTON        = 7,
              NO_PRESS          = 0,
              };

        struct  Buttons  {
              char       left   : 1;
              char       right  : 1;
              char       center : 1;
              };

        enum Event       {
              NO_EVENT          = 0,
              MOVE              = 1,
              LBUTTON_DOWN      = 2,
              LBUTTON_UP        = 4,
              RBUTTON_DOWN      = 8,
              RBUTTON_UP        = 16,
              CBUTTON_DOWN      = 32,
              CBUTTON_UP        = 64
              };

        Mouse();
        ~Mouse();

/////////////////////////////////////////////////////////////////////////////
////�����樠������ ��� � �஢�ઠ �� ������
/////////////////////////////////////////////////////////////////////////////
    static        void            Reset();
    static        BOOL            IsMouse();

/////////////////////////////////////////////////////////////////////////////
////����� ��誨 �� �� SVGA �����
/////////////////////////////////////////////////////////////////////////////
    static        void            SetHardText(int first,int last);
    static        void            SetSoftText(int scr, int cursor);
    static        void            HideMouse();
    static        void            ShowMouse();

/////////////////////////////////////////////////////////////////////////////
////����� � ���न��⠬� ��誨
/////////////////////////////////////////////////////////////////////////////
    static        void            MoveTo(int x,int y);
    static        void            GetPos(int& x,int& y);
    static        int             GetScanX();
    static        int             GetScanY();

    static        void            SetBoundRect(int left,int top,int right,int bottom);

    static        void            SetDoubleSpeed(int speed);

    static        void            SetSpeed(int xs,int ys);
    static        void            GetSpeed(int& xs,int& ys,int& dbl);

/////////////////////////////////////////////////////////////////////////////
////����� � �������� ��誨
/////////////////////////////////////////////////////////////////////////////
    static        Buttons         GetButton();
    static        BOOL            IsPress(Button button = ANY_BUTTON);
    static        BOOL            IsDblClick(int speed);


/////////////////////////////////////////////////////////////////////////////
////��⠭���� ��ࠡ��稪� ���짮��⥫�
/////////////////////////////////////////////////////////////////////////////
    static        void            SetHandler(Event mask,void far* handler);
    static        void            ClearAllHandlers() { Reset(); }

  private:
           MouseMessage*        ms_handler;
};

/////////////////////////////////////////////////////////////////////////////
////������ ����� ��� ��� �����祪 ��ࠡ��稪�� ᮮ�饭�� ���
/////////////////////////////////////////////////////////////////////////////
class   MouseHandler
{
  public:
          MouseHandler(Mouse::Event,void far*);
          virtual ~MouseHandler();
  protected:
          MouseHandler();
  private:
          Mouse::Event          event;
};

/////////////////////////////////////////////////////////////////////////////
////�����-�����窠 ��� ��ࠡ��稪� ᮮ�饭�� (Mouse Move)
/////////////////////////////////////////////////////////////////////////////
class   MouseMove: public MouseHandler
{
  public:
        MouseMove(void (far*)(int,int,Mouse::Buttons));
  protected:
   friend  void  far      _move_handler(int,int,Mouse::Buttons);
   static  void (far*     user_handler)(int,int,Mouse::Buttons);
};

/////////////////////////////////////////////////////////////////////////////
////�����-�����窠 ��� ��ࠡ��稪� ᮮ�饭�� (Mouse Button Press)
/////////////////////////////////////////////////////////////////////////////
class   MousePress: public MouseHandler
{
  public:
        MousePress(Mouse::Event,void (far*)(int,int,int,Mouse::Buttons));
  protected:
   friend  void  far      _press_handler(int,int,int,Mouse::Buttons);
   static  void (far*     user_handler)(int,int,int,Mouse::Buttons);
};

/////////////////////////////////////////////////////////////////////////////
///�����-�����窠 ��� ࠡ��� � ������쭮� ��।�� ᮡ�⨩
/////////////////////////////////////////////////////////////////////////////

struct  MouseMoveMessage;
struct  ButtonDownMessage;
struct  ButtonsUpMessage;

class   MouseMessage
{
  public:
          MouseMessage();
          ~MouseMessage();

  private:
   static      MousePress*                   handler;
   static      ButtonDownMessage*            ms;

   static      int                           count;

   static      void  far             _handler(int,int,int,Mouse::Buttons);
};

/////////////////////////////////////////////////////////////////////////////
///�������� ��� ���ᠭ�� ��������� ᮮ�饭��
/////////////////////////////////////////////////////////////////////////////
struct  MouseMoveMessage: public Message
{
  int   x,
        y;
};

struct  ButtonDownMessage: public MouseMoveMessage
{
  Mouse::Buttons   state;
};

struct  ButtonUpMessage: public ButtonDownMessage {};


/////////////////////////////////////////////////////////////////////////////
///����� �믮����騩 �����প� ��� � ०���� VESA
/////////////////////////////////////////////////////////////////////////////
class   MouseScreen
{
  public:
          enum  State
          {
                HIDE = FALSE,
                SHOW = TRUE
          };

          MouseScreen(Screen* dest,BinFile* file);
          MouseScreen(Screen* dest,const char* name);

          virtual ~MouseScreen();

          void          open(Screen* screen);

/////////////////////////////////////////////////////////////////////////////
///����᪨� ��ࠧ �����
/////////////////////////////////////////////////////////////////////////////
          void          SetCursor(tSprite* sprite);
          void          SetCursor(BinFile* file);
          void          SetCursor(const char* name);

/////////////////////////////////////////////////////////////////////////////
///����ﭨ� ����� (SHOW, HIDE)
/////////////////////////////////////////////////////////////////////////////
          void          ShowMouse(State);
          void          ShowMouse();
          void          HideMouse();

          BOOL          IsVisible() { return visible; }

/////////////////////////////////////////////////////////////////////////////
///��६�饭�� ����� � ������� �࠭� ���न���� ( auto ShowMouse )
/////////////////////////////////////////////////////////////////////////////
          void          MoveTo(int x,int y);

/////////////////////////////////////////////////////////////////////////////
///����ᮢ�� ��設��� �࠭�
/////////////////////////////////////////////////////////////////////////////
          void          Draw();

          void          _showmouse(int x,int y);
          void          _hidemouse();

  protected:
          MouseScreen();
          MouseScreen(Screen* dest);

  private:
                   Screen*       owner;          //�࠭ ��������

                   int           oldX,           //��� HideMouse
                                 oldY;
                   int           width,          //ࠧ���� �����
                                 height;
                   BOOL          visible;        //���ﭨ� �����

                   tSprite*      cursor;         //��ࠧ ����� (tSprite)
                   tImage*       store;          //��ࠧ �����⮩ ������
};

