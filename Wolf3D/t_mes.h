#ifndef __TYPE_MESSAGES__
#define __TYPE_MESSAGES__

#include "t_id.h"
#include <message.h>

/////////////////////////////////////////////////////////////////////////////
/////Основные сообщения (и их типы)
/////////////////////////////////////////////////////////////////////////////

enum
{
  GAME_SEQUENCE = 0xFFF,
  TICK,
  NOT_PROCESS
};

struct  GameMessage: public Message
{
  ID            objectId;       //Получатель сообщения
  ID            callBack;       //Посылатель
};

/////////////////////////////////////////////////////////////////////////////
/////Сообщение не обработано
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

        int     tickNum;        //Изменение времени от последнего сообщения
};

struct  GameTickMessage: public GameMessage
{
        GameTickMessage () { id = TICK; }
};

#endif