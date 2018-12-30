#ifndef __GAME_TELEPORT__
#define __GAME_TELEPORT__

#include "g_object.h"

/////////////////////////////////////////////////////////////////////////////
////Телепортатор
/////////////////////////////////////////////////////////////////////////////
class   Teleport: public GameObject
{
  public:
                                Teleport(ArgPtr);
                                Teleport(BinFile*);
                  virtual       ~Teleport();

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return TELEPORT; }
    virtual     void            dump (ostream&) const;

////////////////////////////////////////////////////////////////////////////
//////Всё что относится к телепорту
////////////////////////////////////////////////////////////////////////////
    virtual     void            trans (ID transObject);
    virtual     void            tick  ();

                BOOL            cross () const { return telepPos.isCross; }
                void            cross (BOOL x) { telepPos.isCross = x;    }

  protected:
    virtual     void            refresh();

  protected:
             TeleportRes        telepRes;
             TeleportExempl     telepPos;

             int                animPos;
             int                last;
};

/////////////////////////////////////////////////////////////////////////////
/////Портал
/////////////////////////////////////////////////////////////////////////////
class   Portal: public Teleport
{
  public:
                                Portal(ArgPtr);
                                Portal(BinFile*);
                  virtual       ~Portal();

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);

    virtual     GameObjects     type () const  { return PORTAL; }
    virtual     void            dump (ostream&) const;

/////////////////////////////////////////////////////////////////////////////
/////Перемещение объекта в следующий уровень
/////////////////////////////////////////////////////////////////////////////
    virtual     void            trans (ID transObject);

  protected:
                char            levelName [128];

};


#endif
