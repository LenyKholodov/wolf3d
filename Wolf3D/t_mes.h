#ifndef __TYPE_MESSAGES__
#define __TYPE_MESSAGES__

#include "t_id.h"
#include <message.h>

/////////////////////////////////////////////////////////////////////////////
/////�᭮��� ᮮ�饭�� (� �� ⨯�)
/////////////////////////////////////////////////////////////////////////////

enum
{
  GAME_SEQUENCE = 0xFFF,
  TICK,
  NOT_PROCESS
};

struct  GameMessage: public Message
{
  ID            objectId;       //�����⥫� ᮮ�饭��
  ID            callBack;       //���뫠⥫�
};

/////////////////////////////////////////////////////////////////////////////
/////����饭�� �� ��ࠡ�⠭�
/////////////////////////////////////////////////////////////////////////////
struct  GameNotProcess: public GameMessage
{
                GameNotProcess(ID dest,int messageId)
                  : obj (dest), mesId (messageId)
                { id = NOT_PROCESS; }

        ID      obj;
        int     mesId;
};

struct  GameSequence: public GameMessage
{
        GameSequence () { id = GAME_SEQUENCE; }

        int     tickNum;        //��������� �६��� �� ��᫥����� ᮮ�饭��
};

struct  GameTickMessage: public GameMessage
{
        GameTickMessage () { id = TICK; }
};

#endif