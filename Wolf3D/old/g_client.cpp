#include "g_client.h"

void            GameClient::Load (BinFile*)
{
}

void            GameClient::Save (BinFile*)
{
}

BOOL            GameClient::Exec (GameMessage* mes)
{
  switch (mes->id)
  {
    case SEQUENCE_MAP_PTR:
    {

    }
    default:    return FALSE;
  }
}

void            GameClient::dump (ostream& os) const
{
  os<<"Client of server "<<GetServer()<<endl;
}

GameClient::GameClient  (ArgPtr)
{
}

GameClient::GameClient  (BinFile*);
GameClient::~GameClient ();