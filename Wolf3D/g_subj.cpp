#include "g_subj.h"
#include "g_system.h"

make<Subject,SUBJECT>   makeSubject;

void            Subject::Load (BinFile* file)
{
  GameObject::Load (file);

  SubjectTag    tag;

  file->read (&tag,sizeof(tag));

  x             = tag.exempl.x;
  y             = tag.exempl.y;
  subjRes       = tag.res;
}

void            Subject::Save (BinFile* file)
{
  GameObject::Save (file);

  SubjectTag    tag;

  tag.exempl.x  = x;
  tag.exempl.y  = y;
  tag.res       = subjRes;

  file->write (&tag,sizeof(tag));
}

BOOL            Subject::Exec (GameMessage* mes)
{
  switch (mes->id)
  {
    case TICK:
    {
      if (clock()-lastRespawn > subjRes.respawnTime)
         Respawn ();

      return TRUE;
    }
    case INCLUDE:
    {
      Include (((SubjectIncludeMessage*)mes)->playerOwner);
      return TRUE;
    }
    case TELEPORT_TO:
    {
      TeleportToMessage* message = (TeleportToMessage*)mes;

      x = message->x;
      y = message->y;

      refresh (FALSE);

      return TRUE;
    }
    case GET_SUBJECT_INFO:
    {
      SubjectGetInfoMessage*    message = (SubjectGetInfoMessage*)mes;
      SubjectInfoMessage        info (subjRes);

      owner->PutMessage (&info,message->dest,sizeof(info),id());

      return TRUE;
    }
    default:    return FALSE;
  }
}

void            Subject::dump (ostream& os) const
{
  os<<"Subject: X: "<<x<<" Y: "<<y<<" Count: "<<
       subjRes.count<<endl;
}

BOOL            Subject::Include (ID player)
{
  SubjectIncludeOkMessage mes (subjRes);

  owner->PutMessage (&mes,player,sizeof(mes));

  PlaySoundMessage play (subjRes.giveSound,GetX(),GetY());

  owner->PutMessage (&play,sizeof(play));

  if (subjRes.respawnTime == 0)  delete this;
  else
  {
    owner->AddActive (id());
    lastRespawn = clock();
  }

  return TRUE;
}

void    Subject::Respawn ()
{
  owner->DelActive (id());
  refresh (FALSE);

  PlaySoundMessage play (subjRes.respawnSound,GetX(),GetY());

  owner->PutMessage (&play,sizeof(play));
}

void    Subject::refresh (BOOL close)
{
  if (!close)
  {
    Block block;

    block.type              = Map::SPRITE;
    block.cross             = 1;
    block.id.sprite.z       = SPRITE_BOTTOM;
    block.id.sprite.id      = subjRes.mapSprite;
    block.id.sprite.face    = 0;
    block.id.sprite.xfrac   = FX_HALF>>12;   //middle
    block.id.sprite.yfrac   = FX_HALF>>12;
    block.id.sprite.already = 0;
    block.id.sprite.alpha   = 0;

    owner->SetBlock (Index(y,x),block);
    owner->SetObject (Index(y,x),id());
  }
  else
  {
    Block block;

    block.type              = Map::FREE;
    block.cross             = 1;

    owner->SetBlock (Index(y,x),block);

    if (owner->GetObject (Index(y,x)) == id())
       owner->SetObject (Index(y,x),ID(-1,-1));

    x = y = -1;
  }
}

Subject::Subject  (ArgPtr ptr)
{
  CreateSubject* arg = (CreateSubject*)ptr;

  if (arg == NULL)
     return;

  subjRes = arg->resourceClass;

  x       = arg->param().x;
  y       = arg->param().y;

  refresh (FALSE);
}

Subject::Subject  (BinFile* file)
{
  Load (file);
}

Subject::~Subject ()
{
  refresh (TRUE);
  owner->DelActive (id()); //??
}
