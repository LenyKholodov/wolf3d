#ifndef __GAME_POOL__
#define __GAME_POOL__

#include "g_object.h"

/////////////////////////////////////////////////////////////////////////////
//////�� ��� ��஢�� ��ꥪ⮢
/////////////////////////////////////////////////////////////////////////////
struct  PoolNode;
class   ErrorObject;
class   GamePool
{
  public:
                  GamePool  (int cl_num,int cl_size = GAME_CLASTER_SIZE);
         virtual  ~GamePool ();

/////////////////////////////////////////////////////////////////////////////
//////�������� ��ꥪ⮢ (� �� 㤠�����)
/////////////////////////////////////////////////////////////////////////////
    PoolIndex     CreateObject ();
    void          DeleteObject (PoolIndex);
    void          ClearAll     ();

/////////////////////////////////////////////////////////////////////////////
//////��㦥���
/////////////////////////////////////////////////////////////////////////////
    BOOL          Belong         (void*) const;
    int           GetClasterSize ()      const  { return mClaster; }
    int           GetPoolSize    ()      const  { return mPoolSize; }
    int           GetClasterNum  ()      const  { return mClasterNum; }

/////////////////////////////////////////////////////////////////////////////
//////����� � ��ꥪ⠬
/////////////////////////////////////////////////////////////////////////////
    GameObject*   GetObject    (PoolIndex);

  protected:
          ErrorObject*         mError;
          uchar*               mPool;     //�� ��ꥪ⮢
          PoolNode*            mPoolPtr;  //�����⥫� �� ��ꥪ��
          PoolIndex            mFirst;    //���� ��ꥪ� � ᯨ᪥
          int                  mClaster,
                               mPoolSize,
                               mClasterNum;
};

//////////////////////////�� ��஢�� ����⮢////////////////////////////////

//extern  GamePool*       gamePool;

#endif