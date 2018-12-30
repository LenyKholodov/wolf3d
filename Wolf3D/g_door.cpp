#include "g_door.h"
#include "g_load.h"
#include "g_system.h"

make<Door,DOOR>         makeDoor;

void            Door::Load (BinFile* file)
{
  Triger::Load (file);

  DoorTag       tag;

  file->read (&tag,sizeof(DoorTag));

  doorRes       = tag.res;

  mDoorState    = tag.mDoorState;
  mDoorStep     = tag.mDoorStep;
  mDoorWait     = tag.mDoorWait;
}

void            Door::Save (BinFile* file)
{
  Triger::Save (file);

  DoorTag       tag;

  tag.res           = doorRes;
  tag.mDoorState    = mDoorState;
  tag.mDoorStep     = mDoorStep;
  tag.mDoorWait     = mDoorWait;

  file->write (&tag,sizeof(DoorTag));
}

BOOL            Door::Exec (GameMessage* mes)
{
  TrigerPressMessage* message = (TrigerPressMessage*)mes;

  switch (mes->id)
  {
    case TRIGER_PRESS:
     Inv();
     return TRUE;
    case TICK:
    {
      if (!mDoorWait)
      {
        if (mDoorState >= 127 && mDoorStep == -doorRes.doorStep) //дверь, готовящаяся к закрытию
           Off();

        uint    index = Index(GetY(),GetX());

        if (owner->GetBlock(index)->type != Map::DOOR)
           return TRUE;

        mDoorState += mDoorStep;

        if (mDoorState <= 0)            //close door END
           owner->DelActive (id());

        if (mDoorState >= 127)
        {
           mDoorWait = doorRes.doorWaitTime;//DOOR_WAIT_TIME;
           mDoorStep = -doorRes.doorStep;
        }

        if (mDoorState > 127)    mDoorState = 127;
        if (mDoorState < 0)      mDoorState = 0;
        
                //cross

        Block block = *owner->GetBlock (Index(GetY(),GetX()));

        block.type              = Map::DOOR;
        block.cross             = (mDoorState>doorRes.doorCrossState) ? 1 : 0;
        block.id.door.id        = doorRes.textId;
        block.id.door.state     = mDoorState;

        owner->SetBlock (index,block);
      }
      else
        mDoorWait--;

      return TRUE;
    }
    default:    return  Triger::Exec (mes);
  }
}

void            Door::dump (ostream& os) const
{
  os<<"Simple door ("<<mDoorState<<") #"<<doorRes.textId<<endl;
}

void            Door::On   ()
{
  Triger::On();

  PlaySoundMessage  play (doorRes.openSound,GetX(),GetY());

  owner->PutMessage (&play,sizeof(play),id());

  if (mDoorState == 0)
    owner->AddActive(id(),PR_DOOR);

  mDoorStep = doorRes.doorStep;
}

void            Door::Off  ()
{
  Triger::Off();

  PlaySoundMessage  play (doorRes.closeSound,GetX(),GetY());

  owner->PutMessage (&play,sizeof(play),id());

  if (mDoorWait)
     mDoorWait = 0;

  mDoorStep  = -doorRes.doorStep;
//  mDoorState = 127-1;
}

Door::Door(ArgPtr ptr)
     : Triger (ptr), mDoorState (0), mDoorWait (0)
{
  CreateDoor* arg = (CreateDoor*)ptr;

  if (arg == NULL)
     return;

  doorRes               = arg->resourceClass;

  Block block;

  block.type            = Map::DOOR;
  block.cross           = 0;
  block.id.door.id      = doorRes.textId;
  block.id.door.state   = 0;

  uint  index           = Index(GetY(),GetX());
  mDoorStep             = doorRes.doorStep;

  owner->SetBlock  ( index,block);
  owner->SetObject ( index,id());
}

Door::Door(BinFile* file)
{
  Load(file);
}

Door::~Door ()
{
  Block block;

  block.type            = Map::FREE;
  block.cross           = 1;

  uint  index           = Index(GetY(),GetX());

  owner->SetBlock  (index,block);
  owner->SetObject (index,ID(-1,-1));

  owner->DelActive (id());
}