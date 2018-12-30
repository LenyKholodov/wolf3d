#ifndef __GAME_IQ__
#define __GAME_IQ__

#include "g_wplyr.h"

#include <fixed.h>
#include <memory/memory.h>

///////////////////////���� ������ ��� ����⢥����� ��⥫���///////////////

typedef uchar IQMark;   //enum 0..99

/////////////////////////////////////////////////////////////////////////////
////������� ��� ���ᠭ�� ����� (!) ��⥫���㠫쭮�� ������
/////////////////////////////////////////////////////////////////////////////
struct  IQClass
{
        enum {
             ATACK,
             FIND,
             RUN_AWAY,
             GO,
             RANDOM,
             ALARM
        };

        unsigned  prob [IQ_MAX_PROBS];
};

/////////////////////////////////////////////////////////////////////////////
////������ ��⮬��
/////////////////////////////////////////////////////////////////////////////

struct  IQParam
{
        Fixed           dist;           //����ﭨ� �� 楫�
        Angle           angle;          //㣮� ����� �����஬ � 楫��
        WeaponPlayer*   monstr;         //������
        Player*         target;         //楫�
        GameSystem*     owner;          //��஢�� �ࢥ�
        int             tuda_suda;      //����譮���   0..255
};

class   GameIQ
{
  publuc:
        virtual IQMark  analyse (const IQParam&)   = 0;
        virtual void    process (const IQParam&)   = 0;
};

/////////////////////////////////////////////////////////////////////////////
//////////////////////������ ��⮬��� ��� ࠧ��� ����⢨�//////////////////
/////////////////////////////////////////////////////////////////////////////
        //������ � 楫�
class   TurnToTarget: public GameIQ
{
  public:
        virtual IQMark  analyse (Player*,Player*);
        virtual void    process (WeaponPlayer*,Player*,int);
};
        //�⠪�
class   AtackTarget: public GameIQ
{
  public:
        virtual IQMark  analyse (Player*,Player*);
        virtual void    process (WeaponPlayer*,Player*,int);
};
        //��᫥������� 楫�
class   FolowTarget: public GameIQ
{
  public:
        virtual IQMark  analyse (Player*,Player*);
        virtual void    process (WeaponPlayer*,Player*,int);
};
        //��������
class   RunAway: public GameIQ 
{
  public:
        virtual IQMark  analyse (Player*,Player*);
        virtual void    process (WeaponPlayer*,Player*,int);
};
        //����
class   SearchForTarget: public GameIQ
{
  public:
        virtual IQMark  analyse (Player*,Player*);
        virtual void    process (WeaponPlayer*,Player*,int);
};
        //���砩���
class   RandomIQ: public GameIQ
{
  public:
        virtual IQMark  analyse (Player*,Player*);
        virtual void    process (WeaponPlayer*,Player*,int);
};

/////////////////////////////////////////////////////////////////////////////
////���⥬� �ࠢ����� ����⢥��� ��⥫��⮬
/////////////////////////////////////////////////////////////////////////////
class   GameIQSystem
{
  public:
                        GameIQSystem  (const IQClass&);
                        ~GameIQSystem ();

/////////////////////////////////////////////////////////////////////////////
////�����諥��� � ����⢨�
/////////////////////////////////////////////////////////////////////////////
                void    think         (WeaponPlayer* monstr,
                                       Player* target,
                                       int targetIQ=0
                                       );

/////////////////////////////////////////////////////////////////////////////
////����㧪� - ��࠭����
/////////////////////////////////////////////////////////////////////////////
                void    Load    (BinFile*);
                void    Save    (BinFile*);

  protected:
    static      GameIQ*         table [IQ_MAX_PROBS];  //������ ��⮤�� IQ
                IQClass         iqRes;                 //������ ������� ������
};

#endif