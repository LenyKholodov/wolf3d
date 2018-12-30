#ifndef __TYPE_WEAPON__
#define __TYPE_WEAPON__

#include "t_box.h"
#include "t_player.h"
#include "t_res.h"
#include "t_mes.h"

//////////////////���ᠭ�� �奬 ����ᮢ ��� ��㦨�//////////////////////////
/////////////////////////////////////////////////////////////////////////////
//////��������! �� ������ ���� ����㠫��� �㭪権
/////////////////////////////////////////////////////////////////////////////
struct  WeaponRes: public ResourceHeader
{
  struct Shot
  {
    int16        fly_sound;     //��� �����
    int16        bah_sound;     //��� ���뢠
    char         fly_anim [ANIM_NUM];   //A������ �����
    char         bah_anim [ANIM_NUM];   //������� ���뢠
    Fixed        speed;         //������� �����
    uchar        force;         //������� ᨫ�
    uchar        radius;        //������ ��ࠦ���� �� ���뢥
    Fixed        max_dist;      //���ᨬ��쭠� ���쭮��� ����⢨�
    int16        sprites;       //GameSprite number (in sprite cache)
    uchar        back_force;    //�������� �� ��ࠦ���� ��ப� (���ਬ�� ���㪠)
  }shot;

  BoxTypes       gameBox;       //��஡��
  int            speed;         //������� ��५��
  int            count;
  int            max;           //����� ��஡��

  int16          p_sound;          //��� ���ᨢ�
  char           p_anim [ANIM_NUM];//�������
  int16          p_changeTime;     //�६� ᬥ�� ���஢ �����樨 ���ᨢ�
  int16          a_sound;          //��� ��⨢�
  char           a_anim [ANIM_NUM];//
  int16          a_changeTime;     //�६� ᬥ�� ���஢ �����樨 ��⨢�
  int16          sprites;          //SpriteList number (in spriteList cache)
  int16          sprite3D;         //for the map render

     BOOL        Load (const char*);
     BOOL        Load (BinFile*);
     void        Save (const char*);
     void        Save (BinFile*);

};

struct  WeaponExempl
{
  ID    box;    //��஡�� � ���஭���
  int   count;  //�������⢮ ���஭�� (�� ��ࢮ� ��樠����樨)
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
/////��� ��� ��࠭���� Weapon
/////////////////////////////////////////////////////////////////////////////
struct    WeaponTag
{
  WeaponRes     res;
  WeaponExempl  exempl;
  int           last;   //�६����� ����� ????
  ID            owner;  //��ப ��������
  int           state;
  int           iterator;
  BOOL          shotOff;
};

/////////////////////////////////////////////////////////////////////////////
/////��� ��� ��࠭���� Shot
/////////////////////////////////////////////////////////////////////////////
struct    ShotTag
{
  WeaponRes::Shot  shot;
  Fixed            x ,y;
  Angle            angle;

  int              state;  //⥪��� �⠤�� ���뢠
  Fixed            dist;   //�ன������ ����ﭨ�

  int              oldIndex;

  Block            block;
  Block            oldBlock;

  ID               oldId;

  int              bahIndex, crossIndex;
  ID               owner;
};

/////////////////////////////����饭�� ��㦨�////////////////////////////////

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
////����� ��५��
/////////////////////////////////////////////////////////////////////////////
struct WeaponShotOnMessage: public GameMessage
{
        WeaponShotOnMessage () { id = WP_SHOT_ON; }
};

/////////////////////////////////////////////////////////////////////////////
////�४���� ��५��
/////////////////////////////////////////////////////////////////////////////
struct WeaponShotOffMessage: public GameMessage
{
        WeaponShotOffMessage () { id = WP_SHOT_OFF; }
};

/////////////////////////////////////////////////////////////////////////////
////��� ���஭��
/////////////////////////////////////////////////////////////////////////////
struct WeaponNoPatronMessage: public GameMessage
{
        WeaponNoPatronMessage (ID wp): weapon (wp) { id = WP_NO_PATRON; }

        ID      weapon;

};

/////////////////////////////////////////////////////////////////////////////
////����� 䠧� ��㦨� � �㪠� (���뫠�� ��㦨� ᮧ��⥫�)
/////////////////////////////////////////////////////////////////////////////
struct  WeaponChangeFaceMessage: public GameMessage
{
        WeaponChangeFaceMessage (int _face)
           : face (_face) { id = WP_ANIM_CHANGE;}

        int     face;   //���� ��㦨� � �㪠�
};

/////////////////////////////////////////////////////////////////////////////
////����� �� ������� ��㦨� � �㪠� (���뫠�� ��ப)
/////////////////////////////////////////////////////////////////////////////
struct  WeaponSetChangeModeMessage: public GameMessage
{
        WeaponSetChangeModeMessage (ID back) : owner (back)
           { id = WP_ANIM_MODE_ON;}

        ID      owner;  //�����䨪��� ��ப�
};

/////////////////////////////////////////////////////////////////////////////
////����� �� ������� ��㦨� � �㪠� (���뫠�� ��ப)
/////////////////////////////////////////////////////////////////////////////
struct  WeaponSetOffChangeModeMessage: public GameMessage
{
        WeaponSetOffChangeModeMessage (ID back) : owner (back)
           { id = WP_ANIM_MODE_OFF;}

        ID      owner;  //�����䨪��� ��ப�
};

/////////////////////////////////////////////////////////////////////////////
////�⢥� �� ������� ��㦨� � �㪠� (���뫠�� ��㦨�)
/////////////////////////////////////////////////////////////////////////////
struct  WeaponSetChangeModeOkMessage: public GameMessage
{
        WeaponSetChangeModeOkMessage (int _sprite,int _face)
                                     : sprite(_sprite), face (_face)
                    { id = WP_ANIM_MODE_ANSWER;}

        int     sprite, face;
};

/////////////////////////////////////////////////////////////////////////////
////� ᪮�쪮 � ��� ��⠫��� ���஭��?
/////////////////////////////////////////////////////////////////////////////
struct  WeaponGetCountMessage: public GameMessage
{
        WeaponGetCountMessage ()
        { id = WP_GET_COUNT; }
};

/////////////////////////////////////////////////////////////////////////////
////� ��� ��⠫��� N ���஭��
/////////////////////////////////////////////////////////////////////////////
struct  WeaponChangeCountMessage: public GameMessage
{
        WeaponChangeCountMessage (ID patronBox)
                     : box (patronBox)
        { id = WP_CHANGE_COUNT; }

        ID      box;    //��஡�� � ���஭���
};

/////////////////////////////////////////////////////////////////////////////
////��⠭���� ��஡��
/////////////////////////////////////////////////////////////////////////////
struct  WeaponSetBoxMessage: public GameMessage
{
        WeaponSetBoxMessage (ID patronBox)
                     : box (patronBox)
        { id = WP_SET_BOX; }

        ID      box;    //��஡�� � ���஭���
};

/////////////////////////////////////////////////////////////////////////////
////�஢�ઠ ���ﭨ� ��㦨�
/////////////////////////////////////////////////////////////////////////////
struct  WeaponInfoMessage: public GameMessage
{
        WeaponInfoMessage (const WeaponRes& x): res(x) { id = WP_INFO; }

        WeaponRes       res;
};

/////////////////////////////////////////////////////////////////////////////
////�஢�ઠ ���ﭨ� ��㦨� (�����)
/////////////////////////////////////////////////////////////////////////////
struct  WeaponGetInfoMessage: public GameMessage
{
        WeaponGetInfoMessage () { id = WP_GET_INFO; }
};

#endif