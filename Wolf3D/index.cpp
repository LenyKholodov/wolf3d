#include <dirpak.h>

void main(int argc,char* argv[])
{
  if(argc<2)
  {
    cout<<"usage: index <dirpak-name>\n";
    return;
  }
  DirPak dpak;
  if(!dpak.openDP(argv[1],DEFAULT_CACHE_SIZE,FALSE))
  {
    cout<<"error: can't open dirpak\n";
    return;
  }
  if(dpak.indexw())
    cout<<"Ok"<<endl;
  else
    cout<<"Failed"<<endl;
  dpak.closeDP();
  cout<<".end"<<endl;
}
