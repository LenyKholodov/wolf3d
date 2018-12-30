#ifndef __TYPE_EFFECT__
#define __TYPE_EFFECT__

#include "t_defs.h"
#include "t_res.h"

#include <binfile.h>

////////////////////////////Схема ресурса для эффекта///////////////////////

struct  EffectRes:public ResourceHeader
{
  int   maxPos;

     BOOL        Load (const char*);
     BOOL        Load (BinFile*);
     void        Save (const char*);
     void        Save (BinFile*);
};

////////////////////////////Описание экземпляра эффекта/////////////////////

struct  EffectExempl
{
  int   changeTime;
};

/////////////////////////////Создание эффекта///////////////////////////////

typedef CreateExemplar <EffectRes,EffectExempl> CreateEffect;

////////////////////////////Спрайтовый эффект/////////////////////////////////

struct  SpriteEffectRes:public EffectRes
{
  int   changeTime;      //
  int   mapSprite;       //Спрайт
  BOOL  cross;           //Можно ли пересечь
  char  anim [ANIM_NUM]; //Анимация

     BOOL        Load (const char*);
     BOOL        Load (BinFile*);
     void        Save (const char*);
     void        Save (BinFile*);
};

struct  SpriteEffectExempl:public EffectExempl
{
  int   x,y;             //Координаты
};

struct  SpriteEffectTag
{
  SpriteEffectRes       res;
  SpriteEffectExempl    exempl;
};

typedef CreateExemplar <SpriteEffectRes,SpriteEffectExempl> CreateSpriteEffect;

////////////////////////////Световой   эффект/////////////////////////////////

struct  LightEffectRes:public EffectRes
{
  int   changeTime;      //
  Fixed dist;            //Яркость
  char  anim [ANIM_NUM]; //Анимация

     BOOL        Load (const char*);
     BOOL        Load (BinFile*);
     void        Save (const char*);
     void        Save (BinFile*);
};

struct  LightEffectExempl:public EffectExempl
{
  int     x,y;             //Координаты
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