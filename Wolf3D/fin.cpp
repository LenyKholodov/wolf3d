#include <process.h>
#include <debug/debug.h>

#include "fin.h"

Finall*         fin = new Finall();

FinClient::~FinClient ()
{
  if (owner)
     owner->DelClient (this);
}

Finall::Finall ():finList(NULL), curPos (NULL), isDelete (FALSE)
{ }

Finall::~Finall ()
{
  if (finList == NULL)
     exit (0);

  isDelete = TRUE;
  FinClient* next = finList;
  for (FinClient* del=finList;del;del=next)
  {
    next = del->next;
    delete del;
  }

  dout<<"Finall: Exit from system"<<endl;

  exit (0);
}

void    Finall::AddClient (FinClient* client)
{
        //настройки списка
  if (finList == NULL)
    finList = curPos = client;
  else
  {
    curPos->next = client;
    curPos       = client;
  }
        //настройки клиента
  if (client->owner)
     client->owner->DelClient(client);
  client->owner = this;
  client->next  = NULL;
}

void    Finall::DelClient (FinClient* client)
{
  if (isDelete || finList==NULL)
     return;

  if (curPos == client)
    curPos = finList = NULL;
  else if (finList == client)
    finList = client->next;
  else
  {
    FinClient* prev = finList;
    for (FinClient* del=finList->next;del;del=del->next,prev=prev->next)
      if (del == client)
      {
        prev->next      = del->next;
        client->owner   = NULL;
        break;
      }
  }
}