#ifndef __GAME_IQ__
#define __GAME_IQ__

#include "g_wplyr.h"

#include <fixed.h>
#include <memory/memory.h>

///////////////////////Типы данных для искуственного интелекта///////////////

typedef uchar IQMark;   //enum 0..99

/////////////////////////////////////////////////////////////////////////////
////Структура для описания класса (!) интелектуального монстра
/////////////////////////////////////////////////////////////////////////////
struct  IQClass
{
        enum {
             ATACK,
             FIND,
             RUN_AWAY,
             GO,
             RANDOM,
             ALARM
        };

        unsigned  prob [IQ_MAX_PROBS];
};

/////////////////////////////////////////////////////////////////////////////
////Конечный автомат
/////////////////////////////////////////////////////////////////////////////

struct  IQParam
{
        Fixed           dist;           //расстояние до цели
        Angle           angle;          //угол между монстром и целью
        WeaponPlayer*   monstr;         //монстр
        Player*         target;         //цель
        GameSystem*     owner;          //игровой сервер
        int             tuda_suda;      //погрешность   0..255
};

class   GameIQ
{
  publuc:
        virtual IQMark  analyse (const IQParam&)   = 0;
        virtual void    process (const IQParam&)   = 0;
};

/////////////////////////////////////////////////////////////////////////////
//////////////////////Конечные автоматы для разных действий//////////////////
/////////////////////////////////////////////////////////////////////////////
        //Поворот к цели
class   TurnToTarget: public GameIQ
{
  public:
        virtual IQMark  analyse (Player*,Player*);
        virtual void    process (WeaponPlayer*,Player*,int);
};
        //Атака
class   AtackTarget: public GameIQ
{
  public:
        virtual IQMark  analyse (Player*,Player*);
        virtual void    process (WeaponPlayer*,Player*,int);
};
        //Преследование цели
class   FolowTarget: public GameIQ
{
  public:
        virtual IQMark  analyse (Player*,Player*);
        virtual void    process (WeaponPlayer*,Player*,int);
};
        //Убегание
class   RunAway: public GameIQ 
{
  public:
        virtual IQMark  analyse (Player*,Player*);
        virtual void    process (WeaponPlayer*,Player*,int);
};
        //Поиск
class   SearchForTarget: public GameIQ
{
  public:
        virtual IQMark  analyse (Player*,Player*);
        virtual void    process (WeaponPlayer*,Player*,int);
};
        //Случайное
class   RandomIQ: public GameIQ
{
  public:
        virtual IQMark  analyse (Player*,Player*);
        virtual void    process (WeaponPlayer*,Player*,int);
};

/////////////////////////////////////////////////////////////////////////////
////Система управления искуственным интелектом
/////////////////////////////////////////////////////////////////////////////
class   GameIQSystem
{
  public:
                        GameIQSystem  (const IQClass&);
                        ~GameIQSystem ();

/////////////////////////////////////////////////////////////////////////////
////Размышления и действия
/////////////////////////////////////////////////////////////////////////////
                void    think         (WeaponPlayer* monstr,
                                       Player* target,
                                       int targetIQ=0
                                       );

/////////////////////////////////////////////////////////////////////////////
////Загрузка - сохранение
/////////////////////////////////////////////////////////////////////////////
                void    Load    (BinFile*);
                void    Save    (BinFile*);

  protected:
    static      GameIQ*         table [IQ_MAX_PROBS];  //Таблица методов IQ
                IQClass         iqRes;                 //Шаблон данного монстра
};

#endif