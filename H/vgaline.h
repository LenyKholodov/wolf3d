//VGA.H
#ifndef VGA_LINE_H
#define VGA_LINE_H

#ifdef __WATCOMC__
#pragma library ("vgaline.lib")
#endif

const int COPY_PUT=0;
const int INV_FONE_PUT=1;

struct ColorType{
 char blue;
 char green;
 char red;
 char reserved;
};

struct SpriteType{
 int  width,height;
 char *data;
};

void VGALineInit();
void CloseGraph();
void ClearViewPort();
void WaitRetrance();

void PutPixel(const int,const int,const int);
int  GetPixel(const int,const int);

void SetColor(const int);
int  GetColor();
void SetBkColor(const int);
int  GetBkColor();
void SetFillColor(const int);
int  GetFillColor();
void MoveTo(const int,const int);
int  GetX();
int  GetY();
int  GetMaxX();
int  GetMaxY();

void Line(const int,const int,const int,const int);
void LineTo(const int,const int);
void LineRel(const int,const int);
void Rectangle(const int,const int,const int,const int);
void Bar(const int,const int,const int,const int);
void Circle(const int,const int,const int);
void Fill(const int,const int,const int);

void SetRGBPalette(const char,const char,const char,const char);
void GetRGBPalette(const char,char &,char &,char &);
void SetPalette(const ColorType[256]);

void CreateImage(SpriteType &,const unsigned int);
void DeleteImage(SpriteType &);
void GetiImage(const int,const int,const int,const int,SpriteType &);
void PutImage(int,int,const SpriteType,const int);

void UpsetX(const int,const int,const int,const int);
void UpsetY(const int,const int,const int,const int);
void UpsetScreenX();
void UpsetScreenY();
#endif