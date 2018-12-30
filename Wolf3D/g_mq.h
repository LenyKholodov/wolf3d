#ifndef __MAIN_GAME_QUEUE__
#define __MAIN_GAME_QUEUE__

#include "g_types.h"
#include "g_object.h"

#include <message.h>

/////////////////////////////////////////////////////////////////////////////
//////��।� ��஢�� ᮮ�饭��
/////////////////////////////////////////////////////////////////////////////
    //�����஢��� ���������
class   MainQueue: public GameObject, public MessageHandler
{
  public:
                        MainQueue  (long = QUEUE_SIZE,long = MAX_MESSAGES);
                        MainQueue  (BinFile*);
//              virtual   ~MainQueue ()   {}

////////////////////////////////////////////////////////////////////////////
//////�᭮��� ����ࠪ�� �㭪樨 ��� ��� ��஢�� ��ꥪ⮢
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     BOOL            Exec (GameMessage*)  { return FALSE; }

    virtual     GameObjects     type () const      { return MAIN_QUEUE; }
    virtual     void            dump (ostream&) const;

////////////////////////////////////////////////////////////////////////////
//////����� ᮡ�⢥��� � ��।��
////////////////////////////////////////////////////////////////////////////
    virtual     void            PutMessage      (GameMessage*,ID,int size,ID = ID(-1,-1));

////////////////////////////////////////////////////////////////////////////
//////��ࠢ����� �������
////////////////////////////////////////////////////////////////////////////
                void*           operator new (size_t x)    { return ::operator new (x); }
                void            operator delete (void* x)  { ::operator delete (x); }

  protected:
                void*           operator new (size_t,GameObjects);

};

//extern MainQueue*    gameMessageQueue;

#endif