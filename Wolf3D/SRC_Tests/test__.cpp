#include "g_pool.h"
#include "g_ttable.h"
#include "g_ring.h"
#include "g_mq.h"
#include "g_load.h"
#include "g_system.h"

#include <debug.h>
#include <time.h>
#include <string.h>

class TestObject: public GameObject
{
  public:
          uchar x[1000];

                TestObject (ArgPtr = NULL) {}
                TestObject (BinFile*) {}

    virtual     void            Load (BinFile*) { dout <<"TEST LOAD"<<endl;}
    virtual     void            Save (BinFile*) { dout <<"TEST SAVE"<<endl; }
    virtual     BOOL            Exec (GameMessage*) { return TRUE; }
};

//////Регистрация нового класса игровых объектов

make<TestObject,TEST_OBJECT> makeTestObject;

struct TestMessage: public GameMessage
{
  uchar test;
};

void    TestCPS       (ostream&,int);
void    Test          (ostream&);
void    TestMQ        (ostream&,int);
void    TestRing      (ostream&,GameObjects);
void    TestTypeTable (ostream&);
void    TestPool      (ostream&);

GameSystem*    gameSystem;

void    main()
{
  gameSystem = new GameSystem (NULL,MAX_SLOTS*10,0xFFF);

  TestTypeTable(dout);
  TestCPS  (dout,10000000);
  Test     (dout);
  TestRing (dout,TEST_OBJECT);
  TestMQ   (dout,40);
  TestPool (dout);

  dout<<"Type table size: "<<sizeof(*gameTypeTable)<<endl;

  delete gameSystem;
}

void    TestCPS (ostream& os,int total)
{
  os<<"Create & delete object test"<<endl;

  TestObject* obj;

  clock_t start = clock();

  for (int i=0;i<total;i++)
  {
    obj  = new(TEST_OBJECT) TestObject;
    delete obj;
  }

  clock_t end   = clock();

  clock_t delta = end - start;

  os<<"Total:         "<<total<<endl;
  os<<"Time:          "<<(float)delta/CLK_TCK<<endl;
  os<<"In one second: "<<(float)total/((float)delta/CLK_TCK)<<endl;
}

void    Test(ostream & os)
{
  for (int i=0;i<102;i++)
  {
    TestObject* x = new(TEST_OBJECT) TestObject;
    if (x->id().id<0 || x->id().id>MAX_SLOTS)
    {
      os<<"Error! id ="<<x->id().id<<endl;
      break;
    }
    delete x;
  }

  TestObject* obj  = new(TEST_OBJECT) TestObject;
  TestObject* obj1 = new(TEST_OBJECT) TestObject;
  TestObject* obj2 = new(TEST_OBJECT) TestObject;

  memset (obj->x,0,1000);

  os<<"YO-HO-HO: "<<(int)obj->x[678]<<endl;

  obj->dump  (os);
  obj1->dump (os);

  delete obj;

  obj = new(TEST_OBJECT) TestObject;

  obj->dump  (os);
  obj1->dump (os);
  obj2->dump (os);

  delete obj;
  delete obj2;
  delete obj1;
}

void    TestMQ (ostream & os,int total)
{
  os<<"Test Main Game Queue"<<endl;

  TestMessage   test;
  TestMessage*  x;

  test.id    = 456;
  test.test  = 123;

  ID    id;

  id.type = -1;
  id.id   = -1;

  for (int i = 0;i<total;i++)
  {
      for (int j=0;j<40;j++)
          gameMessageQueue->PutMessage (&test,id,sizeof(test));

      while (!gameMessageQueue->IsEmpty())
      {
          x = (TestMessage*)gameMessageQueue->GetMessage ();

          if (x->id != 456)
          {
            os<<"Error! "<<x->id<<endl;
            return;
          }
      }
  }
}

void    TestRing (ostream& os,GameObjects type)
{
  os<<"Ring test"<<endl;

  for (int i=0;i<MAX_SLOTS*2;i++)
      TestObject* x = new (TEST_OBJECT) TestObject;

  Ring*  ring  = gameTypeTable->GetRing (TEST_OBJECT);
  Ring*  ring1 = gameTypeTable->GetRing (TEST_OBJECT);

  if (ring != ring1)
     os << "Ha-Ha-Ha!"<<endl;

  GameMessage mes;

  os<<ring->Exec (&mes)<<endl;
//  ring->dump(os);

  ring1->Del (ID (0,1));

  dout<<"OK"<<endl;

//  ring1->dump(os);
}

void    TestTypeTable (ostream& os)
{
  TestObject* array [MAX_SLOTS*2];

  dout<<"1)"<<MAX_SLOTS*1.5<<endl;
  for (int i=0;i<MAX_SLOTS*1.5;i++)
  {
      array[i] = new (TEST_OBJECT) TestObject;
      array[i]->dump(dout);
  }
  dout<<"2) "<<MAX_SLOTS/2<<endl;
  for (i=0;i<MAX_SLOTS/2;i++)
      delete array [i];
  dout<<"3)"<<MAX_SLOTS<<endl;
  for (i=0;i<MAX_SLOTS;i++)
  {
      array[i] = new (TEST_OBJECT) TestObject;
      array[i]->dump(dout);
  }
}

void    TestPool      (ostream& os)
{
  GameObject* test = gameSystem->GetObject(ID(17,4));

  test->dump(os);
}

