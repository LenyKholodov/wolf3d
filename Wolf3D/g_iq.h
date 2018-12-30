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
//////////////////////Конечные автоматы для разных действий//////////////////
/////////////////////////////////////////////////////////////////////////////

BOOL    IsWallNear      (Player*,Map*);
BOOL    OpenDoor        (Player* monstr,const Block*);
void    TurnToAngle     (Player*,Angle);

        //Атака
class   AtackTarget: public GameIQ
{
  public:
        virtual IQMark            analyse (const IQParam&);
        virtual IQClass::Actives  process (const IQParam&);
};
        //Поиск
class   FarFollowTarget: public GameIQ
{
  public:
        virtual IQMark            analyse (const IQParam&);
        virtual IQClass::Actives  process (const IQParam&);
};
        //Ближнее преследование
class   NearFollowTarget: public GameIQ
{
  public:
        virtual IQMark            analyse (const IQParam&);
        virtual IQClass::Actives  process (const IQParam&);
};
        //Strafe-защита
class   StrafeProtect: public GameIQ
{
  public:
        virtual IQMark            analyse (const IQParam&);
        virtual IQClass::Actives  process (const IQParam&);
};
        //Осматривается вокруг себя
class   ViewOut: public GameIQ
{
  public:
        virtual IQMark            analyse (const IQParam&);
        virtual IQClass::Actives  process (const IQParam&);
};
        //Случайное движение
class   RandomMove: public GameIQ
{
  public:
        virtual IQMark            analyse (const IQParam&);
        virtual IQClass::Actives  process (const IQParam&);
};

#endif