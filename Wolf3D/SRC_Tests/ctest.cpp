#include "cache.cpp"
#include "memory.cpp"
#include <iostream.h>

class   DCache:public Cache
{
  public:
          DCache(long id_num,long cache_size): Cache(id_num,cache_size) {}
  protected:
   virtual   void       LoadData(long id,CacheList* block)
       { block->data = new int (id); }
};

void    main()
{
  Cache* cache = new DCache(100L,4L);
  int*   tmp;
  tmp = (int*)(cache->GetData(1));
  cout<<*tmp<<endl;
  tmp = (int*)(cache->GetData(2));
  cout<<*tmp<<endl;
  tmp = (int*)(cache->GetData(3));
  cout<<*tmp<<endl;
  tmp = (int*)(cache->GetData(5));
  cout<<*tmp<<endl;
  tmp = (int*)(cache->GetData(6));
  cout<<*tmp<<endl;
  tmp = (int*)(cache->GetData(1));
  cout<<*tmp<<endl;  
  tmp = (int*)(cache->GetData(7));
  cout<<*tmp<<endl;
  tmp = (int*)(cache->GetData(8));
  cout<<*tmp<<endl;
  delete cache;
}
