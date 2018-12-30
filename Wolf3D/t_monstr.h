#ifndef __MONSTR_RESOURCE__
#define __MONSTR_RESOURCE__

#include "t_res.h"
#include "t_wplr.h"

#include "t_iq.h"

////////////////////////////�������� ������/////////////////////////////////

typedef CreateWeaponPlayer CreateMonstr;

/////////////////////////////////////////////////////////////////////////////
/////���� ��� ������
/////////////////////////////////////////////////////////////////////////////

struct  Target
{
  ID            id;             //��뫪� �� 楫�
  Fixed         x, y;           //���न���� 楫�
  int           skin;           //⨯ ������ (� �� �᫨ �������?)
  int           IQ;             //��⥫��� 楫� ��� ��������� � ᥪ㭤�
                                //max IQ = 15
};

/////////////////////////////////////////////////////////////////////////////
/////Monstr tag
/////////////////////////////////////////////////////////////////////////////

struct  MonstrTag
{
  IQClass::Actives  curMethod [2];
  IQParam           param;
  IQClass           iqRes;
  Target            target;
  Fixed             speed;

  int               skin;
};

#endif