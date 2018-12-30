#ifndef __GAME_BOX__
#define __GAME_BOX__

#include "g_subj.h"

/////////////////////////////////////////////////////////////////////////////
///////Коробка с чем-то (патроны, жизнь, броня и т.д)
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
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*)       ;
    virtual     void            Save (BinFile*)       ;
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return GAME_BOX; }
    virtual     void            dump (ostream&) const;

        //ПРенадлежность какой-либо коробке
    virtual     BoxTypes        belong () const { return (BoxTypes)-1; }

    virtual     BOOL            Include (ID);
    virtual     void            Include (int count);
    virtual     void            Exclude (int x,int y);
    virtual     void            Use     ();  //state = ON
    virtual     void            Use     (int count);  //использовать ресурс на count

    virtual     void            AddStore(ID gameBox); //пополнить запас

                void            refresh (BOOL close = FALSE);   //синхронизация с картой

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