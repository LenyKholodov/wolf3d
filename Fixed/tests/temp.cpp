void    FormATan(BinFile* file)
{
  char header[] = "const short __atan[8192] = \r{\r\t";
  file->write(header,strlen(header));

  char    buffer[100];
  int     count=0;

  double  step = tan(57.0)/8192;        //57
  double  _tan = 0;
  Angle   x;

  BOOL    already = -1;

  for (int ij=0;ij<8192;ij++,_tan+=step)
  {
   x = float2angle(atan(_tan));

   if (x>=ANGLE_30/2 && already<0)
   {
     step /=1.2;
     already = 0;
   }
   else if (x>=ANGLE_30 && already<1)
   {
       step /= 1.2;
       already = 1;
   }
   else if (x>=ANGLE_45 && already<2)
   {
       step /= 1.5;
       already = 2;
   }
   else if (x>=ANGLE_60 && already<3)
   {
       step /= 2.5;
       already = 3;
   }
   else if ((x>=ANGLE_60+ANGLE_10) && already<4)
   {
       step /=2.0;
       already = 4;
   }
   else if ((x>=ANGLE_60*1.25) && already<5)
   {
       step /=15.0;
       already = 5;
   }

   ltoa(x,buffer,10);

   if (count++==8)
   {
      file->write("\r\t",strlen("\r\t"));
      count=0;
   }
   file->write(buffer,strlen(buffer));
   file->write(", ",strlen(", "));
  }
  file->write("\r};\r\r",strlen("\r};\r\r"));
  cout<<float2fixed(_tan)<<endl;
}

/*void    FormATan(BinFile* file)
{
  char header[] = "const short __atan[8192] = \r{\r\t";
  file->write(header,strlen(header));

  char    buffer[100];
  int     count=0;

  Fixed   all  = float2fixed(FL_ANGLE_90);
  ulong   step;
  ulong   last = all;

  Angle*  tTable = new Angle [last];

  for (int ij=0;ij<all;ij++)
   tTable[ij] = float2angle(atan(fixed2float(ij)))*FL_PI;       //!!

  step = last/8192;

  ij = 0;
  for (int i=0;ij<8192;ij++,i+=step)
  {
   ltoa(tTable[i],buffer,10);

   if (count++==8)
   {
      file->write("\r\t",strlen("\r\t"));
      count=0;
   }
   file->write(buffer,strlen(buffer));
   file->write(", ",strlen(", "));
  }
  file->write("\r};\r\r",strlen("\r};\r\r"));
  delete tTable;
} */