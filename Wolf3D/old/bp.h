#ifndef __ACCESS_CTRL__
#define __ACCESS_CTRL__

#include <debug.h>

class  BP
{
  private:
     int    handler;
  public:
     BP();
     BP(void far* addr,int mode = 2,int op_size = 4);
     BP(const BP& bp);
     operator = (const BP& bp);
     ~BP();

     void       Reset() const;

     operator int();

     friend ostream& operator<<(ostream& os,const BP& bp)
     {
       Debug::GetBPState(bp.handler) ?
          os<<"Access's taken place" :
          os<<"Access has not taken place";
       return os;
     }
};

#endif