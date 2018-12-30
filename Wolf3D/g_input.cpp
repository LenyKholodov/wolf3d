#include "g_input.h"
#include "g_system.h"
#include "g_player.h"

#include <keyboard.h>
#include <mouse.h>

#include <string.h>

static   char      InputTranslator::bindTable [0x80+0xF][2][32];

void            InputTranslator::Clear       ()
{
  message_queue.Clear ();
}

void            InputTranslator::Bind   (int code,State state,const char* string)
{
  strcpy (bindTable[code][state],string);
}

void            InputTranslator::UnBind (int code)
{
  bindTable [code][DOWN][0] = 0;
  bindTable [code][UP][0]   = 0;
}

const char*     InputTranslator::GetBind(int code,State state)
{
  return bindTable[code][state];
}

BOOL     InputTranslator::Exec (GameMessage* mes)
{
  switch (mes->id)
  {
    case TICK:
    {
      Translate();
      return TRUE;
    }
    default:    return  FALSE;
  }
}

void     InputTranslator::dump (ostream& os) const
{
  os<<"Input translator. Main player "<<mainPlayer<<
      "; call back "<<callBack<<endl;
}

void     InputTranslator::OnlyConsole (BOOL x)
{
  mConsole = x;
  mLine    = x ? CONSOLE_LINE : BIND_LINE;

  message_queue.Clear();
}

void     InputTranslator::Translate()
{
  NetTick  ();

  while (!message_queue.IsEmpty())
  {
    Message*        message = (Message*)message_queue.GetMessage();

    if (message == NULL)
        return;

    switch (message->id)
    {
      case TIME_CLICK:  break; ///Сделать
      case KEY_DOWN:
      {
        KeyPress*       k_message = (KeyPress*)message;
        int             scan      = k_message->scan,
                        ascii     = k_message->chr;

        if (scan == KEY_TILDA && !mConsole)
        {
           if (mLine == CONSOLE_LINE) mLine = BIND_LINE;
           else                       mLine = CONSOLE_LINE;

           KeyDownMessage      key (scan,ascii);

           owner->PutMessage (&key,callBack,sizeof(key),id());

           continue;
        }

        if (mLine == BIND_LINE && !mPause)
          Translate (scan,DOWN);
        else
        {
            KeyDownMessage      key (scan,ascii);

            owner->PutMessage (&key,callBack,sizeof(key),id());
        }

        break;
      }
      case KEY_UP:
      {
        if (mLine == BIND_LINE && !mPause)
        {
          KeyPress*       k_message = (KeyPress*)message;
          int             scan      = k_message->scan;

          Translate (scan,UP);
        }

        break;
      }
      case Mouse::MOVE:
      {
        if (mPause)
           break;

        MouseMoveMessage* mes = (MouseMoveMessage*)message;

        if (mouseX == -1 && mouseY == -1)
        {
          mouseX = mes->x;
          mouseY = mes->y;

          break;
        }
        else
        {
          int dx = mes->_x - mouseX,
              dy = mes->_y - mouseY;

          mouseX = mes->_x;
          mouseY = mes->_y;

          if (mLine == BIND_LINE)
          {
            if (bindTable[MS_MOVE][DOWN][0])
            {
               char bufdx[12], bufdy[12];
               int  len  = strlen (bindTable[MS_MOVE][DOWN]);
               char* str = bindTable[MS_MOVE][DOWN];

               strcat (str," ");
               strcat (str,itoa (dx,bufdx,10));
               strcat (str," ");
               strcat (str,itoa (dy,bufdy,10));

               Translate(str);

               str[len] = 0;
            }
          }

          break;
        }
      }
      case Mouse::LBUTTON_DOWN:
      case Mouse::RBUTTON_DOWN:
      case Mouse::CBUTTON_DOWN:
      {
        if (mLine == BIND_LINE && !mPause)
        {
            int mes_id;

            if (message->id == Mouse::LBUTTON_DOWN) mes_id=MS_LBUTTON;
            if (message->id == Mouse::RBUTTON_DOWN) mes_id=MS_RBUTTON;
            if (message->id == Mouse::CBUTTON_DOWN) mes_id=MS_CBUTTON;

            Translate (mes_id,DOWN);
        }

        break;
      }
      case Mouse::LBUTTON_UP:
      case Mouse::CBUTTON_UP:
      case Mouse::RBUTTON_UP:
      {
        if (mLine == BIND_LINE && !mPause)
        {
            int mes_id;

            if (message->id == Mouse::LBUTTON_UP) mes_id=MS_LBUTTON;
            if (message->id == Mouse::RBUTTON_UP) mes_id=MS_RBUTTON;
            if (message->id == Mouse::CBUTTON_UP) mes_id=MS_CBUTTON;

            Translate (mes_id,UP);
        }

        break;
      }
      case NET_SEQUENCE:
      {
        NetPacket*    packet = (NetPacket*)message;

        if (packet->objectId != ID(-1,-1))
          owner->PutMessage (packet,packet->objectId,packet->size,packet->callBack);
        else
          owner->PutMessage (packet,packet->size,packet->callBack);

        break;
      }
    }
  }
}

void            InputTranslator::Translate(int key,int state)
{
  if (bindTable[key][state][0])
    command(bindTable[key][state]);
}

void            InputTranslator::Translate(char* str)
{
  command(str);
}

InputTranslator::InputTranslator  (ID pl)
                : mainPlayer (pl), mouseX(-1), mouseY(-1),
                  mLine(BIND_LINE), mPause (FALSE), mConsole(FALSE)
{
  owner->AddActive (id());
  message_queue.Clear();
}

InputTranslator::InputTranslator  (ArgPtr)
                : mainPlayer (ID(-1,-1)), mouseX(-1), mouseY(-1),
                  mLine(BIND_LINE), mPause (FALSE), mConsole(FALSE)
{
}

InputTranslator::InputTranslator  (BinFile* file)
                : mainPlayer (ID(-1,-1)), mouseX(-1), mouseY(-1),
                  mLine(BIND_LINE), mPause (FALSE), mConsole(FALSE)
{
  Load (file);
}

InputTranslator::~InputTranslator ()
{
  owner->DelActive (id());
}

