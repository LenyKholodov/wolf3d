#ifndef __RING__
#define __RING__

#include "g_ttable.h"

struct  RingNode
{
  ID            id;     //��� �����
  int16         next,
                prev;

  int16         nextFree;       //��� ��⥬�
};

        //�த㬠�� 㤠�����

/////////////////////////////////////////////////////////////////////////////
/////������殢���� �����
/////////////////////////////////////////////////////////////////////////////
struct  RingNode;
class   Ring: public GameObject
{
  public:
                                Ring (int max = MAX_RING_SIZE) ;
                                Ring (BinFile*);
                  virtual       ~Ring ();

////////////////////////////////////////////////////////////////////////////
//////�᭮��� ����ࠪ�� �㭪樨 ��� ��� ��஢�� ��ꥪ⮢
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return RING; }
    virtual     void            dump (ostream&) const;

////////////////////////////////////////////////////////////////////////////
//////����� � ����஬
////////////////////////////////////////////////////////////////////////////
                BOOL            More ()   const;
                BOOL            IsEmpty() const { return mFirst == -1; }
                int             GetPos()  const { return mCurrent; }
                void            Reset();       //�� ��砫� ᯨ᪠
                ID              Next ();       //in active iterator
                ID              Prev ();       //in active iterator
                ID              Current () const;

    virtual     int             Add         (ID);
    virtual     void            Del         ();
    virtual     void            Del         (ID);
    virtual     void            ClearAll    ();

                int             count       () const { return mCount; }
                BOOL            IsBelong    (ID) const ;

  protected:
             int                mCount;
             int                mFirst;
             int                mFirstFree;
             int                mCurrent;
             RingNode*          mPool   ;
};

#endif