#include  <message.h>
#include <iostream.h>

struct  BlaBla:public Message
{
  int   x,y;
};

void    main()
{
//  MessageHandler        handler (0xFF);

  BlaBla        test;
  test.size     = sizeof(BlaBla);
  test.x        = 1;
  test.y        = 1;

  for (int i=0;i<32000;i++)
    message_queue.PutMessage(&test,sizeof(BlaBla));

  const BlaBla* test1  = (const BlaBla*)message_queue.GetMessage();

  cout<<"X = "<<test1->x<<" Y = "<<test1->y<<endl;
}
