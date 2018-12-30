#ifndef __PALETTE__
#define __PALETTE__

#include <screen.h>

////////////////////////////////////////////////////////////////////////////
////Нормированная палитра
////Данные представлены в формате: 8-bit #rrrgggbb
////////////////////////////////////////////////////////////////////////////
class   Palette
{
  private:
           Screen*      dest;
           RGBStruct*   palette;
  public:
          Palette(Screen* dest);
          ~Palette() { delete palette; }

          void            ResetPalette();
  static  void            ResetPalette(Screen*);
          RGBStruct*      GetPalette() const { return palette; }

  static  int             rgb(int r,int g,int b)
                {return ((r>>5)<<5)+((g>>5)<<2)+(b>>6); }
  static  void            rgb(int color,int& r,int& g,int& b) {
                          r = (color>>5)<<5;
                          g = (color & 0x1C)<<3;
                          b = (color & 3)<<6;
          }
};

#endif