#include "console.h"

BOOL tConsole::crBugMode=TRUE;

tConsole* tConsole::create(int w,int h)
{
  tConsole* ptr;
  if((ptr=(tConsole*)malloc(tConsole::size(w,h)))!=NULL)
  {
    ptr->width=w;
    ptr->height=h;
    ptr->clear();
  }
  return ptr;
}

int tConsole::putc(int c)
{
  switch(c)
  {
    case 0x00:                    break;
    case '\r': if(!crBugMode)
               {
                 clearline();
                 break;
               }
    case '\n': newline();   return 1;
    case '\t':
               {
                  int offset=8-(pos%8);
                  if(pos+offset>=width)
                  {
                    newline();
                    return 1;
                  }
                  pos+=offset;
                  cptr+=offset;
               }
               break;
    case 0x08: if(pos)
               {
                 *--cptr=' ';
                 pos--;
               }
               break;
    default:   if(pos<width)
               {
                 pos++;
                 *cptr++=c;
                 break;
               }
               newline();
               pos++;
               *cptr++=c;
               return 1;
  }
  return 0;
}
