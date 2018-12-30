#ifndef __GAME_MONSTR__
#define __GAME_MONSTR__

#include "g_player.h"
#include "g_iq.h"

////////////////////////////////////////////////////////////////////////////
//////Система, управляющая игроком (виртуальный интерфейс)
////////////////////////////////////////////////////////////////////////////
class   Men : public GameObject
{
  public:
                Men  (ArgPtr);
                Men  (BinFile*);
     virtual    ~Men ();

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return MEN; }
    virtual     void            dump (ostream&) const;

////////////////////////////////////////////////////////////////////////////
//////Искуственный интелект
////////////////////////////////////////////////////////////////////////////
    virtual     void            think () = 0;
    virtual     void            tick  () = 0;

    virtual     void            sleep   (BOOL = FALSE,ID wakeUp = ID(-1,-1));

                //Игрок, который зависит от монстра
                ID              player () const  { return mPlayer; }

    private:
                ID              mPlayer;
};

////////////////////////////////////////////////////////////////////////////
//////Монстр (тупой)
////////////////////////////////////////////////////////////////////////////
class    Monstr: public Men
{
  public:
                Monstr  (ArgPtr);
                Monstr  (BinFile*);
     virtual    ~Monstr ();

                enum SetEnum
                {
                  SET_ON  = 1,
                  SET_OFF =-1,
                  NOSET   = 0
                };

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return GAME_MONSTR; }
    virtual     void            dump (ostream&) const;

////////////////////////////////////////////////////////////////////////////
//////Искуственный интелект
////////////////////////////////////////////////////////////////////////////
    virtual     void            think () ;
    virtual     void            tick  () ;

    virtual     void            sleep   (BOOL,ID);

  protected:
    static      GameIQ*         table [IQ_MAX_PROBS];  //Таблица методов IQ
                IQParam         param;
                GameIQ*         curMethod;             //Текущее действие

                Target          target;                //Цель для монстра
                IQClass         iqRes;                 //Шаблон данного монстра

                clock_t         lastTime;

};

/////////////////////////////////////////////////////////////////////////////
///////Вооруженный игрок
/////////////////////////////////////////////////////////////////////////////
class   WeaponPlayer: public Player
{
  public:
                WeaponPlayer  (ArgPtr);
                WeaponPlayer  (BinFile*);
     virtual    ~WeaponPlayer ();

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return WEAPON_PLAYER; }

////////////////////////////////////////////////////////////////////////////
//////Функции относящиеся к управлению игроком
////////////////////////////////////////////////////////////////////////////
                void            Fire ();
    virtual     void            tick ();

  protected:
                ID              weapon;

};

extern  int     HARD_LEVEL;

#endif