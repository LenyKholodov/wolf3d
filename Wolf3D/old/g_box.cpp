#include "g_box.h"
#include "g_system.h"
#include "map.h"

make<GameBox,GAME_BOX>  makeGameBox;

void            GameBox::Load (BinFile* file)
{
  Subject::Load (file);

  GameBoxTag    tag;

  file->read (&tag,sizeof(tag));

//  x             = tag.x;
//  y             = tag.y;
//  mCount        = tag.count;
//  takeSoundId   = tag.takeSoundId;
//  useSoundId    = tag.useSoundId;
//  spriteId      = tag.spriteId;
  boxRes = tag.box;
}

void            GameBox::Save (BinFile* file)
{
  Subject::Save (file);

  GameBoxTag    tag;

//  tag.x             = x;
//  tag.y             = y;
//  tag.count         = mCount;
//  tag.takeSoundId   = takeSoundId;
//  tag.useSoundId    = useSoundId;
//  tag.spriteId      = spriteId;

  tag.box = boxRes;

  file->write (&tag,sizeof(tag));
}

BOOL           GameBox::Exec (GameMessage* mes)
{
  switch (mes->id)
  {
    case ALREADY_INCLUDE:
    {
      SubjectIncludeOkMessage* message = (SubjectIncludeOkMessage*)mes;

      boxRes.count += message->count;

      return TRUE;
    }
    default: return Subject::Exec (mes);
  }
}

void            GameBox::dump (ostream& os) const
{
  os<<"Game box "<<type()<<" ("<<count()<<")"<<endl;
}

void            GameBox::AddStore      (ID box)
{
  SubjectIncludeMessage mes (id());

  owner->PutMessage (&mes,box,sizeof(mes));
}

void            GameBox::Include (int count)
{
  boxRes.count += count;
}

BOOL            GameBox::Include (ID player)
{
  if (Subject::Include (player))
  {
    PlaySoundMessage mes(boxRes.takeSoundId);

    owner->PutMessage (&mes,sizeof(mes));

    refresh (TRUE);

    return TRUE;
  }

  return FALSE;
}

void            GameBox::Exclude (int _x,int _y)
{
  Subject::Exclude ();

  boxRes.x = _x;
  boxRes.y = _y;

  refresh ();
}

void            GameBox::Use     ()
{
  PlaySoundMessage mes(boxRes.useSoundId);

  owner->PutMessage (&mes,sizeof(mes));
}

void            GameBox::Use ( int c )
{
  boxRes.count -= c;

  if      (boxRes.count<0 && boxRes.count+c>=0)  boxRes.count = 0;
  else if (boxRes.count<0)                    boxRes.count = -1;

  GameBox::Use ();      //!!
}

void            GameBox::refresh (BOOL close)
{
  if (!close)
  {
    Block block;

    block.type              = Map::SPRITE;
    block.cross             = 1;
    block.id.sprite.id      = boxRes.spriteId >= 0 ? boxRes.spriteId : 0;
    block.id.sprite.xfrac   = FX_HALF>>12;   //middle
    block.id.sprite.yfrac   = FX_HALF>>12;
    block.id.sprite.already = 0;
    block.id.sprite.alpha   = 0;

    owner->SetBlock (Index(boxRes.y,boxRes.x),block);
    owner->SetObject (Index(boxRes.y,boxRes.x),id());
  }
  else
  {
    Block block;

    block.type              = Map::FREE;
    block.cross             = 1;

    owner->SetBlock (Index(boxRes.y,boxRes.x),block);

    if (owner->GetObject (Index(boxRes.y,boxRes.x)) == id())
       owner->SetObject (Index(boxRes.y,boxRes.x),ID(-1,-1));

    boxRes.x = boxRes.y = -1;
  }

}

/*GameBox::GameBox  (int __count,int __x,int __y,int __tsID,int __usesID,int __sprID)
        : mCount (__count), x(__x), y(__y),
          takeSoundId (__tsID),
          useSoundId  (__usesID),
          spriteId    (__sprID)
{
  if (x>=0 && y>=0) refresh ();
} */

GameBox::GameBox (ArgPtr ptr)
        : Subject(ptr)
{
  if (ptr == NULL)
     return;

  CreateGameBox* arg = (CreateGameBox*)ptr;

//  mCount        = arg->count;
//  takeSoundId   = arg->takeSoundId;
//  useSoundId    = arg->useSoundId;
//  spriteId      = arg->spriteId;
//  x             = arg->x;
//  y             = arg->y;

  boxRes = arg->box;

  if (boxRes.x>=0 && boxRes.y>=0) refresh ();
}

GameBox::GameBox  (BinFile* file)
{
  Load (file);

  if (boxRes.x>=0 && boxRes.y>=0) refresh ();
}

GameBox::~GameBox ()
{        }
