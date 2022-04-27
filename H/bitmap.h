#ifndef BITMAP_H
#define BITMAP_H
#include <stdio.h>
class Bitmap
{
 protected:
  int  width,height;
  int  bpp;
  char *data;
  int  offset;
  Bitmap(): width(0), height(0), bpp(8), offset(0),data(NULL) { };
  virtual ~Bitmap() {  };
 public:
  inline  virtual int GetWidth() { return width; }
  inline  virtual int GetHeight() { return height; };
  inline  virtual int GetBPP() { return bpp; };
  inline  virtual int GetPixel(int x,int y)
    { return data[(height-y)*width+x]; }
  virtual void Load(const char *)=0;
  virtual void Put(int x,int y);
};

class BMP: public Bitmap
{
 public:
  BMP():Bitmap() { };
  BMP(const char *);
  virtual ~BMP();
  virtual void Load(const char *);
};
#endif