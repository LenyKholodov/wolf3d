#ifndef __GAME_IQ_TYPES__
#define __GAME_IQ_TYPES__

#include "t_defs.h"

typedef unsigned IQMark ;

/////////////////////////////////////////////////////////////////////////////
////Структура для описания класса (!) интелектуального монстра
/////////////////////////////////////////////////////////////////////////////
struct  IQClass
{
        enum Actives {
             ATACK,
             FAR_FOLLOW,
             NEAR_FOLLOW,
             VIEW_OUT,
             RANDOM_MOVE
        };

        unsigned  prob [IQ_MAX_PROBS];
};

/////////////////////////////////////////////////////////////////////////////
////Конечный автомат
/////////////////////////////////////////////////////////////////////////////

class   WeaponPlayer;
class   Player;
class   GameSystem;

struct  IQParam
{
        GameSystem*             owner;          //игровой сервер
        WeaponPlayer*           monstr;         //монстр
        ID                      target;         //Цель
        Fixed                   dist;           //расстояние до цели
        Angle                   angle;          //угол между монстром и целью
        Fixed                   step;           //шаг монстра
        int                     tuda_suda;      //погрешность   0..255
};


#endif