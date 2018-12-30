#include <iostream.h>

#include <vesa.h>
#include <vga.h>
#include <ascreen.h>

#include <fin.h>

#include <string.h>
#include <stdio.h>

#include "keyboard.h"
#include "timer.h"
#include "render.h"

//#define NDEBUG
#include "debug.h"

Angle     ROTATE_STEP       = float2angle(500.0/12000);
Fixed     WALK_STEP         = float2fixed(500.0/6000.0);
Fixed     STRAFE_STEP       = float2fixed(WALK_STEP/4);
double    FLOOR_HEIGHT_STEP = 10.0;
double    FLOOR_C_STEP      = 0.0001;
double    RC_FOCUS_STEP     = 0.05;
double    RC_VANGLE_STEP    = 0.017;
int       modes[]           = { 0x13, VESA640x480x8,VESA800x600x8,
                                VESA1024x768x8 };

int       curModePos        = 3;
int       mode              = modes[curModePos];

Fixed           locX,
                locY;
Angle           angle       = ANGLE_0;

double          floorHeight = 100.0;
double          floorC      = 0.01;
double          rcFocus     = 0.5;
double          rcVAngle    = 1.0;

Fixed          curX         = int2fixed(128),
               curY         = int2fixed(128);
Angle          curAngle     = ANGLE_270;

ulong          totalFrames = 0;

Map*            map         = new Map("maps/wolf3d.map");
Render*         draw;
Keyboard        kb;
SystemTimer     timer;

VESAdevice      vesa;
vga13hScreen    vga;
tBinFont* font              = tBinFont::load ("font.fon");

TextCache*      wtf;
MMT*            mmt;

Screen*         screen;
Screen*         frame_screen;

void    PrintPosition (Fixed,Fixed);
void    PrintAngle    (Angle);
void    PrintFPS      (uint fps);
void    PrintMode     (Screen* screen);
void    PrintFloor    (double,double);

class   Destruct: public FinClient
{
  public:
          virtual ~Destruct();
};

void main()
{
 if (vesa.open() != VESAdevice::OK)
 {
   dout<<"Error open VESA mode"<<endl;
   return;
 }
 if (vesa.SetMode(mode) != VESAdevice::OK)
 {
   dout<<"Error open VESA mode "<<hex<<mode<<dec<<endl;
   return;
 }

 dout<<"Wolf 3D: Load textures..."<<endl;
 MMT::SetPool(12,TEXT_SIZE);
 wtf = new TextCache("textures/wall.wtf",12);
 mmt = new MMT      ("textures/floor1.mmt");

 dout<<"Wolf 3D: Init destruct system..."<<endl;
 fin->AddClient(new Destruct());

 uint xclear,yclear;

 map->FindClear(xclear,yclear);

 locX = int2fixed(xclear);
 locY = int2fixed(yclear);

 screen       = vesa.CurrentScreen();
 frame_screen = new aScreen(20,20,screen->width()-20,screen->height()-40,screen);

 draw   = new Render(frame_screen);

 draw->StagePtr()->SetMap(map);
 draw->StagePtr()->SetTexture(wtf);
 draw->FloorPtr()->SetTexture(mmt);
 draw->SkyPtr()->SetTexture(mmt);

 draw->FloorPtr()->SetParam(Floor::H,floorHeight);
 draw->FloorPtr()->SetParam(Floor::D0,100);
 draw->FloorPtr()->SetParam(Floor::C,floorC);
 draw->StagePtr()->SetParam(RayCast::WALL_FEEL,12);
 draw->StagePtr()->SetParam(RayCast::VIEW_ANGLE,rcVAngle);
 draw->StagePtr()->SetParam(RayCast::FOCUS,rcFocus);
 draw->Reset();

 ROTATE_STEP     = float2angle((float)screen->height()/12000);
 WALK_STEP       = float2fixed((float)screen->height()/6000.0);

 const Message*       message;
 timer.SetRefreshTime(CLICK_SEC);

 PrintFPS(0);
 PrintAngle(angle);
 PrintPosition(locX,locY);
 PrintMode(screen);
 PrintFloor(floorHeight,floorC);

        //Для демонстрации дверей
 Block* door_o  = &map->GetBlock(0);
 Block* door_c  = &map->GetBlock(0);
 BOOL   opening = FALSE,
        closing = FALSE;
 uint   pauseClose = 0;
 BOOL   pauseFlag = FALSE;

 dout<<"Starting render!!!"<<endl;

 while (!kb.IsPress(KEY_ESC))
 {
//  if (curX!=locX || curY!=locY || curAngle!=angle || closing || opening)
    draw->DrawView(curX=locX,curY=locY,curAngle=angle);
  draw->SwapBuffers();

  timer.IncTotal();

  message = message_queue.GetMessage();

  if (message->id == TIME_CLICK)
  {
    TickMessage* tick = (TickMessage*)message;
    PrintFPS(tick->count);

    if (pauseFlag)
       if (--pauseClose == 0)
       {
         pauseFlag = FALSE;
         closing = TRUE;
         door_c->type = Map::DOOR;
       }
  }

  if (kb.IsPress())
  {
   Fixed     x,y;
   if (kb.IsPress(KEY_W) || kb.IsPress(KEY_UPARROW))
   {
     x=locX+mul(WALK_STEP,cos(angle));
     y=locY+mul(WALK_STEP,sin(angle));
     if (map->GetType(Index(fixed2int(y),fixed2int(x))) == Map::FREE)
     {
      locX=x;
      locY=y;
      PrintPosition(locX,locY);
     }
   }
   if (kb.IsPress(KEY_S) || kb.IsPress(KEY_DOWNARROW))
   {
     x=locX-mul(WALK_STEP,cos(angle));
     y=locY-mul(WALK_STEP,sin(angle));
     if (map->GetType(Index(fixed2int(y),fixed2int(x))) == Map::FREE)
     {
      locX=x;
      locY=y;
      PrintPosition(locX,locY);
     }
   }
   if (kb.IsPress(KEY_A))
   {
     x=locX+mul(WALK_STEP,cos(angle-ANGLE_90));
     y=locY+mul(WALK_STEP,sin(angle-ANGLE_90));
     if (map->GetType(Index(fixed2int(y),fixed2int(x))) == Map::FREE)
     {
      locX=x;
      locY=y;
      PrintPosition(locX,locY);
     }
   }
   if (kb.IsPress(KEY_D))
   {
     x=locX-mul(WALK_STEP,cos(angle-ANGLE_90));
     y=locY-mul(WALK_STEP,sin(angle-ANGLE_90));
     if (map->GetType(Index(fixed2int(y),fixed2int(x))) == Map::FREE)
     {
      locX=x;
      locY=y;
      PrintPosition(locX,locY);
     }
   }

   if (kb.IsPress(KEY_LEFTARROW))
   {
     angle-=ROTATE_STEP;
     PrintAngle(angle);
   }
   if (kb.IsPress(KEY_RIGHTARROW))
   {
     angle+=ROTATE_STEP;
     PrintAngle(angle);
   }

   if (kb.IsPress(KEY_INS))
   {
     draw->FloorPtr()->SetParam(Floor::D0,floorHeight-=FLOOR_HEIGHT_STEP);
     draw->FloorPtr()->Reset();
     PrintFloor(floorHeight,floorC);
   }
   if (kb.IsPress(KEY_DEL))
   {
     draw->FloorPtr()->SetParam(Floor::D0,floorHeight+=FLOOR_HEIGHT_STEP);
     draw->FloorPtr()->Reset();
     PrintFloor(floorHeight,floorC);
   }

   if (kb.IsPress(KEY_O))
   {
     draw->FloorPtr()->SetParam(Floor::C,floorC-=FLOOR_C_STEP);
     draw->FloorPtr()->Reset();
     PrintFloor(floorHeight,floorC);
   }
   if (kb.IsPress(KEY_L))
   {
     draw->FloorPtr()->SetParam(Floor::C,floorC+=FLOOR_C_STEP);
     draw->FloorPtr()->Reset();
     PrintFloor(floorHeight,floorC);
   }

   if (kb.IsPress(KEY_END))
   {
     draw->StagePtr()->SetParam(RayCast::FOCUS,rcFocus-=RC_FOCUS_STEP);
     draw->StagePtr()->Reset();
   }
   if (kb.IsPress(KEY_HOME))
   {
     draw->StagePtr()->SetParam(RayCast::FOCUS,rcFocus+=RC_FOCUS_STEP);
     draw->StagePtr()->Reset();
   }

   if (kb.IsPress(KEY_MINUS))
   {
     draw->StagePtr()->SetParam(RayCast::VIEW_ANGLE,rcVAngle-=RC_VANGLE_STEP);
     draw->StagePtr()->Reset();
   }
   if (kb.IsPress(KEY_PLUS))
   {
     draw->StagePtr()->SetParam(RayCast::VIEW_ANGLE,rcVAngle+=RC_VANGLE_STEP);
     draw->StagePtr()->Reset();
   }

   if (kb.IsPress(KEY_SPACE))
   {
       x=locX+cos(angle);
       y=locY+sin(angle);
       if (map->GetType(Index(fixed2int(y),fixed2int(x))) == Map::DOOR)
       {
          opening  = TRUE;
          door_o   = &map->GetBlock(Index(fixed2int(y),fixed2int(x)));
       }
   }

   if (kb.IsPress(KEY_F9))  curModePos = 0;
   if (kb.IsPress(KEY_F10)) curModePos = 1;
   if (kb.IsPress(KEY_F11)) curModePos = 2;
   if (kb.IsPress(KEY_F12)) curModePos = 3;

   if (kb.IsPress(KEY_F9) || kb.IsPress(KEY_F10) || kb.IsPress(KEY_F11)
                           || kb.IsPress(KEY_F12))
   {
       dout<<"Wolf 3D: Close screen..."<<endl;
       delete frame_screen;

       if (vesa.SetMode(modes[curModePos]) != VESAdevice::OK)
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

       frame_screen = new aScreen(20,20,screen->width()-40,screen->height()-40,screen);

       draw->Open(frame_screen);
       draw->StagePtr()->SetTexture(wtf);
       draw->FloorPtr()->SetTexture(mmt);
       draw->SkyPtr()->SetTexture(mmt);

//       draw->StagePtr()->SetMap(map);

       ROTATE_STEP     = float2angle((float)screen->height()/12000);
       WALK_STEP       = float2fixed((float)screen->height()/6000.0);

       PrintFPS(0);
       PrintAngle(angle);
       PrintPosition(locX,locY);
       PrintMode(screen);
       dout<<"Wolf 3D: Change mode OK"<<endl;
   }

   if (opening)
     if (door_o->id.door.state++ == 127)
     {
        opening = FALSE;
        door_c  = door_o;
        door_o->type = Map::FREE;
        pauseFlag = TRUE;
        pauseClose = 5;
     }
   if (closing)
     if (--door_c->id.door.state == 0)
        closing = FALSE;
  }
 }
 delete fin;
}

Destruct::~Destruct()
{
 dout<<"Wolf 3D: Close ray casting..."<<endl;
 delete draw;
 dout<<"Wolf 3D: Close map..."<<endl;
 delete map;
 dout<<"Wolf 3D: Close screen..."<<endl;
 delete frame_screen;
 dout<<"Wolf 3D: Delete textures..."<<endl;
 delete wtf;
 delete mmt;
 delete MMT::GetPool();
 dout<<"Wolf 3D: Close video mode"<<endl;
 vesa.close();
 free (font);
}

void    PrintPosition(Fixed x,Fixed y)
{
      char      _x[100],
                _y[100];
      itoa(fixed2int(x),_x,10);
      itoa(fixed2int(y),_y,10);

      screen->bar(0,screen->height()-20,100,screen->height(),0);
      screen->outtext(5,screen->height()-10,"X: ",0xFF,font);
      screen->outtext(25,screen->height()-10,_x,0xFF,font);

      screen->outtext(50,screen->height()-10,"Y: ",0xFF,font);
      screen->outtext(70,screen->height()-10,_y,0xFF,font);
}

void    PrintAngle(Angle ang)
{
      char      _ang[100];

      itoa(angle2int(ang),_ang,10);

      screen->bar(100,screen->height()-10,180,screen->height(),0);
      screen->outtext(100,screen->height()-10,"Angle: ",0xFF,font);
      screen->outtext(150,screen->height()-10,_ang,0xFF,font);
}

void    PrintFPS (uint fps)
{
      char _fps[100];
      itoa(fps,_fps,10);
      screen->bar (screen->width()-70,0,screen->width(),20,0);
      screen->outtext(screen->width()-70,5,"FPS: ",0xFF,font);
      screen->outtext(screen->width()-30,5,_fps,0xFF,font);
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

      screen->bar(screen->width()-220,screen->height()-20,
                  screen->width(),screen->height(),0);
      screen->outtext(screen->width()-220,screen->height()-20,_mode,0xFF,font);
}

void    PrintFloor(double d0,double c)
{
      char      _c[100],
                _d0[100];
      sprintf(_d0,"%3.1f",d0);
      sprintf(_c,"%3.4f",c);
//      itoa(fixed2int(x),_x,10);
//      itoa(fixed2int(y),_y,10);

      screen->bar(180,screen->height()-10,350,screen->height(),0);
      screen->outtext(180,screen->height()-10,"D0: ",0xFF,font);
      screen->outtext(205,screen->height()-10,_d0,0xFF,font);

      screen->outtext(250,screen->height()-10,"C: ",0xFF,font);
      screen->outtext(270,screen->height()-10,_c,0xFF,font);
}
