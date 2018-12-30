#include "cross.h"
#include "map.h"
#include "defs.h"

#include <tables.h>

const int      STEP_BITS        = 7;
const int      STEP             = (1 << STEP_BITS);
const int      STEP_MASK        = (STEP - 1);
const int      VIEW_ANGLE       = ANGLE_360-ANGLE_1;
const int      WIDTH            = angle2int(VIEW_ANGLE)>>2;

enum
{
  HORZ_WALL = 0,
  VERT_WALL
};

static   uFixed   hdist,                //Horizontal distance
                  vdist;                //Vertical distance

static   uint     vxTile, vyTile,       //Вертикальные ячейки
                  hxTile, hyTile;       //Горизонтальные ячейки
static   int      dyTile,               //Horizontal trass step (ячейки)
                  dxTile;               //Vertical trass step (ячейки)
static   Fixed    vxInter, vyInter,     //Vertical trass
                  hxInter, hyInter,     //Horizontal trass
                  vxStep, vyStep,       //Vertical steps
                  hxStep, hyStep;       //Horizontal steps

static   int      hray,                 //Horizontal ray trass flag
                  vray;                 //Vertical ray trass flag
static   int      casting;              //Trassing flag

void    FindCross (Fixed x,
                   Fixed y,
                   Angle phi,
                   Fixed maxDist,
                   Map* map,
                   CrossIndex& ind)
{
   uint           hIndex,vIndex;        //Index for trass
   uint           xstart = fixed2int (x),
                  ystart = fixed2int (y);

   int            curIndex = Index (ystart,xstart);

   Block*         posPtr;

   vxTile = hxTile = xstart;            //First trass (ячейки)
   vyTile = hyTile = ystart;

   hdist  = vdist  = FIXED_OUT_OF_MAP; //!!

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
                   hdist = FIXED_OUT_OF_MAP;
                   casting--;
           }
           else
           {
             posPtr   = map->GetBlock(hIndex = Index(hyTile,hxTile));

             if (!posPtr->cross)
             {
                 hdist  = mul(hyInter-y,isin(phi));
                 hray   = 0;

                 casting--;
                 continue;
             }
             else
             {
                 hyTile  += dyTile;
                 hyInter += hyStep;
                 hxInter += hxStep;
             }
           }
     }
     if (vray)
     {
           vyTile = fixed2int(vyInter);
           if (vxTile>MAX_X_TILE || vyTile>MAX_Y_TILE)
           {
                   vray  = 0;
                   vdist = FIXED_OUT_OF_MAP;
                   casting--;
           }
           else
           {
             posPtr   = map->GetBlock(vIndex = Index(vyTile,vxTile));

             if (!posPtr->cross)
             {
                 vdist  = mul(vxInter-x,icos(phi));
                 vray  = 0;

                 casting--;
                 continue;
             }
             else
             {
                 vxInter += vxStep;
                 vyInter += vyStep;
                 vxTile  += dxTile;
             }

           }
     }
   }

   if (hdist<vdist && hdist<maxDist ||
      (hdist == FIXED_OUT_OF_MAP && vdist == FIXED_OUT_OF_MAP)) //??
   {
     ind.crossIndex = Index (hyTile,hxTile);
     ind.prevIndex  = Index (hyTile-dyTile,fixed2int(hxInter-hxStep));
     ind.dist       = hdist;

     if (phi<ANGLE_180) ind.side = WEST;
     else               ind.side = EAST;
   }
   else if (vdist<maxDist)
   {
     ind.crossIndex = Index (vyTile,vxTile);
     ind.prevIndex  = Index (fixed2int(vyInter-vyStep),vxTile-dxTile);
     ind.dist       = vdist;

     if (phi>ANGLE_90 && phi<ANGLE_270)  ind.side = NORTH;
     else                                ind.side = SOURTH;
   }
   else
   {
     ind.crossIndex  = -1;
     ind.prevIndex   = -1;
     ind.side        = NORTH;
     ind.dist        = maxDist;
   }

}

Angle   FindAngle       (Fixed x1,Fixed y1,Fixed x2,Fixed y2)
{
  Flag  flag ("FIND_ANGLE");

  Angle angle = atan( abs( div(y2-y1,abs(x2-x1)+512) ) );

  if (x1>x2) angle  = ANGLE_180 - angle;
  if (y1>y2) angle  = - angle;

  return angle;
}

int             FindSprites  (Fixed x,Fixed y,Angle angle,uint* vis_index,Map* map)
{
        int         i0 = fixed2int(x);
        int         j0 = fixed2int(y);
        Fixed       u0 = x & STEP_MASK;
        Fixed       v0 = y & STEP_MASK;
        Fixed       vx = cos (angle);
        Fixed       vy = sin (angle);
        Fixed       nx = vy;
        Fixed       ny = -vx;
        Fixed       da = ( ( nx >> 7 ) * ( VIEW_ANGLE >> 8 ) ) / WIDTH;
        Fixed       db = ( ( ny >> 7 ) * ( VIEW_ANGLE >> 8 ) ) / WIDTH;
        Fixed       a  = vx - ( nx >> 7 ) * ( VIEW_ANGLE >> 8 );
        Fixed       b  = vy - ( ny >> 7 ) * ( VIEW_ANGLE >> 8 );
        Fixed       u, v, u1, v1;
        Fixed       a1, b1;
        Fixed       d, dx, dy;

        int        i, j;
        int        di, dj;

        Block*      pos ;
        int         count  = 0;

        for ( int k = 0; k < WIDTH; k++ )
        {
                if ( a > 0 )
                {
                        di = 1;
                        u  = STEP - u0;
                        a1 = a;
                }
                else
                {
                        di = -1;
                        u  = u0;
                        a1 = -a;
                }

                if ( b > 0 )
                {
                        dj = 1;
                        v  = STEP - v0;
                        b1 = b;
                }
                else
                {
                        dj = -1;
                        v  = v0;
                        b1 = -b;
                }

                dx = u - STEP;
                dy = v - STEP;
                u *= b1;
                v *= a1;
                u1 = b1 << STEP_BITS;
                v1 = a1 << STEP_BITS;
                i  = i0;
                j  = j0;

                for (pos=map->GetBlock (Index(j,i));pos->cross || pos->type==Map::MONSTR;
                     pos=map->GetBlock (Index(j,i)))
                {
                        if (pos->type==Map::MONSTR)
                        {
                          if (pos->id.sprite.already == 0  && count<MAX_VIS_SPRITES)
                                {
                                   vis_index [count++]       = Index (j,i);
                                   pos->id.sprite.already    = 1;
                                }
                        }


                        if ( u >= v )
                        {
                                dy  += STEP;
                                u   -= v;
                                v    = v1;
                                j   += dj;
                        }
                        else
                        {
                                dx  += STEP;
                                v   -= u;
                                u    = u1;
                                i   += di;
                        }


                }

                a += da;
                b += db;
        }

   for (k=0;k<count;k++)
     map->GetBlock(vis_index[k])->id.sprite.already = 0;

   return count;
}

BOOL            IsVisible    (Fixed x,Fixed y,Fixed _x,Fixed _y,Angle _angle)
{
  Fixed dx = x-_x,
        dy = y-_y;

  if      (dx<0  && dy<0)   return _angle >= ANGLE_180 && _angle <= ANGLE_270;
  else if (dx<0  && dy>=0)  return _angle >= ANGLE_90  && _angle <= ANGLE_180;
  else if (dx>=0 && dy<0)   return _angle >= ANGLE_270 && _angle <= ANGLE_360;
  else if (dx>=0 && dy>=0)  return _angle >= 0         && _angle <= ANGLE_90;
  else return FALSE;

  return FALSE;
}

BOOL            IsCross      (Fixed _x1,Fixed _y1,Fixed _x2,Fixed _y2,Map* map)
{
  Block* block;
  int   x1 = fixed2int(_x1),
        y1 = fixed2int(_y1),
        x2 = fixed2int(_x2),
        y2 = fixed2int(_y2);

  int   dx = abs ( x2 - x1 );
  int   dy = abs ( y2 - y1 );
  int   sx = x2 >= x1 ? 1 : -1;
  int   sy = y2 >= y1 ? 1 : -1;

  if ( dy <= dx )
  {
    int        d  = ( dy << 1 ) - dx;
    int        d1 = dy << 1;
    int        d2 = ( dy - dx ) << 1;

    block = map->GetBlock(Index(y1,x1));

    if (block->type == Map::DOOR || block->type == Map::WALL ||
        block->type == Map::TRIGER)
          return FALSE;

    for ( int x = x1 + sx, y = y1, i = 1; i <= dx; i++, x += sx )
    {
      if ( d > 0 )
      {
         d += d2;
         y += sy;
      }
      else
        d += d1;

      block = map->GetBlock (Index(y,x));

      if (block->type == Map::DOOR || block->type == Map::WALL ||
          block->type == Map::TRIGER)
            return FALSE;
    }
  }
  else
  {
    int        d  = ( dx << 1 ) - dy;
    int        d1 = dx << 1;
    int        d2 = ( dx - dy ) << 1;

    block = map->GetBlock(Index(y1,x1));

    if (block->type == Map::DOOR || block->type == Map::WALL ||
        block->type == Map::TRIGER)
          return FALSE;

    for ( int x = x1, y = y1 + sy, i = 1; i <= dy; i++, y += sy )
    {
      if ( d > 0 )
      {
        d += d2;
        x += sx;
      }
      else
        d += d1;

      block = map->GetBlock (Index(y,x));

      if (block->type == Map::DOOR || block->type == Map::WALL ||
          block->type == Map::TRIGER)
            return FALSE;
    }
  }

  return TRUE;
}

