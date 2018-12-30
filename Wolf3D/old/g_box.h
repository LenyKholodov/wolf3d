#ifndef __GAME_BOX__
#define __GAME_BOX__

#include "g_subj.h"

/////////////////////////////////////////////////////////////////////////////
///////��஡�� � 祬-� (���஭�, �����, �஭� � �.�)
/////////////////////////////////////////////////////////////////////////////
class   GameBox: public Subject
{
  public:
//                        GameBox  (int count = -1,
//                                  int x = -1,int y = -1,
//                                  int tsID   = 0,
//                                  int usesID = 0,
//                                  int sprID  = 0);
                        GameBox  (ArgPtr);
                        GameBox  (BinFile*);
         virtual        ~GameBox ();

////////////////////////////////////////////////////////////////////////////
//////�᭮��� ����ࠪ�� �㭪樨 ��� ��� ��஢�� ��ꥪ⮢
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*)       ;
    virtual     void            Save (BinFile*)       ;
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return GAME_BOX; }
    virtual     void            dump (ostream&) const;

        //�������������� �����-���� ��஡��
    virtual     BoxTypes        belong () const { return (BoxTypes)-1; }

    virtual     BOOL            Include (ID);
    virtual     void            Include (int count);
    virtual     void            Exclude (int x,int y);
    virtual     void            Use     ();  //state = ON
    virtual     void            Use     (int count);  //�ᯮ�짮���� ����� �� count

    virtual     void            AddStore(ID gameBox); //��������� �����

                void            refresh (BOOL close = FALSE);   //ᨭ�஭����� � ���⮩

                int             count   () const { return boxRes.count; }

  protected:
             GameBoxRes         boxRes;
};

/*class   Health: public GameBox
{
  public:
                        Health  (int count = PLAYER_HEALTH)
                         :GameBox (count,-1,-1,0,0,0) {}

    virtual     BoxTypes        belong () const { return HEALTH; }
};

class   Protect: public GameBox
{
  public:
                        Protect (int count = PLAYER_PROTECT)
                         :GameBox (count,-1,-1,0,0,0) {}

    virtual     BoxTypes        belong () const { return PROTECT; }
};*/

#endif