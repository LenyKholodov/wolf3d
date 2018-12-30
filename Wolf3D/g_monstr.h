#ifndef __GAME_MONSTR__
#define __GAME_MONSTR__

#include "g_player.h"
#include "g_iq.h"

////////////////////////////////////////////////////////////////////////////
//////���⥬�, �ࠢ����� ��ப�� (����㠫�� ����䥩�)
////////////////////////////////////////////////////////////////////////////
class   Men : public GameObject
{
  public:
                Men  (ArgPtr);
                Men  (BinFile*);
     virtual    ~Men ();

////////////////////////////////////////////////////////////////////////////
//////�᭮��� ����ࠪ�� �㭪樨 ��� ��� ��஢�� ��ꥪ⮢
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return MEN; }
    virtual     void            dump (ostream&) const;

////////////////////////////////////////////////////////////////////////////
//////����⢥��� ��⥫���
////////////////////////////////////////////////////////////////////////////
    virtual     void            think () = 0;
    virtual     void            tick  () = 0;

    virtual     void            sleep   (BOOL = FALSE,ID wakeUp = ID(-1,-1));

                //��ப, ����� ������ �� ������
                ID              player () const  { return mPlayer; }

    protected:
                ID              mPlayer;
};

////////////////////////////////////////////////////////////////////////////
//////������ (�㯮�)
////////////////////////////////////////////////////////////////////////////
class    Monstr: public Men
{
  public:
                Monstr  (ArgPtr);
                Monstr  (BinFile*);
     virtual    ~Monstr ();

                enum SetEnum
                {
                  SET_ON  = 1,
                  SET_OFF =-1,
                  NOSET   = 0,

                  CURRENT_METHOD = 0,
                  PREV_METHOD
                };

////////////////////////////////////////////////////////////////////////////
//////�᭮��� ����ࠪ�� �㭪樨 ��� ��� ��஢�� ��ꥪ⮢
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return GAME_MONSTR; }
    virtual     void            dump (ostream&) const;

////////////////////////////////////////////////////////////////////////////
//////����⢥��� ��⥫���
////////////////////////////////////////////////////////////////////////////
    virtual     void            think () ;
    virtual     void            tick  () ;

    virtual     void            sleep   (BOOL,ID);
                void            alarm   (ID __who__);

                BOOL            IsFriend (ID);

  protected:
    static      GameIQ*           table [IQ_MAX_PROBS];  //������ ��⮤�� IQ
                IQClass::Actives  curMethod [2];         //����饥 ����⢨�
                IQParam           param;

                Target            target;                //���� ��� ������
                IQClass           iqRes;                 //������ ������� ������
                Fixed             speed;                 //������� �������� ������
                int               skin;                  //��� ������

                clock_t           lastTime;

};

/////////////////////////////////////////////////////////////////////////////
///////����㦥��� ��ப
/////////////////////////////////////////////////////////////////////////////
class   WeaponPlayer: public Player
{
  public:
                WeaponPlayer  (ArgPtr);
                WeaponPlayer  (BinFile*);
     virtual    ~WeaponPlayer ();

////////////////////////////////////////////////////////////////////////////
//////�᭮��� ����ࠪ�� �㭪樨 ��� ��� ��஢�� ��ꥪ⮢
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return WEAPON_PLAYER; }

////////////////////////////////////////////////////////////////////////////
//////�㭪樨 �⭮��騥�� � �ࠢ����� ��ப��
////////////////////////////////////////////////////////////////////////////
                void            Fire ();
    virtual     void            tick ();

  protected:
                ID              weapon;


};

extern  int     HARD_LEVEL;

#endif