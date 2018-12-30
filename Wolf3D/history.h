#ifndef __HISTORY__
#define __HISTORY__

#include <types.h>
#include <string.h>

class tHistory
{
  BOOL updatehistory(void);
public:
  int width,height;
  char *cline,*clselect,*cchar;
  char data[1];
  inline static int size(int w,int h)  { return sizeof(tHistory)-1+w*h; }
  inline int size(void) const                { return size(width,height); }
  static tHistory* create(int w,int h);
  inline void clear(void)                { cline=clselect=cchar=data; memset(data,0,width*height); }
  inline void clearline(void)               { *(cchar=cline)='\0'; }
  BOOL putc(int c);
  BOOL up(void);
  BOOL down(void);
  BOOL remember(void);
};

#endif