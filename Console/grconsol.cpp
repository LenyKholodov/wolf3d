#define NDEBUG

#include <console/console.h>
#include <console/command.h>
#include <console/history.h>
#include <console/comdefs.h>

#include <fstream.h>
#include <strstrea.h>
#include <keyboard.h>
#include <fin.h>

VESAdevice      vesa;
Command         command;
grConsole       grconsole;
Keyboard        keyb;

class   Destruct: public FinClient
{
  public:
          virtual ~Destruct();
};

void main(void)
{
  debug("Starting..."<<endl);

  fin->AddClient(new Destruct);

  tHistory* history=tHistory::create(256,20);
  if(history==NULL)
  {
    debug("Can't create history list"<<endl);
    delete fin;
  }
  if(vesa.open()!=VESAdevice::OK)
  {
    debug("Can't init vesa"<<endl);
    delete fin;
  }
  if(vesa.SetMode(VESA640x480x8)!=VESAdevice::OK)
  {
    debug("Can't open requered mode"<<endl);
    delete fin;
  }

  Screen* mscr=GimmeMirror(vesa.CurrentScreen());
  Screen* mscr2=GimmeMirror(mscr);
  grconsole.open(mscr,"font.fnt");
  if(mscr->bpp()==8)
  {
    vesa.CurrentScreen()->PutPalette(192,64,((mScreen8bit*)mscr)->GetPalette()+192);
    grconsole.textColor=0x0F;
  }
  else
  {
    grconsole.textColor=0xFFFF;
    grconsole.etColor=0xF800;
  }
  grconsole.ShowSpeed=50;
  grconsole.MaxHeight=400;
  oConsolestream conout(&grconsole);
  grconsole.DoShow();
  debug("Running..."<<endl);
  mscr2->cls();
  int i=0;
//////////////////////////////////////////////////////////////////////////////
  conout<<"Starting..."<<endl;
  conout<<"Allocate varpool...";
  if(!command.vpool.resize(256))
  {
    conout<<"no memory"<<endl;
    delete fin;
  }
  conout<<"Ok"<<endl;
  conout<<"Allocate parstack...";
  if(command.pstack.maxsize(100)!=100)
  {
    conout<<"no memory"<<endl;
    delete fin;
  }
  conout<<"Ok"<<endl;
  command.SetStream(&conout);
  conout<<"Default commands...";
  command.vpool.setf("exit",CmdExit);
  command.vpool.setf("help",CmdHelp);
  command.vpool.setf("list",CmdList);
  command.vpool.setf("cls",CmdCls);
  command.vpool.setf("set",CmdSet);
  command.vpool.setf("kill",CmdKill);
  command.vpool.setf("alias",CmdAlias);
  command.vpool.setf("dump",CmdDump);
  command.vpool.link("con_show_speed",(long*)&grconsole.ShowSpeed);
  conout<<"Ok"<<endl;
  conout<<"Load autoexec...";
  ifstream ifs("autoexec.cfg",ios::text | ios::in);
  if(ifs)
  {
    for(;ifs;mscr->refresh())
    {
      ifs.getline(history->cline,history->width);
      command(history->cline);
      history->remember();
      grconsole.tick();
    }
    ifs.close();
    conout<<"Ok"<<endl;
  }
  else
    conout<<"failed"<<endl;
  conout<<']';

  ostream* os = new ostream(new strstreambuf(100));

  istream conin(os->rdbuf());
  int x;

  *os<<"21"<<(char)10<<3;
  conin>>x;
  conout<<x;

  delete os;
//////////////////////////////////////////////////////////////////////////////
  for(int c;;mscr->refresh())
  {
      switch(c=keyb.GetChar())
      {
        case 27: delete fin; break;
        case '`':
        case '~': // open/close console...
                 if(grconsole.visible())
                   grconsole.DoHide();
                 else
                   grconsole.DoShow();
                 break;
        case 0:  break;
        case 72:
                      if(history->up())
                 {
                   grconsole.console()->clearline();
                   conout<<']'<<history->cline;
                 }
                 break;
        case 80:
                      if(history->down())
                 {
                   grconsole.console()->clearline();
                   conout<<']'<<history->cline;
                 }
                 break;
        case 13: //debug("Exec command: "<<history->cline<<endl);
                 conout<<endl;
                      command(history->cline);
                 history->remember();
                 conout<<']';
                 break;
        default: if(history->putc(c))
                   conout<<char(c);
      }
    for(int j=0;j<mscr2->width();j++)
      mscr2->vline(j,0,mscr2->height(),i+j);
    i++;
    mscr2->refresh();
    grconsole.tick();
  }
}

Destruct::~Destruct()
{
  grconsole.close();
  vesa.close();
  exit(0);
}
