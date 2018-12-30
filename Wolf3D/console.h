#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <debug.h>
#include "bkeffect.h"
#include <misc.h>

class tConsole
{
  char* cptr;
public:
  static BOOL crBugMode;
  int width,height,pos,cline;
  char data[1];
  static int size(int w,int h) { return sizeof(tConsole)-1+w*h; }
  int size(void) const                { return size(width,height); }
  static tConsole* create(int w,int h);
  void clear(void)                { pos=cline=0; memset(cptr=data,' ',width*height); }
  void clearline(void)               { pos=0; memset(cptr=(data+cline*width),' ',width); }
  void newline(void)               { if(++cline>=height) cline=0; clearline(); }
  int  putc(int c);
};


class grConsole
{
protected:
  Screen            *mEScreen;      // effect screen
  ScreenEffect        *mSe;           // effect object
  tBinFont         *mFnt;                // console font
  int                mFntWidth;
  Screen            *mDrawScreen;   // draw screen
  tConsole*        mConsole;
  BOOL                mFntOwner,mESOwner;
  int                 mCursorBlinkValue;
  BOOL                  mVisible;
  int             mCurrentHeight;
  void                drawText(Screen* scr,int color);
  void                StartInit(void);
public:
  int  MaxHeight;
  BOOL eLink;
  int  textColor,
       etColor,
       bkColor;
  int  ShowSpeed;
  int  CursorBlinkSpeed;
  BOOL open (Screen* dS,tBinFont* font,Screen* e=NULL);
  BOOL open (Screen* dS,BinFile* font,Screen* e=NULL)
     { return open(dS,tBinFont::load(font),e)?mFntOwner=TRUE:FALSE; }
  BOOL open (Screen*);
  BOOL open(Screen* dS,const char* font,Screen* e=NULL)
     { return open(dS,tBinFont::load(font),e)?mFntOwner=TRUE:FALSE; }
  void putc(int c);
  void clear(void);
  void tick(void);
  void close(void);
  BOOL visible(void) const         { return mVisible; }
  void DoShow(void)                { mVisible=TRUE; }
  void DoHide(void)                { mVisible=FALSE; }
  BOOL IsValid(void) const { return mConsole!=NULL; }

  inline tConsole* console(void) { return mConsole; }

  grConsole(void)                                        { StartInit(); }
  grConsole(Screen* dS,tBinFont* font,Screen* e=NULL)
                                                { StartInit(); open(dS,font,e); }
  grConsole(Screen* dS,BinFile* font,Screen* e=NULL)
                                                 { StartInit(); open(dS,font,e); }
  grConsole(Screen* dS,const char* font,Screen* e=NULL)
                                                      { StartInit(); open(dS,font,e); }

  ~grConsole(void) { close(); }
};


#pragma pack(1);

class consoleBase;

class consoleBuf: public streambuf
{
public:
  virtual int overflow(int d);  // поведение по переполнению буфера записи
  virtual int underflow(void);  // поведение по переполнению буфера чтения
protected:
  grConsole* mGrc;
  consoleBuf(grConsole* grc):streambuf() { unbuffered(1); mGrc=grc; };
  virtual ~consoleBuf(void);
  friend class consoleBase;
};

class consoleBase : public virtual ios
{
  protected:
    consoleBase(void* fn):buf((grConsole*)fn) { ios::init(&this->buf); }
    consoleBuf buf;
};

class oConsolestream: public consoleBase, public ostream
{
  public:
    oConsolestream(void* fn):consoleBase(fn),ostream(&buf) {}
};

#pragma pack();

#endif