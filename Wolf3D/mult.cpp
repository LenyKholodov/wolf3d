#include "g_multi.h"
#include "g_mac.h"
#include "fail.h"

#include <keyboard.h>
#include <mouse.h>

void            Done        ();

void            NetStart        ();
void            NetClose        ();
void            InitGraph       ();
void            InitHardware    ();

Debug           debug;
Keyboard*       kb;
Mouse*          ms;
IPX*            ipx;
cdromDevice*    cd;
VESAdevice      vesa;
vga13hScreen    vga;
MultiGame*      game     = NULL;
Screen*         screen   = NULL;
fstream         debugout ("runlog",ios::out | ios::truncate);
BOOL		firstVGA = FALSE;

void    main ()
{
  dout = debugout;

  fin->AddClient(new Destruct());

  InitHardware();
  InitCommand ();
  DefaultCommand ();
  LoadAutoexec();
  InitGraph   ();
  NetStart    ();

  game = new MultiGame (&vesa,screen);

  if (!game)
     return;

  game->MainLoop ();

  Done ();
}

void    NetStart ()
{
  dout << "Net start"<<endl;

  if (!ipx->OpenSocket (CLIENT_READ) || !ipx->OpenSocket (CLIENT_WRITE))
  {
    dout<<"Error open socket "<<endl;
    return;
  }

  for (int i=0;i<MAX_READ_PACKETS-4;i++)
    ipx->Listen (CLIENT_READ);
}

void    NetClose ()
{
  ipx->CloseSocket (CLIENT_READ);
  ipx->CloseSocket (CLIENT_WRITE);
}

void            SendLoadMessage (const char* message)
{
  if (game) game->SendLoadMessage (message);
}

void            SendLevelName (const char* message)
{
  if (game) game->SendLevelName (message);
}

void            InitGraph   ()
{
 cout<<"Init VESA..."<<endl;

 if (vesa.open() != VESAdevice::OK)
 {
   cout<<"Error open VESA mode. Open VGA mode"<<endl;

   if (!vga.open())
   {
     cout<<"Error open VGA mode"<<endl;
     delete fin;
   }
   else
     screen = &vga;
 }
 else
 {
   cout<<"Init graph mode..."<<endl;

   if (vesa.SetMode(VESA640x480x8) != VESAdevice::OK)
   {
     cout<<"Error open VESA mode "<<endl;
     return;
   }

   screen = vesa.CurrentScreen();
 }

 dout<<"Init graph mode OK"<<endl;
}

void            InitHardware()
{
  cout<<"Init hardware"<<endl;
  cout<<"Init keyboard..."<<endl;

  message_queue.Recreate (INPUT_QUEUE_SIZE,MAX_MESSAGES);

  kb = new Keyboard;

  if (IsBad (kb))
  {
    cout<<"Wrong keyboard"<<endl;
    delete fin;
  }

  cout<<"Keyboard present"<<endl;

  for (int i=0;i<0x80;i++)
      kb->IgnoreRepeatMap[i]=TRUE;

  cout<<"Init mouse..."<<endl;

  if (!Mouse::IsMouse ())
    cout<<"Mouse not present"<<endl;
  else
    cout<<"Mouse present"<<endl;

  ms = new Mouse;

  if (ms)
  {
    ms->SetSpeed(32,32);
    ms->SetDoubleSpeed(8);
  }

  cout<<"Init ipx->.."<<endl;

  ipx = new IPX  ;

  cout<<"Init CD-ROM..."<<endl;

  cd = new cdromDevice;

  if(cd->installed())
    cout<<"CD-ROM installed"<<endl
        <<"drives count="<<cd->DrivesCount()<<endl
        <<"First drive is "<<cd->FirstDrive()<<endl;
  else
    cout<<"CD-ROM's not found"<<endl;

  cout<<"Current drive is "<<cd->CurrentDrive()<<endl;
}

void            Done        ()
{
 dout<<"Destruct game system"<<endl;

 NetClose();

 delete game;
 delete kb;
 delete ms;
 delete cd;
 delete ipx;

 game = NULL;
 cd   = NULL;
 kb   = NULL;
 ms   = NULL;
 ipx  = NULL;

 vesa.close();
 vga.close();

 dout<<"Good buy"<<endl;
}

void    SendIPX (const addrnode_t& addr,socket_t socket,NetPacket* packet,size_t size)
{
  do {
    ipx->tick();
  }while (!ipx->write (packet,size,socket,addr));
}

void    GetLocal (addrnode_t& addr)
{
  ipx->local (addr);
}

void    NetTick ()
{
  ipx->RelinquishControl();
  ipx->tick ();
}

void   Send            (GameMessage* mes,int size,GameObjects type)
{
  if (game == NULL)
     return;

  ID id;

  switch (type)
  {
    case SINGLE_GAME: id = game->id(); break;
    case GAME_PLAYER: id = game->mainplayer(); break;
    default:          id = ID(-1,-1); break;
  }

  game->system()->PutMessage (mes,id,size);
}

void            Send            (GameMessage* mes,int size,ID id)
{
  if (game == NULL)
     return;

  game->system()->PutMessage (mes,id,size);
}

cdromDevice*    GetCDROM    ()
{
  return cd;
}

void     ChangeScreen    (int mode)
{
  dout = debugout;

  Screen*     screen;

  dout<<"Change mode..."<<endl;
  if (vesa.SetMode(modes[mode]) != VESAdevice::OK)
    if (!vga.open())
    {
         vesa.close();
         dout<<"Error change video mode. Mode not present"<<endl;
         return;
    }
    else
      screen = &vga;
  else
      screen = vesa.CurrentScreen();

  ViewChangeScreenMessage message (screen);

  Send (&message,sizeof(message),SINGLE_GAME);

  dout<<"Change mode OK"<<endl;
}
