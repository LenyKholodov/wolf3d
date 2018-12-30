#include "fixed.h"

ulong     mask  = MAKELONG(0xFFFF,127);
long      value = MAKELONG(3,13);

void    main()
{
  union    TextIndex
  {
    long        _fx;
    struct        X
    {
      ulong        reserved : 16;
      ulong        _int     : 7;
    }x;
  };
  TextIndex     x;
  x._fx = value;
  cout<<x.x._int<<endl;
  cout<<((x._fx>>16)&mask)<<endl;
}