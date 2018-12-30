void    LightCompile::SimpleCalc(int _x,int _y)
{
   uFixed         x    = int2fixed(_x),
                  y    = int2fixed(_y);
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

   uint           txtOffs;
   uint           hIndex,vIndex;        //Index for trass
   Fixed          out_of_map = int2fixed(OUT_OF_MAP);

   Block&         position = map->GetBlock(0);   //For faster trass

   for (Angle phi=0;phi<ANGLE_360;phi++) //!!
   {
        vxTile = hxTile = _x;            //First trass (ячейки)
        vyTile = hyTile = _y;

        hdist  = vdist  = out_of_map;

        hray = abs(sin(phi))<=FX_VERY_LOW ? 0 : 1;   //Проверка на совпадение
        vray = abs(cos(phi))<=FX_VERY_LOW ? 0 : 1;   //луча с осями
        casting = hray+vray;                         //Num of rays

        if (phi>=ANGLE_270 || phi<=ANGLE_90)    //Vertical ray settings
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
        if (phi<=ANGLE_180)     //Horizontal ray settings
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
                  position = map->GetBlock(hIndex = Index(hyTile,hxTile));
                  if (position.type != Map::FREE)
                  {
                     hdist  = mul(hyInter-y,isin(phi));
                     hray  = 0;
                     casting--;
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
                        casting--;
                }
                else
                {
                  position = map->GetBlock(vIndex = Index(vyTile,vxTile));
                  if (position.type != Map::FREE)
                  {
                          vdist  = mul(vxInter-x,icos(phi));
                          vray   = 0;
                          casting--;
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

        if (vdist >= out_of_map && hdist >= out_of_map) //Is out of map
           continue;

        if (hdist<vdist)        //Check for lowest distance
        {
//          hdist = mul(hdist,cos(phi));
          txtOffs  = fixed2int(frac(hxInter)<<7);
          if (hyInter>y)
             txtOffs = 127 - txtOffs;

          if (phi<ANGLE_180)
             ChangeLightLevel(hIndex,EAST,txtOffs>>DIAPOS_SHIFT,hdist>>(DISTANCE_LIGHT_SHIFT-2));
          else
             ChangeLightLevel(hIndex,WEST,txtOffs>>DIAPOS_SHIFT,hdist>>(DISTANCE_LIGHT_SHIFT-2));
        }
        else
        {
//          vdist = mul(vdist,cos(phi));
          txtOffs  = fixed2int(frac(vyInter)<<7);
          if (vxInter<x)
             txtOffs = 127 - txtOffs;

          if (phi>ANGLE_90 && phi<ANGLE_270)
             ChangeLightLevel(vIndex,NORTH,txtOffs>>DIAPOS_SHIFT,vdist>>(DISTANCE_LIGHT_SHIFT-2));
          else
             ChangeLightLevel(vIndex,SOURTH,txtOffs>>DIAPOS_SHIFT,vdist>>(DISTANCE_LIGHT_SHIFT-2));
        }

        //*= cos
   }
}