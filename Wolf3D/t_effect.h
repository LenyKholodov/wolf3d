#ifndef __TYPE_EFFECT__
#define __TYPE_EFFECT__

#include "t_defs.h"
#include "t_res.h"

#include <binfile.h>

////////////////////////////�奬� ����� ��� ��䥪�///////////////////////

struct  EffectRes:public ResourceHeader
{
  int   maxPos;

     BOOL        Load (const char*);
     BOOL        Load (BinFile*);
     void        Save (const char*);
     void        Save (BinFile*);
};

////////////////////////////���ᠭ�� ������� ��䥪�/////////////////////

struct  EffectExempl
{
  int   changeTime;
};

/////////////////////////////�������� ��䥪�///////////////////////////////

typedef CreateExemplar <EffectRes,EffectExempl> CreateEffect;

////////////////////////////��ࠩ⮢� ��䥪�/////////////////////////////////

struct  SpriteEffectRes:public EffectRes
{
  int   changeTime;      //
  int   mapSprite;       //��ࠩ�
  BOOL  cross;           //����� �� ������
  char  anim [ANIM_NUM]; //�������

     BOOL        Load (const char*);
     BOOL        Load (BinFile*);
     void        Save (const char*);
     void        Save (BinFile*);
};

struct  SpriteEffectExempl:public EffectExempl
{
  int   x,y;             //���न����
};

struct  SpriteEffectTag
{
  SpriteEffectRes       res;
  SpriteEffectExempl    exempl;
};

typedef CreateExemplar <SpriteEffectRes,SpriteEffectExempl> CreateSpriteEffect;

////////////////////////////���⮢��   ��䥪�/////////////////////////////////

struct  LightEffectRes:public EffectRes
{
  int   changeTime;      //
  Fixed dist;            //�મ���
  char  anim [ANIM_NUM]; //�������

     BOOL        Load (const char*);
     BOOL        Load (BinFile*);
     void        Save (const char*);
     void        Save (BinFile*);
};

struct  LightEffectExempl:public EffectExempl
{
  int     x,y;             //���न����
  Fixed   dist;            //Max dist
};

struct  LightEffectTag
{
  LightEffectRes       res;
  LightEffectExempl    exempl;

  int                   num;
};

typedef CreateExemplar <LightEffectRes,LightEffectExempl> CreateLightEffect;

#endif