#include "level.h"

#include <debug\debug.h>

int     main()
{
  LevelCompile  lComp;

  lComp.SetLevelName   ("  εγ©!");
  lComp.SetMap         ("maps/wolf3d.map");
  lComp.SetLightMap    ("maps/wolf3d.lmf");
  lComp.SetTextCache   ("textures/w3d.wtf");
  lComp.SetSpriteCache ("textures/w3d.wsc");
  lComp.SetBGIFont     (0,"fonts/trip.chr");
  lComp.SetBinFont     (0,"fonts/font.fnt");
  lComp.SetFloorText   ("textures/floor1.mmt");
  lComp.SetSkyText     ("textures/floor1.mmt");

  lComp.Save           ("maps/wolf3d.lev");

  return 0;
}
