#ifndef __GAME_IQ__
#define __GAME_IQ__

#include <fixed.h>
#include <memory/memory.h>

#include "g_system.h"

/////////////////////////////////////////////////////////////////////////////
////������� ��� ���ᠭ�� ����� (!) ��⥫���㠫쭮�� ������
/////////////////////////////////////////////////////////////////////////////
struct  IQClass
{
        enum Actives {
             ATACK,
             GO,             
             TURN_ANGLE,
             FIND,
             RANDOM,
             OPEN_DOOR,
             CHANGE_DIR
        };

        unsigned  prob [IQ_MAX_PROBS];
};

/////////////////////////////////////////////////////////////////////////////
////������ ��⮬��
/////////////////////////////////////////////////////////////////////////////

class   WeaponPlayer;
class   Player;

struct  IQParam
{
        IQClass::Actives        previos;        //�।��饥 ����⢨�
        Fixed                   dist;           //����ﭨ� �� 楫�
        Angle                   angle;          //㣮� ����� �����஬ � 楫��
        WeaponPlayer*           monstr;         //������
        Player*                 target;         //楫�
        GameSystem*             owner;          //��஢�� �ࢥ�
        int                     tuda_suda;      //����譮���   0..255
};

class   GameIQ
{
  public:
        virtual IQClass::Actives analyse (const IQParam&)   = 0;
        virtual void             process (const IQParam&)   = 0;
};

/////////////////////////////////////////////////////////////////////////////
//////////////////////������ ��⮬��� ��� ࠧ��� ����⢨�//////////////////
/////////////////////////////////////////////////////////////////////////////
        //������ � 楫�
class   TurnToTarget: public GameIQ
{
  public:
        virtual IQClass::Actives  analyse (const IQParam&);
        virtual void              process (const IQParam&);
};
        //�⠪�
class   AtackTarget: public GameIQ
{
  public:
        virtual IQClass::Actives  analyse (const IQParam&);
        virtual void              process (const IQParam&);
};
        //��᫥������� 楫�
class   FolowTarget: public GameIQ
{
  public:
        virtual IQClass::Actives  analyse (const IQParam&);
        virtual void              process (const IQParam&);
};
        //��������
class   RunAway: public GameIQ
{
  public:
        virtual IQClass::Actives  analyse (const IQParam&);
        virtual void              process (const IQParam&);
};
        //����
class   SearchForTarget: public GameIQ
{
  public:
        virtual IQClass::Actives  analyse (const IQParam&);
        virtual void              process (const IQParam&);
};
        //���砩���
class   RandomIQ: public GameIQ
{
  public:
        virtual IQClass::Actives  analyse (const IQParam&);
        virtual void              process (const IQParam&);
};
        //����⨥ ����
class   OpenDoor: public GameIQ
{
  public:
        virtual IQClass::Actives  analyse (const IQParam&);
        virtual void              process (const IQParam&);
};
        //��������� ���ࠢ����� ��������
class   ChangeDir: public GameIQ
{
  public:
        virtual IQClass::Actives  analyse (const IQParam&);
        virtual void              process (const IQParam&);
};

#endif