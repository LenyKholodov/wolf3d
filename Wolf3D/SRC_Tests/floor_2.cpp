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
                mask1,mask2,index;

  uchar*        curImage;
  int           curLevel = -1;

  for (int row = 0;row<height;row++,dist++,_scale++,
      vptr1+=halfWidth,vptr2+=quartWidth)
  {
    level       = *_scale;

    shift       = 7-level;                 //textHeight
    mask1       = int2fixed(127>>level);
    mask2       = mask1<<shift;

    u1 = u2 = (x+mul(*dist,_cos))>>level;
    v1 = v2 = (y+mul(*dist,_sin))>>level<<shift;
    du      = mul(*dist,Csin)>>level;
    dv      = mul(*dist,Ccos)>>level<<shift;

    if (curLevel != level)      //opt
        curImage = (uchar*)mmt->GetTexture(curLevel=level)->data;

    vptr1 += quartWidth;

    for (int col = quartWidth-1;col>=0;col--)
    {
      index = fixed2int((v1&mask2)+(u1 & mask1));

      *--vptr1 = *(uint16*)(curImage+index);

      index = fixed2int((v2&mask2)+(u2 & mask1));

      *vptr2++ = *(uint16*)(curImage+index);

      u1  -= du; v1 -= dv;
      u2  += du; v2 += dv;
    }
  }
}