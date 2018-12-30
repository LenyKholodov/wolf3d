#include <debug/clock.h>
#include <string.h>


TimePoint::TimePoint (const char* _name)
          : name (new char [strlen(_name)+1])
{
  strcpy (name,_name);

  last = clock();
}

TimePoint::TimePoint (const char* _name,clock_t x)
          : name (new char [strlen(_name)+1])
{
  strcpy (name,_name);

  last = clock();
}

TimePoint::TimePoint (const TimePoint& x)
         : last (x.last), name (new char [strlen(x.name)+1])
{
  strcpy (name,x.name);
}

TimePoint&  TimePoint::operator = (const TimePoint& x)
{
  last = x.last;

  delete name;
  name = new char [strlen(x.name)+1];

  strcpy (name,x.name);

  return *this;
}

clock_t     TimePoint::operator = (const clock_t x)
{
  last = x;

  return x;
}

int TimePoint::refresh ()
{
  clock_t tmp = last;

  last = clock();

  return clock()-tmp;
}

int TimePoint::next    ()
{
  int tmp = refresh();

  dout<<"Time point (tick) '"<<name<<"': "<<last<<endl;

  return tmp;
}

float TimePoint::sec    ()
{
  float tmp = (float)refresh()/CLK_TCK;

  dout<<"Time point (sec) '"<<name<<"': "<<tmp<<endl;

  return tmp;
}
