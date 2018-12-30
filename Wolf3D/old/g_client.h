#ifndef __GAME_CLIENT__
#define __GAME_CLIENT__

#include "g_system.h"

/////////////////////////////////////////////////////////////////////////////
//////������ - ����䥩� �� ������� ��設�
/////////////////////////////////////////////////////////////////////////////
class    GameClient: public GameObject
{
  public:
                                GameClient  (ArgPtr = NULL);
                                GameClient  (BinFile*);
            virtual             ~GameClient ();

            ID                  GetServer () const { owner->GetDispatch(); }

////////////////////////////////////////////////////////////////////////////
//////�᭮��� ����ࠪ�� �㭪樨 ��� ��� ��஢�� ��ꥪ⮢
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return GAME_CLIENT; }
    virtual     void            dump (ostream&) const;
};

#endif