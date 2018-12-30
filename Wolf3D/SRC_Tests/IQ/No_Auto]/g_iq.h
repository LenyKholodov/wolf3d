#ifndef __GAME_IQ__
#define __GAME_IQ__

#include <fixed.h>
#include <memory/memory.h>

#include "g_system.h"

typedef unsigned IQMark ;

/////////////////////////////////////////////////////////////////////////////
////Структура для описания класса (!) интелектуального монстра
/////////////////////////////////////////////////////////////////////////////
struct  IQClass
{
        enum Actives {
             ATACK,
             GO,
             TURN_ANGLE,
             FIND,
             RANDOM
        };

        unsigned  prob [IQ_MAX_PROBS];
};

/////////////////////////////////////////////////////////////////////////////
////Конечный автомат
/////////////////////////////////////////////////////////////////////////////

class   WeaponPlayer;
class   Player;

struct  IQParam
{
        Fixed                   dist;           //расстояние до цели
        Angle                   angle;          //угол между монстром и целью
        WeaponPlayer*           monstr;         //монстр
        Player*                 target;         //цель
        GameSystem*             owner;          //игровой сервер
        int                     tuda_suda;      //погрешность   0..255
};

class   GameIQ
{
  public:
        virtual IQMark           analyse (const IQParam&)   = 0;
        virtual void             process (const IQParam&)   = 0;
};

/////////////////////////////////////////////////////////////////////////////
//////////////////////Конечные автоматы для разных действий//////////////////
/////////////////////////////////////////////////////////////////////////////
        //Поворот к цели
class   TurnToTarget: public GameIQ
{
  public:
        virtual IQMark            analyse (const IQParam&);
        virtual void              process (const IQParam&);
};
        //Атака
class   AtackTarget: public GameIQ
{
  public:
        virtual IQMark            analyse (const IQParam&);
        virtual void              process (const IQParam&);
};
        //Преследование цели
class   FolowTarget: public GameIQ
{
  public:
        virtual IQMark            analyse (const IQParam&);
        virtual void              process (const IQParam&);
};
        //Поиск
class   SearchForTarget: public GameIQ
{
  public:
        virtual IQMark            analyse (const IQParam&);
        virtual void              process (const IQParam&);
};
        //Случайное
class   RandomIQ: public GameIQ
{
  public:
        virtual IQMark            analyse (const IQParam&);
        virtual void              process (const IQParam&);
};


#endif