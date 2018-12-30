#include <types.h>
#include <iostream.h>

struct X
{
  uint16        type : 3;
  union ID
  {
    struct Door
    {
      uint16    id    : 6;
      uint16    state : 7;
    }door;
    uint16      id    : 13;
  }id;
};

void    main()
{
  X     x;
  x.type = 6;
  x.id.id = 65;

  cout<<x.id.door.state<<endl;
  cout<<x.id.id<<endl;
}