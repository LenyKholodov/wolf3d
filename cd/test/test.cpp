#include <cd.h>

void main(void)
{
  cdromDevice cdrom;
  if(cdrom.installed())
    cout<<"CD-ROM installed"<<endl
        <<"drives count="<<cdrom.DrivesCount()<<endl
        <<"First drive is "<<cdrom.FirstDrive()<<endl;
  else
    cout<<"CD-ROM's not found"<<endl;
  cout<<"Current drive is "<<cdrom.CurrentDrive()<<endl;
  // running cool player!
  cout<<"select drive: letter key (a-z)"<<endl
      <<"prev[1] next[2] pause/continue[3/4] eject[5] load[6] lock[7] unlock[8]"<<endl;
  int trackno=0;
  for(int key;(key=getch())!=27;)
  {
    if((key>='a' && key<='z') || (key>='A' && key<='Z'))
    {
      cout<<"Change drive to:"<<key<<'\r';
      cdrom.CurrentDrive(key);
      continue;
    }
    switch(key)
    {
      case '1':
                         if(--trackno<cdrom.loaudio())
                  trackno=cdrom.loaudio();
                cout<<"Change track to:"<<trackno<<'\r';
                cdrom.play(trackno);
                break;
      case '2':
                         if(++trackno>cdrom.hiaudio())
                  trackno=cdrom.hiaudio();
                cout<<"Change track to:"<<trackno<<'\r';
                cdrom.play(trackno);
                break;
      case '3': cdrom.pause();
                cout<<"pause             \r";
                break;
      case '4': cdrom.resume();
                cout<<"continue             \r";
                break;
      case '5': cdrom.eject(); break;
      case '6': cdrom.load(); break;
      case '7': cdrom.lock(TRUE); break;
      case '8': cdrom.lock(FALSE); break;
    }
  }
}