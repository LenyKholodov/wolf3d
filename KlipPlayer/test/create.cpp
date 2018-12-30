#include <klip.h>
#include <stdlib.h>
#include <iostream.h>

int     speed = CLK_TCK>>5,frameCount = 0;
char    *dirPak,
        *klipName="KLIP",
        *soundFile = "SOUND.WAV";

void    main(int argc,char* argv[])
{
  if (argc<3)
  {
    cout<<"dirpak.klp framesCount [klipname] [soundFile] [speed]";
    return;
  }

  switch (argc)
  {
    case 6: speed      = atoi(argv[5]); cout<<speed<<" ("<<CLK_TCK/speed<<")"<<endl;
    case 5: soundFile  = argv[4];       cout<<soundFile<<endl;
    case 4: klipName   = argv[3];       cout<<klipName<<endl;
    case 3: frameCount = atoi(argv[2]); cout<<frameCount<<endl;
    case 2: dirPak     = argv[1];       cout<<dirPak<<endl;
  }

  KlipPlayer player (dirPak,klipName,frameCount,soundFile,(float)CLK_TCK/(float)speed);

  player.Save (dirPak);
}
