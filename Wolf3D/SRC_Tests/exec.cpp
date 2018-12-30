#include <process.h>
#include <iostream.h>

void    main(int argc,char* argv[])
{
  cout<<spawnl (0,argv[1],NULL)<<endl;
}