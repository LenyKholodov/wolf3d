/*

MMT*    mmt    (tImage* src)
{
  int        len      = src->width*src->height;
  int        level    = 0;
  int        width    = src->width,
             height   = src->height;
  int        w        = width,
             h        = height;
  uchar *    curText  [128];
  uunt16*    readPos     = src->data;

  do
  {
    curText[level]  = (uchar*)malloc (len*2+0x100);

    uint16  *readPos  = readPos,
            *writePos = (uint16*)curText [level];

    for (int y=0;y<height;y+=2)
    {
      readPos = readPos+y*src->width;
      for (int x=0;x<width;x+=2,readPos+=2,writePos++)
      {
//        *writePos = (*readPos + *(readPos+1) + *(readPos+width) +
//                    *(readPos+width+1))/4;
      }
    }

    width  /= 2;
    height /= 2;
    len    /= 4;
    level  += 1;
  }while (width>4 && height>4);

  MMT* text = makeMMT (curText,level,src->width,src->height,2);

  for (int i=0;i<level;i++)
      free (curText [i]);

  return text;
}
*/

  int   width  = src->width,
        height = src->height;

  uchar*  list[100];

  list[0]         = (uchar*)malloc(width*height*2);

  memcpy(list[0],src->data,width*height*2);

  int w = width,
      h = height;

  for (int level = 1;w>4 && h>4;level++)
  {
    int pw              = w;
    w/=2;
    h/=2;
    uint16*    prev  = (uint16*)list[level-1];
    list[level]      = (uchar*)malloc(w*h*2);
    uint16*    tmp   = (uint16*)list[level];

    for (int j=0;j<h;j++,prev+=pw)
    {
        for (int i=0;i<w;i++,prev+=2,tmp++)
        {
          int   r,g,b;

          r     = (findRed (*prev)      + findRed (*(prev+1))+
                   findRed (*(prev+pw)) + findRed (*(prev+pw+1)))/4;

          g     = (findGreen (*prev)      + findGreen (*(prev+1))+
                   findGreen (*(prev+pw)) + findGreen (*(prev+pw+1)))/4;

          b     = (findBlue (*prev)      + findBlue (*(prev+1))+
                   findBlue (*(prev+pw)) + findBlue (*(prev+pw+1)))/4;

          *tmp  = rgb (r,g,b);
        }
    }
  }