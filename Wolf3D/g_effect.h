#ifndef __GAME_EFFECTS__
#define __GAME_EFFECTS__

#include "g_object.h"

/////////////////////////////////////////////////////////////////////////////
////Эффект
/////////////////////////////////////////////////////////////////////////////
class  Effect: public GameObject
{
  public:
                                Effect(ArgPtr);
                                Effect(BinFile*);
                  virtual       ~Effect() {}

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return EFFECT; }
    virtual     void            dump (ostream&) const;

    virtual     void            tick  () {}
                int             pos   () const { return mPos; }
                void            pos   (int x) { mPos = x;}

  protected:
             int        mChangeTime;
             int        mPos;
             int        mMaxPos;
             int        mLast;
};

/////////////////////////////////////////////////////////////////////////////
/////Спрайтовый эффект
/////////////////////////////////////////////////////////////////////////////
class  SpriteEffect: public Effect
{
  public:
                                SpriteEffect(ArgPtr);
                                SpriteEffect(BinFile*);
                  virtual       ~SpriteEffect();

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);

    virtual     GameObjects     type () const  { return SPRITE_EFFECT; }
    virtual     void            dump (ostream&) const;

    virtual     void            tick ();
                void            refresh();

  protected:
             SpriteEffectRes    sprRes;
             int                x,y;
};

/////////////////////////////////////////////////////////////////////////////
/////Световой эффект
/////////////////////////////////////////////////////////////////////////////
struct LightClient
{
  int   index;
  Side  side;
  Fixed dist;
};

class  LightEffect: public Effect
{
  public:
                                LightEffect(ArgPtr);
                                LightEffect(BinFile*);
                  virtual       ~LightEffect();

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);

    virtual     GameObjects     type () const  { return LIGHT_EFFECT; }
    virtual     void            dump (ostream&) const;

    virtual     void            tick ();
                void            refresh();

  protected:
             LightEffectRes     lightRes;
             LightEffectExempl  lightExempl;

             int                num;
             LightClient        indexTable [MAX_WALL_LIGHT];
};

#endif