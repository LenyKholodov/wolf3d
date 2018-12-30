#include "cache.h"

#include <iostream.h>
#include <sbinfile.h>

//#define NDEBUG
#define FAST_ACSESS_CACHE
#include <debug\debug.h>

/////////////////////implementation of class Cache///////////////////////////

void*  Cache::GetData(long id)
{
#ifndef NDEBUG
  if (id>=id_size)                      //Проверка на границы
     return NULL;
#endif

  CacheList*   block = fast[id];        //Доступ к таблице быстрого доступа

  if (block->id == id)                  //Если в кластере наши данные
  {
     ChangeReq(block);                  //Изменение приоритета
     return block->data;
  }

  fast[id] = block = last->prev;        //Для быстрого доступа

  block->id = id;                       //Устанавливаем свой идентификатор

  ChangeReq(block);                     //Изменение приоритета
  LoadData(id,block);                   //Загружаем данные (virtual)

  return block->data;
}

void    Cache::ChangeReq(CacheList* block)
{
#ifndef  FAST_ACSESS_CACHE
    if (block == first->next)
       return;
#endif

    block->prev->next = block->next;    //Удаляем элемент из списка
    block->next->prev = block->prev;

    block->prev       = first;          //И добавляем в начало списка
    block->next       = first->next;
    first->next       = first->next->prev = block;
}

CacheList* Cache::Alloc(CacheList* prev,CacheList* next)
{
  memblock_t* tmp = queue_pool->Alloc();

  if (tmp == NULL)
     return NULL;

  CacheList* el = (CacheList*)tmp->data;
  el->id        = -1;   //No data
  el->prev      = prev;
  el->next      = next;

  return el;
}

void    Cache::Close  ()
{
  if (queue_pool) delete queue_pool;
  if (fast)       delete fast;

  queue_pool = NULL;
  fast       = NULL;

  cache_size = 0;
  cl_size    = 0;
  id_size    = 0;
}

void    Cache::Open   (long id_num,long cache_num,long cl_sz)
{
  Cache::Close ();

  cache_size = cache_num;
  id_size    = id_num;
  cl_size    = cl_sz;

  queue_pool = new Pool(cache_size+2,sizeof(CacheList)+cl_size); //size+last+first
  fast       = new CacheList* [id_num];

  if (!queue_pool || !fast)
     D_Error("Cache: No free memory fo system pools");

  first      = Alloc();
  last       = Alloc();
  last->next = first->prev = NULL;      //Begin and end of list
  if (first == NULL || last == NULL)
     D_Error ("Cache: No free memory for system queue");

  for (CacheList* iter = first;TRUE;iter=iter->next)
      if ((iter->next=Alloc(iter))==NULL)
         break;

  iter->next = last;
  last->prev = iter;

  if (last->prev == first || first->next==last)
     D_Error("Cache: NULL system queue");

  for (long i=0;i<id_num;i++)
      fast[i] = first->next;
}

Cache::Cache ()
      :queue_pool (NULL), fast (NULL),
       cache_size (0), cl_size (0), id_size (0),
       first (NULL), last (NULL)
{ }

Cache::Cache(long id_num,long cache_num,long cl_sz)
      :queue_pool (NULL), fast (NULL)
{
  Open (id_num,cache_num,cl_sz);
}

Cache::~Cache()
{
  Close ();
}

////////////////////implementation of class NameCache////////////////////////

char*   NameCache::GetName(int i) const { return pool + files[i]; }

NameCache::NameCache(char* names[],int names_num,long id_num,long cache_sz,long cl_sz)
          :pool(NULL), Cache(id_num,cache_sz,cl_sz), namesNum (names_num)
{
  long sz  = sizeof(ulong) * (names_num + 1);
  long pos = sz;

  for (int i=0;i<names_num;sz+=strlen(names[i])+1,i++); //Плюс нуль-символ

  pool  = new char [pool_size = sz];
  files = (ulong*)pool;

  for (i=0;i<names_num;pos+=strlen(names[i])+1,i++)
  {
      files[i] = pos;
      strcpy (pool+pos,names[i]);
  }
}

NameCache::NameCache(BinFile* file,long cache_sz)
          : pool (NULL)
{
  Load (file,cache_sz);
}

NameCache::NameCache(const char* name,long cache_sz)
          : pool (NULL)
{
  Load (name,cache_sz);
}

void    NameCache::Close()
{
  Cache::Close();

  if (pool)   delete pool;

  pool = NULL;
}

BOOL    NameCache::Load(BinFile* file,int cache_sz)
{
  NameCache::Close();

  NameCacheHeader header;

  file->reset ();
  file->read  (&header,sizeof(NameCacheHeader));

  if (memcmp(&header.id,"NCCH",4))
  {
     dout<<"Name cache: Wrong file format"<<endl;
     return FALSE;
  }

  Cache::Open (header.num,cache_sz,header.cl_size);

  pool         = new char [pool_size = header.names_size];
  files        = (ulong*)pool;

  file->read (pool,pool_size);

  namesNum = header.names_num;

  return TRUE;
}

BOOL      NameCache::Load(const char* path,int cache_sz)
{
  stdBinFile f;

  if (!f.open(path,BinFile::OPEN_RO))
  {
     dout<<"Name cache: File not found '"<<path<<"'"<<endl;
     return FALSE;
  }

  BOOL status = Load(&f,cache_sz);

  f.close();

  return status;
}

void    NameCache::Save (BinFile* file)
{
  NameCacheHeader  header;

  memcpy(&header.id,"NCCH",4);

  header.num        = GetNum();
  header.names_size = pool_size;
  header.cl_size    = GetClasterSize();
  header.names_num  = GetNamesNum ();

  file->reset ();
  file->write (&header,sizeof(NameCacheHeader));
  file->write (pool,pool_size);
}

void NameCache::Save(const char* path)
{
  stdBinFile f;
  f.open(path,BinFile::OPEN_CR);
  Save(&f);
  f.close();
}
