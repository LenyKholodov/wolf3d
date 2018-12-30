#ifndef __NET_MAP__
#define __NET_MAP__

#include "map.h"

const   int     NET_MAX_BLOCK     = 1024;

struct  NetMapNode
{
  int    index;
  Block  block;
  int    left, right;
};

/////////////////////////////////////////////////////////////////////////////
/////Карта синхронизации графической карты для сети
/////////////////////////////////////////////////////////////////////////////
class   NetMapIndex
{
  public:
                        NetMapIndex  (int size = NET_MAX_BLOCK) ;
        virtual         ~NetMapIndex ();

        void            SetIndex (int,const Block&);
  const Block&          GetIndex (int);
        void            GetBlock (int allocBlock,uint& index,Block&);
        void            Clear    ();

        int             count    () const { return nextAlloc-1; }

  protected:
        int             FindIndex  (int,int);
        int             AllocNode  ();

  protected:
        NetMapNode*     node;
        int             nextAlloc, tableSize;
};

#endif