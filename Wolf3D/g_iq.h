#ifndef __GAME_IQ__
#define __GAME_IQ__

#include <fixed.h>
#include <memory/memory.h>

#include "g_types.h"

class   GameIQ
{
  public:
        virtual IQMark           analyse (const IQParam&)   = 0;
        virtual IQClass::Actives process (const IQParam&)   = 0;
};

/////////////////////////////////////////////////////////////////////////////
//////////////////////������ ��⮬��� ��� ࠧ��� ����⢨�//////////////////
/////////////////////////////////////////////////////////////////////////////

BOOL    IsWallNear      (Player*,Map*);
BOOL    OpenDoor        (Player* monstr,const Block*);
void    TurnToAngle     (Player*,Angle);

        //�⠪�
class   AtackTarget: public GameIQ
{
  public:
        virtual IQMark            analyse (const IQParam&);
        virtual IQClass::Actives  process (const IQParam&);
};
        //����
class   FarFollowTarget: public GameIQ
{
  public:
        virtual IQMark            analyse (const IQParam&);
        virtual IQClass::Actives  process (const IQParam&);
};
        //������� ��᫥�������
class   NearFollowTarget: public GameIQ
{
  public:
        virtual IQMark            analyse (const IQParam&);
        virtual IQClass::Actives  process (const IQParam&);
};
        //Strafe-����
class   StrafeProtect: public GameIQ
{
  public:
        virtual IQMark            analyse (const IQParam&);
        virtual IQClass::Actives  process (const IQParam&);
};
        //�ᬠ�ਢ����� ����� ᥡ�
class   ViewOut: public GameIQ
{
  public:
        virtual IQMark            analyse (const IQParam&);
        virtual IQClass::Actives  process (const IQParam&);
};
        //���砩��� ��������
class   RandomMove: public GameIQ
{
  public:
        virtual IQMark            analyse (const IQParam&);
        virtual IQClass::Actives  process (const IQParam&);
};

#endif