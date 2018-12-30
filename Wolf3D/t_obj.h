#ifndef __TYPE_GAME_OBJECTS__
#define __TYPE_GAME_OBJECTS__

#include "t_defs.h"

/////////////////////////////////////////////////////////////////////////////
////Типы игровых объектов
/////////////////////////////////////////////////////////////////////////////
////Внимание! не больше MAX_TYPES.
////Все объекты требующие загрузки должны быть зарегистрированы в "g_load.h"
/////////////////////////////////////////////////////////////////////////////
enum    GameObjects
{
        NULL_OBJECT = 0,
        RING,
        TYPE_TABLE,
        TEST_OBJECT,
        ERROR_OBJECT,
        MAIN_QUEUE,
        ACTIVE_LIST,
        GAME_MAP,
        GAME_SERVER,
        GAME_CLIENT,
        GAME_PLAYER,            //10
        INPUT_TRANSLATOR,
        GAME_VIEW,
        TRIGER,
        DOOR,
        SINGLE_GAME,
        SUBJECT,
        GAME_BOX,
        SHOT,
        WEAPON,
        WP_SHOTGUN,                 //20
        WEAPON_PLAYER,
        MEN,
        GAME_MONSTR,
        USER_PLAYER,
        TELEPORT,
        PORTAL,
        EFFECT,
        SPRITE_EFFECT,
        LIGHT_EFFECT,
        MULTIPLAYER,
        MULTI_GAME
};

        //строгое соответствие с типами!
static  ulong   gameTags [MAX_TYPES] =
{
        *((ulong*)"NULL"),
        *((ulong*)"RING"),
        *((ulong*)"TYPE"),
        *((ulong*)"TEST"),
        *((ulong*)"ERR "),
        *((ulong*)"MQ  "),
        *((ulong*)"ML  "),
        *((ulong*)"GMAP"),
        *((ulong*)"GSER"),
        *((ulong*)"GCLI"),
        *((ulong*)"PLAY"),
        *((ulong*)"INP "),
        *((ulong*)"VIEW"),
        *((ulong*)"TRIG"),
        *((ulong*)"DOOR"),
        *((ulong*)"GAME"),
        *((ulong*)"SUBJ"),
        *((ulong*)"BOX "),
        *((ulong*)"SHOT"),
        *((ulong*)"WEAP"),
        *((ulong*)"WPLY"),
        *((ulong*)"MEN "),
        *((ulong*)"MONS"),
        *((ulong*)"USER"),
        *((ulong*)"TELE"),
        *((ulong*)"PORT"),
        *((ulong*)"EFCT"),
        *((ulong*)"SEFT"),
        *((ulong*)"LIGH"),
        *((ulong*)"MULT"),
        *((ulong*)"MG  ")
};

/////////////////////////////////////////////////////////////////////////////
//////Таг для сохранения всех объектов
/////////////////////////////////////////////////////////////////////////////
struct  GameObjectTag
{
  ulong         tag;    //Таг для проверки корректности
};

#endif