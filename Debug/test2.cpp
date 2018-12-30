#include <debug.h>
#include <types.h>

Debug   debug;

void    main()
{
  EFLAGS        eflags;

  dout<<eflags<<endl;

  eflags.ZF = TRUE;
  eflags.set();
  eflags.load();

  dout<<eflags<<endl;
}