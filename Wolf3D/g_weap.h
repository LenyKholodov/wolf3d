#ifndef __GAME_WEAPON__
#define __GAME_WEAPON__

#include "g_object.h"
#include "g_box.h"

/////////////////////////////////////////////////////////////////////////////
//////Снаряд
/////////////////////////////////////////////////////////////////////////////
class   Shot: public GameObject
{
  public:
                        Shot (ArgPtr);      //ptr = *Weaponres::Shot
                        Shot (BinFile*);
         virtual        ~Shot ();

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*) ;
    virtual     void            Save (BinFile*) ;
    virtual     BOOL            Exec (GameMessage*) ;

    virtual     GameObjects     type () const  { return SHOT; }
    virtual     void            dump (ostream&) const;

////////////////////////////////////////////////////////////////////////////
//////Функции относящиеся к снаряду
////////////////////////////////////////////////////////////////////////////
    virtual     void            tick    ();
    virtual     void            bah     (int);  //взрыв
                BOOL            IsCross ();     //есть ли столкновение

  protected:
                void            Present (int index) const;

  protected:
             Fixed              x, y;
             Angle              angle;
             WeaponRes::Shot    shotRes;

             int                state;  //текущая стадий взрыва
             Fixed              dist;   //пройденное расстояние

             int                oldIndex;

             Block              block;
             Block              oldBlock;

             ID                 oldId;

             int                bahIndex,
                                crossIndex;

             int                last;
};

/////////////////////////////////////////////////////////////////////////////
//////Оружие
/////////////////////////////////////////////////////////////////////////////
class   Weapon: public GameObject
{
  public:
                        Weapon (ArgPtr);
                        Weapon (BinFile*);
          virtual       ~Weapon ();

                        enum State {
                              PASSIVE = 0,
                              ACTIVE,
                              NO_ANIM = -1
                        };

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*) ;
    virtual     void            Save (BinFile*) ;
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return WEAPON; }
    virtual     void            dump (ostream&) const;

////////////////////////////////////////////////////////////////////////////
//////Относится только к оружию
////////////////////////////////////////////////////////////////////////////
    virtual     void            ShotOn  ();
    virtual     void            ShotOff ();
                void            SetBox  (ID);

    virtual     void            Fire    (Fixed,Fixed,Angle);
                void            refresh (BOOL);

////////////////////////////////////////////////////////////////////////////
//////Какой коробке пренадлежит оружие
////////////////////////////////////////////////////////////////////////////
    virtual     BoxTypes        belong ()      { return weapRes.gameBox; }

  protected:
             WeaponRes          weapRes;
             int                last;
             ID                 box;

             int                lastAnim;   //animation
             int                state;
             int                iterator;
             char*              faceTable    [2];
             int                faceAnimTime [2];

             BOOL               shotOff;

             int                weaponId;
};


#endif