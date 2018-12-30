#ifndef __SPRITE_LIST_CACHE__
#define __SPRITE_LIST_CACHE__

#include "cache.h"
#include <grtypes.h>

//const   int     SPRITE_LIST_SIZE = 450000;
extern  int       SPRITE_LIST_SIZE;

/////////////////////////////////////////////////////////////////////////////
////Класс кэш экранных спрайтов
/////////////////////////////////////////////////////////////////////////////
class   SpriteListCache: public NameCache
{
  public:
                  SpriteListCache  (char* names[],
                               long id_num,
                               long cache_sz,
                               long cl_sz);
                  SpriteListCache  (BinFile* file,long cache_sz=1);
                  SpriteListCache  (const char* name,long cache_sz=1);
    virtual       ~SpriteListCache () { Close (); }

          virtual void          Close       ();

                  tSprite *     GetData (int id,int face);
                  void    *     GetData (int id) { return Cache::GetData(id); }

  protected:
                  int*        idOffs;   //смещения отн. начала таблицы
                  int*        index;    //номер спрайта

  protected:
          virtual void          LoadData    (long id,CacheList* dest);
                  int           Research    (const char*);
                  void          Recalc      ();
};

#endif