#ifndef __GAME_SUBJECT__
#define __GAME_SUBJECT__

#include "g_object.h"

/////////////////////////////////////////////////////////////////////////////
//////�।��� (���᪠, ��㦨� � �.�.) ����騩 �� ����
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
/////�।��� ����� �����
/////////////////////////////////////////////////////////////////////////////
    virtual     BOOL            Include  (ID player);
                void            refresh (BOOL = FALSE);

                int             GetX() const { return x; }
                int             GetY() const { return y; }

  protected:
                void            Respawn ();

  protected:
                SubjectRes      subjRes;        //����-�奬�
                int             x,y;            //���न����
                clock_t         lastRespawn;    //�६� ��᫥ �����
};

#endif