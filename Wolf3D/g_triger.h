#ifndef __GAME_TRIGER__
#define __GAME_TRIGER__

#include "g_object.h"
#include "map.h"

/////////////////////////////////////////////////////////////////////////////
//////�ਣ�� - ������ - ���饬 ��� ��� (��� ����)
/////////////////////////////////////////////////////////////////////////////
class   Triger: public GameObject
{
  public:
                                Triger(ArgPtr);
                                Triger(BinFile*);
                  virtual       ~Triger();

                enum    State
                {
                  ON   = 1,
                  OFF  = 0
                };

////////////////////////////////////////////////////////////////////////////
//////�᭮��� ����ࠪ�� �㭪樨 ��� ��� ��஢�� ��ꥪ⮢
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return TRIGER; }
    virtual     void            dump (ostream&) const;

////////////////////////////////////////////////////////////////////////////
//////����⢥��� �ࠢ����� �ਣ�஬
////////////////////////////////////////////////////////////////////////////
    virtual     void    On    ()           { mState = ON;  }
    virtual     void    Off   ()           { mState = OFF; }

                int     GetX  () const     { return trigExempl.x; }
                int     GetY  () const     { return trigExempl.y; }
                Side    GetSide  () const  { return trigExempl.side;  }
                State   GetState () const  { return mState; }

                void    Inv   ()
                        { if (GetState() == ON) Off();
                          else                  On();
                        }

    protected:
               State            mState;

               TrigerExempl     trigExempl;
};

#endif