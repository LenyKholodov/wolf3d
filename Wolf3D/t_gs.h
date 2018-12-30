#ifndef  __TYPE_GAME_SYSTEM__
#define  __TYPE_GAME_SYSTEM__

#include "t_obj.h"
#include "t_mes.h"

#include "map.h"

//////////////////////////��� GameSystem/////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//////����饭�� ��ᯥ���� �ࠢ����� ��⥬��
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
  SEQUENCE_OBJECT,              //����஭����� �� �⪥ (७���-����)
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
///////Call-back ᮧ��� ��ꥪ�
/////////////////////////////////////////////////////////////////////////////
struct  AlreadyCreateMessage: public GameMessage
{
        AlreadyCreateMessage (ID x):obj (x)  { id = ALREADY_CREATE; }

        ID      obj;    //����⢥��� - � �� ���뫠��
};

/////////////////////////////////////////////////////////////////////////////
///////Call-back ᮧ��� ��ꥪ�
/////////////////////////////////////////////////////////////////////////////
struct  AlreadyDeleteMessage: public AlreadyCreateMessage
{
        AlreadyDeleteMessage (ID x):AlreadyCreateMessage (x)
        { id = ALREADY_DELETE; }
};

/////////////////////////////////////////////////////////////////////////////
///////����� �� ᮧ����� ��ꥪ�; owner - ����易⥫�� call-back
/////////////////////////////////////////////////////////////////////////////
template <class CreateClass>
struct  CreateObjectMessage: public GameMessage
{
        CreateObjectMessage (ID owner = ID(-1,-1)):creater (owner)
                           { id  = CREATE_OBJECT; }

        ID            creater;        //�����뢠�騩 ��ꥪ�
        GameObjects   type;           //��� ��ꥪ�
        int           size;           //������ ⠡���� ��㬥�⮢
        CreateClass   arg;            //��㬥��� ��� ᮧ�����
};

        //��� ��⥬�
struct  __CreateObjectMessage: public GameMessage
{
        ID            creater;
        GameObjects   type;
        int           size;
        uchar         data[1];
};

/////////////////////////////////////////////////////////////////////////////
///////����� �� 㤠����� ��ꥪ�
/////////////////////////////////////////////////////////////////////////////
struct  DeleteObjectMessage: public GameMessage
{
        DeleteObjectMessage (ID x): obj(x) { id = DELETE_OBJECT; }

        ID      obj;
};

/////////////////////////////////////////////////////////////////////////////
///////����� �� ���������� � ��⨢
/////////////////////////////////////////////////////////////////////////////
struct  AddInActiveMessage: public GameMessage
{
        AddInActiveMessage (ID x,int prior = 15)
           :obj(x), priority (prior) { id  = ADD_IN_ACTIVE; }

        ID      obj;
        int     priority;       //�ਮ��� 0..32

};

/////////////////////////////////////////////////////////////////////////////
///////����� �� 㤠����� �� ��⨢�
/////////////////////////////////////////////////////////////////////////////
struct  DeleteFromActiveMessage: public GameMessage
{
        DeleteFromActiveMessage (ID x): obj(x) { id = DEL_FROM_ACTIVE; }

        ID      obj;
};

/////////////////////////////////////////////////////////////////////////////
///////���������� ������
/////////////////////////////////////////////////////////////////////////////
struct  AddClientMessage: public GameMessage
{
        AddClientMessage (ID x): client (x) { id = ADD_CLIENT; }

        ID      client;
};

/////////////////////////////////////////////////////////////////////////////
///////�������� ������
/////////////////////////////////////////////////////////////////////////////
struct  DelClientMessage: public GameMessage
{
        DelClientMessage (ID x): client (x) { id = DEL_CLIENT; }

        ID      client;
};

/////////////////////////////////////////////////////////////////////////////
///////����஭����� ���ﭨ� � �����⮢
/////////////////////////////////////////////////////////////////////////////
struct  SequenceObjectMessage: public GameMessage
{
        SequenceObjectMessage () { id = SEQUENCE_OBJECT; }
        SequenceObjectMessage (uint index,Block bl)
               : mapIndex(index), block (bl)
                              { id = SEQUENCE_OBJECT; }

        Block   block;          //����஭����� ७���  �����
        uint    mapIndex;       //����஭����� ��஢�� �����
};

struct  SequenceMapPtrMessage: public GameMessage
{
        SequenceMapPtrMessage (Map* x): map(x) { id = SEQUENCE_MAP_PTR; }

        Map*    map;
};

/////////////////////////////////////////////////////////////////////////////
///////���ந�������� ��㪮���� ᮮ�饭��
/////////////////////////////////////////////////////////////////////////////
struct  PlaySoundMessage: public GameMessage
{
        PlaySoundMessage (int soundId,int _x,int _y)
                         : sound (soundId), x(_x), y(_y)
        { id = PLAY_SOUND; }

        int     sound;      //���ਯ�� ��㪠
        int     x, y;       //���न���� ��ꥪ� �����饣� ���
};

/////////////////////////////////////////////////////////////////////////////
///////�������� �ࢥ� - ᮮ�饭�� �����⠬ "�������� ������� ����������"
/////////////////////////////////////////////////////////////////////////////
struct  HaltSystemMessage: public GameMessage
{
        HaltSystemMessage () { id = HALT_SYSTEM; }
};

/////////////////////////////////////////////////////////////////////////////
/////�� ����� ���, �㢠�?
/////////////////////////////////////////////////////////////////////////////
struct  EchoOnMessage: public GameMessage
{
        EchoOnMessage (ID x): object (x) { id = ECHO_ON; }

        ID      object;         //��ꥪ� �����訩 ���
};

/////////////////////////////////////////////////////////////////////////////
/////����㧨�� ��ꥪ�
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
////����㧪� ��ꥪ� �ந��諠 �ᯥ譮
/////////////////////////////////////////////////////////////////////////////
struct  AlreadyLoadMessage: public GameMessage
{
        AlreadyLoadMessage (ID _id,int _cache = -1,int _idres = -1)
             : object (_id), cacheId(_cache), idRes (_idres)
         { id = ALREADY_LOAD; }

        int     cacheId, idRes;
        ID      object;         //����㦥�� ��ꥪ�
};

/////////////////////////////////////////////////////////////////////////////
////��㧠
/////////////////////////////////////////////////////////////////////////////
struct  PauseGameMessage: public GameMessage
{
        PauseGameMessage ()
         { id = PAUSE_SYSTEM; }
};

/////////////////////////////////////////////////////////////////////////////
/////��� ��� ��࠭���� ��஢��� �ࢥ�
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