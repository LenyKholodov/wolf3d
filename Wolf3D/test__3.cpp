#define MEM_TEST
#ifdef  MEM_TEST
        #include "mem_test.h"
#endif

#include <iostream.h>
#include <vesa.h>

#include "fin.h"
#include <keyboard.h>
#include <mouse.h>
#include "palette.h"

#include "g_single.h"
#include "g_player.h"

#include <debug\debug.h>

Debug           debug;
Keyboard*       kb;
Mouse*          ms;
VESAdevice      vesa;
SingleGame*     game;

void            DefaultKeys ();

void main()
{
 dout              = cout;

 dout<<"Wolfenstein 3D: Final"<<endl<<"Copyright by UNT Games Studio 2001"<<endl;

 dout<<"Init keyboard..."<<endl;

 kb = new Keyboard;

 if (IsBad (kb))
 {
   dout<<"Wrong keyboard"<<endl;
   return;
 }

 dout<<"Keyboard present"<<endl;

 kb->IgnoreRepeatMap[KEY_CLEFT]=TRUE;
 kb->IgnoreRepeatMap[KEY_CRIGHT]=TRUE;
 kb->IgnoreRepeatMap[KEY_CUP]=TRUE;
 kb->IgnoreRepeatMap[KEY_CDOWN]=TRUE;
 kb->IgnoreRepeatMap[KEY_A]=TRUE;
 kb->IgnoreRepeatMap[KEY_S]=TRUE;
 kb->IgnoreRepeatMap[KEY_D]=TRUE;
 kb->IgnoreRepeatMap[KEY_W]=TRUE;
 kb->IgnoreRepeatMap[KEY_E]=TRUE;
 kb->IgnoreRepeatMap[KEY_Q]=TRUE;
 kb->IgnoreRepeatMap[KEY_RCTRL]=TRUE;
 kb->IgnoreRepeatMap[KEY_SPACE]=TRUE;

 dout<<"Init mouse..."<<endl;

 if (!Mouse::IsMouse ())
   dout<<"Mouse not present"<<endl;
 else
   dout<<"Mouse present"<<endl;

 ms = new Mouse;

 if (ms)
 {
   ms->SetSpeed(32,32);
   ms->SetDoubleSpeed(8);
 }

 dout<<"Init VESA..."<<endl;

 if (vesa.open() != VESAdevice::OK)
 {
   dout<<"Error open VESA mode"<<endl;
   return;
 }

 dout<<"VESA open OK"<<endl;
 dout<<"Init graph mode..."<<endl;

 if (vesa.SetMode(VESA640x480x8) != VESAdevice::OK)
 {
   dout<<"Error open VESA mode "<<endl;
   return;
 }

 dout<<"Init graph mode OK"<<endl;

 game = new SingleGame (vesa.CurrentScreen(),"maps/wolf3d/wolf3d.lev");

 game->MainLoop ();

 delete game;
 delete kb;
 delete ms;

 dout<<"Good buy"<<endl;

 #ifdef MEM_TEST
        dout<<"Max memory "<<maxSize<<endl;
 #endif
}

void            DefaultKeys ()
{
}

