#include "g_defs.h"

#include <string.h>
#include <sbinfile.h>
#include <misc.h>

#include <debug/debug.h>

BOOL            ResourceIndex::Load (const char* name)
{
  stdBinFile file;

  if (!file.open(name,BinFile::OPEN_RO))
  {
    dout<<"Resource table: File not found "<<name<<endl;
    return FALSE;
  }

  BOOL  state = Load(&file);

  file.close();

  return state;
}

void            ResourceIndex::Save (const char* name)
{
  stdBinFile file;

  if (!file.open(name,BinFile::OPEN_CR))
  {
    dout<<"Resource table: Can't create file "<<name<<endl;
    return;
  }

  Save (&file);

  file.close();
}

void            ResourceIndex::Save (BinFile* file)
{
  file->write (&tableSize,sizeof(tableSize));
  file->write (&nextAlloc,sizeof(nextAlloc));
  file->write (node,tableSize*sizeof(ResourceNode));
}

BOOL            ResourceIndex::Load (BinFile* file)
{
  int resSize ;

  file->read (&resSize,sizeof(resSize));

  if (node) delete node;

  node = new ResourceNode [tableSize = resSize];

  if (node == NULL)
  {
    dout<<"Resource table: Wrong file format"<<endl;
    return FALSE;
  }

  file->read (&nextAlloc,sizeof(nextAlloc));
  file->read (node,tableSize*sizeof(ResourceNode));

  return TRUE;
}

int             ResourceIndex::FindIndex (int i,ulong crc)
{
  if        (node [i].crc == crc)
            return i;
  else if   (node [i].left  != -1 && crc<node[i].crc)
            return FindIndex (node[i].left,crc);
  else if   (node [i].right != -1 && crc>node[i].crc)
            return FindIndex (node[i].right,crc);
  else
            return i;
}

void            ResourceIndex::SetIndex (const char* name,int index)
{
  ulong   crc = CRC32(name,strlen(name));
  int     i   = FindIndex (0,crc);

  if (node[i].crc == crc)
    node[i].index = index;
  else
  {
    if (crc<node[i].crc)
    {
      if ((node[i].left = AllocNode ()) == -1)
         return;

      i = node[i].left;
    }
    else if (crc>node[i].crc)
    {
      if ((node[i].right = AllocNode ()) == -1)
         return;

      i = node[i].right;
    }

    node[i].left  = -1;
    node[i].right = -1;
    node[i].crc   = crc;
    node[i].index = index;
  }
}

int             ResourceIndex::GetIndex (const char* name)
{
  ulong   crc = CRC32(name,strlen(name));
  int     i   = FindIndex (0,crc);

  if (node[i].crc == crc)       return node[i].index;
  else
#ifndef NDEBUG
  {
    dout<<"Wrong resource name: '"<<name<<"'"<<endl;
#endif
    return -1;
#ifndef NDEBUG
  }
#endif
}

void            ResourceIndex::Clear    ()
{
  node [0].left  = -1;
  node [0].right = -1;
  node [0].crc   = 0;

  nextAlloc      = 1;
}

int             ResourceIndex::AllocNode ()
{
  return nextAlloc++;
}

ResourceIndex::ResourceIndex  (int size)
              :node (new ResourceNode [size]),
               tableSize (size)
{
  Clear ();
}

ResourceIndex::ResourceIndex  (const char* name)
              :node(NULL)
{
  Load (name);
}

ResourceIndex::ResourceIndex  (BinFile* file)
              :node(NULL)
{
  Load (file);
}

ResourceIndex::~ResourceIndex ()
{
  if (node) delete node;
}

