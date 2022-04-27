#ifndef __GRAPH_1__
#define __GRAPH_1__
#ifdef __BORLANDC__
       #include     <graphics.h>
#endif
#ifdef __WATCOMC__

#include <grtypes.h>
#include <vesa.h>

void  arc(int x, int y, int stangle, int endangle,int radius);
void  bar(int left,int top,int right,int bottom);
void  bar3d(int left,int top,int right,int bottom,int depth, int topflag);
void  circle(int x,int y,int radius);
void  cleardevice(void);
void clearviewport(void);
void  initgraph(int* __graphdriver,int* __graphmode,const char* __path);
void  setgraphmode(int mode);
void  closegraph(void);
void  restorecrtmode(void);
void  detectgraph(int* __graphdriver,int* __graphmode);
char* getdrivername( void );
char* getmodename( int __mode_number );
char* grapherrormsg(int __errorcode) { return NULL;}

void drawpoly(int numpoints,const int* polypoints);
void ellipse(int x,int y,int stangle,int endangle,int xradius,int yradius);
void fillellipse(int x,int y,int xradius,int yradius );
void fillpoly(int numpoints,const int* polypoints);
void floodfill(int x,int y,long border);
long getbkcolor(void);
long getcolor(void);
RGBStruct* getdefaultpalette( void );
void  getfillpattern(char* __pattern);
void  getfillsettings(void* __fillinfo);
int    getgraphmode(void);
void   getimage(int left,int top,int right,int bottom,tImage* bitmap);
void  getlinesettings(void* lineinfo);
long getmaxcolor(void);
int  getmaxmode(void);
int  getmaxx(void);
int  getmaxy(void);
long getpixel(int x,int y);
void getpalette(RGBStruct* __palette);
long getpalettesize( void );
void getviewsettings(cRect* viewport);
int  getx(void);
int  gety(void);
void graphdefaults(void);
int  graphresult(void);
long imagesize(int left,int top,int right,int bottom);
void line(int x1,int y1,int x2,int y2);
void linerel(int dx,int dy);
void lineto(int x,int y);
void moverel(int dx,int dy);
void moveto(int x,int y);
void outtext(const char* __textstring);
void outtextxy(int x,int y,const char* textstring);
void pieslice(int x,int y,int stangle,int endangle,int radius);
void putimage(int left,int top,const tImage* bitmap,int op=0);
void putpixel(int x,int y,long color);
void rectangle(int left,int top,int right,int bottom);
void sector(int X,int Y,int StAngle,int EndAngle,int XRadius,int YRadius );
void setallpalette(const RGBStruct* __palette);
void setaspectratio( int __xasp, int __yasp );
void getaspectratio(int* __xasp, int* __yasp);
void setbkcolor(int __color);
void setcolor(int __color);
void setfillpattern(const char* __upattern, int __color);
void setfillstyle(int __pattern, int __color);
unsigned setgraphbufsize(unsigned __bufsize);

void setlinestyle(int __linestyle, unsigned __upattern,int __thickness);
void setpalette(int __colornum, int __color);
void setrgbpalette(int __colornum,int __red, int __green, int __blue);
void settextjustify(int __horiz, int __vert);
void settextstyle(tBinFont* fnt);
void setviewport(int __left, int __top, int __right, int __bottom,int __clip=0);
void  setvisualpage(int __page);
void  setactivepage(int __page);
void  setwritemode( int __mode );
int   textheight(const char* __textstring);
int   textwidth(const char far *__textstring);

int registerbgidriver(void (*driver)(void)) {return -1;}
int registerfarbgidriver(void far *__driver) { return -1;}

#endif
#endif