static int   __xSpr   = -5;
static Fixed __dSpr   = 1223;

void    RayCast::ProcessSprite (Fixed xView,Fixed yView,
                                Fixed xSprite,Fixed ySprite,
                                Angle angle)
{
        //not optimize
  double dx   = fixed2float(xSprite - xView);
  double dy   = fixed2float(ySprite - yView);

  double _sin = fixed2float(sin(angle));
  double _cos = fixed2float(cos(angle));

  double locY  = dx*_cos+dy*_sin;
  double locX  = dx*_sin-dy*_cos;

  double  proj   = (float)(screenWidth/2);
  double  xscale =  proj / locY;

  int     xstart = locX*xscale;
          xstart = screenWidth/2-xstart;

          __xSpr = xstart;

  Fixed dist     = float2fixed(fabs(dx)+fabs(dy));

  if (xstart<0 || xstart>screenWidth)
  {
     __dSpr = out_of_map;
     return ;
  }
  else
     __dSpr = dist;

        //Сформировать структуру и занести её в список
        //Продумать систему отслеживания повторного вызова

//  VisSprite*    vSprite = new VisSprite;

//  vSprite->dist         = dist;
//  vSprite->xstart       = xstart;
//  vSprite->ystart       = h1[(dist>>shift) & wallMask]/screenWidth; //opt

//        SimpleSprite*   sprite;

}

void   RayCast::DrawSprites ()
{
  if (__dSpr>=out_of_map)
     return;

  TextParam param;

  param.dist = __dSpr;
  param.side = NORTH;
  param.id   = 0;
  param.x    = __xSpr; //!!
  param.txtOffs = 4;
  param.index = 2;

  DrawLine(&param);

        //SpriteList ->reset()
}
