#ifndef __VIDEO_H
#define __VIDEO_H
void  cls(int col,int);
void  wind(int x1,int y1,int x2,int y2,int col,int);
void  ramka(int x1,int y1,int x2,int y2,int col);
void  doubleramka(int x1,int y1,int x2,int y2,int col);
void  svgainit(unsigned int regim);
void  pixel(unsigned int x,unsigned int y,int c);
void  setcol(unsigned int index,int r,int g,int b);
void  hide(void);
void  show(void);
#endif