#ifndef __GAME_IQ_TYPES__
#define __GAME_IQ_TYPES__

#include "t_defs.h"

typedef unsigned IQMark ;

/////////////////////////////////////////////////////////////////////////////
////������� ��� ���ᠭ�� ����� (!) ��⥫���㠫쭮�� ������
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
////������ ��⮬��
/////////////////////////////////////////////////////////////////////////////

class   WeaponPlayer;
class   Player;
class   GameSystem;

struct  IQParam
{
        GameSystem*             owner;          //��஢�� �ࢥ�
        WeaponPlayer*           monstr;         //������
        ID                      target;         //����
        Fixed                   dist;           //����ﭨ� �� 楫�
        Angle                   angle;          //㣮� ����� �����஬ � 楫��
        Fixed                   step;           //蠣 ������
        int                     tuda_suda;      //����譮���   0..255
};


#endif