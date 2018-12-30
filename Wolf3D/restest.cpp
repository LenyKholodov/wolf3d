#include "g_defs.h"

#include <debug\debug.h>

void    main ()
{
  ResourceTable res;

  res.SetIndex ("HELLO",13);
  res.SetIndex ("WORLD",189);
  res.SetIndex ("X",167);
  res.SetIndex ("Y",11);

  res.Save ("x.res");
  res.Load ("x.res");

  dout<<res.GetIndex ("WORLD")<<endl;
}