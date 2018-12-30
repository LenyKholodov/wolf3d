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

   Block          position;

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
                  position   = *map->GetBlock(hIndex = Index(hyTile,hxTile));
//                  posPtr   = map->GetBlock(hyTile,hxTile);

                  switch (position.type)
                  {
                    case Map::TRIGER:
                    case Map::WALL:
                      hdist  = mul(hyInter-y,isin(phi));
                      hray   = 0;
                      casting--;
                      continue;

                    case Map::DOOR:
                      textureParam.txtOffs  = fixed2int(frac(hxInter)<<7);
                      textureParam.txtOffs  = 127 - textureParam.txtOffs;
                      if (position.id.door.state <= textureParam.txtOffs)
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
                                                     &position);
                         position.id.sprite.already = 1;
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
                  position   = *map->GetBlock(vIndex = Index(vyTile,vxTile));

                  switch (position.type)
                  {
                    case Map::TRIGER:
                    case Map::WALL:
                      vdist  = mul(vxInter-x,icos(phi));
                      vray  = 0;
                      casting--;
                      continue;

                    case Map::DOOR:
                      textureParam.txtOffs  = fixed2int(frac(vyInter)<<7);
//                      if (vxInter<x)  textureParam.txtOffs = 127 - textureParam.txtOffs;
                      if (position.id.door.state <= textureParam.txtOffs)
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
                                                    &position);
                        position.id.sprite.already = 1;
                      }
                      break;
                  }

                  vxInter += vxStep;
                  vyInter += vyStep;
                  vxTile  += dxTile;

                }
          }
        }

        if (hdist<vdist && hdist<out_of_map)        //Check for lowest distance
        {
          position       = *map->GetBlock(textureParam.index = hIndex);
          textureParam.txtOffs  = fixed2int(frac(hxInter)<<7);

          if (phi<ANGLE_180) textureParam.side = WEST;
          else textureParam.side = EAST;

          if (position.type == Map::WALL)
          {
            if (hyInter>y)  textureParam.txtOffs = 127 - textureParam.txtOffs;

            switch (textureParam.side)
            {
              case EAST:  textureParam.id   = position.id.wall.id_E; break;
              case WEST:  textureParam.id   = position.id.wall.id_W; break;
            }

          }
          else if (position.type == Map::DOOR)
          {
               textureParam.txtOffs += position.id.door.state;
               textureParam.txtOffs  = 127 - textureParam.txtOffs;
               textureParam.id       = position.id.door.id;
          }
          textureParam.dist     = hdist;
        }
        else if (vdist<out_of_map)
        {
          position       = *map->GetBlock(textureParam.index = vIndex);
          textureParam.txtOffs  = fixed2int(frac(vyInter)<<7);

          if (phi>ANGLE_90 && phi<ANGLE_270)  textureParam.side = NORTH;
          else                                textureParam.side = SOURTH;

          if (position.type == Map::WALL)
          {
            if (vxInter<x)  textureParam.txtOffs = 127 - textureParam.txtOffs;
            switch (textureParam.side)
            {
              case NORTH:  textureParam.id   = position.id.wall.id_N; break;
              case SOURTH: textureParam.id   = position.id.wall.id_S; break;
            }
          }
          else if (position.type == Map::DOOR)
          {
               textureParam.txtOffs  -= position.id.door.state;
               textureParam.id        = position.id.door.id;
          }
          textureParam.dist           = vdist;
        }
        else
        {
          zbuffer [i] = out_of_map;
          continue;     //out_of_map
        }

        textureParam.dist    = mul(textureParam.dist,cos(rayAngle[i]));  //fish-eye
        textureParam.x       = i;
        zbuffer[i]           = textureParam.dist;                    //Set z-buffer value

                 //Draw vertical span
        DrawLine();
   }

   if (spriteSystem)
          spriteSystem->DrawView();
}