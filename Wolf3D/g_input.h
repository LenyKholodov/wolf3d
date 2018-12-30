#ifndef  __INPUT_TRANSLATOR__
#define  __INPUT_TRANSLATOR__

#include "g_types.h"
#include "g_object.h"

/////////////////////////////////////////////////////////////////////////////
//////�������� ᮮ�饭�� �� ��⥬��� ��।� � ��⮢��� ����
/////////////////////////////////////////////////////////////////////////////
class   InputTranslator: public GameObject
{
  public:
                                InputTranslator  (ArgPtr = NULL);
                                InputTranslator  (BinFile*);
                                InputTranslator  (ID mainPlayer);
            virtual             ~InputTranslator ();

////////////////////////////////////////////////////////////////////////////
//////�᭮��� ����ࠪ�� �㭪樨 ��� ��� ��஢�� ��ꥪ⮢
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile* file)      {  GameObject::Load (file); }
    virtual     void            Save (BinFile* file)      {  GameObject::Save (file); }
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return INPUT_TRANSLATOR; }
    virtual     void            dump (ostream&) const;

////////////////////////////////////////////////////////////////////////////
//////���࠭���� ᮮ�饭��
////////////////////////////////////////////////////////////////////////////
                void            Translate();
                void            Translate(int key,int state);
                void            Translate(char*);

                void            OnlyConsole (BOOL x);
                void            Clear       ();

                ID              player () const { return mainPlayer; }
                void            player (ID x)   { mainPlayer = x;}

////////////////////////////////////////////////////////////////////////////
//////����� � ��㧥
////////////////////////////////////////////////////////////////////////////
                void            pause (BOOL x) { mPause = x; }

///////////////////////////////////////////////////////////////////////////
//////������ ����
///////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//////���訢��
////////////////////////////////////////////////////////////////////////////
                enum    State
                {
                  DOWN=0,
                  UP,
                  CONSOLE_LINE=0,
                  BIND_LINE
                };

     static     void            Bind   (int code,State,const char*);
     static     void            UnBind (int code);
     static     const char*     GetBind(int code,State);

////////////////////////////////////////////////////////////////////////////
//////��⮤ ��࠭��樨 (console_line or bind_line)
////////////////////////////////////////////////////////////////////////////
                void            SetLine (State x) {mLine=x;}

  protected:
             static   char      bindTable [0x80+0xF][2][32];

  protected:
                ID              mainPlayer;
                State           mLine;
                BOOL            mPause;
                BOOL            mConsole;
                int             mouseX, mouseY;
};

#endif