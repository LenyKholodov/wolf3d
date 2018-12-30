#ifndef __GAME_SUBJECT__
#define __GAME_SUBJECT__

#include "g_object.h"

/////////////////////////////////////////////////////////////////////////////
//////�।��� (���᪠, ��㦨� � �.�.)
////////////////////////////////////////////////////////////////////////////
class   Subject: public GameObject
{
  public:
                        Subject  (ArgPtr);
                        Subject  (BinFile*);
          virtual       ~Subject ();

////////////////////////////////////////////////////////////////////////////
//////�᭮��� ����ࠪ�� �㭪樨 ��� ��� ��஢�� ��ꥪ⮢
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*)     ;
    virtual     void            Save (BinFile*)     ;
    virtual     BOOL            Exec (GameMessage*) ;

    virtual     GameObjects     type () const  { return SUBJECT; }
    virtual     void            dump (ostream&) const;

/////////////////////////////////////////////////////////////////////////////
/////�।��� ����� �����, �������� � �ᯮ�짮����
/////////////////////////////////////////////////////////////////////////////
    virtual     BOOL            Include (ID player);
    virtual     void            Exclude ();

    virtual     void            Use     ()      = 0;

  protected:
                ID              playerOwner;    //��ப �������
};

#endif