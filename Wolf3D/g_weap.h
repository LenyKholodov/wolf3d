#ifndef __GAME_WEAPON__
#define __GAME_WEAPON__

#include "g_object.h"
#include "g_box.h"

/////////////////////////////////////////////////////////////////////////////
//////�����
/////////////////////////////////////////////////////////////////////////////
class   Shot: public GameObject
{
  public:
                        Shot (ArgPtr);      //ptr = *Weaponres::Shot
                        Shot (BinFile*);
         virtual        ~Shot ();

////////////////////////////////////////////////////////////////////////////
//////�᭮��� ����ࠪ�� �㭪樨 ��� ��� ��஢�� ��ꥪ⮢
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*) ;
    virtual     void            Save (BinFile*) ;
    virtual     BOOL            Exec (GameMessage*) ;

    virtual     GameObjects     type () const  { return SHOT; }
    virtual     void            dump (ostream&) const;

////////////////////////////////////////////////////////////////////////////
//////�㭪樨 �⭮��騥�� � ᭠���
////////////////////////////////////////////////////////////////////////////
    virtual     void            tick    ();
    virtual     void            bah     (int);  //����
                BOOL            IsCross ();     //���� �� �⮫��������

  protected:
                void            Present (int index) const;

  protected:
             Fixed              x, y;
             Angle              angle;
             WeaponRes::Shot    shotRes;

             int                state;  //⥪��� �⠤�� ���뢠
             Fixed              dist;   //�ன������ ����ﭨ�

             int                oldIndex;

             Block              block;
             Block              oldBlock;

             ID                 oldId;

             int                bahIndex,
                                crossIndex;

             int                last;
};

/////////////////////////////////////////////////////////////////////////////
//////��㦨�
/////////////////////////////////////////////////////////////////////////////
class   Weapon: public GameObject
{
  public:
                        Weapon (ArgPtr);
                        Weapon (BinFile*);
          virtual       ~Weapon ();

                        enum State {
                              PASSIVE = 0,
                              ACTIVE,
                              NO_ANIM = -1
                        };

////////////////////////////////////////////////////////////////////////////
//////�᭮��� ����ࠪ�� �㭪樨 ��� ��� ��஢�� ��ꥪ⮢
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*) ;
    virtual     void            Save (BinFile*) ;
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return WEAPON; }
    virtual     void            dump (ostream&) const;

////////////////////////////////////////////////////////////////////////////
//////�⭮���� ⮫쪮 � ��㦨�
////////////////////////////////////////////////////////////////////////////
    virtual     void            ShotOn  ();
    virtual     void            ShotOff ();
                void            SetBox  (ID);

    virtual     void            Fire    (Fixed,Fixed,Angle);
                void            refresh (BOOL);

////////////////////////////////////////////////////////////////////////////
//////����� ��஡�� �७������� ��㦨�
////////////////////////////////////////////////////////////////////////////
    virtual     BoxTypes        belong ()      { return weapRes.gameBox; }

  protected:
             WeaponRes          weapRes;
             int                last;
             ID                 box;

             int                lastAnim;   //animation
             int                state;
             int                iterator;
             char*              faceTable    [2];
             int                faceAnimTime [2];

             BOOL               shotOff;

             int                weaponId;
};


#endif