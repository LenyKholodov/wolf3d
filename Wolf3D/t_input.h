#ifndef __TYPE_INPUT__
#define __TYPE_INPUT__

/////////////////////Для InputTranslator/////////////////////////////////////

enum InputMessageId
{
  INPUT_KEY_DOWN   = INPUT_LIMIT,
  INPUT_KEY_UP
};

enum MouseMessagesId
{
  MS_MOVE = 0x80,
  MS_LBUTTON,
  MS_RBUTTON,
  MS_CBUTTON,
};

static const char* MSNames [0xF] =
{
  "MS_MOVE",
  "MS_LBUTTON",
  "MS_RBUTTON",
  "MS_CBUTTON"
};

/////////////////////////////////////////////////////////////////////////////
//////Если нажата левая клавиша: необходимая для управления интерфейсом
/////////////////////////////////////////////////////////////////////////////
struct  KeyDownMessage: public GameMessage
{
        KeyDownMessage (int x,int _x): key(x), ascii(_x)
                  { id = INPUT_KEY_DOWN; }

        int     key;
        int     ascii;
};

struct  KeyUpMessage: public GameMessage
{
        KeyUpMessage (int x): key(x)
                  { id = INPUT_KEY_UP; }

        int     key;
};

/////////////////////////////////////////////////////////////////////////////
//////Крыса!!!
/////////////////////////////////////////////////////////////////////////////

struct MSMoveMessage: public GameMessage
{
        MSMoveMessage (int _x,int _y): dx(_x), dy(_y)
                      { id = MS_MOVE; }

        int dx,dy;      //перемещения по OX & OY
};

template <MouseMessagesId __id>
struct MSButtonPress: public GameMessage
{
       MSButtonPress (int _x,int _y):x(_x),y(_y) { id = __id; }

       int      x,y;
};

typedef MSButtonPress<MS_LBUTTON>  MSLButtonDown;
typedef MSButtonPress<MS_RBUTTON>  MSRButtonDown;
typedef MSButtonPress<MS_CBUTTON>  MSCButtonDown;

#endif