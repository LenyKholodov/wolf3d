#ifndef __TYPE_GAME_PLAYER__
#define __TYPE_GAME_PLAYER__

#include "t_mes.h"
#include "t_res.h"
#include "map.h"

/////////////////////////��� Player//////////////////////////////////////////

struct  PlayerRes: public ResourceHeader
{
  enum States {
       NORM = 0,
       CROSS_SHOT,
       ATACK,
       DEATH,

       LUCK,
       HEY,

       MAX_STATES   = 4,
       STATE_SIZE   = 4,
       END_OF_STATE = 255

  };

  int           mapSprite;                      //������

  int16         sound   [16];                   //��㪨
  char          sprite  [MAX_STATES][ANIM_NUM]; //�������

  uint16        iq      [IQ_MAX_PROBS];         //IQ
  Fixed         speed;                          //������� ����

     BOOL        Load (const char*);
     BOOL        Load (BinFile*);
     void        Save (const char*);
     void        Save (BinFile*) ;
};

struct  PlayerExempl
{
  int           health,
                protect;
  Fixed         x,y;
  Angle         angle;
};

/////////////////////////////////////////////////////////////////////////////
//////������� ��� ᮧ����� ��ப� (ArgPtr)
/////////////////////////////////////////////////////////////////////////////

typedef CreateExemplar <PlayerRes,PlayerExempl> CreatePlayer;

/////////////////////////////////////////////////////////////////////////////
/////��� ��� ��࠭���� ��ப�
/////////////////////////////////////////////////////////////////////////////
struct    PlayerTag
{
  PlayerExempl  exempl;
  PlayerRes     res;

  int        mStatePos;
  int        mState;
  BOOL       mDeath;

  Block      mBlock,
             mOldBlock;

  ID         mOldId;
};

////////////////////�����䨪���� ��஡�� � ���஭���///////////////////////

enum    BoxTypes
{
  HEALTH = 0,
  PROTECT,
  SHOTGUN
};

/////////////////////////////////////////////////////////////////////////////
/////����饭��
/////////////////////////////////////////////////////////////////////////////

                //�����-� ���� �� �� �������� ������
enum    PlayerMessagesId
{
  PLAYER_MOVE = GAME_PLAYER_LIMIT,      //63

  ADD_MOVE_FORWARD,
  SUB_MOVE_FORWARD,
  ADD_MOVE_BACK,
  SUB_MOVE_BACK,

  ADD_TURN_LEFT,
  SUB_TURN_LEFT,
  ADD_TURN_RIGHT,                       //70
  SUB_TURN_RIGHT,

  ADD_STRAFE_LEFT,
  SUB_STRAFE_LEFT,
  ADD_STRAFE_RIGHT,
  SUB_STRAFE_RIGHT,

  ADD_FIRE,
  SUB_FIRE,

  RESET_ACTIVE,

  GET_PLAYER_INFO,
  PLAYER_INFO,                          //80
  PRESS,

  CROSS_SHOT,
  LUCKY_SHOT,

  SLEEP_OFF,
  PLAYER_DEATH,

  I_SEE_YOU,

  HEY,
  ALARM
};

/////////////////////////////////////////////////////////////////////////////
//////��஢�� ᮡ�⨥ (��������� �����, strafe, OPEN)
/////////////////////////////////////////////////////////////////////////////
struct  PlayerMessage: public GameMessage
{
        PlayerMessage (int mesType)          { id =  mesType; }

        Angle           angle;
        Fixed           dist;
};

/////////////////////////////////////////////////////////////////////////////
//////������ ����
/////////////////////////////////////////////////////////////////////////////
struct  PlayerResetActiveMessage: public GameMessage
{
        PlayerResetActiveMessage() { id = RESET_ACTIVE; }
};

/////////////////////////////////////////////////////////////////////////////
//////����� � ���ﭨ�
/////////////////////////////////////////////////////////////////////////////
struct  GetPlayerInfoMessage: public GameMessage
{
        GetPlayerInfoMessage (ID dest): destId (dest) { id = GET_PLAYER_INFO; }

        ID      destId;
};

/////////////////////////////////////////////////////////////////////////////
//////������� ���ﭨ�
/////////////////////////////////////////////////////////////////////////////
struct  PlayerInfoMessage: public GameMessage
{
        PlayerInfoMessage (Fixed _x,Fixed _y,Angle _ang,int _h = 100,int _pr = 100,int sprite = -1)
               : x (_x), y(_y), angle(_ang), protect (_pr), health (_h), skin(sprite)
             { id = PLAYER_INFO; }

        Fixed      x, y;
        Angle      angle;

        int        health, protect;
        int        skin;                //��� ������ - ��� ��� ��ࠧ
};

/////////////////////////////////////////////////////////////////////////////
//////�㢠� - � ⥡� ������
/////////////////////////////////////////////////////////////////////////////
struct  PlayerCrossShot: public GameMessage
{
        PlayerCrossShot (int count = 0,ID x = ID(-1,-1))
            :subHealth (count), player(x)     { id = CROSS_SHOT; }

        int     subHealth;      //�� ᪮�쪮
        ID      player;         //�� ��५�
};

/////////////////////////////////////////////////////////////////////////////
//////��⪮ ��५���
/////////////////////////////////////////////////////////////////////////////
struct  PlayerLuckyShot: public GameMessage
{
        PlayerLuckyShot (ID x): dest (x) { id = LUCKY_SHOT; }

        ID      dest;   //� ���� �����
};

/////////////////////////////////////////////////////////////////////////////
//////����५ +
/////////////////////////////////////////////////////////////////////////////
struct  PlayerShotOn: public GameMessage
{
        PlayerShotOn () { id = ADD_FIRE; }
};

/////////////////////////////////////////////////////////////////////////////
//////����५ -
/////////////////////////////////////////////////////////////////////////////
struct  PlayerShotOff: public GameMessage
{
        PlayerShotOff () { id = SUB_FIRE; }
};

/////////////////////////////////////////////////////////////////////////////
//////����饭��� "��᭨��" ⮬� �� ᮧ��� ��ப�
/////////////////////////////////////////////////////////////////////////////
struct  PlayerSleepOff: public GameMessage
{
        PlayerSleepOff (ID sleep_off_chuv): player (sleep_off_chuv)
                { id = SLEEP_OFF; }

        ID      player;
};

/////////////////////////////////////////////////////////////////////////////
//////����饭�� "���� � ����!" ��।����� �ᯥ५�⥫� ��ப� ��� ��������
/////////////////////////////////////////////////////////////////////////////
struct  PlayerISeeYou: public GameMessage
{
        PlayerISeeYou (ID who): player (who)
                { id = I_SEE_YOU; }

        ID      player;
};

/////////////////////////////////////////////////////////////////////////////
//////������ ��ப�
/////////////////////////////////////////////////////////////////////////////
struct  PlayerDeath: public GameMessage
{
        PlayerDeath (ID __who__): beach (__who__) { id = PLAYER_DEATH; }

        ID      beach;  //��� ����訩 祫����, ����� 㡨� ��ப�
};

/////////////////////////////////////////////////////////////////////////////
//////��稬!
/////////////////////////////////////////////////////////////////////////////
struct  PlayerHeyMessage: public GameMessage
{
        PlayerHeyMessage () { id = HEY; }
};

/////////////////////////////////////////////////////////////////////////////
//////�ॢ���
/////////////////////////////////////////////////////////////////////////////
struct  PlayerAlarmMessage: public GameMessage
{
        PlayerAlarmMessage (ID _target,int _skin): skin(_skin), target (_target)
        { id = ALARM; }

        ID      target; //��� ���� �㦭� 㡨��
        int     skin;   //��� ������
};

#endif