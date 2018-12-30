#include <vesa.h>
#include <keyboard.h>
#include <klip.h>

#include <debug/debug.h>

VESAdevice vesa;
Keyboard   kb;

void    main(int argc,char* argv[])
{
  if (argc < 2)
  {
    dout<<"klipname.klp"<<endl;
    return;
  }

  if (vesa.open() != VESAdevice::OK)
  {
    dout<<"Error open VESA"<<endl;
    return;
  }
  if (vesa.SetMode(VESA320x200x16) != VESAdevice::OK)
  {
    dout<<"Error open VESA mode"<<endl;
    return;
  }

  BOOL              pause = FALSE;
  KlipPlayer        player (argv[1],vesa.CurrentScreen());
  MidasSoundShell*  sound = new MidasSoundShell;

  player.InitSound (sound);
  player.GoMusic();

  kb.IgnoreRepeatMap[KEY_SPACE]=TRUE;

  clock_t start = clock();

  while (!kb.IsPress (KEY_ESC) && !player.end())
  {
    if (kb.IsPress (KEY_SPACE))
      pause = !pause;
    if (pause)
      continue;
    player.tick();
  }

  clock_t  end = clock();

  dout<<"Total frames : "<<player.count()<<endl;
  dout<<"Total time   : "<<end-start<<endl;
  dout<<"FPS          : "<<CLK_TCK*(float)player.count()/(float)(end-start)<<endl;

  vesa.close();
}