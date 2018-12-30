#ifndef __TIMER__
#define __TIMER__

#include <memory\memory.h>
#include <message.h>

#define  TICK_NODES  128

//////////////////////////////////////////////////////////////////////////////
//Всего лишь таймер
//////////////////////////////////////////////////////////////////////////////
class Timer
{
  private:
           int   mPause;
           struct TickNode {
                  Timer*       tc;
                  TickNode*    next;
           };
           TickNode*    node;
           TickNode*    next;
    static Pool*        pool;
  protected:
           void            DeleteAll();
           void            Next();
           TickNode*       Init(Timer* tc,TickNode* next);
  public:
          Timer();
          ~Timer() { DeleteAll(); }

          void       pause() { mPause = !mPause; }
  inline  int        IsPause() { return mPause; }
  virtual void       tick();

          void       AddClient    (Timer* tc);
          void       DeleteClient (Timer* tc);
};

/////////////////////////////////////////////////////////////////////////////
///Класс-оболочка посылающая сообщения в системную очередь
/////////////////////////////////////////////////////////////////////////////
enum TimeRefresh
{
  CLICK_1       = 1,
  CLICK_2       = 2,
  CLICK_4       = 4,
  CLICK_8       = 8,
  CLICK_16      = 16,
  CLICK_SEC     = 18,
  CLICK_32      = 32,
  CLICK_64      = 64,
  CLICK_128     = 128,
  CLICK_256     = 256,
  CLICK_ELSE    = 0
};

class   SystemTimer
{
  public:
          enum  {
                TimerInt        = 0x1C
                };

          SystemTimer();
          ~SystemTimer();

          void  SetRefreshTime(int);
          void  IncTotal() { total++; }
  protected:
     static  void      interrupt   _handler();
     static  void      interrupt   (far* old_timer)();
  private:
     static int     pos;
     static int     max_pos;

     static int     count;
     static int     total;      //for time refresh
};

struct  TickMessage: public Message
{
  int   num;
  int   count;
};

#endif