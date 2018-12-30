#ifndef __TYPE_TRIGER__
#define __TYPE_TRIGER__

////////////////////////Для Triger///////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//////Сообщения тригеру
/////////////////////////////////////////////////////////////////////////////
enum  TrigerMessageId
{
  TRIGER_PRESS         = TRIGER_LIMIT,
  TRIGER_ON,
  TRIGER_OFF
};

struct  TrigerPressMessage: public GameMessage
{
        TrigerPressMessage (Side x): side (x)  { id = TRIGER_PRESS; }

        Side    side;   //Направление (нормаль игрока)
};

struct  TrigerOnMessage: public GameMessage
{
        TrigerOnMessage ()  { id = TRIGER_ON; }

};

struct  TrigerOffMessage: public GameMessage
{
        TrigerOffMessage ()  { id = TRIGER_OFF; }
};

/////////////////////////////////////////////////////////////////////////////
//////Создание тригера (ArgPtr)
/////////////////////////////////////////////////////////////////////////////
struct   TrigerExempl
{
  Side          side;   // (N,E,S,W)
  int           x,y;    // map coords (integer format)
};

typedef CreateExemplar <char,TrigerExempl> CreateTriger;

/////////////////////////////////////////////////////////////////////////////
/////Таг для сохранения тригера
/////////////////////////////////////////////////////////////////////////////
struct    TrigerTag
{
  int           mState;

  TrigerExempl  exempl;
};

#endif