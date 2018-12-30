#include "level.h"
#include <conio.h>

void    main ()
{
  Level*       level = LevelCompile::LoadLevel ("maps/wolf3d.lev",12,12);

  MidasSoundShell* shell  = level->GetSound()->CreateShell (16);

  shell->play (0,64,0);

  getch();

  delete shell;
  delete level;
}