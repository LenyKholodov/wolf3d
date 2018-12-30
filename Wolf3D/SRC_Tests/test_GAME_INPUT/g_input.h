#ifndef  __INPUT_TRANSLATOR__
#define  __INPUT_TRANSLATOR__

#include "g_types.h"
#include "g_object.h"

typedef void (* KeyFunc)(int scan,char ascii);

/////////////////////////////////////////////////////////////////////////////
//////Ретрасляция сообщений от системной очереди к готовому виду
/////////////////////////////////////////////////////////////////////////////
class   InputTranslator: public GameObject
{
  public:
                                InputTranslator  (ID mainPlayer);
            virtual             ~InputTranslator ();

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*)             { }
    virtual     void            Save (BinFile*)             { }
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return INPUT_TRANSLATOR; }
    virtual     void            dump (ostream&) const;

////////////////////////////////////////////////////////////////////////////
//////Ретрансляция сообщений
////////////////////////////////////////////////////////////////////////////
                void            AddKeyMessage (int,KeyFunc);
                void            DelKeyMessage (int);
                void            Translate();

  protected:
                ID              mainPlayer;
                KeyFunc**       ptrTable;          //таблица преобразований
};

#endif