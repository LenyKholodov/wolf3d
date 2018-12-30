#include <malloc.h>     //for the NULL
#include <dos.h>
#include <message.h>

#include "timer.h"

#include "debug.h"

/////////////////////////////////////////////////////////////////////////////
////Данный класс позволяет реализовать отладку в реальном режиме времени
/////////////////////////////////////////////////////////////////////////////
class   DebugTimer:public Timer
{
  protected:
             void       AddClient(Timer* tc) { }
             void       DeleteClient(Timer* tc) { }
     virtual void       tick()  { }
             void       pause() { }
             void       Next()  { }
             TickNode*  Init(Timer* tc,TickNode* next) { return NULL; }
             void       DeleteAll() { }
  public:
          DebugTimer()  { }
};

Pool*           Timer::pool = new Pool(TICK_NODES,sizeof(Timer::TickNode));
DebugTimer      dTimer;

Timer::TickNode* Timer::Init(Timer* tc,TickNode* next)
{
  memblock_t* tmp = pool->Alloc();
  TickNode* el = (TickNode*)(tmp->data);
  el->tc = tc;
  el->next = next;
  return el;
}

void   Timer::Next() {   next = next->next; }

void   Timer::tick()
{
  if (IsPause())
     return;
  next->tc->tick();
  Next();
}

void   Timer::AddClient(Timer* tc) {  next->next = Init(tc,next->next); }

void   Timer::DeleteClient(Timer* tc)
{
  for (TickNode* tmp = next->next;tmp!=next;tmp=tmp->next)
      if (tmp->next->tc == tc)
      {
        TickNode* del = tmp->next;
        tmp->next = tmp->next->next;
        delete del;
      }
}

void   Timer::DeleteAll()
{
  for (TickNode* tmp = next->next;tmp!=next;tmp=tmp->next)
      if (tmp->next->tc != &dTimer)
      {
        TickNode* del = tmp->next;
        tmp->next = tmp->next->next;
        delete del;
      }
}

Timer::Timer():mPause(FALSE)
{
  next = node = Init(&dTimer,NULL);
  node->next  = node;               //circle queue
}

int                     SystemTimer::count                = 0;
int                     SystemTimer::pos                  = 0;
int                     SystemTimer::max_pos              = CLICK_SEC;
int                     SystemTimer::total                = 0;
void interrupt          (far* SystemTimer::old_timer)()   = NULL;
static  TickMessage     message;

void interrupt SystemTimer::_handler()
{
  if (pos++ == max_pos)
  {
    message.id    = TIME_CLICK;
    message.num   = pos-1;
    message.count = total;
    pos           = 0;
    total         = 0;

    message_queue.PutMessage(&message,sizeof(TickMessage));
  }
  (*old_timer)();
}

void    SystemTimer::SetRefreshTime(int ticks)
{
  max_pos = ticks;
}

SystemTimer::SystemTimer()
{
  if (count++==0)
  {
        //Save old handler
    old_timer = _dos_getvect(TimerInt);

        //and set new handler
    _dos_setvect(TimerInt,_handler);
  }

  pos     = 0;
  max_pos = CLICK_SEC;
}

SystemTimer::~SystemTimer()
{
  if (--count==0)
    _dos_setvect(TimerInt,old_timer);
}