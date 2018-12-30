#include <types.h>

#include "map.h"

struct  StackBlock
{
  Map::Block*        object;         //�����⥫� �� ��ꥪ�
  int                x,y;            //��� ���न����
};

class   ObjectStack
{
  private:
           StackBlock           **up,    //Stack
                                **down;
  public:
          ObjectStack(int max = 0xFF);
          ~ObjectStack();

          void          Push(StackBlock* object);
          StackBlock*   Pop ();
  inline  BOOL          IsEmpty()  { return up == down; }
  inline  void          ClearAll() { up = down; }
  inline  int           GetCount() { return (int)(up-down); }
};