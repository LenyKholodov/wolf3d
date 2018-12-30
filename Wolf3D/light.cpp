#include "render.h"
#include "tables.h"

#include <string.h>
#include <sbinfile.h>

#include <debug\debug.h>

/////////////implementation of class Light Table/////////////////////////////

LightTable::LightTable()
{
  float k       = 1.0;
  float DELTA   = 1.0/MAX_LIGHT_LEVELS;
  for (int i=0;i<MAX_LIGHT_LEVELS;i++,k-=DELTA)
      form(i,k);
}

void    LightTable::form(int level,float k)
{
  int           r,g,b;
  float         red,green,blue;
  uchar*        table = light[level];

  for (int src=0;src<256;src++)
  {
    Palette::rgb(src,r,g,b);

    red   = k*(float)r;                  //??
    green = k*(float)g;
    blue  = k*(float)b;
    table[src] = Palette::rgb(red,green,blue);
  }
}

////////////////implementation of Relight system/////////////////////////////

class   RayLight: public RayCast
{
  public:
        RayLight(LightCompile*);

  protected:
    virtual void DrawLine();
    virtual void SetTexture(void*) {}

  protected:
    LightCompile*       comp;
};

RayLight::RayLight (LightCompile* lcomp)
         : RayCast(LIGHT_DETAIL,10), comp(lcomp)
{
  Reset();
}

void    RayLight::DrawLine()
{
    comp->ChangeLightLevel(textureParam.index,textureParam.side,
                         textureParam.txtOffs>>DIAPOS_SHIFT,
                         textureParam.dist>>(DISTANCE_LIGHT_SHIFT-LIGHT_Q));
}

//////////////implementation of classes LightMap-LightCompile////////////////

void    LightCompile::SetMap(Map* m) { map = m; }       //not!!!

LightLevel      LightCompile::GetLightLevel(uint index,Side side,LightDiapos pos)
{
  return lightMap[index].level[(side<<NUM_DIAPOS_SHIFT)+pos];
}

LightLevel      LightCompile::GetLightLevel(uint x,uint y,Side side,LightDiapos pos)
{
  return fastTable[y][x].level[(side<<NUM_DIAPOS_SHIFT)+pos];
}

void    LightCompile::SetLightLevel(uint index,Side side,LightDiapos pos,LightLevel lev)
{
  lightMap[index].level[(side<<NUM_DIAPOS_SHIFT)+pos] = lev;
}

void    LightCompile::ChangeLightLevel(uint index,Side side,LightDiapos pos,LightLevel lev)
{
  if (lightMap[index].level[(side<<NUM_DIAPOS_SHIFT)+pos]>lev)
     lightMap[index].level[(side<<NUM_DIAPOS_SHIFT)+pos] = lev;
}

void    LightCompile::reset(LightLevel back_ground)
{
  memset (lightMap,back_ground,MAP_WIDTH*MAP_HEIGHT*sizeof(LightBlock));
}

void    LightCompile::recalc(LightLevel back_ground)
{
  reset(back_ground);
  recalc();
}

void    LightCompile::recalc()
{
  for (int y = 0;y<MAP_HEIGHT;y++)
      for (int x = 0;x<MAP_WIDTH;x++)
          if (map->GetType(Index(y,x)) == Map::LIGHT)
          {
             SimpleCalc(x,y);
             map->GetBlock(Index(y,x))->cross = 1;
          }
}

void    LightCompile::SimpleCalc(int _x,int _y)
{
  for (ulong angle = ANGLE_0;angle<ANGLE_360;angle+=ANGLE_60)
       relight(int2fixed(_x),int2fixed(_y),angle);
}

void    LightCompile::relight(Fixed _x,Fixed _y,Angle ang)
{
  rc->DrawView(_x,_y,ang);
}

void    LightCompile::Save(BinFile* file)
{
  if (file == NULL)
     return;

  LMFHeader     lmf;

  memcpy(lmf.id,"LMF ",4);
  lmf.width          = MAP_WIDTH;
  lmf.height         = MAP_HEIGHT;
  lmf.max_level      = MAX_LIGHT_LEVELS;

  file->reset();
  file->write(&lmf,sizeof(LMFHeader));
  file->write(lightMap,MAP_HEIGHT*MAP_WIDTH*sizeof(LightBlock));
}

void    LightCompile::Save(const char* path)
{
  stdBinFile f;
  f.open(path,BinFile::OPEN_RW);
  Save(&f);
  f.close();
}

BOOL    LightCompile::Load(BinFile* file)
{
  if (file == NULL)
     return FALSE;

  LMFHeader     lmf;

  file->reset();
  file->read(&lmf,sizeof(LMFHeader));

  if (memcmp(&lmf.id,"LMF ",4))
  {
     D_String("Light compile: Wrong file format");
     return FALSE;
  }

  if (lmf.width != MAP_WIDTH)
     dout<<"Light compile: Warning! width != MAP_WIDTH"<<endl;
  if (lmf.height != MAP_HEIGHT)
     dout<<"Light compile: Warning! height != MAP_HEIGHT"<<endl;
  if (lmf.max_level != MAX_LIGHT_LEVELS)
     dout<<"Light compile: Warning! max_levels != MAX_LIGHT_LEVELS"<<endl;

  CreateMap ();

  file->read(lightMap,MAP_WIDTH*MAP_HEIGHT*sizeof(LightBlock));

  return TRUE;
}

BOOL    LightCompile::Load(const char* path)
{
  stdBinFile f;
  f.open(path,1);
  BOOL status=Load(&f);
  f.close();
  return status;
}

void    LightCompile::CreateMap()
{
  if (lightMap) delete lightMap;
  lightMap = new LightBlock [MAP_WIDTH*MAP_HEIGHT];

  memset (lightMap,255,MAP_WIDTH*MAP_HEIGHT*sizeof(LightBlock));

  LightBlock*   pos = lightMap;
  for (int i = 0;i<MAP_HEIGHT;i++,pos+=MAP_WIDTH)
      fastTable[i] = pos;

  rc = new RayLight(this);
  rc->SetMap(map);
}

LightCompile::LightCompile(BinFile* file,Map* m)
             :map(m),lightMap(NULL),rc(NULL)

{ Load(file); }

LightCompile::LightCompile(const char* path,Map* m)
             :map(m),lightMap(NULL), rc(NULL)

{ Load(path); }

LightCompile::LightCompile(Map* m)
             :lightMap(NULL),map(m),rc(NULL)
{
  CreateMap();
}

LightCompile::~LightCompile()
{
  delete rc;
  if (lightMap) delete lightMap;
}
