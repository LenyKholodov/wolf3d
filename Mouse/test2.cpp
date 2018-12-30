#include <mouse.h>

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

static  int     xc, yc;

void   _loadds far     h(int x,int y)
{
  xc = x;
  yc = y;
}

void    main()
{
  MouseMove ms_move(h);
  while (!kbhit())
  {
    cout<<"X = "<<xc<<" Y = "<<yc<<endl;
  }
}