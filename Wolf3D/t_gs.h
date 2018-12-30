#ifndef  __TYPE_GAME_SYSTEM__
#define  __TYPE_GAME_SYSTEM__

#include "t_obj.h"
#include "t_mes.h"

#include "map.h"

//////////////////////////Для GameSystem/////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//////Сообщения диспетчеру управления системой
/////////////////////////////////////////////////////////////////////////////

enum    GameSystemMessagesId
{
  CREATE_OBJECT =       GAME_SYSTEM_LIMIT,      //0
  DELETE_OBJECT,
  ALREADY_CREATE,
  ALREADY_DELETE,
  ALREADY_LOAD,
  ADD_IN_ACTIVE,
  DEL_FROM_ACTIVE,
  SEQUENCE_OBJECT,              //Синхронизация по сетке (рендер-карта)
  SEQUENCE_MAP_PTR,
  ADD_CLIENT,
  DEL_CLIENT,                                   //10
  LOAD_OBJECT,
  PAUSE_SYSTEM,
  HALT_SYSTEM,
  PLAY_SOUND,
  ECHO_ON
};

/////////////////////////////////////////////////////////////////////////////
///////Call-back создан объект
/////////////////////////////////////////////////////////////////////////////
struct  AlreadyCreateMessage: public GameMessage
{
        AlreadyCreateMessage (ID x):obj (x)  { id = ALREADY_CREATE; }

        ID      obj;    //Собственно - а кто посылает
};

/////////////////////////////////////////////////////////////////////////////
///////Call-back создан объект
/////////////////////////////////////////////////////////////////////////////
struct  AlreadyDeleteMessage: public AlreadyCreateMessage
{
        AlreadyDeleteMessage (ID x):AlreadyCreateMessage (x)
        { id = ALREADY_DELETE; }
};

/////////////////////////////////////////////////////////////////////////////
///////Запрос на создание объекта; owner - необязательный call-back
/////////////////////////////////////////////////////////////////////////////
template <class CreateClass>
struct  CreateObjectMessage: public GameMessage
{
        CreateObjectMessage (ID owner = ID(-1,-1)):creater (owner)
                           { id  = CREATE_OBJECT; }

        ID            creater;        //Заказывающий объект
        GameObjects   type;           //Тип объекта
        int           size;           //Размер таблицы аргументов
        CreateClass   arg;            //Аргументы для создания
};

        //Для системы
struct  __CreateObjectMessage: public GameMessage
{
        ID            creater;
        GameObjects   type;
        int           size;
        uchar         data[1];
};

/////////////////////////////////////////////////////////////////////////////
///////Запрос на удаление объекта
/////////////////////////////////////////////////////////////////////////////
struct  DeleteObjectMessage: public GameMessage
{
        DeleteObjectMessage (ID x): obj(x) { id = DELETE_OBJECT; }

        ID      obj;
};

/////////////////////////////////////////////////////////////////////////////
///////Запрос на добавление в актив
/////////////////////////////////////////////////////////////////////////////
struct  AddInActiveMessage: public GameMessage
{
        AddInActiveMessage (ID x,int prior = 15)
           :obj(x), priority (prior) { id  = ADD_IN_ACTIVE; }

        ID      obj;
        int     priority;       //Приоритет 0..32

};

/////////////////////////////////////////////////////////////////////////////
///////Запрос на удаление из актива
/////////////////////////////////////////////////////////////////////////////
struct  DeleteFromActiveMessage: public GameMessage
{
        DeleteFromActiveMessage (ID x): obj(x) { id = DEL_FROM_ACTIVE; }

        ID      obj;
};

/////////////////////////////////////////////////////////////////////////////
///////Добавление клиента
/////////////////////////////////////////////////////////////////////////////
struct  AddClientMessage: public GameMessage
{
        AddClientMessage (ID x): client (x) { id = ADD_CLIENT; }

        ID      client;
};

/////////////////////////////////////////////////////////////////////////////
///////Удаление клиента
/////////////////////////////////////////////////////////////////////////////
struct  DelClientMessage: public GameMessage
{
        DelClientMessage (ID x): client (x) { id = DEL_CLIENT; }

        ID      client;
};

/////////////////////////////////////////////////////////////////////////////
///////Синхронизация состояния у клиентов
/////////////////////////////////////////////////////////////////////////////
struct  SequenceObjectMessage: public GameMessage
{
        SequenceObjectMessage () { id = SEQUENCE_OBJECT; }
        SequenceObjectMessage (uint index,Block bl)
               : mapIndex(index), block (bl)
                              { id = SEQUENCE_OBJECT; }

        Block   block;          //Синхронизация рендер  карты
        uint    mapIndex;       //Синхронизация игровой карты
};

struct  SequenceMapPtrMessage: public GameMessage
{
        SequenceMapPtrMessage (Map* x): map(x) { id = SEQUENCE_MAP_PTR; }

        Map*    map;
};

/////////////////////////////////////////////////////////////////////////////
///////Воспроизведение звукового сообщения
/////////////////////////////////////////////////////////////////////////////
struct  PlaySoundMessage: public GameMessage
{
        PlaySoundMessage (int soundId,int _x,int _y)
                         : sound (soundId), x(_x), y(_y)
        { id = PLAY_SOUND; }

        int     sound;      //Дескриптор звука
        int     x, y;       //Координаты объекта издающего звук
};

/////////////////////////////////////////////////////////////////////////////
///////Удаление сервера - сообщения клиентам "НАСТУПИЛ БОЛЬШОЙ ДИСКОННЕКТ"
/////////////////////////////////////////////////////////////////////////////
struct  HaltSystemMessage: public GameMessage
{
        HaltSystemMessage () { id = HALT_SYSTEM; }
};

/////////////////////////////////////////////////////////////////////////////
/////Ты слышишь звук, чувак?
/////////////////////////////////////////////////////////////////////////////
struct  EchoOnMessage: public GameMessage
{
        EchoOnMessage (ID x): object (x) { id = ECHO_ON; }

        ID      object;         //Объект издавший звук
};

/////////////////////////////////////////////////////////////////////////////
/////Загрузить объект
/////////////////////////////////////////////////////////////////////////////
template <class Type>
struct  LoadObjectMessage: public    GameMessage
{
        LoadObjectMessage (int _cache,int _id,Type _param)
          : cacheId (_cache), idRes (_id), param(_param), parSize (sizeof(Type))
                   { id = LOAD_OBJECT; }

        int              cacheId;
        int              idRes;
        int              parSize;
        Type             param;
};

struct  _LoadObjectMessage: public    GameMessage
{
        int              cacheId;
        int              idRes;
        int              parSize;
        char             data [1];
};


/////////////////////////////////////////////////////////////////////////////
////Загрузка объекта произошла успешно
/////////////////////////////////////////////////////////////////////////////
struct  AlreadyLoadMessage: public GameMessage
{
        AlreadyLoadMessage (ID _id,int _cache = -1,int _idres = -1)
             : object (_id), cacheId(_cache), idRes (_idres)
         { id = ALREADY_LOAD; }

        int     cacheId, idRes;
        ID      object;         //Загруженый объект
};

/////////////////////////////////////////////////////////////////////////////
////Пауза
/////////////////////////////////////////////////////////////////////////////
struct  PauseGameMessage: public GameMessage
{
        PauseGameMessage ()
         { id = PAUSE_SYSTEM; }
};

/////////////////////////////////////////////////////////////////////////////
/////Таг для сохранения игрового сервера
/////////////////////////////////////////////////////////////////////////////
struct  ServerTag: public GameObjectTag
{
                    ServerTag () { tag = SERVER_TAG; }

  ID                gameServer;
  ID                gameActiveList;
  ID                clientMap  [MAX_CLIENTS];
  int               clientCount;
  BOOL              pauseState;
  ID                reTrans;
};

#endif