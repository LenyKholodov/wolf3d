#include "g_netmap.h"

#include <debug/debug.h>

int             NetMapIndex::FindIndex (int i,int index)
{
  if        (node [i].index == index)
            return i;
  else if   (node [i].left  != -1 && index<node[i].index)
            return FindIndex (node[i].left,index);
  else if   (node [i].right != -1 && index>node[i].index)
            return FindIndex (node[i].right,index);
  else
            return i;
}

void            NetMapIndex::SetIndex (int index,const Block& block)
{
  int     i   = FindIndex (0,index);

  if (node[i].index == index)
    node[i].block = block;
  else
  {
    if (index<node[i].index)
    {
      if ((node[i].left = AllocNode ()) == -1)
         return;

      i = node[i].left;
    }
    else if (index>node[i].index)
    {
      if ((node[i].right = AllocNode ()) == -1)
         return;

      i = node[i].right;
    }

    node[i].left  = -1;
    node[i].right = -1;
    node[i].index = index;
    node[i].block = block;
  }
}

const Block&    NetMapIndex::GetIndex (int index)
{
  int     i   = FindIndex (0,index);

  if (node[i].index == index)
     return node[i].block;
  else
  {
    dout<<"No block "<<index<<endl;
    return  node [0].block; //??
  }
}

void            NetMapIndex::GetBlock (int i,uint& index,Block& block)
{
  if (++i >= nextAlloc)
     return;

  block = node[i].block;
  index = node[i].index;
}

void            NetMapIndex::Clear    ()
{
  node [0].left  = -1;
  node [0].right = -1;
  node [0].index = -1;

  nextAlloc      = 1;
}

int             NetMapIndex::AllocNode ()
{
  return nextAlloc++;
}

NetMapIndex::NetMapIndex  (int size)
              :node (new NetMapNode [size]),
               tableSize (size)
{
  Clear ();
}

NetMapIndex::~NetMapIndex ()
{
  if (node) delete node;
}

