#ifndef __GAME_DOOR__
#define __GAME_DOOR__

#include "g_triger.h"

class   Door: public Triger
{
  public:
                                Door(ArgPtr);
                                Door(BinFile*);
              virtual           ~Door ();

////////////////////////////////////////////////////////////////////////////
//////�᭮��� ����ࠪ�� �㭪樨 ��� ��� ��஢�� ��ꥪ⮢
////////////////////////////////////////////////////////////////////////////
    virtual     BOOL            Exec (GameMessage*);
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     GameObjects     type () const  { return DOOR; }
    virtual     void            dump (ostream&) const;

////////////////////////////////////////////////////////////////////////////
//////����⢥��� �ࠢ����� ������
////////////////////////////////////////////////////////////////////////////
    virtual     void    On   ();
    virtual     void    Off  ();

  protected:
             DoorRes    doorRes;

             int        mDoorState;      //��᪮�쪮 �����
             int        mDoorStep;
             int        mDoorWait;
};

#endif