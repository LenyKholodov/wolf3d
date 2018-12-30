#include "fixed.h"
#incldue "map.h"

struct  ObjectPos
{
  friend class ObjectLIst;
  public:
        Fixed           distance;
        Block*          block;
  protected:
        ObjectPos*      next;
};

/////////////////////////////////////////////////////////////////////////////
/////Список объектов (спрайты,монстры) в упорядоченном состоянии
/////Нормально работает только в сочетании с Ray Casting
/////////////////////////////////////////////////////////////////////////////
class   ObjectList
{
  public:
                        ObjectList(int num = 512);
                        ObjectList(int,void*);
                        ~ObjectList();

     void               AddObject(Fixed,Block*);
     ObjectPos*         GetNext();
                
  protected:
             Block*             lastBlock;
             ObjectPos*         firstPos,
                                lastPos;

             ObjectPos*         pool;
};