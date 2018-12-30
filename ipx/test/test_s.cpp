#include <debug/debug.h>
#include <string.h>
#include <keyboard.h>

#include <ipx.h>

enum {
     QUESTION = 1234,
     ANSWER
};

struct         Question: public Message
{
        Question (addrnode_t& _addr): addr (_addr) { id = QUESTION ; }

        char         data [10];
        addrnode_t   addr;
};

struct         Answer: public Message
{
        Answer (addrnode_t& _addr): addr (_addr) { id = ANSWER; }

        addrnode_t    addr;
};

socket_t      NET_QUEST    = 0x4567;
socket_t      NET_DATA     = 0x4568;

IPX             ipx;
Keyboard        kb;
addrnode_t      addr;

void    main()
{
  addrnode_t serverAddr;
  ipx.local (serverAddr);

  Question quest (addr);

  memset (addr.node,0xFF,6);

  if (!ipx.OpenSocket (NET_QUEST) || !ipx.OpenSocket (NET_DATA))
     return;

  ipx.Listen (NET_QUEST);

  while (!kb.IsPress(KEY_ESC))
  {
    if (!message_queue.IsEmpty ())
    {
      Message * mes = (Message*)message_queue.GetMessage ();

      if (mes->id == QUESTION)
      {
        Question* quest = (Question*)mes;
        Answer    answer (serverAddr);

        dout<<quest->addr<<endl;

        ipx.write (&answer,sizeof(answer),NET_DATA,quest->addr);
      }
    }
    ipx.Dispatch();
  }

  ipx.CloseSocket (NET_QUEST);
  ipx.CloseSocket (NET_DATA);
}