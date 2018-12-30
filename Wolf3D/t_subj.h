#ifndef __TYPE_SUBJECT__
#define __TYPE_SUBJECT__

#include "t_mes.h"
#include "t_defs.h"
#include "t_res.h"

#include <binfile.h>

////////////////////////////�奬� ����� ��� �।���///////////////////////

struct  SubjectRes:public ResourceHeader
{
        //��� �⮡ࠦ���� �� ����
  int           mapSprite;
  int           giveSound;

        //�ਤ����� ��ꥪ�� - �������⥫�
  int           count;          //���-��...
  int           gameCache, id;  //��뫪� �� 䠩� � ����ᮬ
  GameObjects   type;           //⨯ ⮣� �� ������
  int           boxClass;       //����� ��஡�� � ���஭��� (�筥� ����� ��㦨�)

        //Respawn
  clock_t       respawnTime;    //�६� ��� respawn'a
  int           respawnSound;   //��� respawn'a

     BOOL        Load (const char*);
     BOOL        Load (BinFile*);
     void        Save (const char*);
     void        Save (BinFile*);
};

////////////////////////////���ᠭ�� ������� �।���/////////////////////

struct  SubjectExempl
{
  int           x,y;
};

////////////////////////////��� ��� ��࠭���� �।���//////////////////////

struct  SubjectTag
{
  SubjectRes    res;
  SubjectExempl exempl;
};

/////////////////////////////�������� �।���///////////////////////////////

typedef CreateExemplar <SubjectRes,SubjectExempl> CreateSubject;

////////////////////////////����饭�� �।����///////////////////////////////

enum SubjectMessageId
{
  INCLUDE = SUBJECT_LIMIT,
  EXCLUDE,
  USE,
  ALREADY_INCLUDE,
  GET_SUBJECT_INFO,
  SUBJECT_INFO
};

struct  SubjectGetInfoMessage: public GameMessage
{
        SubjectGetInfoMessage (ID x):dest(x) { id = GET_SUBJECT_INFO; }

        ID      dest;   //�� ��訢��
};

struct  SubjectInfoMessage: public GameMessage
{
        SubjectInfoMessage (const SubjectRes& x): res(x) { id = SUBJECT_INFO; }

        SubjectRes      res;    //������ �奬�
};

struct  SubjectIncludeMessage: public GameMessage
{
        SubjectIncludeMessage (ID owner): playerOwner (owner)
           { id = INCLUDE; }

        ID      playerOwner;
};

struct  SubjectExcludeMessage: public GameMessage
{
        SubjectExcludeMessage () { id = EXCLUDE; }
};

struct  SubjectUseMessage: public GameMessage
{
        SubjectUseMessage () { id = USE; }
};

struct  SubjectIncludeOkMessage: public GameMessage
{
        SubjectIncludeOkMessage (const SubjectRes& x)
           :res (x)
          { id = ALREADY_INCLUDE; }

        SubjectRes      res;    //������ �奬�
};

#endif