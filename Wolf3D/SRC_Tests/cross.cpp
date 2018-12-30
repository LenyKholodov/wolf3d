int             FindSprites  (Fixed x,Fixed y,Angle angle,uint* vis_index,Map* map)
{
   uint           vxTile, vyTile,       //Вертикальные ячейки
                  hxTile, hyTile;       //Горизонтальные ячейки
   int            dyTile,               //Horizontal trass step (ячейки)
                  dxTile;               //Vertical trass step (ячейки)
   Fixed          vxInter, vyInter,     //Vertical trass
                  hxInter, hyInter,     //Horizontal trass
                  vxStep, vyStep,       //Vertical steps
                  hxStep, hyStep;       //Horizontal steps

   int            hIndex ,vIndex;

   int            hray,                 //Horizontal ray trass flag
                  vray;                 //Vertical ray trass flag
   int            casting;              //Trassing flag

   int            count  = 0;           //Count of sprites

   uint           xstart = fixed2int (x),
                  ystart = fixed2int (y);

   Block          position;
   Block*         posPtr     = NULL;

   int            alpha= angle>>13;
   Angle          phi;
   Angle          step = ANGLE_1<<2;

   for (Angle ang=0;ang<ANGLE_360-step;ang+=step)
   {
        phi    = angle + ang;

        vxTile = hxTile = xstart;            //First trass (ячейки)
        vyTile = hyTile = ystart;

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

                  position = *posPtr;

                  switch (position.type)
                  {
                    case Map::TRIGER:
                    case Map::WALL:
                      hray   = 0;
                      casting--;
                      continue;

                    case Map::DOOR:
                    {
                      int offs  = fixed2int(frac(hxInter)<<7);
                      offs      = 127 - offs;
                      if (position.id.door.state <= offs)
                      {
                        hray   = 0;
                        casting--;
                        continue;
                      }
                      break;
                    }

                    case Map::MONSTR:
                      if (position.id.sprite.already == 0  && count<MAX_VIS_SPRITES &&
                          !((position.id.sprite.alpha-alpha)>>2))
                            {
                               vis_index [count++]       = hIndex;
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
                  position = *posPtr;

                  switch (position.type)
                  {
                    case Map::TRIGER:
                    case Map::WALL:
                      vray  = 0;
                      casting--;
                      continue;

                    case Map::DOOR:
                    {
                      int offs  = fixed2int(frac(vyInter)<<7);
                      if (position.id.door.state <= offs)
                      {
                        vray   = 0;
                        casting--;
                        continue;
                      }
                      break;
                    }

                    case Map::MONSTR:
                      if (position.id.sprite.already == 0 && count<MAX_VIS_SPRITES &&
                           !((position.id.sprite.alpha-alpha)>>2))
                            {
                               vis_index [count++]       = vIndex;
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
   }

        //Структура already не очищается !!!

   for (int i=0;i<count;i++)
     map->GetBlock(vis_index[i])->id.sprite.already = 0;

   return count;
}