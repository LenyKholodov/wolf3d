#include <vesa.h>
#include <vga.h>
#include <keyboard.h>

#include <console/command.h>
#include <console/comdefs.h>

#include "fin.h"
#include "cons.h"
#include "palette.h"

#include "g_single.h"
#include "g_server.h"

#include <debug\debug.h>

Keyboard        kb;

VESAdevice      vesa;
vga13hScreen    vga;
Palette         palette (NULL);
Screen*         screen  = NULL;

Console*        console = NULL;

tBinFont*       systemFont = tBinFont::load ("fonts/font.fnt");

BOOL     Init            (int argc,char* argv []);
BOOL     InitVESA        ();
Screen*  InitMode        (int = 0x13) ;
Console* InitConsole     (Screen*,tBinFont*);

void     InitCommand     ();
void     DefaultCommand  ();
void     LoadAutoexec    ();

void     ChGrMode        (VarPool&, ParmStack&, ostream&);

void    main (int argc,char* argv[])
{
  InitVESA    ();
  InitCommand ();

  screen  = InitMode    ();
  console = InitConsole (screen,systemFont);

  screen  -> cls();
  console -> show();

  DefaultCommand();
  LoadAutoexec  ();

  while (1)
  {
        if (message_queue.IsEmpty())
           continue;

        KeyPress* k_message = (KeyPress*)message_queue.GetMessage();

        if (k_message->id == KEY_DOWN)
        {
          console->putc (k_message->chr);
          console->tick();
        }
  }

  if (console) delete console;
  if (screen)  delete screen;

  vesa.close();
  vga.close();
}

BOOL    InitVESA ()
{
  if (vesa.open() != VESAdevice::OK)
  {
    dout<<"Error open VESA"<<endl;
    return FALSE;
  }
  return TRUE;
}

Screen* InitMode (int i)
{
  Screen*       systemScreen;

  if (vesa.SetMode (modes[i]) != VESAdevice::OK)
     if (!vga.open ())
     {
       dout<<"Error open graphics mode"<<endl;
       return NULL;
     }
     else systemScreen = &vga ;
  else systemScreen = vesa.CurrentScreen();

  systemScreen->PutPalette (palette.GetPalette());

  return GimmeMirror (systemScreen);
}

Console*    InitConsole (Screen* scr,tBinFont* font)
{
  return new Console (scr,font);
}

void    LoadAutoexec ()
{
  char  buf [1024];

  dout<<"Load autoexec...";
  ifstream ifs("autoexec.cfg",ios::text | ios::in);
  if(ifs)
  {
    for(;ifs;) //screen->refresh()
    {
      ifs.getline(buf,1024);
      command(buf);
      if (console) console->PutString (buf);
    }
    ifs.close();
    dout<<"Ok"<<endl;
  }
  else
    dout<<"failed"<<endl;
}

void    InitCommand ()
{
  dout<<"Starting..."<<endl;
  dout<<"Allocate varpool...";
  if(!command.vpool.resize(256))
  {
    dout<<"no memory"<<endl;
    delete fin;
  }
  dout<<"Ok"<<endl;
  dout<<"Allocate parstack...";
  if(command.pstack.maxsize(100)!=100)
  {
    dout<<"no memory"<<endl;
    delete fin;
  }
  dout<<"Ok"<<endl;
  command.SetStream(&dout);
}

void    DefaultCommand()
{
  dout<<"Default commands...";
  command.vpool.setf("exit",CmdExit);
  command.vpool.setf("help",CmdHelp);
  command.vpool.setf("list",CmdList);
  command.vpool.setf("cls",CmdCls);
  command.vpool.setf("set",CmdSet);
  command.vpool.setf("kill",CmdKill);
  command.vpool.setf("alias",CmdAlias);
  command.vpool.setf("dump",CmdDump);

        //Your commands
  command.vpool.setf("grmode",ChGrMode);

  dout<<"Ok"<<endl;
}

//////////////////////////////For Command////////////////////////////////////

void    ChGrMode      (VarPool&, ParmStack& parm, ostream& os)
{
  if (parm.size()<1)
  {
    os<<"Too few parameters"<<endl;
    return;
  }

  if (console == NULL)
     return;

  if (screen)   delete screen;

  screen =        InitMode(parm[0].value.i);

  screen->cls();

  console->open (screen,systemFont);
  console->show ();
}