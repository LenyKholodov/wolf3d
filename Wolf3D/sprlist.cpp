#include "sprlist.h"

#include <sbinfile.h>
#include <debug/debug.h>

/*static void* curMem = NULL;

static void* AllocEmul (size_t size) { return curMem; }

tSprite* SpriteList::GetData (int id,int face)
{

}

void   SpriteListCache::LoadData(long id,CacheList* dest)
{
  curMem = dest->data;
  tSpriteList::load (GetName(id),AllocEmul);
}

SpriteListCache::SpriteListCache(char* names[],long id_num,long cache_sz,long cl_sz)
          :NameCache(names,id_num,id_num,cache_sz,cl_sz)
{   }

SpriteListCache::SpriteListCache(BinFile* file,long cache_sz)
          :NameCache(file,cache_sz)
{   }

SpriteListCache::SpriteListCache(const char* name,long cache_sz)
          :NameCache(name,cache_sz)
{   }

*/

////////////////////implementation of class SpriteListCache /////////////////////

void  LoadSpriteList (const char* name,int face,void* dest)
{
        //Open file
        //Read & process Header check for face
        //load offs table
        //jump to sprite
        //load this

  stdBinFile      file;
  ImageListHeader header;

  if (!file.open(name,BinFile::OPEN_RO))
  {
     dout<<"Sprite list: Error open file '"<<name<<"'"<<endl;
     return ;
  }

  file.reset();
  file.read(&header,sizeof(header));

  if(header.Id!=SpriteListId)
  {
    dout<<"Sprite list cache: Wrong file format "<<name<<endl;
    return ;
  }

  if (face >= header.count) face = header.count-1;

  static ulong  listOffs [1024];

  listOffs [header.count] = file.length();     //??

  file.read (listOffs,header.count*sizeof(ulong));
  file.seek (listOffs [face]+sizeof(header));
  file.read (dest,listOffs[face+1]-listOffs[face]);
}

void          SpriteListCache::LoadData(long id,CacheList* dest)
{
  int _id   = index[id];
  int _face = id-idOffs[_id];

  LoadSpriteList (GetName (_id),_face,(void*)dest->data);
}

int    SpriteListCache::Research (const char* _name)
{
  stdBinFile*    file = new stdBinFile (_name,BinFile::OPEN_RO);

  ImageListHeader header;

  file->reset();
  file->read(&header,sizeof(header));

  if(header.Id!=SpriteListId)
  {
    dout<<"Sprite list cache: Wrong file format "<<_name<<endl;
    return 0;
  }

  delete file;

  return header.count;
}

void              SpriteListCache::Recalc ()
{
  if (idOffs)   delete idOffs;
  if (index)    delete index;

  if (GetNamesNum() > 8192 )
  {
    dout<<"Sprite list cache: Too large!"<<endl;
    return;
  }

  idOffs = new int [GetNamesNum ()];

  int   len [8192];     //max posible
  int   pos    = 0;

  for (int i=0;i<GetNamesNum();i++)
  {
      idOffs[i] = pos;
      len[i]    = Research(GetName(i));
      pos      += len [i];
  }

  Cache::Open (pos,GetCacheSize(),GetClasterSize());

  index  = new int [GetNum()];

  for (i=0,pos=0;i<GetNamesNum();pos+=len[i],i++)
      for (int j=0;j<len[i];index[pos+j] = i,j++);
}

void              SpriteListCache::Close       ()
{
  NameCache::Close();

  if (idOffs)   delete idOffs;
  if (index)    delete index;

  idOffs = NULL;
  index  = NULL;
}

tSprite *         SpriteListCache::GetData (int id,int face)
{
  return (tSprite*)Cache::GetData (idOffs[id]+face);
}

SpriteListCache::SpriteListCache (char* name[],long id_num,long cache_sz,long cl_sz)
            : idOffs(NULL), index(NULL),
              NameCache(name,id_num,id_num,cache_sz,cl_sz)
{     }

SpriteListCache::SpriteListCache (BinFile* file,long cache_sz)
            : idOffs(NULL), index(NULL),
              NameCache (file,cache_sz)
{
  Recalc ();
}

SpriteListCache::SpriteListCache (const char* name,long cache_sz)
             : idOffs(NULL), index(NULL),
              NameCache (name,cache_sz)
{
  Recalc ();
}
