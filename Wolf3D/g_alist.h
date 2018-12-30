#ifndef __ACTIVE_LIST__
#define __ACTIVE_LIST__

#include "g_ring.h"
#include "g_load.h"

typedef unsigned char priority_t;

/////////////////////////////////////////////////////////////////////////////
/////���᮪ ��ꥪ⮢, ����� �ॡ��� ��ਮ���᪮� ��।�� �ࠢ�����
/////////////////////////////////////////////////////////////////////////////
class   ActiveList: public Ring
{
  public:
                        ActiveList (int max = MAX_RING_SIZE);
                        ActiveList (ArgPtr = NULL);
                        ActiveList (BinFile*);
    virtual             ~ActiveList ();

    virtual     BOOL    Exec (GameMessage*);

////////////////////////////////////////////////////////////////////////////
//////�᭮��� ����ࠪ�� �㭪樨 ��� ��� ��஢�� ��ꥪ⮢
////////////////////////////////////////////////////////////////////////////
    virtual     GameObjects     type () const  { return ACTIVE_LIST; }

    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);

////////////////////////////////////////////////////////////////////////////
//////��।�� �ࠢ����� ��⨢�� ��ꥪ⠬
////////////////////////////////////////////////////////////////////////////
                void            NextTick        ();   //��⨢�� �����
                void            TickAll         ();

    virtual     int             Add             (ID,priority_t = 0);


  protected:
             uchar*             mPriority ;
             int                mPos;
};

#endif