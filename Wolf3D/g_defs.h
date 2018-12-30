#ifndef __GAME_DEFS__
#define __GAME_DEFS__

#include <binfile.h>

const   int     MAX_RESOURCES     = 1024;
const   int     MAX_RES_NAME_LEN  = 64;

struct  ResourceNode
{
  ulong  crc;
  int    index;
  int    left, right;
};

/////////////////////////////////////////////////////////////////////////////
/////Оболочка для связи файла ресурсов с программой
/////////////////////////////////////////////////////////////////////////////
/////Удалять нельзя
/////////////////////////////////////////////////////////////////////////////
class   ResourceIndex
{
  public:
                        ResourceIndex  (int size = MAX_RESOURCES) ;
                        ResourceIndex  (const char* name);
                        ResourceIndex  (BinFile* file);
        virtual         ~ResourceIndex ();

        void            SetIndex (const char*,int);
        int             GetIndex (const char*);
        void            Clear    ();

        BOOL            Load (const char*);
        void            Save (const char*);
        BOOL            Load (BinFile*);
        void            Save (BinFile*);

  protected:
        int             FindIndex  (int,ulong crc);
        int             AllocNode  ();

  protected:
        ResourceNode*   node;
        int             nextAlloc, tableSize;
};

typedef ResourceIndex ResourceTable;

#endif