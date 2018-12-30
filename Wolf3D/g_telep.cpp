#include "g_telep.h"
#include "g_anim.h"
#include "g_system.h"
#include "defs.h"

make<Teleport,TELEPORT> makeTeleport;
make<Portal,PORTAL>     makePortal;

/////////////////////////implementation of class Teleport////////////////////

void            Teleport::Load (BinFile* file)
{
  GameObject::Load (file);

  TeleportTag tag;

  file->read (&tag,sizeof(tag));

  telepRes = tag.res;
  telepPos = tag.exempl;
  animPos  = tag.animPos;
  last     = clock();
}

void            Teleport::Save (BinFile* file)
{
  GameObject::Save (file);

  TeleportTag tag;

  tag.res     = telepRes;
  tag.exempl  = telepPos;
  tag.animPos = animPos;

  file->write (&tag,sizeof(tag));
}

BOOL            Teleport::Exec (GameMessage* mes)
{
  switch (mes->id)
  {
    case TICK:
    {
      tick();
      return TRUE;
    }
    default: return FALSE;
  }
}

void            Teleport::dump (ostream& os) const
{
  os<<"Teleport (,"<<telepPos.xFrom<<","<<telepPos.yFrom<<")-("<<
                     telepPos.xTo<<","<<telepPos.xTo<<")"<<endl;
}

void            Teleport::trans (ID transObject)
{
        //Проигрываем звук

  PlaySoundMessage soundMes (telepRes.sound,telepPos.xFrom,telepPos.yFrom);

  owner->PutMessage (&soundMes,sizeof(soundMes));

        //Переносим объект

  TeleportToMessage telepTo (telepPos.xTo,telepPos.yTo);

  owner->PutMessage (&telepTo,transObject,sizeof(telepPos));
}

void            Teleport::tick  ()
{
  int index    = Index (telepPos.yFrom,telepPos.xFrom);
  ID     check = owner->GetObject(index);
  Block* block = owner->GetBlock(index);

  if (check != id() || block->type != Map::PORTAL)
  {
    trans   (check);
    refresh ();
  }
}

void            Teleport::refresh()
{
  int temp     = 0;
  int face     = NextFace (telepRes.anim,animPos,temp);
  Block* block = owner->GetBlock(Index (telepPos.yFrom,telepPos.xFrom));

  block->type              = Map::PORTAL;
  block->cross             = cross();
  block->id.sprite.z       = SPRITE_CENTER;
  block->id.sprite.id      = telepRes.sprite;
  block->id.sprite.face    = face;
  block->id.sprite.xfrac   = FX_HALF>>12;
  block->id.sprite.yfrac   = FX_HALF>>12;
  block->id.sprite.alpha   = 0;
  block->id.sprite.already = 0;

  owner->SetBlock(Index (telepPos.yFrom,telepPos.xFrom),*block);
}

Teleport::Teleport(ArgPtr ptr)
         : last (clock())
{
  if (ptr == NULL)
     return;

  CreateTeleport* arg = (CreateTeleport*)ptr;

  telepRes  = arg->resourceClass;
  telepPos  = arg->param();
  animPos   = 0;

  owner->AddActive (id());
  owner->SetObject (Index(telepPos.yFrom,telepPos.xFrom),id());

  refresh();
}

Teleport::Teleport(BinFile* file)
{
  Load (file);
}

Teleport::~Teleport()
{
  owner->DelActive (id());
  owner->SetObject (Index(telepPos.yFrom,telepPos.xFrom),ID(-1,-1));

  Block* block  = owner->GetBlock(Index (telepPos.yFrom,telepPos.xFrom));
  block->type   = Map::FREE;
  block->cross  = 1;

  owner->SetBlock(Index (telepPos.yFrom,telepPos.xFrom),*block);
}

//////////////////////////////implementation of class Portal/////////////////

void            Portal::Load (BinFile* file)
{
  Teleport::Load (file);

  file->read (levelName,sizeof(levelName));     //!!!!!
}

void            Portal::Save (BinFile* file)
{
  Teleport::Save (file);

  file->write (levelName,sizeof(levelName));    //!!!!!
}

void            Portal::dump (ostream& os) const
{
  Teleport::dump(os);
  os<<"Portal level name '"<<levelName<<"'"<<endl;
}

void            Portal::trans (ID transObject)
{
        //Проигрываем звук

  PlaySoundMessage soundMes (telepRes.sound,telepPos.xFrom,telepPos.yFrom);

  owner->PutMessage (&soundMes,sizeof(soundMes));

        //Переносим объект

  PortalChangeLevelMessage message(levelName,telepPos.xTo,telepPos.yTo);

  owner->PutMessage (&message,transObject,sizeof(message));
}

Portal::Portal(ArgPtr ptr)
       :Teleport (ptr)
{
  if (ptr == NULL)
     return;

  CreatePortal* arg = (CreatePortal*)ptr;
  strcpy (levelName,arg->param().levelName);
}

Portal::Portal(BinFile* file)
{
  Load (file);
}

Portal::~Portal()
{  }