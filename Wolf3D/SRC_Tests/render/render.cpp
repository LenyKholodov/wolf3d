#include <misc.h>
#include <vga.h>

#include "render.h"

#include <math.h>
#include <time.h>

#include <debug\debug.h>

LightTable      shadow;         //shadow table

////////////////////implementation of class Ray Casting/////////////////////

int     RayCast::GetShift (int width)
{
  for (int i=0;width;i++,width>>=1);

  return i - 1;
}

BOOL    RayCast::SetParam (int id,double value)
{
  switch (id)
  {
    case VIEW_ANGLE:
         widthFlag = TRUE;
         viewAngle = value;
         if (spriteSystem)
           spriteSystem->RenderPtr()->SetParam(RenderSprite::VIEW_ANGLE,value);
         break;
    case FOCUS:
         heightFlag = TRUE;
         focus = value;
         if (spriteSystem)
           spriteSystem->RenderPtr()->SetParam(RenderSprite::FOCUS,value);
         break;
    case WALL_HEIGHT:
         heightFlag = TRUE;
         wallHeight = value;
         break;
    case WALL_FEEL:
         heightFlag = TRUE;
         wallFeel = value>0 ? value : 12;
         break;
    case DETAIL_ANGLE:
         detailQ = value;
         break;
    case MAX_SPRITES:
         spriteSystem->ListPtr()->SetMaxCount((int)value);
         break;
    default:  return FALSE;
  }
  return TRUE;
}

double  RayCast::GetParam(int id)
{
  switch (id)
  {
    case VIEW_ANGLE:  return   viewAngle;
    case FOCUS:       return   focus;
    case WALL_HEIGHT: return   wallHeight;
    case WALL_FEEL:   return   wallFeel;
    case DETAIL_ANGLE:return   detailQ;
    case MAX_SPRITES: return   spriteSystem->ListPtr()->GetMaxCount();
    default:          return   -1;
  }
}

void    RayCast::ClearZBuffer(Fixed   value)
{
   memsetd(zbuffer,value,screenWidth);
}

void    RayCast::Reset()
{
   if (spriteSystem)
      spriteSystem->Reset();

   dout<<"Ray casting: Reset system tables..."<<endl;

   if (widthFlag)
   {
          //Установка угла детализации
     detailAngle = float2angle(detailQ);

     double view=tan(viewAngle/2.0);
     for (int i=0;i<screenWidth;i++)
     {
       rayAngle[i] = float2angle(atan(-view+2.0*i*view/(float)(screenWidth-1)));
       zbuffer[i]  = out_of_map;
     }

   }

   if (heightFlag)
   {
     if (h1)         delete h1;
     if (h2)         delete h2;
     if (dyTable)    delete dyTable;
     if (txtOffs)    delete txtOffs;

     long htSize = (int)pow(2,wallFeel);

     h1          = new int     [htSize];
     h2          = new int     [htSize];
     dyTable     = new Fixed   [htSize];
     txtOffs     = new int     [htSize];

          //Таблица высот и дельт
     Fixed   dist;
     double h,d;
     long _h1,_h2;
     shift = 16-(wallFeel/2);
     wallMask = htSize-1;
     for (long ij=0;ij<htSize;ij++)
     {
      dist = ij<<shift;//8
      if (dist>=out_of_map)              //Max posible length
      {
        h1[ij] = 0;
        h2[ij] = 0;
        continue;
      }
      d   = fixed2float(dist);
      h   = focus/d;
      h  *= screenHeight;

      double   dy    = d*128.0/(focus*screenHeight); //text height
      dyTable[ij]    = float2fixed(dy);

      _h1 = (screenHeight-h)/wallHeight;
      _h2 = h;

      if (_h1<=0)
      {
          txtOffs[ij] = dy*(h-screenHeight)/wallHeight;
          _h1 = 0;
          _h2 = screenHeight;
      }
      else                  txtOffs[ij] = 0;

      _h1 *= screenWidth;

      h1[ij] = _h1;
      h2[ij] = _h2;

     }
   }

   heightFlag = FALSE;
   widthFlag  = FALSE;
}

void    RayCast::DrawView(Fixed x,Fixed y,Angle angle)
{
   Angle          phi;                  //Ray casting angle
   uFixed         hdist,                //Horizontal distance
                  vdist;                //Vertical distance

   uint           vxTile, vyTile,       //Вертикальные ячейки
                  hxTile, hyTile;       //Горизонтальные ячейки
   int            dyTile,               //Horizontal trass step (ячейки)
                  dxTile;               //Vertical trass step (ячейки)
   Fixed          vxInter, vyInter,     //Vertical trass
                  hxInter, hyInter,     //Horizontal trass
                  vxStep, vyStep,       //Vertical steps
                  hxStep, hyStep;       //Horizontal steps

   int            hray,                 //Horizontal ray trass flag
                  vray;                 //Vertical ray trass flag
   int            casting;              //Trassing flag

   uint           hIndex,vIndex;        //Index for trass
   uint           xstart = fixed2int (x),
                  ystart = fixed2int (y);

   TextParam      param;
   Angle          curPhi;         //For optimize

   Block          position;
   Block*         posPtr     = NULL;

   if (spriteSystem)    spriteSystem->Reset(x,y,angle);

   for (int i=0;i<screenWidth;i++)
   {
        phi    = angle+rayAngle[i];      //Вычисление угла

        vxTile = hxTile = xstart;            //First trass (ячейки)
        vyTile = hyTile = ystart;

        hdist  = vdist  = out_of_map; //!!

        hray = abs(sin(phi))<=FX_VERY_LOW ? 0 : 1;   //Проверка на совпадение
        vray = abs(cos(phi))<=FX_VERY_LOW ? 0 : 1;   //луча с осями
        casting = hray+vray;                         //Num of rays

        if ((phi>=ANGLE_270 || phi<=ANGLE_90) && vray)    //Vertical ray settings
        {
                vxTile++;
                vxStep  = FX_ONE;
                vyStep  = tan(phi);
                vxInter = int2fixed(vxTile);
                dxTile  = 1;
        }
        else
        {
                vxTile--;
                vxStep  = -FX_ONE;
                vyStep  = -tan(phi);
                vxInter = int2fixed(vxTile+1);
                dxTile  = -1;
        }
        if (phi<=ANGLE_180 && hray)     //Horizontal ray settings
        {
                hyTile++;
                hyStep   = FX_ONE;
                hxStep   = ctan(phi);
                hyInter  = int2fixed(hyTile);
                dyTile   = 1;
        }
        else
        {
                hyTile--;
                hyStep   = -FX_ONE;
                hxStep   = -ctan(phi);
                hyInter  = int2fixed(hyTile+1);
                dyTile   = -1;
        }

        hxInter  = x+mul(ctan(phi),hyInter-y);
        vyInter  = y+mul(vxInter-x,tan(phi));

                //Срезать старшее слово для индекса
        while (casting)
        {
          if (hray)
          {
                hxTile = fixed2int(hxInter);
                if (hxTile>MAX_X_TILE || hyTile>MAX_Y_TILE)
                {
                        hray  = 0;
                        casting--;
                }
                else
                {
                  posPtr   = map->GetBlock(hIndex = Index(hyTile,hxTile));
//                  posPtr   = map->GetBlock(hyTile,hxTile);
                  position = *posPtr;

                  switch (position.type)
                  {
                    case Map::TRIGER:
                    case Map::WALL:
                      hdist  = mul(hyInter-y,isin(phi));
                      hray   = 0;
                      casting--;
                      continue;

                    case Map::DOOR:
                      param.txtOffs  = fixed2int(frac(hxInter)<<7);
                      param.txtOffs  = 127 - param.txtOffs;
                      if (position.id.door.state <= param.txtOffs)
                      {
                        hdist  = mul(hyInter-y,isin(phi));
                        hray   = 0;
                        casting--;
                        continue;
                      }
                      break;

                    case Map::MONSTR:
                    case Map::SPRITE:
                      if (position.id.sprite.already == 0 && spriteSystem)
                      {
                         spriteSystem->ProcessSprite(int2fixed(hxTile),
                                                     int2fixed(hyTile),
                                                     phi,
                                                     posPtr);
                         posPtr->id.sprite.already = 1;
                      }
                      break;
                  }

                  hyTile  += dyTile;
                  hyInter += hyStep;
                  hxInter += hxStep;
                }
          }
          if (vray)
          {
                vyTile = fixed2int(vyInter);
                if (vxTile>MAX_X_TILE || vyTile>MAX_Y_TILE)
                {
                        vray  = 0;
                        casting--;
                }
                else
                {
                  posPtr   = map->GetBlock(vIndex = Index(vyTile,vxTile));
//                  posPtr   = map->GetBlock(vyTile,vxTile);
                  position = *posPtr;

                  switch (position.type)
                  {
                    case Map::TRIGER:
                    case Map::WALL:
                      vdist  = mul(vxInter-x,icos(phi));
                      vray  = 0;
                      casting--;
                      continue;

                    case Map::DOOR:
                      param.txtOffs  = fixed2int(frac(vyInter)<<7);
//                      if (vxInter<x)  param.txtOffs = 127 - param.txtOffs;
                      if (position.id.door.state <= param.txtOffs)
                      {
                        vdist  = mul(vxInter-x,icos(phi));
                        vray   = 0;
                        casting--;
                        continue;
                      }
                      break;

                    case Map::MONSTR:
                    case Map::SPRITE:
                      if (position.id.sprite.already == 0 && spriteSystem)
                      {
                        spriteSystem->ProcessSprite(int2fixed(vxTile),
                                                    int2fixed(vyTile),
                                                    phi,
                                                    posPtr);
                        posPtr->id.sprite.already = 1;
                      }
                      break;
                  }

                  vxInter += vxStep;
                  vyInter += vyStep;
                  vxTile  += dxTile;

                }
          }
        }

//        if (vdist >= out_of_map && hdist >= out_of_map) //Is out of map
//           continue;

        if (hdist<vdist)        //Check for lowest distance
        {
          position       = *map->GetBlock(param.index = hIndex);
          param.txtOffs  = fixed2int(frac(hxInter)<<7);

          if (phi<ANGLE_180) param.side = WEST;
          else param.side = EAST;

          if (position.type == Map::WALL)
          {
            if (hyInter>y)  param.txtOffs = 127 - param.txtOffs;

            switch (param.side)
            {
              case EAST:  param.id   = position.id.wall.id_E; break;
              case WEST:  param.id   = position.id.wall.id_W; break;
            }

          }
          else if (position.type == Map::DOOR)
          {
               param.txtOffs += position.id.door.state;
               param.txtOffs  = 127 - param.txtOffs;
               param.id       = position.id.door.id;
          }
          param.dist     = hdist;
        }
        else
        {
          position       = *map->GetBlock(param.index = vIndex);
          param.txtOffs  = fixed2int(frac(vyInter)<<7);

          if (phi>ANGLE_90 && phi<ANGLE_270)  param.side = NORTH;
          else                                param.side = SOURTH;

          if (position.type == Map::WALL)
          {
            if (vxInter<x)  param.txtOffs = 127 - param.txtOffs;
            switch (param.side)
            {
              case NORTH:  param.id   = position.id.wall.id_N; break;
              case SOURTH: param.id   = position.id.wall.id_S; break;
            }
          }
          else if (position.type == Map::DOOR)
          {
               param.txtOffs  -= position.id.door.state;
               param.id        = position.id.door.id;
          }
          param.dist           = vdist;
        }

        param.dist = mul(param.dist,cos(rayAngle[i]));      //fish-eye
        param.x    = i;

        zbuffer[i] = param.dist;      //Set z-buffer value

                 //Draw vertical span
        DrawLine(&param);
   }

   if (spriteSystem)
          spriteSystem->DrawView();
}

RayCast::RayCast(Screen* video,BOOL autoreset)
        :zbuffer(new Fixed [video->width()]),
         rayAngle(new Angle [video->width()]),
         screen(video),
         h1(NULL),h2(NULL),dyTable(NULL),spriteSystem(NULL),
         txtOffs(NULL),
         widthFlag(TRUE), heightFlag (TRUE)
{
  dout<<"Ray casting: Set default params"<<endl;

  SetParam(VIEW_ANGLE,NORM_VIEW_ANGLE);  //60 deg
  SetParam(FOCUS,NORM_FOCUS);
  SetParam(WALL_HEIGHT,NORM_WALL_HEIGHT);
  SetParam(WALL_FEEL,NORM_WALL_FEEL);
  SetParam(DETAIL_ANGLE,NORM_DETAIL_ANGLE);

  out_of_map   = int2fixed(OUT_OF_MAP);
  screenWidth  = screen->width();
  screenHeight = screen->height();

  if (autoreset) RayCast::Reset();
}

RayCast::RayCast(int w,int h,BOOL autoreset)
        :zbuffer(new Fixed [w]),
         rayAngle(new Angle [w]),
         screen(NULL),
         h1(NULL),h2(NULL),dyTable(NULL),screenWidth(w),screenHeight(h),
         spriteSystem(NULL),txtOffs(NULL),
         widthFlag(TRUE), heightFlag (TRUE)
{
  dout<<"Ray casting: Set default params"<<endl;

  SetParam(VIEW_ANGLE,NORM_VIEW_ANGLE);  //60 deg
  SetParam(FOCUS,NORM_FOCUS);
  SetParam(WALL_HEIGHT,NORM_WALL_HEIGHT);
  SetParam(WALL_FEEL,NORM_WALL_FEEL);
  SetParam(DETAIL_ANGLE,NORM_DETAIL_ANGLE);

  out_of_map   = int2fixed(OUT_OF_MAP);

  if (autoreset) RayCast::Reset();
}

RayCast::~RayCast()
{
  if (spriteSystem) delete spriteSystem;

  delete zbuffer;
  delete rayAngle;
  delete h1,h2;
  delete txtOffs;
  delete dyTable;
}

/////////////////////implementation of class Stage Render////////////////////

void    StageRender::SetTexture(void* text)
{
  wtf           = (TextCache*)text;
  textFlag      = TRUE;
//  Reset();
}

void    StageRender::Reset()
{
   if (heightFlag || textFlag)
   {
     RayCast::Reset();

     if (scaleTable) delete scaleTable;

     long htSize = (int)pow(2,wallFeel);

     scaleTable = new int     [htSize];

          //Установка таблицы высот для построения столбцов
     if (wtf)
     {
       Fixed   dist;
       double  d;

       shift = 16-(wallFeel/2);
       wallMask = htSize-1;
       for (long ij=0;ij<htSize;ij++)
       {
        dist = ij<<shift;
        if (dist>=out_of_map)              //Max posible length
          continue;

        d   = fixed2float(dist);

        double   dy    = d*128.0/(focus*screenHeight);    //text height

        scaleTable[ij] = 0;
        while (dy>=1.0)
        {
          dy /= 2;
          scaleTable[ij]++;
          dyTable[ij]>>=1;
        }
        if (scaleTable[ij]>= ((MMT*)wtf->GetData(0))->count())
           scaleTable[ij]  = ((MMT*)wtf->GetData(0))->count()-1;
       }
     }

     textFlag   = FALSE;
     heightFlag = FALSE;
   }

   if (widthFlag)
      RayCast::Reset ();
}

StageRender::StageRender(Screen* video,BOOL autoreset)
            :RayCast(video,FALSE),
             wtf(NULL),scaleTable(NULL),
             textFlag (TRUE)
{
  if (autoreset) StageRender::Reset();
}

StageRender::~StageRender()
{
  delete scaleTable;
}

//////////////////////implementation of class RayCast8bit////////////////////

void   RayCast8bit::DrawLine(TextParam* param)
{
#pragma pack(1)
    union      TextIndex
    {
      Fixed     _fx;
      struct    X
      {
              uint16     reserved;
              uchar      index;
      }x;
    }y;
#pragma pack(0)

    MMT*   image    = (MMT*)wtf->GetData(param->id);

    int    index    = (param->dist>>shift) & wallMask;  //Index in height tables
    int    level    = scaleTable[index];
    int    height   = h2[index];
    Fixed  dy       = dyTable[index];
    int    shf      = 7-level;        //textHeight
           y._fx    = 0;
    uchar* vptr     = frame+h1[index]+param->x;
    uchar* pic      = (uchar*)image->GetTexture(level)->data+(param->txtOffs>>level); //opt
           pic     += txtOffs[index]*128;

    for (int j=0;j<height;j++,y._fx+=dy,vptr += screenWidth)
        *vptr = pic[y.x.index<<shf];
}

RayCast8bit::RayCast8bit(fScreen8bit* video,BOOL autoreset)
            : StageRender(video,FALSE)
{
  frame = video->GetPtr();

  int h1 = (screenHeight-50)/wallHeight + 50/wallHeight;
  spriteSystem = new SpriteSystem (new RenderSprite8bit(video,zbuffer,h1),
                                   new SpriteList(MAX_VIS_SPRITES),
                                   TRUE);

  if (autoreset) RayCast8bit::Reset ();
}

/////////////////implementation of class RayCastShadow///////////////////////

void    RayCastShadow::DrawLine(TextParam* param)
{
#pragma pack(1)
    union      TextIndex
    {
      Fixed     _fx;
      struct    X
      {
            uint16     reserved;
            uchar      index;
      }x;
    }y;
#pragma pack(0)

    MMT*   image    = (MMT*)wtf->GetData(param->id);

    int    llevel   = param->dist>>DISTANCE_LIGHT_SHIFT;
    uchar* ltable   = shadow.light[llevel<MAX_LIGHT_LEVELS-1 ? llevel : MAX_LIGHT_LEVELS-1];
    int    index    = (param->dist>>shift) & wallMask;  //Index in height tables
    int    level    = scaleTable[index];
    int    height   = h2[index];
    Fixed  dy       = dyTable[index];
    int    shf      = 7-level;        //textHeight
           y._fx    = 0;
    uchar* vptr     = frame+h1[index]+param->x;
    uchar* pic      = (uchar*)image->GetTexture(level)->data+(param->txtOffs>>level); //opt
           pic     += txtOffs[index]*128;

    for (int j=0;j<height;j++,y._fx+=dy,vptr += screenWidth)
        *vptr = ltable[pic[y.x.index<<shf]];
}

RayCastShadow::RayCastShadow(fScreen8bit* video,BOOL autoreset)
              : RayCast8bit(video,FALSE)
{
  if (autoreset) RayCastShadow::Reset ();
}

////////////////////implementation of class RayCastLightMap//////////////////

RayCastLightMap::RayCastLightMap(fScreen8bit* video,BOOL autoreset)
               :RayCast8bit(video,FALSE), lightMap(NULL)
{
  if (autoreset) RayCastLightMap::Reset();
}

RayCastLightMap::~RayCastLightMap()
{  }

void    RayCastLightMap::DrawLine(TextParam* param)
{
#pragma pack(1)
    union      TextIndex
    {
      Fixed     _fx;
      struct    X
      {
            uint16     reserved;
            uint16     index;
      }x;
    }y;
#pragma pack(0)
    MMT*    image    = (MMT*)wtf->GetData(param->id);

    int     llevel   = lightMap->GetLightLevel(param->index,param->side,
                                              param->txtOffs>>DIAPOS_SHIFT)
                      +(param->dist>>DISTANCE_LIGHT_SHIFT);
    uchar*  ltable   = shadow.light[llevel<MAX_LIGHT_LEVELS-1 ? llevel : MAX_LIGHT_LEVELS-1];
    int     index    = (param->dist>>shift) & wallMask;  //Index in height tables
    int     level    = scaleTable[index];
    int     height   = h2[index];
    Fixed   dy       = dyTable[index];
    int     shf      = 7-level;        //textHeight
            y._fx    = 0;

    register uchar*  vptr     = frame+h1[index]+param->x;
    uchar*  pic      = (uchar*)image->GetTexture(level)->data+(param->txtOffs>>level); //opt
             pic     += txtOffs[index]*128;

    for (register int j=0;j<height;j++,y._fx+=dy,vptr+=screenWidth)
        *vptr = ltable[pic[y.x.index<<shf]];
}

//////////////////////implementation of class SimpleStageRender//////////////

SimpleStageRender::SimpleStageRender(fScreen8bit* video): RayCast(video)
{
  frame = video->GetPtr();
}

void    SimpleStageRender::DrawLine(TextParam* param)
{
    int    llevel   = param->dist>>18;
    uchar* ltable   = shadow.light[llevel<MAX_LIGHT_LEVELS-1 ? llevel : MAX_LIGHT_LEVELS-1];
    int    index    = (param->dist>>shift) & wallMask;  //Index in height tables
    int    height   = h2[index];

    uchar* vptr     = frame+h1[index]+param->x;

    for (int j=0;j<height;j++,vptr += screenWidth)
        *vptr = ltable[WALL_V_COLOR];
}

/////////////////implementatio of class RayCast16bit/////////////////////////

RayCast16bit::RayCast16bit(fScreen16bit* video)
             :RayCast(video,FALSE),
             frame(video->GetPtr())
{
  if (autoreset) RayCast16bit::Reset ();
}

void    RayCast16bit::DrawLine(TextParam* param)
{
  //opt
#pragma pack(1)
    union      TextIndex
    {
      Fixed     _fx;
      struct    X
      {
            uint       reserved  : 16;
            uint       index     : 8;
      }x;
    }y;
#pragma pack(0)


}

////////////////////implementation of class Floor////////////////////////////

BOOL    Floor::SetParam(int id,double value)
{
  switch (id)
  {
    case H:
         h          = value;
         heightFlag = TRUE;
         return TRUE;
    case D0:
         d0         = value;
         heightFlag = TRUE;
         return TRUE;
    case VIEW_ANGLE:
         viewAngle  = value;
         angleFlag  = TRUE;
         return TRUE;
    case FIRST_LINE:
         height       = screenHeight-value;
         if (screenHeight == 200) height++; //no
         frame        = screen->GetPtr()+(ulong)(screenWidth*(uint)value);
         heightFlag   = TRUE;
         return TRUE;
    case WALL_HEIGHT:
         wallHeight  = value;
         angleFlag   = TRUE;
         return TRUE;
    default: return FALSE;
  }
}

double   Floor::GetParam(int id)
{
  switch (id)
  {
    case H:             return     h;
    case D0:            return     d0;
    case DELTA:         return     delta;
    case C:             return     c;
    case FIRST_LINE:    return     screenHeight-height;
    case VIEW_ANGLE:    return     viewAngle;
    case WALL_HEIGHT:   return     wallHeight;
    default:            return     -1;
  }
}

void    Floor::Reset()
{
        //Инициализация
  c      = viewAngle / (float)screenWidth;
  delta  = h         / (float)height;

  if (angleFlag)
  {
    dout<<"Floor: Reset angle tables..."<<endl;

    Angle a;
    for (long ij=0;ij<8192;ij++)
    {
      a = ij<<3;
      double d = angle2float(a);
      CSin[ij] = float2fixed  (sin(d)*c*wallHeight);
      CCos[ij] = -float2fixed (cos(d)*c*wallHeight);
    }
  }

  if (heightFlag)
  {
    dout<<"Floor: Reset distance tables..."<<endl;

    if (distTable)     delete distTable;
    if (scaleTable)    delete scaleTable;

    distTable  = new Fixed   [height];
    scaleTable = new uint    [height];

    double scale;
    for (int row = 0;row<height;row++)
    {
//      distTable[row]  = float2fixed(h*d0/((1+row)*delta));
      distTable [row] = float2fixed(h*d0/(h-(height-row)*delta));
      if (!mmt)
         continue;
      scale =  c*h*d0/((1+row)*delta); //view angle
      scaleTable[row] = 0;
      while (scale>1.0)
      {
        scale     /= 2;
        scaleTable[row]++;
      }
      if (scaleTable[row]>=mmt->count())
         scaleTable[row] = mmt->count()-1;
    }
  }

  angleFlag  = FALSE;
  heightFlag = FALSE;
}

void    Floor::SetTexture(MipMapText* text)
{
  mmt         = text;
  heightFlag  = TRUE;
//  Reset();
}

void    Floor::DrawView(Fixed x,Fixed y,Angle angle)
{
  uint16*        vptr1 = (uint16*)frame;
  uint16*        vptr2 = vptr1 + quartWidth;

  const  Fixed*        dist   = distTable;
  const  uint*         _scale = scaleTable;
  const  Fixed         Csin   = CSin[angle>>3],
                       Ccos   = CCos[angle>>3];
  const  Fixed         _sin   = sin(angle),
                       _cos   = cos(angle);

  Fixed  du, dv;

  Fixed  v1, u1,
         v2, u2;

  uint   i,j;

  uint          shift,
                level,
                mask;

  uchar*        curImage;
  int           curLevel = -1;

  for (int row = 0;row<height;row++,dist++,_scale++,
      vptr1+=halfWidth,vptr2+=quartWidth)
  {
    level       = *_scale;

    shift       = 7-level;      //textHeight
    mask        = 127>>level;   //opt        textHeight

    u1 = u2 = (x+mul(*dist,_cos))>>level;
    v1 = v2 = (y+mul(*dist,_sin))>>level;
    du      = mul(*dist,Csin)>>level;
    dv      = mul(*dist,Ccos)>>level;

    if (curLevel != level)      //opt
        curImage = (uchar*)mmt->GetTexture(curLevel=level)->data;

    vptr1 += quartWidth;

        //Оптимизировать по размеру >>shift!
    for (int col = quartWidth-1;col>=0;col--)
    {
      i = fixed2int(u1) & mask;
      j = fixed2int(v1) & mask;
      *--vptr1 = *(uint16*)(curImage+i+(j<<shift));
      i = fixed2int(u2) & mask;
      j = fixed2int(v2) & mask;
      *vptr2++ = *(uint16*)(curImage+i+(j<<shift));

      u1  -= du; v1 -= dv;
      u2  += du; v2 += dv;
    }
  }
}

Floor::Floor(fScreen8bit* scr,BOOL autoreset)
  :screen(scr),
   CCos(new Fixed [8192]),CSin(new Fixed [8192]),
   distTable(NULL),scaleTable(NULL),
   mmt(NULL),
   angleFlag (TRUE), heightFlag (TRUE)
{
  dout<<"Floor: Set default params..."<<endl;

  screenWidth  = screen->width();
  screenHeight = screen->height();
  halfWidth    = screen->width()/2;
  quartWidth   = halfWidth/2;
  frame        = screen->GetPtr();

  SetParam(H,100.0);
  SetParam(D0,100.0);
  SetParam(VIEW_ANGLE,NORM_VIEW_ANGLE);
  SetParam(FIRST_LINE,100);

  if (autoreset) Floor::Reset();
}

Floor::~Floor()
{
  delete distTable;
  delete scaleTable;
  delete CSin;
  delete CCos;
}

////////////////////////////implementation of class Floor Shadow/////////////

FloorShadow::FloorShadow(fScreen8bit* video,BOOL autoreset)
            :Floor(video,FALSE),lightTable(NULL)
{
  if (autoreset) FloorShadow::Reset ();
}

FloorShadow::~FloorShadow()
{
  if (lightTable) delete lightTable;
}

void    FloorShadow::Reset()
{
  BOOL hF = heightFlag;

  Floor::Reset();

  if (hF)
  {
    dout<<"Floor shadow: Reset system tables..."<<endl;

    if (lightTable) delete lightTable;
    lightTable    = new uchar [height];

    float light_x = MAX_LIGHT_LEVELS/14.0,     // /2.0
          light_delta = (float)MAX_LIGHT_LEVELS/(14.0*(float)height); // /2.0

    for (int i=0;i<height;i++,light_x-=light_delta)    //??
        lightTable[i] = (int)light_x;
  }
}

void    FloorShadow::DrawView(Fixed x,Fixed y,Angle angle)
{
  uchar*        vptr1 = (uchar*)frame;
  uchar*        vptr2 = vptr1 + halfWidth;

         uchar*        light  = shadow.light[0];
  const  Fixed*        dist   = distTable;
  const  uint*         _scale = scaleTable;
  const  Fixed         Csin   = CSin[angle>>3],
                       Ccos   = CCos[angle>>3];
  const  Fixed         _sin   = sin(angle),
                       _cos   = cos(angle);

  Fixed  du, dv;

  Fixed  v1, u1,
         v2, u2;

  uint   i,j;

  uint          shift,
                level,
                mask;

  uchar*        curImage;
  int           curLevel = -1;

  for (int row = 0;row<height;row++,dist++,_scale++,
      vptr1+=screenWidth,vptr2+=halfWidth)
  {
    light       = shadow.light[lightTable[row]];
    level       = *_scale;

    shift       = 7-level;      //text height
    mask        = 127>>level;   //opt text height

    u1 = u2 = (x+mul(*dist,_cos))>>level;
    v1 = v2 = (y+mul(*dist,_sin))>>level;

    du      = mul(*dist,Csin)>>level;
    dv      = mul(*dist,Ccos)>>level;

    if (curLevel != level)      //opt
        curImage = (uchar*)mmt->GetTexture(curLevel=level)->data;

    vptr1 += halfWidth;

        //Оптимизировать по размеру >>shift!
    for (int col = halfWidth-1;col>=0;col--)
    {
      i = fixed2int(u1) & mask;
      j = fixed2int(v1) & mask;
      *--vptr1 = light[curImage[i+(j<<shift)]];
      i = fixed2int(u2) & mask;
      j = fixed2int(v2) & mask;
      *vptr2++ = light[curImage[i+(j<<shift)]];

      u1  -= du; v1 -= dv;
      u2  += du; v2 += dv;
    }
  }
}

//////////////////implementation of class FloorUp////////////////////////////////

FloorUp::FloorUp(fScreen8bit* scr,BOOL autoreset)
    :Floor(scr,FALSE)
{
  SetParam (H,100.0);
  SetParam (D0,100.0);
  SetParam (LAST_LINE,100);
  SetParam (VIEW_ANGLE,1.0);

  if (autoreset) FloorUp::Reset ();
}

void   FloorUp::Reset()
{
  BOOL hF = heightFlag;

  Floor::Reset();

  if (hF)
  {
    dout<<"FloorUp: Reset system tables..."<<endl;

          //Пересчет таблиц для потолка (обратны полу)
    for (int i=0;i<height/2;i++)
    {
      Fixed scaleSwap           = scaleTable [height-i-1];
      scaleTable [height-i-1]   = scaleTable [i];
      scaleTable [i]            = scaleSwap;

      Fixed distSwap            = distTable [height-i-1];
      distTable [height-i-1]    = distTable [i];
      distTable [i]             = distSwap;
    }
  }
}

BOOL    FloorUp::SetParam(int id,double value)
{
  switch (id)
  {
    case LAST_LINE:
         height     = value;
         frame      = screen->GetPtr();
         heightFlag = TRUE;
         return TRUE;
    default: return Floor::SetParam(id,value);
  }
}

double   FloorUp::GetParam(int id)
{
  switch (id)
  {
    case LAST_LINE:     return     height;
    default:            return     Floor::GetParam(id);
  }
}

//////////////////////////implementation of class FloorUpShadow//////////////////

FloorUpShadow::FloorUpShadow(fScreen8bit* scr)
          :FloorShadow(scr,FALSE)
{
  if (autoreset) FloorUpShadow::Reset ();
}

void   FloorUpShadow::Reset()
{
  BOOL hF = heightFlag;

  FloorShadow::Reset();

  if (hF)
  {
    dout<<"FloorUp: Reset system tables..."<<endl;

          //Пересчет таблиц для потолка (обратны полу)
    for (int i=0;i<height/2;i++)
    {
      Fixed scaleSwap           = scaleTable [height-i-1];
      scaleTable [height-i-1]   = scaleTable [i];
      scaleTable [i]            = scaleSwap;

      Fixed distSwap            = distTable [height-i-1];
      distTable [height-i-1]    = distTable [i];
      distTable [i]             = distSwap;

      uchar lightSwap           = lightTable [height-i-1];
      lightTable [height-i-1]   = lightTable [i];
      lightTable [i]            = lightSwap;
    }
  }
}

BOOL    FloorUpShadow::SetParam(int id,double value)
{
  switch (id)
  {
    case LAST_LINE:
         height     = value;
         frame      = screen->GetPtr();
         heightFlag = TRUE;
         return TRUE;
    default: return Floor::SetParam(id,value);
  }
}

double   FloorUpShadow::GetParam(int id)
{
  switch (id)
  {
    case LAST_LINE:     return     height;
    default:            return     Floor::GetParam(id);
  }
}

//////////////////////////implementation of class Render/////////////////////

BOOL    Render::SetParam(int id,double value)
{
  switch (id)
  {
    case VIEW_ANGLE:
    {
         stage->SetParam (RayCast::VIEW_ANGLE,value);
         floor->SetParam (Floor::VIEW_ANGLE,value);
         sky->SetParam   (Floor::VIEW_ANGLE,value);
         return TRUE;
    }

    case WALL_HEIGHT:
    {
         stage->SetParam(RayCast::WALL_HEIGHT,value);
         double screenHeight = screen->height();

         double h = (float)screenHeight/value;

         double skyHeight    = h;
         double floorHeight  = screenHeight-h;

         sky->SetParam   (Floor::LAST_LINE,h);
         sky->SetParam   (Floor::WALL_HEIGHT,floorHeight/skyHeight);
         floor->SetParam (Floor::FIRST_LINE,h);
         floor->SetParam (Floor::WALL_HEIGHT,1.0);
         return TRUE;
    }

    case FOCUS:
    {
         stage->SetParam (RayCast::FOCUS,value);

         double screenHeight = screen->height();

         double h = (float)screenHeight/
                           stage->GetParam (RayCast::WALL_HEIGHT);

         double skyHeight    = h;
         double floorHeight  = screenHeight-h;

         floor->SetParam (Floor::H,floorHeight);
         floor->SetParam (Floor::D0,value*128.0);

         sky->SetParam   (Floor::H,skyHeight);
         sky->SetParam   (Floor::D0,value*128.0);

         return TRUE;
    }
  }

  return FALSE;
}

double Render::GetParam(int id)
{
  switch (id)
  {
    case VIEW_ANGLE:    return stage->GetParam(RayCast::VIEW_ANGLE);
    case WALL_HEIGHT:   return stage->GetParam(RayCast::WALL_HEIGHT);
    default:            return -1;
  }
}

void    Render::Reset()
{
  stage->Reset();
  floor->Reset();
  sky->Reset();
}

void    Render::ClearBuffer(int id,int value)
{
  if (id & FRAME_BUFFER)  screen->cls(value);
  if (id & Z_BUFFER)      stage->ClearZBuffer(value);
}

void    Render::ClearBuffer(int id,Fixed value)
{
  if (id & FRAME_BUFFER)  screen->cls(fixed2int(value));
  if (id & Z_BUFFER)      stage->ClearZBuffer(fixed2int(value));
}

void    Render::NormPalette()
{
  dout<<"Render: Normalize palette ...";
  game_palette->ResetPalette();
  dout<<"OK"<<endl;
}

void    Render::DrawView(Fixed x,Fixed y,Angle angle)
{
  sky->DrawView   (x<<7,-y<<7,ANGLE_360-angle);
  floor->DrawView (x<<7,-y<<7,ANGLE_360-angle);

  stage->DrawView(x,y,angle);
}

int     Render::GetFPS(Fixed x,Fixed y,Angle a)
{
  dout<<"Render: Test FPS..."<<endl;
  Angle rot = ANGLE_360 - ANGLE_1;
  int    total = 0;
  double start=clock();
  for (Angle angle = 0;angle<rot;angle+=ANGLE_1,total++)
  {
      DrawView(x,y,angle);
      screen->refresh();
  }

  double totalTime=(clock()-start)/CLK_TCK;
  double fps = (float)total/totalTime;
  dout<<"Render: Total frames = "<<total<<endl;
  dout<<"Render: Total time   = "<<totalTime<<endl;
  dout<<"Render: FPS          = "<<fps<<endl;
  DrawView(x,y,a);    //Restore default settings

  return fps;    //frame-per-second
}

void    Render::Close()
{
  dout<<"Render: Close stage render..."<<endl;
  if (stage)  delete stage;
  stage  = NULL;

  dout<<"Render: Close floor render..."<<endl;
  if (floor)  delete floor;
  floor  = NULL;

  dout<<"Render: Close sky render..."<<endl;
  if (sky)  delete sky;
  sky  = NULL;

  dout<<"Render: Close system palette...";
  if (game_palette) delete game_palette;
  game_palette = NULL;
  dout<<"OK"<<endl;
}

BOOL    Render::Open(Screen* video,BOOL autoreset)
{
  Close();
  dout<<"Render: Open second screen"<<endl;
  screen       = video;

  dout<<"Render: Open stage render"<<endl;      //switch
  stage = new RayCastLightMap ((fScreen8bit*)screen,FALSE);

  dout<<"Render: Open floor render"<<endl;
  floor = new FloorShadow ((fScreen8bit*)screen,FALSE);

  dout<<"Render: Open sky render"<<endl;
  sky = new FloorUp ((fScreen8bit*)screen,FALSE);

  SetParam (Render::WALL_HEIGHT,stage->GetParam(RayCast::WALL_HEIGHT));
  SetParam (Render::FOCUS,NORM_FOCUS);

  dout<<"Render: Open game palette"<<endl;
  game_palette = new Palette(video);

  SetPalette (game_palette->GetPalette());

  if (autoreset) Render::Reset ();

  return TRUE;
}

Render::Render(Screen* video,BOOL autoreset)
       :game_palette(NULL),
        floor(NULL),stage(NULL),sky(NULL),screen(NULL)
{
  Open(video,autoreset);

  dout<<"Render: Construct OK"<<endl;
}

Render::~Render()
{
  Close();
}

