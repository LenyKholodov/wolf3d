#include "g_object.h"
#include "g_system.h"

#include <debug\debug.h>

void*   GameObject::operator new (size_t,GameObjects type,ID cb,GameSystem* owner)
{
  if (owner == NULL)
     return NULL;

  GameObject* obj = (GameObject*) owner->CreateObject (type);

  if (obj == NULL)
     return NULL;

  obj->owner      = owner;
  obj->callBack   = cb;
  obj->mAttr      = DEFAULT_ATTR;

        //call back for creater
  if (cb)
  {
    AlreadyCreateMessage ok (obj->id());
    owner->PutMessage (&ok,cb,sizeof(ok));
  }

  return obj;
}

void*   GameObject::operator new (size_t,ID id,ID cb,GameSystem* owner)
{
  if (owner == NULL)
     return NULL;

  GameObject* obj = (GameObject*) owner->CreateObject (id);

  if (obj == NULL)
     return NULL;

  obj->owner      = owner;
  obj->callBack   = cb;
  obj->mAttr      = DEFAULT_ATTR;

        //call back for creater
  if (cb)
  {
    AlreadyCreateMessage ok (obj->id());
    owner->PutMessage (&ok,cb,sizeof(ok));
  }

  return obj;
}

void    GameObject::operator delete (void* x)
{
  if (x == NULL) return;

  GameObject* obj = (GameObject*)x;

  if (obj == NULL || obj->owner == NULL)
     return;

  if (obj->callBack)
  {
    AlreadyDeleteMessage ok (obj->id());
    obj->owner->PutMessage (&ok,obj->callBack,sizeof(ok));
  }

  obj->owner->DisposeObject (obj->id());
}

void     GameObject::dump (ostream& os) const
{
  os<<"ID (type =  "<<mID.type<<"), (index = "
                    <<mID.id<<")"<<endl;
}

void     GameObject::Load (BinFile* file)
{
  file->read (&mAttr,sizeof(mAttr));
  file->read (&callBack,sizeof(callBack));
}

void     GameObject::Save (BinFile* file)
{
  file->write (&mAttr,sizeof(mAttr));
  file->write (&callBack,sizeof(callBack));
}

GameObject::GameObject(ArgPtr ptr)
{ }

GameObject::GameObject(BinFile* file)
{
}
