#include "g_input.h"
#include "g_system.h"
#include "g_player.h"

//#include <message.h>
#include <keyboard.h>
//#include <mouse.h>
//#include <timer.h>

BOOL     InputTranslator::Exec (GameMessage* mes)
{
  switch (mes->id)
  {
    case TICK:  Translate();    return TRUE;
    default:    return  FALSE;
  }
}

void     InputTranslator::dump (ostream& os) const
{
  os<<"Input translator. Main player "<<mainPlayer<<
      "; call back "<<callBack<<endl;
}

void     InputTranslator::Translate ()
{
  while (!message_queue.IsEmpty())
  {
    Message* message = (Message*)message_queue.GetMessage();

    switch (message->id)
    {
      case TIME_CLICK: break;
      case KEY_DOWN:
      {
        KeyPress* k_mes = (KeyPress*)message;

        if (ptrTable[k_mes->scan])
        {
          _KeyMessage* _mes = ptrTable [k_mes->scan];

          owner->PutMessage ((GameMessage*)_mes->message,_mes->dest,_mes->size);
        }
      }
    }
  }
}

/*void     InputTranslator::Translate()
{
                //На первых порах сойдёт
  while (!message_queue.IsEmpty())
  {
    Message*        message = (Message*)message_queue.GetMessage();
    PlayerMessage   playerMessage (PLAYER_MOVE);

    if (message == NULL)
        return;

    switch (message->id)
    {
      case TIME_CLICK:  break; ///Сделать
      case KEY_DOWN:
      {
        KeyPress*       k_message = (KeyPress*)message;

        switch (k_message->scan)
        {
          case KEY_W:
          case KEY_CUP:     playerMessage.id   = ADD_MOVE_FORWARD;  break;
          case KEY_S:
          case KEY_CDOWN:   playerMessage.id   = ADD_MOVE_BACK;     break;
          case KEY_CLEFT:   playerMessage.id   = ADD_TURN_LEFT;     break;
          case KEY_CRIGHT:  playerMessage.id   = ADD_TURN_RIGHT;    break;
          case KEY_A:       playerMessage.id   = ADD_STRAFE_LEFT;   break;
          case KEY_D:       playerMessage.id   = ADD_STRAFE_RIGHT;  break;
          case KEY_E:
          {
            PlayerShot  message;

            owner->PutMessage (&message,mainPlayer,sizeof(PlayerShot));

            continue;
          }

          case KEY_SPACE:
            playerMessage.id   = PRESS;
            break;

          case KEY_F6:
          {
            GameSaveMessage message ("save/default.sav");

            owner->PutMessage (&message,callBack,sizeof(message));

            break;
          }
          case KEY_F7:
          {
            GameLoadMessage message ("save/default.sav");

            owner->PutMessage (&message,callBack,sizeof(message));

            break;
          }

          default:
          {
            KeyDownMessage      key (k_message->scan);

            owner->PutMessage (&key,callBack,sizeof(key));

            continue;
          }
        }

        owner->PutMessage (&playerMessage,mainPlayer,sizeof(playerMessage));

        break;
      }

      case KEY_UP:
      {
        KeyPress*       k_message = (KeyPress*)message;

        switch (k_message->scan)
        {
          case KEY_W:
          case KEY_CUP:     playerMessage.id   = SUB_MOVE_FORWARD;    break;
          case KEY_S:
          case KEY_CDOWN:   playerMessage.id   = SUB_MOVE_BACK;       break;
          case KEY_CLEFT:   playerMessage.id   = SUB_TURN_LEFT;       break;
          case KEY_CRIGHT:  playerMessage.id   = SUB_TURN_RIGHT;      break;
          case KEY_A:       playerMessage.id   = SUB_STRAFE_LEFT;     break;
          case KEY_D:       playerMessage.id   = SUB_STRAFE_RIGHT;    break;

          case KEY_SPACE:       continue;
          case KEY_F6:          continue;
          case KEY_F7:          continue;
          case KEY_E:           continue;
          default:
          {
            KeyUpMessage      key (k_message->scan);

            owner->PutMessage (&key,callBack,sizeof(key));

            continue;
          }
        }

        owner->PutMessage (&playerMessage,mainPlayer,sizeof(playerMessage));

        break;
      }
    }
  }
} */

void    InputTranslator::AddKeyMessage (int key,void* _add)
{
  if (ptrTable[key]) DelKeyMessage (key);

  _KeyMessage* add = (_KeyMessage*)_add;

  ptrTable[key] = (_KeyMessage*)new uchar[add->size+sizeof(ID)+sizeof(int)];

  memcpy (ptrTable[key],&add->message,add->size);
}

void    InputTranslator::DelKeyMessage (int key)
{
  if (ptrTable[key]) delete ptrTable[key];
  ptrTable[key] = NULL;
}

InputTranslator::InputTranslator  (ID pl)
                : mainPlayer (pl),
                  ptrTable (new _KeyMessage* [MAX_KEYS])
{
  for (int i=0;i<MAX_KEYS;ptrTable[i++]=NULL);

  owner->AddActive (id());
}

InputTranslator::~InputTranslator ()
{
  owner->DelActive (id());

  for (int i=0;i<MAX_KEYS;i++)
      if (ptrTable [i])
         delete ptrTable[i];

  delete ptrTable;
}

