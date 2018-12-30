#ifndef __GAME_SUBJECT__
#define __GAME_SUBJECT__

#include "g_object.h"

/////////////////////////////////////////////////////////////////////////////
//////Предмет (аптеска, оружие и т.д.) лежащий на карте
////////////////////////////////////////////////////////////////////////////
class   Subject: public GameObject
{
  public:
                        Subject  (ArgPtr);
                        Subject  (BinFile*);
          virtual       ~Subject ();

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*)     ;
    virtual     void            Save (BinFile*)     ;
    virtual     BOOL            Exec (GameMessage*) ;

    virtual     GameObjects     type () const  { return SUBJECT; }
    virtual     void            dump (ostream&) const;

/////////////////////////////////////////////////////////////////////////////
/////Предмет можно взять
/////////////////////////////////////////////////////////////////////////////
    virtual     BOOL            Include  (ID player);
                void            refresh (BOOL = FALSE);

                int             GetX() const { return x; }
                int             GetY() const { return y; }

  protected:
                void            Respawn ();

  protected:
                SubjectRes      subjRes;        //ресур-схема
                int             x,y;            //координаты
                clock_t         lastRespawn;    //время после взятия
};

#endif