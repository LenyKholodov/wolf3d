#ifndef __TYPE_WEAPON__
#define __TYPE_WEAPON__

#include "t_box.h"
#include "t_player.h"
#include "t_res.h"
#include "t_mes.h"

//////////////////Описания схем ресурсов для оружия//////////////////////////
/////////////////////////////////////////////////////////////////////////////
//////Внимание! Не должно быть виртуальных функций
/////////////////////////////////////////////////////////////////////////////
struct  WeaponRes: public ResourceHeader
{
  struct Shot
  {
    int16        fly_sound;     //Звук полёта
    int16        bah_sound;     //Звук взрыва
    char         fly_anim [ANIM_NUM];   //Aнимация полёта
    char         bah_anim [ANIM_NUM];   //Анимация взрыва
    Fixed        speed;         //Скорость полёта
    uchar        force;         //Убойная сила
    uchar        radius;        //Радиус поражения при взрыве
    Fixed        max_dist;      //Максимальная дальность действия
    int16        sprites;       //GameSprite number (in sprite cache)
    uchar        back_force;    //Возможно ли поражение игрока (например базука)
  }shot;

  BoxTypes       gameBox;       //Коробка
  int            speed;         //Скорость стрельбы
  int            count;
  int            max;           //Лимит коробки

  int16          p_sound;          //Звук пассива
  char           p_anim [ANIM_NUM];//Анимация
  int16          p_changeTime;     //Время смены кадров анимации пассива
  int16          a_sound;          //Звук актива
  char           a_anim [ANIM_NUM];//
  int16          a_changeTime;     //Время смены кадров анимации актива
  int16          sprites;          //SpriteList number (in spriteList cache)
  int16          sprite3D;         //for the map render

     BOOL        Load (const char*);
     BOOL        Load (BinFile*);
     void        Save (const char*);
     void        Save (BinFile*);

};

struct  WeaponExempl
{
  ID    box;    //коробка с патронами
  int   count;  //колличество патронов (при первой инциализации)
};

typedef CreateExemplar <WeaponRes,WeaponExempl> CreateWeapon;

struct  ShotExempl
{
  Fixed                 x ,y;
  Angle                 angle;
  ID                    owner;
};

typedef CreateExemplar <WeaponRes::Shot,ShotExempl> CreateShot;

/////////////////////////////////////////////////////////////////////////////
/////Таг для сохранения Weapon
/////////////////////////////////////////////////////////////////////////////
struct    WeaponTag
{
  WeaponRes     res;
  WeaponExempl  exempl;
  int           last;   //временной барьер ????
  ID            owner;  //игрок владелец
  int           state;
  int           iterator;
  BOOL          shotOff;
};

/////////////////////////////////////////////////////////////////////////////
/////Таг для сохранения Shot
/////////////////////////////////////////////////////////////////////////////
struct    ShotTag
{
  WeaponRes::Shot  shot;
  Fixed            x ,y;
  Angle            angle;

  int              state;  //текущая стадий взрыва
  Fixed            dist;   //пройденное расстояние

  int              oldIndex;

  Block            block;
  Block            oldBlock;

  ID               oldId;

  int              bahIndex, crossIndex;
  ID               owner;
};

/////////////////////////////Сообщения оружию////////////////////////////////

enum WeaponMessagesId
{
  WP_SHOT_ON = WEAPON_LIMIT,
  WP_SHOT_OFF,
  WP_NO_PATRON,
  WP_ANIM_MODE_ON,
  WP_ANIM_MODE_OFF,
  WP_ANIM_MODE_ANSWER,
  WP_ANIM_CHANGE,
  WP_CHANGE_COUNT,
  WP_GET_INFO,
  WP_INFO,
  WP_GET_COUNT,
  WP_SET_BOX
};

/////////////////////////////////////////////////////////////////////////////
////Начать стрельбу
/////////////////////////////////////////////////////////////////////////////
struct WeaponShotOnMessage: public GameMessage
{
        WeaponShotOnMessage () { id = WP_SHOT_ON; }
};

/////////////////////////////////////////////////////////////////////////////
////Прекратить стрельбу
/////////////////////////////////////////////////////////////////////////////
struct WeaponShotOffMessage: public GameMessage
{
        WeaponShotOffMessage () { id = WP_SHOT_OFF; }
};

/////////////////////////////////////////////////////////////////////////////
////Нет патронов
/////////////////////////////////////////////////////////////////////////////
struct WeaponNoPatronMessage: public GameMessage
{
        WeaponNoPatronMessage (ID wp): weapon (wp) { id = WP_NO_PATRON; }

        ID      weapon;

};

/////////////////////////////////////////////////////////////////////////////
////Какая фаза оружия в руках (посылает оружие создателю)
/////////////////////////////////////////////////////////////////////////////
struct  WeaponChangeFaceMessage: public GameMessage
{
        WeaponChangeFaceMessage (int _face)
           : face (_face) { id = WP_ANIM_CHANGE;}

        int     face;   //Фаза оружия в руках
};

/////////////////////////////////////////////////////////////////////////////
////Запрос на анимацию оружия в руках (посылает игрок)
/////////////////////////////////////////////////////////////////////////////
struct  WeaponSetChangeModeMessage: public GameMessage
{
        WeaponSetChangeModeMessage (ID back) : owner (back)
           { id = WP_ANIM_MODE_ON;}

        ID      owner;  //Идентификатор игрока
};

/////////////////////////////////////////////////////////////////////////////
////Запрос на анимацию оружия в руках (посылает игрок)
/////////////////////////////////////////////////////////////////////////////
struct  WeaponSetOffChangeModeMessage: public GameMessage
{
        WeaponSetOffChangeModeMessage (ID back) : owner (back)
           { id = WP_ANIM_MODE_OFF;}

        ID      owner;  //Идентификатор игрока
};

/////////////////////////////////////////////////////////////////////////////
////Ответ на анимацию оружия в руках (посылает оружие)
/////////////////////////////////////////////////////////////////////////////
struct  WeaponSetChangeModeOkMessage: public GameMessage
{
        WeaponSetChangeModeOkMessage (int _sprite,int _face)
                                     : sprite(_sprite), face (_face)
                    { id = WP_ANIM_MODE_ANSWER;}

        int     sprite, face;
};

/////////////////////////////////////////////////////////////////////////////
////А сколько у нас осталось патронов?
/////////////////////////////////////////////////////////////////////////////
struct  WeaponGetCountMessage: public GameMessage
{
        WeaponGetCountMessage ()
        { id = WP_GET_COUNT; }
};

/////////////////////////////////////////////////////////////////////////////
////У нас осталось N патронов
/////////////////////////////////////////////////////////////////////////////
struct  WeaponChangeCountMessage: public GameMessage
{
        WeaponChangeCountMessage (ID patronBox)
                     : box (patronBox)
        { id = WP_CHANGE_COUNT; }

        ID      box;    //Коробка с патронами
};

/////////////////////////////////////////////////////////////////////////////
////Установка коробки
/////////////////////////////////////////////////////////////////////////////
struct  WeaponSetBoxMessage: public GameMessage
{
        WeaponSetBoxMessage (ID patronBox)
                     : box (patronBox)
        { id = WP_SET_BOX; }

        ID      box;    //Коробка с патронами
};

/////////////////////////////////////////////////////////////////////////////
////Проверка состояния оружия
/////////////////////////////////////////////////////////////////////////////
struct  WeaponInfoMessage: public GameMessage
{
        WeaponInfoMessage (const WeaponRes& x): res(x) { id = WP_INFO; }

        WeaponRes       res;
};

/////////////////////////////////////////////////////////////////////////////
////Проверка состояния оружия (запрос)
/////////////////////////////////////////////////////////////////////////////
struct  WeaponGetInfoMessage: public GameMessage
{
        WeaponGetInfoMessage () { id = WP_GET_INFO; }
};

#endif