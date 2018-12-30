#include <iostream.h>
#include <types.h>

ulong    x;
uint16&  y = *(uint16*)((uchar*)&x+2);
extern int f;

void    input() {cin>>x;}
void    output (int d ) { cout<<d; }
signed char   recalc (int _x);
#pragma aux recalc value [BL] = "mov bl,byte ptr eax"

void main()
{
  input();
  int z=recalc(x);
  output(z);
}