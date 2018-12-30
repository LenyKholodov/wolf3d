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
                mask1,mask2,index;

  uchar*        curImage;
  int           curLevel = -1;

  for (int row = 0;row<height;row++,dist++,_scale++,
      vptr1+=screenWidth,vptr2+=halfWidth)
  {
    light       = shadow.light[lightTable[row]];
    level       = *_scale;

    shift       = 7-level;                 //text height
    mask1       = int2fixed(127>>level);   //opt text height
    mask2       = mask1<<shift;

    u1 = u2 = (x+mul(*dist,_cos))>>level;
    v1 = v2 = (y+mul(*dist,_sin))>>level<<shift;

    du      = mul(*dist,Csin)>>level;
    dv      = mul(*dist,Ccos)>>level<<shift;

    if (curLevel != level)      //opt
        curImage = (uchar*)mmt->GetTexture(curLevel=level)->data;

    vptr1 += halfWidth;

    for (int col = halfWidth-1;col>=0;col--)
    {
      index = fixed2int((v1&mask2)+(u1 & mask1));

      *--vptr1 = light[curImage[index]];

      index = fixed2int((v2&mask2)+(u2 & mask1));

      *vptr2++ = light[curImage[index]];

      u1  -= du; v1 -= dv;
      u2  += du; v2 += dv;
    }
  }
}