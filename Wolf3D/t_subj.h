#ifndef __TYPE_SUBJECT__
#define __TYPE_SUBJECT__

#include "t_mes.h"
#include "t_defs.h"
#include "t_res.h"

#include <binfile.h>

////////////////////////////Схема ресурса для предмета///////////////////////

struct  SubjectRes:public ResourceHeader
{
        //Для отображения на карте
  int           mapSprite;
  int           giveSound;

        //Приданное объекту - подключателю
  int           count;          //кол-во...
  int           gameCache, id;  //ссылка на файл с ресурсом
  GameObjects   type;           //тип того что берёшь
  int           boxClass;       //номер коробки с патронами (точнее класс оружия)

        //Respawn
  clock_t       respawnTime;    //время для respawn'a
  int           respawnSound;   //звук respawn'a

     BOOL        Load (const char*);
     BOOL        Load (BinFile*);
     void        Save (const char*);
     void        Save (BinFile*);
};

////////////////////////////Описание экземпляра предмета/////////////////////

struct  SubjectExempl
{
  int           x,y;
};

////////////////////////////Таг для сохранения предмета//////////////////////

struct  SubjectTag
{
  SubjectRes    res;
  SubjectExempl exempl;
};

/////////////////////////////Создание предмета///////////////////////////////

typedef CreateExemplar <SubjectRes,SubjectExempl> CreateSubject;

////////////////////////////Сообщения предмету///////////////////////////////

enum SubjectMessageId
{
  INCLUDE = SUBJECT_LIMIT,
  EXCLUDE,
  USE,
  ALREADY_INCLUDE,
  GET_SUBJECT_INFO,
  SUBJECT_INFO
};

struct  SubjectGetInfoMessage: public GameMessage
{
        SubjectGetInfoMessage (ID x):dest(x) { id = GET_SUBJECT_INFO; }

        ID      dest;   //Кто спрашивал
};

struct  SubjectInfoMessage: public GameMessage
{
        SubjectInfoMessage (const SubjectRes& x): res(x) { id = SUBJECT_INFO; }

        SubjectRes      res;    //Ресурс схема
};

struct  SubjectIncludeMessage: public GameMessage
{
        SubjectIncludeMessage (ID owner): playerOwner (owner)
           { id = INCLUDE; }

        ID      playerOwner;
};

struct  SubjectExcludeMessage: public GameMessage
{
        SubjectExcludeMessage () { id = EXCLUDE; }
};

struct  SubjectUseMessage: public GameMessage
{
        SubjectUseMessage () { id = USE; }
};

struct  SubjectIncludeOkMessage: public GameMessage
{
        SubjectIncludeOkMessage (const SubjectRes& x)
           :res (x)
          { id = ALREADY_INCLUDE; }

        SubjectRes      res;    //Ресурс схема
};

#endif