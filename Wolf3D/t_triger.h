#ifndef __TYPE_TRIGER__
#define __TYPE_TRIGER__

////////////////////////��� Triger///////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//////����饭�� �ਣ���
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

        Side    side;   //���ࠢ����� (��ଠ�� ��ப�)
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
//////�������� �ਣ�� (ArgPtr)
/////////////////////////////////////////////////////////////////////////////
struct   TrigerExempl
{
  Side          side;   // (N,E,S,W)
  int           x,y;    // map coords (integer format)
};

typedef CreateExemplar <char,TrigerExempl> CreateTriger;

/////////////////////////////////////////////////////////////////////////////
/////��� ��� ��࠭���� �ਣ��
/////////////////////////////////////////////////////////////////////////////
struct    TrigerTag
{
  int           mState;

  TrigerExempl  exempl;
};

#endif