#include <sbinfile.h>
#include <mbinfile.h>
#include "fixed.h"

#include <math.h>
#include <stdlib.h>

void    FormSin(BinFile* file);
void    FormInvSin(BinFile* file);
void    FormTan(BinFile* file);

int    main(int argc,char* argv[])
{
  memBinFile file;
  file.open(0xFFFFFF);

  FormSin(&file);
  FormInvSin(&file);
  FormTan(&file);

  stdBinFile res;

  if (argc<2)
     res.open("table.dat",BinFile::OPEN_RW);
  else
     res.open(argv[1],BinFile::OPEN_RW);

  res.write(file.GetBuffer(),file.getpos());

  return 0;
}

void    FormSin(BinFile* file)
{
  char header[] = "const long __sin[8192] = \r{\r\t";
  file->write(header,strlen(header));
  Angle a;
  double angle;
  Fixed  value;
  char    buffer[100];
  int     count=0;
  for (long ij=0;ij<8192;ij++) //!!!8192
  {
   a = ij<<3;
   angle = angle2float(a);
   value = float2fixed(sin(angle));

   ltoa(value,buffer,10);
   if (count++==8)
   {
      file->write("\r\t",strlen("\r\t"));
      count=0;
   }
   file->write(buffer,strlen(buffer));
   file->write(", ",strlen(", "));
  }
  file->write("\r};\r\r",strlen("\r};\r\r"));
}

void    FormInvSin(BinFile* file)
{
  char header[] = "const long __invSin[0x10000] = \r{\r\t";
  file->write(header,strlen(header));
  Angle a;
  double angle;
  Fixed  value;
  char    buffer[100];
  int     count=0;
  for (long ij=0;ij<0x10000;ij++) //!!!8192
  {
   a = ij;
   angle = angle2float(a);
   value = float2fixed(1.0/sin(angle));
   ltoa(value,buffer,10);
   if (count++==8)
   {
      file->write("\r\t",strlen("\r\t"));
      count=0;
   }
   file->write(buffer,strlen(buffer));
   file->write(", ",strlen(", "));
  }
  file->write("\r};\r\r",strlen("\r};\r\r"));
}

void    FormTan(BinFile* file)
{
  char header[] = "const long __tan[0x10000] = \r{\r\t";
  file->write(header,strlen(header));
  Angle a;
  double angle;
  Fixed   value;
  char    buffer[100];
  int     count=0;
  for (long ij=0;ij<0x10000;ij++) //!!!8192
  {
   a = ij;
   angle = angle2float(a);
   value = float2fixed(tan(angle));

   ltoa(value,buffer,10);
   if (count++==8)
   {
      file->write("\r\t",strlen("\r\t"));
      count=0;
   }
   file->write(buffer,strlen(buffer));
   file->write(", ",strlen(", "));
  }
  file->write("\r};\r\r",strlen("\r};\r\r"));
}