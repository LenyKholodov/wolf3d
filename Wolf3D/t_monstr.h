#ifndef __MONSTR_RESOURCE__
#define __MONSTR_RESOURCE__

#include "t_res.h"
#include "t_wplr.h"

#include "t_iq.h"

////////////////////////////Создание монстра/////////////////////////////////

typedef CreateWeaponPlayer CreateMonstr;

/////////////////////////////////////////////////////////////////////////////
/////Цель для монстра
/////////////////////////////////////////////////////////////////////////////

struct  Target
{
  ID            id;             //ссылка на цель
  Fixed         x, y;           //координаты цели
  int           skin;           //тип монстра (а что если коллега?)
  int           IQ;             //интелект цели или попадания в секунду
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