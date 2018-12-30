/////////////////TEST

#include <vesa.h>
#include <klip.h>
#include <keyboard.h>

#include <debug/debug.h>

Debug      debug;
VESAdevice vesa;
Keyboard   kb;

void    main()
{
  vesa.open();
  vesa.SetMode (VESA320x200x16);

  KlipPlayer player ("d:/projects/project/klip/lmp/NoPack/w3d.klp",
                     vesa.CurrentScreen());

  dout<<"Count "<<player.count()<<endl;

//  player.SetScreen();
//  player.speed (CLK_TCK/25);
  player.InitSound(new MidasSoundShell);
  player.GoMusic();

  dout<<"HETE"<<endl;

  static int i = 450;

  clock_t start = clock();

  while (!kb.IsPress (KEY_ESC))
        player.tick();

  clock_t end = clock();

//  while (!kb.IsPress(KEY_ESC));

  vesa.close();

  dout<<"Time: "<<(float)(end-start)/CLK_TCK<<endl;
  dout<<"Total: "<<i<<endl;
  dout<<"FPS: "<<(float)i/(float)(end-start)*CLK_TCK<<endl;
}