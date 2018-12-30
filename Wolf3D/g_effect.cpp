#include "g_effect.h"
#include "g_system.h"
#include "g_anim.h"

#include "cross.h"

#include <time.h>

make<Effect,EFFECT>                 makeEffect;
make<SpriteEffect,SPRITE_EFFECT>    makeSpriteEffect;
make<LightEffect,LIGHT_EFFECT>      makeLightEffect;

///////////////////////implementation of class Effect////////////////////////

void            Effect::Load (BinFile* file)
{
  GameObject::Load (file);

  file->read (&mChangeTime,sizeof(mChangeTime));
  file->read (&mPos,sizeof(mPos));
  file->read (&mMaxPos,sizeof(mMaxPos));
}

void            Effect::Save (BinFile* file)
{
  GameObject::Save (file);

  file->write (&mChangeTime,sizeof(mChangeTime));
  file->write (&mPos,sizeof(mPos));
  file->write (&mMaxPos,sizeof(mMaxPos));
}

BOOL            Effect::Exec (GameMessage* mes)
{
  switch (mes->id)
  {
    case TICK:
    {
      if (clock()-mLast > mChangeTime)
      {
        tick();

        mLast = clock();
      }

      return TRUE;
    }
    default: return FALSE;
  }
}

void            Effect::dump (ostream& os) const
{
  os<<"Effect pos = "<<mPos<<" change_time = "<<mChangeTime<<endl;
}

Effect::Effect(ArgPtr ptr)
       :mLast (clock()), mPos (0)
{
  if (ptr == NULL)
     return;

  CreateEffect* arg = (CreateEffect*)ptr;

  mChangeTime = arg->param().changeTime;
  mMaxPos     = arg->resourceClass.maxPos;

  owner->AddActive (id(),PR_ANIMATION);
}

Effect::Effect(BinFile* file)
{
  Load(file);

  owner->DelActive (id());
}

///////////////////////implementation of class SpriteEffect/////////////////

void            SpriteEffect::Load (BinFile* file)
{
  Effect::Load(file);

  SpriteEffectTag tag;

  file->read (&tag,sizeof(tag));

  sprRes       = tag.res;
  x            = tag.exempl.x;
  y            = tag.exempl.y;

  refresh();
}

void            SpriteEffect::Save (BinFile* file)
{
  Effect::Save(file);

  SpriteEffectTag tag;

  tag.res       = sprRes;
  tag.exempl.x  = x;
  tag.exempl.y  = y;

  file->write (&tag,sizeof(tag));
}

void            SpriteEffect::dump (ostream& os) const
{
  os<<"Sprite  effect"<<endl;
}

void            SpriteEffect::tick ()
{
  refresh();
}

void            SpriteEffect::refresh()
{
  int           temp;
  Block         block;

  block.type              = Map::SPRITE;
  block.cross             = sprRes.cross;
  block.id.sprite.z       = SPRITE_CENTER;
  block.id.sprite.id      = sprRes.mapSprite;
  block.id.sprite.face    = NextFace(sprRes.anim,mPos,temp);
  block.id.sprite.xfrac   = FX_HALF>>12;
  block.id.sprite.yfrac   = FX_HALF>>12;
  block.id.sprite.alpha   = 0;
  block.id.sprite.already = 0;

  owner->SetBlock(Index(y,x),block);
}

SpriteEffect::SpriteEffect(ArgPtr ptr)
             : Effect (ptr)
{
  CreateSpriteEffect* arg = (CreateSpriteEffect*)ptr;

  sprRes      = arg->resourceClass;
  x           = arg->param().x;
  y           = arg->param().y;
  mChangeTime = arg->resourceClass.changeTime;

  if (arg->param().changeTime != -1)
     mChangeTime = arg->param().changeTime;

  refresh();
}

SpriteEffect::SpriteEffect(BinFile* file)
{
  Load(file);
}

SpriteEffect::~SpriteEffect()
{
  Block         block;

  block.type              = Map::FREE;
  block.cross             = 1;

  owner->SetBlock(Index(y,x),block);
}


///////////////////////implementation of class LighttEffect///////////////////

void            LightEffect::Load (BinFile* file)
{
  Effect::Load(file);

  LightEffectTag tag;

  file->read (&tag,sizeof(tag));
  file->read (indexTable,tag.num*sizeof(LightClient));

  lightRes      = tag.res;
  lightExempl   = tag.exempl;
  num           = tag.num;

  refresh();
}

void            LightEffect::Save (BinFile* file)
{
  Effect::Save(file);

  LightEffectTag tag;

  tag.res       = lightRes;
  tag.exempl    = lightExempl;
  tag.num       = num;

  file->write (&tag,sizeof(tag));
  file->write (indexTable,tag.num*sizeof(LightClient));
}

void            LightEffect::dump (ostream& os) const
{
  os<<"Light  effect"<<endl;
}

void            LightEffect::tick ()
{
  int temp;
  Fixed shine    = int2fixed(NextFace (lightRes.anim,mPos,temp));
  LightMap* lMap = owner->GetLightMap();

  for (int i=0;i<num;i++)
  {
    int  index = indexTable[i].index;
    Side side  = indexTable[i].side;
    int  level = fixed2int(mul(shine,FX_MAP_WIDTH-indexTable[i].dist)>>MAP_SHIFT);

    for (int j=0;j<NUM_OF_DIAPOS;j++)
       lMap->SetLightLevel(index,side,j,level); /////LEVEL
  }
}

void            LightEffect::refresh()
{
  CrossIndex index;
  Fixed      x       = int2fixed(lightExempl.x),
             y       = int2fixed(lightExempl.y);
  Fixed      maxDist = lightRes.dist;
  num = 0;

  for (int angle = 0;angle<ANGLE_360;angle+=ANGLE_45)//LIGHT_ANGLE_STEP)
  {
    FindCross (x,y,angle,maxDist,owner->GetMap(),index);

    if (index.crossIndex!=-1)
    {
      if (num==0)
      {
         indexTable [num].side    = index.side;
         indexTable [num].dist    = index.dist;
         indexTable [num++].index = index.crossIndex;
      }
      if (num>0 &&
         indexTable[0].index!=index.crossIndex &&
         indexTable[num-1].index!=index.crossIndex)
      {
         indexTable [num].side    = index.side;
         indexTable [num].dist    = index.dist;
         indexTable [num++].index = index.crossIndex;
      }
    }
  }
}

LightEffect::LightEffect(ArgPtr ptr)
             : Effect (ptr)
{
  CreateLightEffect* arg = (CreateLightEffect*)ptr;

  lightRes    = arg->resourceClass;
  lightExempl = arg->param();
  mChangeTime = arg->resourceClass.changeTime;

  if (arg->param().changeTime != -1)
     mChangeTime = arg->param().changeTime;
  if (arg->param().dist != 0)
     lightRes.dist = arg->param().dist;

  refresh();
}

LightEffect::LightEffect(BinFile* file)
{
  Load(file);
}

LightEffect::~LightEffect()
{     }

