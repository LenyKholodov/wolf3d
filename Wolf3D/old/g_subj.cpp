#include "g_subj.h"
#include "g_system.h"

void            Subject::Load (BinFile* file)
{
  file->read (&playerOwner,sizeof(ID));
}

void            Subject::Save (BinFile* file)
{
  file->write (&playerOwner,sizeof(ID));
}

BOOL            Subject::Exec (GameMessage* mes)
{
  switch (mes->id)
  {
    case INCLUDE:
    {
      Include (((SubjectIncludeMessage*)mes)->playerOwner);
      return TRUE;
    }
    case EXCLUDE:
    {
      Exclude ();
      return TRUE;
    }
    case USE:
    {
      Use ();
      return TRUE;
    }
    default:    return FALSE;
  }
}

void            Subject::dump (ostream& os) const
{
  os<<"Subject; owner "<<playerOwner<<endl;
}

BOOL            Subject::Include (ID player)
{
  if (playerOwner != ID(-1,-1) || player == ID(-1,-1))
    return FALSE;
  else
  {
    playerOwner = player;

    SubjectIncludeOkMessage mes (id(),0);

    owner->PutMessage (&mes,player,sizeof(mes));

    return TRUE;
  }
}

void            Subject::Exclude ()
{
  playerOwner = ID(-1,-1);
}

Subject::Subject  (ArgPtr arg)
        : playerOwner (ID(-1,-1))
{
  ID* id = (ID*)arg;

  if (arg == NULL)
     return;

  playerOwner = *id;
}

Subject::Subject  (BinFile* file)
        : playerOwner (ID(-1,-1))
{
  Load (file);
}

Subject::~Subject ()
{
  playerOwner = ID(-1,-1);
}
