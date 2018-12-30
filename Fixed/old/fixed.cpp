Fixed16  sin(Fixed16 angle)
{
  if (angle<FX_NULL)
     return sin(angle+FX_ANGLE_360);
  if (angle>=FX_ANGLE_360)
     return sin(angle-FX_ANGLE_360);

  if (0<=angle && angle<=FX_ANGLE_90)
       return _fx_sin(angle);
  else if (FX_ANGLE_90<angle && angle<=FX_ANGLE_180)
       return _fx_cos(angle-FX_ANGLE_90);
  else if (FX_ANGLE_180<angle && angle<=FX_ANGLE_270)
       return -_fx_sin(angle-FX_ANGLE_180);
  else
       return -_fx_cos(angle-FX_ANGLE_270);
}

Fixed16  cos(Fixed16 angle)
{
  if (angle<FX_NULL)
     return cos(angle+FX_ANGLE_360);
  if (angle>=FX_ANGLE_360)
     return cos(angle-FX_ANGLE_360);

  if (FX_NULL<=angle && angle<=FX_ANGLE_90)
     return _fx_cos(angle);
  else if (FX_ANGLE_90<angle && angle<FX_ANGLE_180)
     return -_fx_sin(angle-FX_ANGLE_90);
  else if (FX_ANGLE_180<angle && angle<=FX_ANGLE_270)
    return -_fx_cos(angle-FX_ANGLE_180);
  else
    return _fx_sin(angle-FX_ANGLE_270);
}

Fixed16  tan(Fixed16 angle)
{
  if (angle<FX_NULL)
     return tan(angle+FX_ANGLE_360);
  if (angle>=FX_ANGLE_360)
     return tan(angle-FX_ANGLE_360);

  if (FX_NULL<=angle && angle<=FX_ANGLE_90)
     return _fx_tan(angle);
  else if (FX_ANGLE_90<angle && angle<=FX_ANGLE_180)
     return -_fx_ctan(angle-FX_ANGLE_90);
  else if (FX_ANGLE_180<angle && angle<=FX_ANGLE_270)
     return _fx_tan(angle-FX_ANGLE_180);
  else
     return -_fx_ctan(angle-FX_ANGLE_270);
}

Fixed16  ctan(Fixed16 angle)
{
  if (angle<FX_NULL)
     return ctan(angle+FX_ANGLE_360);
  if (angle>=FX_ANGLE_360)
     return ctan(angle-FX_ANGLE_360);

  if (FX_NULL<=angle && angle<=FX_ANGLE_90)
     return _fx_ctan(angle);
  else if (FX_ANGLE_90<angle && angle<=FX_ANGLE_180)
    return -_fx_tan(angle-FX_ANGLE_90);
  else if (FX_ANGLE_180<angle && angle<=FX_ANGLE_270)
    return _fx_ctan(angle-FX_ANGLE_180);
  else
    return -_fx_tan(angle-FX_ANGLE_270);
}