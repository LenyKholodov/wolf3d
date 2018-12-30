#include <iostream.h>
#include <dirpak.h>

void    main(int argc,char* argv[])
{
  if (argc<2)
  {
    cout<<"file.pak"<<endl;
    return;
  }

  DirPak mDirPak;

  if (!mDirPak.openDP (argv[1]))
     return ;

  const tFileListEntry*       list = mDirPak.GetDir();

  for (int i=0;i<mDirPak.count();i++,list++)
      cout<<list->name<<"\t";
}
