#include <memory\space.h>
#include <debug\debug.h>

void    main()
{
  try
  {
    Pool*      pool  = new SystemSpace  ();
    Pool*      spool = new Space  (pool);

    dout<<spool->size()<<endl;

    while (spool->Alloc(4096));

    dout<<spool->count()<<endl;

    delete spool;
    delete pool;
  }

  catch (Pool::Errors error)
  {
    switch ( error )
    {
      case Pool::NO_MEMORY: cout<<"Exception! No memory"<<endl; break;
      case Pool::WRONG_PTR: cout<<"Exception! Wrong ptr for free or realloc"<endl; exit(-1);
      case Pool::WRONG_SPACE: cout<<"Exception! Space base is NULL"<<endl; exit(-1);
      case Pool::WRONG_POOL: cout<<"Exception! BLA-BLA"<<endl; exit(-1);
    }
  }
}