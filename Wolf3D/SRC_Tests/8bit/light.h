#ifndef __LIGHT__
#define __LIGHT__

#include "fixed.h"
#include "map.h"

const int       MAX_LIGHT_LEVELS     = 16;    //макс. кол-во уровней освещения

const   int     DISTANCE_LIGHT_SHIFT = 18;    //сдвиг на затенение
const   int     LIGHT_DETAIL         = 800; //детализация освещения
const   int     RENDER_LIGHT_SHIFT   = 4;   //16*16
const   int     LIGHT_Q              = 4;

const   int     LIGHT_MAP_STEP       = 128;
const   int     NUM_OF_DIAPOS        = 128/LIGHT_MAP_STEP; //text_width = 128
const   int     NUM_DIAPOS_SHIFT     = 0;
const   int     DIAPOS_SHIFT         = 7; //2^DIAPOS_SHIFT = STEP

typedef  uchar LightLevel;
typedef  uint  LightDiapos;

/////////////////////////////////////////////////////////////////////////////
/////Таблица палитр (для разных коэффициентов освещения)
/////////////////////////////////////////////////////////////////////////////
class   LightTable
{
  public:
    uchar   light [MAX_LIGHT_LEVELS][256];

                LightTable();
  protected:
    void     form(int,float);
};

////////////////////////////////////////////////////////////////////////////
////Light Map File Header
////////////////////////////////////////////////////////////////////////////
struct LMFHeader
{
  uchar         id[4]; //LMF#32
  uint          width,
                height;
  uchar         max_level;
};

////////////////////////////////////////////////////////////////////////////
////Класс -  освещенный куб (используется в light map)
////////////////////////////////////////////////////////////////////////////
struct  LightBlock
{
        LightLevel      level[4*NUM_OF_DIAPOS];
};

////////////////////////////////////////////////////////////////////////////
////Класс осуществляющий рассчет освещения для указанной сцены
////////////////////////////////////////////////////////////////////////////
class   RayCast;
class   LightCompile
{
  protected:
             Map*               map;                    //stage map
             LightBlock*        lightMap;               //light map
             LightBlock*        fastTable[MAP_HEIGHT];
             RayCast*           rc;                     //for trass light

  protected:
             void       CreateMap();
             void       SimpleCalc(int x,int y);
             void       relight(Fixed,Fixed,Angle);
  public:
                        LightCompile(Map*);
                        LightCompile(BinFile*,Map* = NULL);
                        LightCompile(const char*,Map* = NULL);
          virtual       ~LightCompile();

          void          SetMap(Map*);

////////////////////////////////////////////////////////////////////////////
////Установка и чтение уровня освещенности
////////////////////////////////////////////////////////////////////////////
          void          SetLightLevel(uint,Side,LightDiapos,LightLevel);
          void          ChangeLightLevel(uint,Side,LightDiapos,LightLevel); //compare for more
                                                           //light level
          LightLevel    GetLightLevel(uint index,Side,LightDiapos);
          LightLevel    GetLightLevel(uint,uint,Side,LightDiapos);

////////////////////////////////////////////////////////////////////////////
////Пересчитывает освещенность сцены в зависимости от уровня яркости
////////////////////////////////////////////////////////////////////////////
          void          reset(LightLevel);
          void          recalc();
          void          recalc(LightLevel);

////////////////////////////////////////////////////////////////////////////
////Загрузка и сохранение карты освещения
////////////////////////////////////////////////////////////////////////////
          void          Save(BinFile*);
          void          Save(const char*);
          BOOL          Load(BinFile*);
          BOOL          Load(const char*);
};

typedef LightCompile    LightMap;

#endif
