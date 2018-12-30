#include "g_triger.h"
#include "g_system.h"

void            Triger::Load (BinFile* file)
{
  GameObject::Load (file);

  TrigerTag     tag;

  file->read (&tag,sizeof(tag));

  trigExempl    = tag.exempl;

  mState        = (State)tag.mState;
}

void            Triger::Save (BinFile* file)
{
  GameObject::Save (file);

  TrigerTag     tag;

  tag.exempl    = trigExempl;
  tag.mState    = mState;

  file->write (&tag,sizeof(tag));
}

BOOL            Triger::Exec (GameMessage* mes)
{
  TrigerPressMessage*   message = (TrigerPressMessage*)mes;

  switch (mes->id)
  {
    case TRIGER_ON:           On();           return TRUE;
    case TRIGER_OFF:          Off();          return TRUE;

    case TRIGER_PRESS:
         if (message->side == GetSide())
         {
           Inv();
           return TRUE;
         }
         else
           return FALSE;

    default:                            return FALSE;
  }
}

void            Triger::dump (ostream& os) const
{
  os<<"Triger state is: ";

  if (GetState () == ON) os<<"ON"<<endl;
  else                   os<<"OFF"<<endl;
}

Triger::Triger(ArgPtr ptr)
       :mState (OFF)
{
  CreateTriger* arg = (CreateTriger*)ptr;

  if (arg == NULL)
     return;

  trigExempl = arg->param();

  owner->SetObject (Index(GetY(),GetX()),id());
}

Triger::Triger(BinFile* file)
{
  Load (file);
}

Triger::~Triger()
{
  owner->SetObject (Index(GetY(),GetX()),ID(-1,-1));
                   //А как насчёт графической карты
}


