#include <console/comdefs.h>
#include <mscreen.h>

#include <debug\debug.h>

#include "fin.h"
#include "keyboard.h"
#include "cons.h"

ostream         grout = cout;

Console::Console (Screen* screen,tBinFont* fnt)
        :conout(&grconsole), createFlag (FALSE),
        frameScreen (NULL)
{
  history = tHistory::create(256,20);
  if(history==NULL)
  {
    grout<<"Can't create history list"<<endl;
    delete fin;
  }

  open (screen,fnt);

  command.vpool.link("con_show_speed",(long*)&grconsole.ShowSpeed);
  command.vpool.link("con_show_height",(long*)&grconsole.MaxHeight);

//  grconsole.DoShow();
  grout<<"Running..."<<endl;

  grout = conout;        //!!

  command.SetStream (&conout);
}

Console::~Console()
{
  grconsole.close();
}

void    Console::PutString (const char* str)
{
  history->remember();
  grconsole.tick();
}

void    Console::tick()
{
  grconsole.tick();
}

void    Console::putc(char c,int key)
{
        switch(key)
        {
          case KEY_TILDA: // open/close console...
                   if(grconsole.visible())
                     grconsole.DoHide();
                   else
                     grconsole.DoShow();
                   break;
          case KEY_ENTER: conout<<endl;
                   command(history->cline);
                   history->remember();
                   conout<<']';
                   break;
          case KEY_CUP:
                 if(history->up())
                 {
                   grconsole.console()->clearline();
                   conout<<']'<<history->cline;
                 }
                 break;
          case KEY_CDOWN:
                 if(history->down())
                 {
                   grconsole.console()->clearline();
                   conout<<']'<<history->cline;
                 }
                 break;
          default:
          {
                 if (c == 0)
                    return;
                 if (history->putc(c))
                     conout<<c;
                 break;
          }
        }
}

void    Console::open (Screen* scr,tBinFont* font)
{
//  grconsole.close();

  BOOL flag = grconsole.visible();

  if (createFlag)
  {
    if (frameScreen) delete frameScreen;

    frameScreen = NULL;

    createFlag = FALSE;
  }

  if (scr->GetType()!=ID_MSCREEN)
  {
    frameScreen = GimmeMirror (scr);
    frameScreen->cls();

    createFlag = TRUE;
  }
  else
    frameScreen = scr;

  grconsole.open(frameScreen,font);

  if(frameScreen->bpp()==8)
    grconsole.textColor=0xFF;
  else
  {
    grconsole.textColor=0xFFFF;
    grconsole.etColor=0xF800;
  }

  grconsole.ShowSpeed=50;
  grconsole.MaxHeight=frameScreen->height()/2;

        //conout history

  grout = conout;
  command.SetStream (&conout);

  if (flag) grconsole.DoShow();
  else      grconsole.DoHide();
}

void    Console::show ()
{
  int showSpeed = grconsole.ShowSpeed,
      maxHeight = grconsole.MaxHeight;

  grconsole.MaxHeight = grconsole.MaxHeight*1.8;
  grconsole.ShowSpeed = 3000;
  grconsole.DoShow();
  grconsole.tick();

  grconsole.ShowSpeed = showSpeed;
  grconsole.MaxHeight = maxHeight;
}

void    Console::hide ()
{
  int showSpeed = grconsole.ShowSpeed,
      maxHeight = grconsole.MaxHeight;

  grconsole.MaxHeight = grconsole.MaxHeight*1.8;
  grconsole.ShowSpeed = 3000;
  grconsole.DoHide();
  grconsole.tick();

  grconsole.ShowSpeed = showSpeed;
  grconsole.MaxHeight = maxHeight;
}
