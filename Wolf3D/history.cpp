#include <malloc.h>
#include "history.h"

BOOL tHistory::remember(void)
{
  if(*cline=='\0')
    return FALSE;
  clselect=cline;
  cline+=width;
  if(cline-data>=width*height)
    cline=data;
  *(cchar=cline)='\0';
  return TRUE;
}

BOOL tHistory::updatehistory(void)
{
  if(clselect==cline || *clselect=='\0')
    return FALSE;
  strcpy(cline,clselect);
  cchar=cline+strlen(cline);
  return TRUE;
}

BOOL tHistory::up(void)
{
  BOOL result=updatehistory();
  if(clselect==data)
    clselect=data+width*(height-1);
  else
    clselect-=width;
  return result;
}

BOOL tHistory::down(void)
{
  BOOL result=updatehistory();
  clselect+=width;
  if(clselect-data>=width*height)
    clselect=data;
  return result;
}

tHistory* tHistory::create(int w,int h)
{
  tHistory* p;
  if((p=(tHistory*)malloc(size(w,h)))!=NULL)
  {
    p->width=w;
    p->height=h;
    p->clear();
  }
  return p;
}

BOOL tHistory::putc(int c)
{
  switch(c)
  {
    case 8:  // back space key
            if(cchar!=cline)
            {
                   *--cchar='\0';
                   return TRUE;
                }
             break;
    case 9: {
                  for(int i=8-(cchar-cline)%8;i-->1;putc(' '));
              return putc(' ');
            }
            break;
    default:
            if(cchar-cline<width-1)
                {
                    *cchar++=c;
                    *cchar='\0';
                    return TRUE;
                }
  }
  return FALSE;
}