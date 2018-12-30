#ifndef  __INPUT_TRANSLATOR__
#define  __INPUT_TRANSLATOR__

#include "g_types.h"
#include "g_object.h"

typedef void (* KeyFunc)(int scan,char ascii);

/////////////////////////////////////////////////////////////////////////////
//////�������� ᮮ�饭�� �� ��⥬��� ��।� � ��⮢��� ����
/////////////////////////////////////////////////////////////////////////////
class   InputTranslator: public GameObject
{
  public:
                                InputTranslator  (ID mainPlayer);
            virtual             ~InputTranslator ();

////////////////////////////////////////////////////////////////////////////
//////�᭮��� ����ࠪ�� �㭪樨 ��� ��� ��஢�� ��ꥪ⮢
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*)             { }
    virtual     void            Save (BinFile*)             { }
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return INPUT_TRANSLATOR; }
    virtual     void            dump (ostream&) const;

////////////////////////////////////////////////////////////////////////////
//////���࠭���� ᮮ�饭��
////////////////////////////////////////////////////////////////////////////
                void            AddKeyMessage (int,KeyFunc);
                void            DelKeyMessage (int);
                void            Translate();

  protected:
                ID              mainPlayer;
                KeyFunc**       ptrTable;          //⠡��� �८�ࠧ������
};

#endif