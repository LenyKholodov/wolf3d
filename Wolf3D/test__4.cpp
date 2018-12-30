//#define MEM_TEST
//#define LOW_MEMORY
#ifdef  MEM_TEST
        #include "mem_test.h"
#endif

#include <iostream.h>
#include <video.h>

#include "fin.h"
#include <keyboard.h>
#include <mouse.h>
#include <cd.h>
#include "palette.h"

#include "cons.h"
#include "g_mac.h"

#include "g_single.h"
#include "g_player.h"

#include "fail.h"

#include <debug\debug.h>

extern 	 	BOOL firstVGA = TRUE;

vga13hScreen    vga;

Debug           debug;
Keyboard*       kb;
Mouse*          ms;
//IPX*            ipx;
cdromDevice*    cd;
VESAdevice      vesa;
SingleGame*     game     = NULL;
Screen*         screen   = NULL;
fstream         debugout ("runlog",ios::out | ios::truncate);

void            InitGraph   ();
void            InitHardware();
void            Done        ();

void*           MemoryLowAlloc   ();
void            MemoryLowFree    (void*);

void            NetStart         ();
void            NetClose         ();

void            Send            (GameMessage*,int size,GameObjects);
void            Send            (GameMessage*,int size,ID id);
void            Send            (GameMessage*,int size);
void            SendLoadMessage (const char*);

void main()
{
 dout = debugout;

 cout<<"Wolfenstein 3D: Final"<<endl<<"Copyright by UNT Games Studio 2001"<<endl;

 void* handler = MemoryLowAlloc();

 fin->AddClient(new Destruct());

 InitHardware();
 InitCommand ();
 DefaultCommand ();
 LoadAutoexec();
 InitGraph   ();
 NetStart    ();

 game = new SingleGame (&vesa,screen);

 if (!game)
 {
   cout<<"Error create single game"<<endl;
   return;
 }

 game->LoadLevel("maps/wolf3d/wolf3d.lev",1,1);
 game->MainLoop ();

 MemoryLowFree(handler);

 Done();
}

void            NetStart         ()
{
//  if (!ipx->OpenSocket (SERVER_READ) || !ipx->OpenSocket (SERVER_WRITE))
//  {
//    dout<<"Error open socket"<<endl;
//    return;
//  }

//  dout<<"NET START OK!"<<endl;

//  for (int i=0;i<MAX_READ_PACKETS-4;i++)
//    ipx->Listen (SERVER_READ);
}

void            NetClose         ()
{
//  ipx->CloseSocket (SERVER_READ);
//  ipx->CloseSocket (SERVER_WRITE);
}

void*    MemoryLowAlloc ()
{
#ifdef LOW_MEMORY
  void* mem = new char[380000000];

  dout<<"Low memory lump "<<mem<<endl;

  return mem;
#endif
  return 0;
}

void    MemoryLowFree (void* handler)
{
#ifdef LOW_MEMORY
       delete handler;
#endif
}

void            InitGraph   ()
{
  if (!firstVGA)
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
  }
  else
  {
    vga.open();
    screen = &vga;
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

//  cout<<"Init IPX..."<<endl;

//  ipx = new IPX  ;

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
// delete ipx;

 game = NULL;
 cd   = NULL;
 kb   = NULL;
 ms   = NULL;
// ipx  = NULL;

 vesa.close();
 vga.close();

 dout<<"Good buy"<<endl;

 #ifdef MEM_TEST
        dout<<"Max memory "<<maxSize<<endl;
 #endif
}

void            SendLoadMessage (const char* message)
{
  if (game) game->SendLoadMessage (message);
}

void            SendLevelName (const char* message)
{
  if (game) game->SendLevelName (message);
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

void            Send            (GameMessage* mes,int size)
{
  if (game == NULL)
     return;

  game->system()->PutMessage (mes,size);
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

void    SendIPX (const addrnode_t& addr,socket_t socket,NetPacket* packet,size_t size)
{
  Flag  flag ("SEND_IPX");

//  do
//  {
//    ipx->tick();
//  }while (!ipx->write (packet,size,socket,addr));
}

void    GetLocal (addrnode_t& addr)
{
//  ipx->local (addr);
}

void    NetTick ()
{
//  ipx->RelinquishControl();
//  ipx->tick ();
}

cdromDevice* GetCDROM    ()
{
  return cd;
}

