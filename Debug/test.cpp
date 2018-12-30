#include <debug\vars.h>

Debug           debug;

void    main()
{
  int    x = 7;
  char*  y = "BLA-BLA";
  double z = 12.345;

  SearchVar<int>    v1 ("X",&x);
  SearchVar<char*>  v2 ("Y",&y);
  SearchVar<double> v3 ("Z",&z);

  debug.Input();

  dout<<"STOP!!! This is the TEST!!!"<<endl;
  dout<<*v1<<endl;

}