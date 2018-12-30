#ifndef __MAIN_GAME_QUEUE__
#define __MAIN_GAME_QUEUE__

#include "g_types.h"
#include "g_object.h"

#include <message.h>

/////////////////////////////////////////////////////////////////////////////
//////Очередь игровых сообщений
/////////////////////////////////////////////////////////////////////////////
    //Потестировать деструкторы
class   MainQueue: public GameObject, public MessageHandler
{
  public:
                        MainQueue  (long = QUEUE_SIZE,long = MAX_MESSAGES);
                        MainQueue  (BinFile*);
//              virtual   ~MainQueue ()   {}

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     BOOL            Exec (GameMessage*)  { return FALSE; }

    virtual     GameObjects     type () const      { return MAIN_QUEUE; }
    virtual     void            dump (ostream&) const;

////////////////////////////////////////////////////////////////////////////
//////Работа собственно с очередью
////////////////////////////////////////////////////////////////////////////
    virtual     void            PutMessage      (GameMessage*,ID,int size,ID = ID(-1,-1));

////////////////////////////////////////////////////////////////////////////
//////Управление памятью
////////////////////////////////////////////////////////////////////////////
                void*           operator new (size_t x)    { return ::operator new (x); }
                void            operator delete (void* x)  { ::operator delete (x); }

  protected:
                void*           operator new (size_t,GameObjects);

};

//extern MainQueue*    gameMessageQueue;

#endif