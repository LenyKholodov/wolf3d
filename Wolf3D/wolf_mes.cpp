//#define MEM_TEST
#ifdef  MEM_TEST
        #include "mem_test.h"
#endif

#include <iostream.h>
#include <fstream.h>

#include <console/command.h>
#include <console/comdefs.h>

#include <video.h>
#include <keyboard.h>
#include <timer.h>

#include <conio.h>
//#include <tables.h>

#include "fin.h"
#include "cons.h"
#include "render.h"
#include "level.h"

#include "palette.h"

#include <string.h>

//#define NDEBUG
#include <debug\debug.h>

//Debug   debug;

const int      titleCount     = 3;

char*          devNames[7]  =        {"UNT Games Studio",
                                      "Programming: ","Leny & Valik",
                                      "General Designer: ","D. Miresky",
                                      "presents","Wolfenstein 3D: Final"};

Angle     ROTATE_STEP       = float2angle(500.0/12000);
Fixed     WALK_STEP         = float2fixed(500.0/6000.0);
Fixed     STRAFE_STEP       = float2fixed(WALK_STEP/4);
double    FLOOR_D0_STEP     = 0.1;

int       curModePos        = 0;
int       mode              = modes[curModePos];

Fixed           locX        = 0,
                locY        = 0;
Angle           angle       = ANGLE_0;

double          floorD0     = 1.0;

float           xscale      = 1;
float           yscale      = 1;

BOOL            walkForward = FALSE;
BOOL            walkBack    = FALSE;
BOOL            rotateLeft  = FALSE;
BOOL            rotateRight = FALSE;
BOOL            strafeLeft  = FALSE;
BOOL            strafeRight = FALSE;
BOOL            doorOpen    = FALSE;
BOOL            consoleLine = TRUE;

ulong           totalFrames = 0;
ulong           fps         = 0;

ulong           textCount   = 4;


Level*          level;
Map*            map;
LightMap*       lMap;
Render*         draw  = NULL;
Console*        con   = NULL;
Keyboard        kb;
SystemTimer     timer;

VESAdevice      vesa;
vga13hScreen    vga;
tBinFont*       font;
tBGIFont*       bgiFont;
Palette*        palette = NULL;
ostream*        os                = new fstream("RunLogo");
//ostream*        os                  = &cout;

TextCache*      wtf;
SpriteCache*    wsc;
MMT*            floorT,
                *skyT;

Screen*         frame_screen = NULL;

void    CopyrightScreen   (int&);

void    PrintFPS      (uint fps);
void    PrintMode     (Screen* screen);
void    ChangeMode    (int mode,int renderType);
void    OpenRender    (Screen*,int renderType);

void     InitCommand     ();
void     DefaultCommand  ();
void     LoadAutoexec    ();

void    GetFPS        (VarPool&, ParmStack&, ostream&);
void    ChGrMode      (VarPool&, ParmStack&, ostream&);
void    GetGrMode     (VarPool&, ParmStack&, ostream&);
void    SetMapPos     (VarPool&, ParmStack&, ostream&);
void    GetMapPos     (VarPool&, ParmStack&, ostream&);
void    DebugPrint    (VarPool&, ParmStack&, ostream&);
void    SetTHLine     (VarPool&, ParmStack&, ostream&);
void    SetTVLine     (VarPool&, ParmStack&, ostream&);
void    PutDoor       (VarPool&, ParmStack&, ostream&);
void    PutLight      (VarPool&, ParmStack&, ostream&);
void    SaveMap       (VarPool&, ParmStack&, ostream&);
void    SetFocus      (VarPool&, ParmStack&, ostream&);
void    SetViewAng    (VarPool&, ParmStack&, ostream&);
void    GrEngineReset (VarPool&, ParmStack&, ostream&);

class   Destruct: public FinClient
{
  public:
          virtual ~Destruct();
};

void main()
{
 InitCommand();

 dout              = cout;

 dout<<"Wolfenstein 3D: Final"<<endl<<"Copyright by UNT Games Studio 2001"<<endl;

 dout<<"Wolf 3D: Load level "<<endl;

 level = LevelCompile::LoadLevel ("maps/wolf3d/wolf3d.lev");

 font     = level->GetBinFont(0);
 bgiFont  = level->GetBGIFont(0);
 wtf      = level->GetTextCache();
 wsc      = level->GetSpriteCache();
 floorT   = level->GetFloorText();
 skyT     = level->GetSkyText();
 map      = level->GetMap();
 lMap     = level->GetLightMap();

// lMap->SetMap (map);
 textCount = wtf->GetNum();

 dout<<"Init VESA..."<<endl;

 if (vesa.open() != VESAdevice::OK)
 {
   dout<<"Error open VESA mode"<<endl;
   return;
 }

 dout<<"VESA open OK"<<endl;
 dout<<"Init graph mode..."<<endl;

// ChangeMode (mode);
 if (vesa.SetMode(mode) != VESAdevice::OK)
 {
   dout<<"Error open VESA mode "<<hex<<mode<<dec<<endl;
   return;
 }

 dout<<"Init graph mode OK"<<endl;

 con          = new Console(vesa.CurrentScreen(),font);
 con->show ();

 palette = new Palette (vesa.CurrentScreen());

 dout<<"Wolf 3D: Init destruct system..."<<endl;
 fin->AddClient(new Destruct());

 OpenRender (vesa.CurrentScreen(),level->GetRenderType());

 uint xclear=0,yclear=0;

 map->FindClear(xclear,yclear);

 locX = int2fixed(xclear);
 locY = int2fixed(yclear);

// ROTATE_STEP     = float2angle((float)frame_screen->height()/12000);
// WALK_STEP       = float2fixed((float)frame_screen->height()/6000.0);

 const Message*       message;
 timer.SetRefreshTime(CLICK_SEC);
 message_queue = 0xFFFF;

        //Для клнтроля клавы
 kb.IgnoreRepeatMap[KEY_CLEFT]=TRUE;
 kb.IgnoreRepeatMap[KEY_CRIGHT]=TRUE;
 kb.IgnoreRepeatMap[KEY_CUP]=TRUE;
 kb.IgnoreRepeatMap[KEY_CDOWN]=TRUE;
 kb.IgnoreRepeatMap[KEY_A]=TRUE;
 kb.IgnoreRepeatMap[KEY_S]=TRUE;
 kb.IgnoreRepeatMap[KEY_D]=TRUE;
 kb.IgnoreRepeatMap[KEY_W]=TRUE;

        //Для демонстрации дверей
 Block* door_o  = map->GetBlock(0);
 Block* door_c  = map->GetBlock(0);
 BOOL   opening = FALSE,
        closing = FALSE;
 uint   pauseClose = 0;
 BOOL   pauseFlag = FALSE;

        //Copyright
 int    titleCount = 3;
 BOOL   titleState = 1;

// con->open(frame_screen,font);

 dout<<"Wolf 3D: Set default commands..."<<endl;

 DefaultCommand();
 LoadAutoexec  ();

 dout<<"Main Loop: Go"<<endl;

 cout<<"UUUAUAUAYAUAUA"<<endl;

 BOOL   vsync = FALSE;

 while (1)
 {
        //Draw stage
  draw->DrawView(locX,locY,angle);
  PrintFPS(fps);

  if (titleState)   CopyrightScreen(titleState);

  con->tick();

  if (vsync) WaitForVsync();

  frame_screen->refresh();

  timer.IncTotal();

  message = message_queue.GetMessage();

  switch (message->id)
  {
    case TIME_CLICK:
    {
      TickMessage* tick = (TickMessage*)message;
      fps = tick->count;

      if (pauseFlag)
         if (--pauseClose <= 0)
         {
           pauseFlag = FALSE;
           closing   = TRUE;
         }
      if (titleState)
        if (--titleCount == 0)
        {
             titleState++;
             titleCount = 3;
        }
      break;
    }
    case KEY_DOWN:       //key down
    {
      KeyPress* k_message = (KeyPress*)message;

      if (consoleLine)
      {
        if (k_message->scan == KEY_TILDA)
           consoleLine = !consoleLine;
//        con->putc (k_message->scan,kb.ShiftState());
        if (k_message->chr)
           con->putc (k_message->chr,k_message->scan);
      }
      else
      switch(k_message->scan)
      {
        case KEY_CUP:
        case KEY_W:          walkForward++ ;     break;
        case KEY_CDOWN:
        case KEY_S:          walkBack++    ;     break;
        case KEY_CLEFT:  rotateLeft  = TRUE; break;
        case KEY_CRIGHT: rotateRight = TRUE; break;
        case KEY_A:          strafeLeft  = TRUE; break;
        case KEY_D:          strafeRight = TRUE; break;
        case KEY_SPACE:      doorOpen    = TRUE; break;
        case KEY_F7:         titleState++;
                             break;
        case KEY_F8:         vsync       = !vsync; break;
        case KEY_F9:         ChangeMode(0,level->GetRenderType());      break;
        case KEY_F10:        ChangeMode(1,level->GetRenderType());      break;
        case KEY_F11:        ChangeMode(2,level->GetRenderType());      break;
        case KEY_F12:        ChangeMode(3,level->GetRenderType());      break;
        case KEY_ESC:        delete fin;         break;

        case KEY_TILDA:      consoleLine = !consoleLine;
        default:             if (k_message->chr) con->putc(k_message->chr,k_message->scan);
      }
      break;
    }
    case KEY_UP:
    {
      if (!consoleLine)
      {

        KeyPress* k_message = (KeyPress*)message;

        switch(k_message->scan)
        {
          case KEY_CUP:
          case KEY_W:          walkForward--;        break;
          case KEY_CDOWN:
          case KEY_S:          walkBack--;           break;
          case KEY_CLEFT:  rotateLeft  = FALSE;  break;
          case KEY_CRIGHT: rotateRight = FALSE;  break;
          case KEY_A:          strafeLeft  = FALSE;  break;
          case KEY_D:          strafeRight = FALSE;  break;
        }
      }
    }
   }

   if (walkForward)
   {
          Fixed     x,y;
          x=locX+mul(WALK_STEP,cos(angle));
          y=locY+mul(WALK_STEP,sin(angle));
          if (map->IsCross(Index(fixed2int(y),fixed2int(x))))
          {
           locX=x;
           locY=y;
          }
   }
   if (walkBack)
   {
          Fixed     x,y;
          x=locX-mul(WALK_STEP,cos(angle));
          y=locY-mul(WALK_STEP,sin(angle));
          if (map->IsCross(Index(fixed2int(y),fixed2int(x))))
          {
           locX=x;
           locY=y;
          }
   }
   if (rotateLeft)          angle-=ROTATE_STEP;
   if (rotateRight)         angle+=ROTATE_STEP;
   if (strafeLeft)
   {
         Fixed x,y;
         x=locX+mul(WALK_STEP,cos(Angle(angle-ANGLE_90)));      //float!
         y=locY+mul(WALK_STEP,sin(Angle(angle-ANGLE_90)));      //float!
         if (map->IsCross(Index(fixed2int(y),fixed2int(x))))
         {
          locX=x;
          locY=y;
         }
   }
   if (strafeRight)
   {
          Fixed x,y;
          x=locX-mul(WALK_STEP,cos(Angle(angle-ANGLE_90)));
          y=locY-mul(WALK_STEP,sin(Angle(angle-ANGLE_90)));
          if (map->IsCross(Index(fixed2int(y),fixed2int(x))))
          {
           locX=x;
           locY=y;
          }
   }
   if (doorOpen)
   {
          Fixed x,y;
          x=locX+cos(angle);
          y=locY+sin(angle);
          if (map->GetType(Index(fixed2int(y),fixed2int(x))) == Map::DOOR)
          {
            opening  = TRUE;
            door_o   = map->GetBlock(Index(fixed2int(y),fixed2int(x)));
          }
          doorOpen = FALSE;
   }

   if (opening)
   {
     if (door_o->id.door.state++ == 32)
        door_o->cross = 1;
     if (door_o->id.door.state == 127)
     {
        opening = FALSE;
        door_c  = door_o;
        pauseFlag = TRUE;
        pauseClose = 3;
     }
   }
   if (closing)
   {
     if (--door_o->id.door.state == 32)
        door_o->cross = 0;
     if (door_c->id.door.state == 0)
     {

        closing       = FALSE;
        door_c->cross = 0;
     }
   }
  }
  delete fin;
}

Destruct::~Destruct()
{
 dout = *os;
 dout<<"Wolf 3D: Close ray casting..."<<endl;
 delete draw;
 delete con;
 dout<<"Wolf 3D: Close level..."<<endl;
 delete level;
 dout<<"Wolf 3D: Close video mode"<<endl;
 delete palette;
 vesa.close();
 dout<<"Wolf 3D: Close debug stream"<<endl;
 dout = cout;
 delete os;
 dout<<"Wolf 3D: Destruct OK. Exit program"<<endl;

#ifdef  MEM_TEST
        dout<<"Max memory alloc "<<maxSize<<endl;
#endif

 dout<<"Good buy!"<<endl;

 exit(0);
}

void            SendLoadMessage (const char*)
{  }

void            SendLevelName (const char*)
{  }

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
      if (con) con->PutString (buf);
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

  command.vpool.setf("timerefresh",GetFPS);
  command.vpool.setf("set_grmode",ChGrMode);
  command.vpool.setf("get_grmode",GetGrMode);
  command.vpool.setf("set_pos",SetMapPos);
  command.vpool.setf("get_pos",GetMapPos);
  command.vpool.setf("db_print",DebugPrint);
  command.vpool.setf("set_hline",SetTHLine);
  command.vpool.setf("set_vline",SetTVLine);
  command.vpool.setf("put_door",PutDoor);
  command.vpool.setf("put_light",PutLight);
  command.vpool.setf("save_map",SaveMap);
  command.vpool.setf("set_focus",SetFocus);
  command.vpool.setf("set_viewangle",SetViewAng);
  command.vpool.setf("gr_reset",GrEngineReset);

  dout<<"Ok"<<endl;
}

void    PrintFPS (uint fps)
{
      char _fps[100];
      itoa(fps,_fps,10);

      frame_screen->outtext(frame_screen->width()-70,5,"FPS: ",0xFF,font);
      frame_screen->outtext(frame_screen->width()-30,5,_fps,0xFF,font);
}

void    PrintMode (Screen* scr)
{
      char _mode[200];
      char temp[200];
      _mode[0] = 0;

      strcat(_mode,"Display mode: ");
      itoa(scr->width(),temp,10);
      strcat(_mode,temp);
      strcat(_mode,"x");
      itoa(scr->height(),temp,10);
      strcat(_mode,temp);
      strcat(_mode,"x");
      itoa(scr->bpp(),temp,10);
      strcat(_mode,temp);
}

void    GetFPS        (VarPool&, ParmStack&, ostream&)
{
  draw->GetFPS(locX,locY,angle);
}

void    GetMapPos        (VarPool&, ParmStack& parm,ostream& os)
{
  dout<<"View pos: x = "<<fixed2int(locX)<<" y = "<<fixed2int(locY)
      <<" angle = "<<angle2int (angle)<<endl;
}

void    SetMapPos        (VarPool&, ParmStack& parm, ostream& os)
{
  if ((uint)parm[0].value.i>MAP_WIDTH || (uint)parm[1].value.i>MAP_HEIGHT)
  {
    os<<"Wrong position"<<endl;
    return;
  }
  switch(parm.size())
  {
    case 3:
      angle = int2angle(parm[2].value.i);
    case 2:
    {
      locX  = int2fixed(parm[0].value.i);
      locY  = int2fixed(parm[1].value.i);

      uint xclear = fixed2int(locX),
           yclear = fixed2int(locY);

      map->FindClear(xclear,yclear);

      locX = int2fixed(xclear);
      locY = int2fixed(yclear);

      break;
    }
    default:
      os<<"To few parameters"<<endl;
      break;
  }
}

void    DebugPrint    (VarPool&, ParmStack&, ostream& os)
{
  os<<"Debug list"<<endl;
  Debug::PrintFlags();
  Debug::PrintVars();
}

void    ChGrMode      (VarPool&, ParmStack& parm, ostream& os)
{
  if (parm.size()<1)
  {
    os<<"Too few parameters"<<endl;
    return;
  }
  ChangeMode(parm[0].value.i,level->GetRenderType());
//  consoleLine = FALSE;
}

void    SetTHLine      (VarPool&, ParmStack& parm, ostream& os)
{
  if (parm.size()<4)
  {
    os<<"Too few parameters"<<endl;
    return;
  }
  if ((uint)parm[0].value.i > MAX_X_TILE)
  {
    os<<"X index must be less 0f 64"<<endl;
    return;
  }
  if ((uint)parm[1].value.i > MAX_Y_TILE)
  {
    os<<"Y index must be less 0f 64"<<endl;
    return;
  }
  if (parm[0].value.i + parm[2].value.i > MAX_X_TILE)
  {
    os<<"Out of right border: x + width < 64"<<endl;
    return;
  }

  Block* block = NULL;

  int   xstart =  Index(parm[0].value.i,parm[1].value.i);
  int   xend   =  xstart + parm[2].value.i;
  int   txtNum =  level->GetResourceTable()->GetIndex (parm[4].value.zs);

  if    (txtNum == -1)
  {
    os<<"Wrong textures name <"<<parm[4].value.zs<<">"<<endl;
    return;
  }

  int   side;
  char* sides [4] = {"N","E","S","W"};

  for (int j=0;j<4;j++)
      if (!memcmp(sides[j],parm[3].value.zs,strlen(sides[j])))
        break;

  if (j == 4)
  {
    os<<"Forth param must be: NORTH (0), EAST (1), SOURTH (2) or WEST (3)"<<endl;
    return;
  }
  else
    side = j;

  for (int i = xstart;i<xend;i++)
  {
    block       = map->GetBlock(i);

    block->type = Map::WALL;

    switch (side)
    {
      case NORTH:   block->id.wall.id_N = txtNum; break;
      case EAST :   block->id.wall.id_E = txtNum; break;
      case SOURTH:  block->id.wall.id_S = txtNum; break;
      case WEST :   block->id.wall.id_W = txtNum; break;
    }
  }

//  map->Save (mapName);
}

void    SetTVLine      (VarPool&, ParmStack& parm, ostream& os)
{
  if (parm.size()<4)
  {
    os<<"Too few parameters"<<endl;
    return;
  }
  if ((uint)parm[0].value.i > MAX_X_TILE)
  {
    os<<"X index must be less 0f 64"<<endl;
    return;
  }
  if ((uint)parm[1].value.i > MAX_Y_TILE)
  {
    os<<"Y index must be less 0f 64"<<endl;
    return;
  }
  if (parm[1].value.i + parm[2].value.i > MAX_Y_TILE)
  {
    os<<"Out of right border: y + height < 64"<<endl;
    return;
  }

  Block* block = NULL;

  int   ystart =  Index(parm[1].value.i,parm[0].value.i);
  int   yend   =  ystart + parm[2].value.i * MAP_WIDTH;
  int   txtNum =  level->GetResourceTable()->GetIndex (parm[4].value.zs);

  if    (txtNum == -1)
  {
    os<<"Wrong textures name <"<<parm[4].value.zs<<">"<<endl;
    return;
  }

  int   side;
  char* sides [4] = {"N","E","S","W"};

  for (int j=0;j<4;j++)
      if (!memcmp(sides[j],parm[3].value.zs,strlen(sides[j])))
        break;

  if (j == 4)
  {
    os<<"Forth param must be: NORTH (0), EAST (1), SOURTH (2) or WEST (3)"<<endl;
    return;
  }
  else
    side = j;


  for (int i = ystart;i<yend;i += MAP_WIDTH)
  {
    block       = map->GetBlock(i);

    block->type = Map::WALL;

    switch (side)
    {
      case NORTH:   block->id.wall.id_N = txtNum; break;
      case EAST :   block->id.wall.id_E = txtNum; break;
      case SOURTH:  block->id.wall.id_S = txtNum; break;
      case WEST :   block->id.wall.id_W = txtNum; break;
    }
  }

//  map->Save (mapName);
}

void    PutLight      (VarPool&, ParmStack& parm, ostream& os)
{
  if (parm.size()<2)
  {
    os<<"Too few parameters"<<endl;
    return;
  }
  if ((uint)parm[0].value.i>MAP_WIDTH || (uint)parm[1].value.i>MAP_HEIGHT)
  {
    os<<"Wrong position"<<endl;
    return;
  }

  Block* bl = map->GetBlock(Index(parm[1].value.i,parm[0].value.i));

  bl->type  = Map::LIGHT;
  bl->id.id = 0;

  lMap->recalc();
}

void    PutDoor       (VarPool&, ParmStack& parm, ostream& os)
{
  if (parm.size()<2)
  {
    os<<"Too few parameters"<<endl;
    return;
  }
  if ((uint)parm[0].value.i>MAP_WIDTH || (uint)parm[1].value.i>MAP_HEIGHT)
  {
    os<<"Wrong position"<<endl;
    return;
  }

  Block* bl = map->GetBlock(Index(parm[1].value.i,parm[0].value.i));

  bl->type          = Map::DOOR;
  bl->id.door.state = 0;

  if (parm.size()>2) bl->id.door.id = atoi(parm[2].value.zs);
  else               bl->id.door.id = 4;
}

void    SaveMap       (VarPool&, ParmStack& parm, ostream& os )
{
  if (parm.size()<1)
  {
    os<<"Too few parameters"<<endl;
    return;
  }

  char  dest [128];

  strcpy (dest,parm[0].value.zs+1);
  dest[strlen(dest)-1] = 0;

  os<<"Save map: "<<dest<<endl;
  map->Save (dest);

  if(parm.size()>1)
  {
    strcpy (dest,parm[1].value.zs+1);
    dest[strlen(dest)-1] = 0;

    os<<"Save light map: "<<dest<<endl;
    lMap->Save (dest);
  }
}

void    GetGrMode     (VarPool&, ParmStack&, ostream& os)
{
  os<<frame_screen->width()<<"x"<<
      frame_screen->height()<<"x"<<
      frame_screen->bpp()<<endl;
}

void    SetFocus      (VarPool&, ParmStack& parm, ostream& os)
{
  if (parm.size()<1)
  {
    os<<"Too few parameters"<<endl;
    return;
  }
  draw->SetParam(Render::FOCUS,parm[0].value.f);
}

void    SetViewAng      (VarPool&, ParmStack& parm, ostream& os)
{
  if (parm.size()<1)
  {
    os<<"Too few parameters"<<endl;
    return;
  }
  draw->SetParam(Render::VIEW_ANGLE,(float)parm[0].value.i/57.0);
}

void    GrEngineReset (VarPool&, ParmStack&, ostream&)
{
  draw->Reset();

  floorD0 = draw->GetParam(Render::FOCUS);
}

//////////////////////////////Copyright system//////////////////////////////

void    CopyrightScreen(int& state)
{
  if (state>titleCount) state = 0;

  switch (state)
  {
    case 1:
      frame_screen->outtext(50.0*xscale - bgiFont->width(devNames[0])/2,
                            50.0*yscale - bgiFont->height('A')/2,
                            devNames[0],
                            Palette::rgb(255,255,255),
                            bgiFont);
      break;
    case 2:
      frame_screen->outtext(50.0*xscale - bgiFont->width(devNames[5])/2,
                            50.0*yscale - bgiFont->height('A')/2,
                            devNames[5],
                            Palette::rgb(255,255,255),
                            bgiFont);
      break;
    case 3:
      frame_screen->outtext(50.0*xscale - bgiFont->width(devNames[6])/2,
                            50.0*yscale - bgiFont->height('A')/2,
                            devNames[6],
                            Palette::rgb(255,255,255),
                            bgiFont);
      break;
    case 4:
    {
      int x1 = 12.0*xscale,
          y1 = 30.0*yscale,
          x2 = x1 + bgiFont->width(devNames[1]) + xscale * 10.0,
          y2 = y1,
          x3 = x1,
          y3 = y1 + bgiFont->height('A')*2,
          x4 = x2,
          y4 = y3;

      frame_screen->outtext(x1,y1,devNames[1],
                            Palette::rgb(255,255,255),
                            bgiFont);
      frame_screen->outtext(x2,y2,devNames[2],
                            Palette::rgb(255,255,255),
                            bgiFont);
      frame_screen->outtext(x3,y3,devNames[3],
                            Palette::rgb(255,255,255),
                            bgiFont);
      frame_screen->outtext(x4,y4,devNames[4],
                            Palette::rgb(255,255,255),
                            bgiFont);
      break;
    }
  }
}

void    OpenRender    (Screen* screen,int renderType)
{
  if (frame_screen) delete frame_screen;

  frame_screen = GimmeMirror(screen);

  if (frame_screen == NULL)
  {
     dout<<"Wolf 3D: Error open second screen"<<endl;
     return ;
  }

  if (con == NULL)      con  = new Console(screen,font);
  else                  con->open (screen,font);

  if (palette == NULL)  palette = new Palette (screen);
  else                  SetPalette (palette->GetPalette());

  con->show ();

  if (draw == NULL)     draw = new Render (frame_screen,renderType,FALSE);
  else                  draw->Open (frame_screen,renderType,FALSE);

//  SetPalette (palette->GetPalette());

  draw->StagePtr()->SetTexture(wtf);
  draw->StagePtr()->SetMap(map);
  draw->StagePtr()->SetLightMap(lMap);
  draw->StagePtr()->SetCache(wsc);
  draw->FloorPtr()->SetTexture(floorT);
  draw->SkyPtr()->SetTexture(skyT);

  draw->Reset();

  xscale       = frame_screen->width()/100.0;
  yscale       = frame_screen->height()/100.0;

  con->open(frame_screen,font);

  ROTATE_STEP     = float2angle((float)screen->height()/12000);
  WALK_STEP       = float2fixed((float)screen->height()/6000.0);
}

void    ChangeMode(int mode,int renderType)
{
       dout = *os;
       Screen*     screen;

       dout<<"Wolf 3D: Change mode..."<<endl;
       if (vesa.SetMode(modes[mode]) != VESAdevice::OK)
         if (!vga.open())
         {
              vesa.close();
              dout<<"Wolf 3D: Error change video mode. Mode not present"<<endl;
              return;
         }
         else
           screen = &vga;
       else
           screen = vesa.CurrentScreen();

       OpenRender (screen,renderType);

       consoleLine = TRUE;

       dout<<"Wolf 3D: Change mode OK"<<endl;
}




