#include "g_ttable.h"
#include "g_ring.h"
#include "g_system.h"
#include "g_load.h"

#include <debug\debug.h>
#include <string.h>

////////////////////////implementation of class TypeTable////////////////////

void*           TypeTable::operator new    (size_t bytes,GameSystem* owner)
{
  TypeTable* x = (TypeTable*)::operator new (bytes);

  x->owner = owner;

  return x;
}

//////////////////Структура для сохранения внутр информации//////////////////

struct  ObjectInfoTag: public GameObjectTag
{
  GameObjects   type;         //Тип объекта
  ID            id;           //Ссылка на объект
  ID            callBack;     //Кто создавал
  int           attr;

  ObjectInfoTag   (GameObjects __type,ID __id = ID(-1,-1),ID cb = ID(-1,-1),int _attr = 0)
        : id (__id), type(__type), callBack (cb), attr(_attr)
         {
           tag  = gameTags [__type];
         }
};

/////////////////////////////////////////////////////////////////////////////

void    TypeTable::Load (BinFile* file)
{
  TypeTableTag  tag (MAX_OBJECTS,MAX_TYPES);

  file->read (&tag,sizeof(tag));

  if (tag.max_types!=MAX_TYPES || tag.max_objects!=MAX_OBJECTS)
  {
    dout<<"Type table: Wrong version. Load failed"<<endl;
    return;
  }

  ClearAll();

        //Возможны ошибки с последующим созданием объектов

  file->read (mNext,sizeof(int16)*MAX_TABLES);
  file->read (mTypes,sizeof(int16)*MAX_TYPES);

  ObjectInfoTag         gameTag (NULL_OBJECT);
  ulong                 error;

  int                   prev = -1;

  for (int i=0;i<tag.numSaves && !file->eof();i++)
  {
      file->read (&gameTag,sizeof(gameTag));

      if (loadFn [gameTag.type] == NULL)
      {
        dout<<"Type table: Error load object "<<gameTag.id
            <<" type "<<gameTag.type
            <<", because it does not have load function"<<endl;
        file->read (&error,sizeof(error));

        Exclude (gameTag.id);

        continue;
      }

#ifdef  __LOAD_GAME_FULL_INFO__
      dout<<"Type table: Load object: "<<gameTag.id<<" Type "<<gameTag.type<<endl;
#endif

      GameObject* object = (*loadFn [gameTag.type])(file,gameTag.id,owner);

      object->callBack = gameTag.callBack;      //Кто создавал
      object->mAttr    = gameTag.attr;          //Атрибуты

      file->read (&error,sizeof(error));

      if (error != END_OF_TAG)
      {
        dout<<"Type table: Error load file. Load failed"<<endl;

        Exclude (gameTag.id);

        return;
      }
  }
}

void    TypeTable::Save (BinFile* file)
{
  TypeTableTag  tag (MAX_OBJECTS,MAX_TYPES);
  GameObject*   current;
  int           numSaves      = 0;
  int           firstFilePos  = file->getpos();

  file->write (&tag,sizeof(tag));
  file->write (mNext,sizeof(int16)*MAX_TABLES);
  file->write (mTypes,sizeof(int16)*MAX_TYPES);

  for (int t = 0;t<MAX_TYPES;t++)
      for (int slot = 0;slot<MAX_SLOTS;slot++)
      {
          if (mTable[t][slot].data == NO_OBJECT)
             continue;

          current = owner->GetObject(ID(t,slot));

          ObjectInfoTag gameTag (current->type(),
                                 ID(t,slot),
                                 current->callBack,
                                 current->mAttr);

          file->write (&gameTag,sizeof(gameTag));

          if (loadFn[current->type()])
             current->Save (file);

          file->write (&END_OF_TAG,sizeof(ulong));

          numSaves++;

#ifdef __LOAD_GAME_FULL_INFO__
          dout<<"TypeTable: Save object: "<<current->id()<<" Type "<<current->type()<<endl;
#endif
      }
#ifndef NDEBUG
      dout<<"Type table: Already saved "<<numSaves<<" objects"<<endl;
#endif

  tag.numSaves   = numSaves;
  int curFilePos = file->getpos ();
  file->seek  (firstFilePos);
  file->write (&tag,sizeof(tag));
  file->seek  (curFilePos);
}

void    TypeTable::dump (ostream& os) const
{
  os<<"No implementation"<<endl;
}

Ring*   TypeTable::GetRing      (GameObjects type)
{
  int   iTable = mTypes [type];

  if (iTable <0 || iTable >MAX_SLOTS)
     return NULL;

  if (mTable [iTable][0].data != NO_OBJECT)
     owner->DeleteObject (ID(iTable,0));

  ID    id (iTable,0);

  Ring* ring = new (id,ID(-1,-1),owner) Ring;

  for (;iTable != END_OF_LIST;iTable = mNext[iTable])
    for (int i=1;i<MAX_SLOTS;i++)   //1 потому что 0 - само кольцо
        if (mTable [iTable][i].data != NO_OBJECT)
        {
           id.id    = i;
           id.type  = iTable;

           ring->Add (id);
        }

  return ring;
}

PoolIndex TypeTable::GetObject    (ID id)
{
#ifndef NDEBUG
  if (id.type>=MAX_TYPES || id.type<0) // 0 - pointer
    return -1;
  if (id.id < 0 || id.id >= MAX_SLOTS)   //0 - pointer
     return -1;
//  if (mSize [id.type] == 0)             //!!!???
//    return -1;
#endif

  return mTable[id.type][id.id].data;
}

int     TypeTable::FindClearType()
{
  for (int i=0;i<MAX_TYPES;i++)
      if (mSize [i] == 0)
        return i;

  return -1;
}

int      TypeTable::RegisterNewType (GameObjects type)
{
  int   lastType = -1;

  if (mTypes[type]>=0 ? mSize[mTypes[type]] : FALSE)
  {
    for (int j=mTypes[type];mNext[j]!=END_OF_LIST;j=mNext[j]);

    lastType = mNext [j] = FindClearType();
  }
  else
    lastType = mTypes [type]    = FindClearType();

  if (lastType)
     mNext [lastType]    = END_OF_LIST;

  return lastType;
}

ID       TypeTable::Include (GameObjects type,PoolIndex index)
{
  ID    id = Include (mTypes [type],index);

  if (id.type == -1)
  {
    int slot = RegisterNewType (type);

    if (slot != -1)
    {
      int      iIndex               = mTable [slot][0].next;

      mTable   [slot][iIndex].data  = index;
      mTable   [slot][0].next       = mTable[slot][iIndex].next;

      mSize    [slot]               = mSize [slot] + 1;

      id.type = slot;
      id.id   = iIndex;
    }
    else
    {
      id.type = -1;
      id.id   = -1;
    }
  }

  return id;
}

ID      TypeTable::Include (int iTable,PoolIndex index)
{
  ID  id;

  id.type = -1;
  id.id   = -1;

  if (iTable<0 || iTable>MAX_SLOTS)
     return id;

  if (mSize [iTable] < MAX_SLOTS && mSize [iTable] > -1)
  {
    int   iIndex = mTable[iTable][0].next;

    if ( iIndex != END_OF_LIST)
    {
       mTable   [iTable][iIndex].data = index;
       mTable   [iTable][0].next      = mTable[iTable][iIndex].next;

       mSize    [iTable]              = mSize [iTable] + 1;

       id.type = iTable;
       id.id   = iIndex;

       return id;
    }

  }

  if (mNext[iTable] != END_OF_LIST)
    return Include (mNext[iTable],index);

  return id;
}

void      TypeTable::Exclude (ID id)
{
#ifndef NDEBUG
  if (id.type < 0 || id.type >= MAX_TYPES)
     return;
  if (id.id < 0 || id.id >= MAX_SLOTS)   //0 - pointer //why <=0
     return;
  if (mSize [id.type] == 0)
     return;
#endif

  mSize [id.type] = mSize [id.type] - 1;

  mTable [id.type][id.id].data = NO_OBJECT;   //Неубирать! Связано с кольцом  

  if (id.id != 0)
  {
    mTable [id.type][id.id].next = mTable [id.type][0].next;
    mTable [id.type][0].next     = id.id;
  }
}

void    TypeTable::SetPoolIndex    (ID id,PoolIndex index)
{
#ifndef NDEBUG
  if (id.type>=MAX_TYPES || id.type<0)   // 0 - pointer
    return ;
  if (id.id < 0 || id.id >= MAX_SLOTS)   //0 - pointer
     return;
#endif

  if (mTable [id.type][id.id].data != NO_OBJECT)     //??
     mTable[id.type][id.id].data = index;
  else
  {
    mTable[id.type][id.id].data = index;
    mSize [id.type]++;

    for (int i=0;i<MAX_SLOTS;i++)
        if (mTable[id.type][i].next == id.id)
        {
          mTable[id.type][i].next = mTable[id.type][id.id].next;
          break;
        }
  }
}

void    TypeTable::ClearAll ()
{
  for (int i=0;i<MAX_TYPES;i++)
  {
    for (int j=0;j<MAX_SLOTS;j++)
    {
       mTable[i][j].data = NO_OBJECT;
       mTable[i][j].next = j+1;
    }
    mTable [i][j-1].next = END_OF_LIST;
  }

  memset (mSize,0,MAX_TYPES*sizeof(int));       //-1

  for (i=0;i<MAX_TYPES;i++)
  {
    mTypes [i] = -1;
    mNext  [i] = END_OF_LIST;
  }
}

TypeTable::TypeTable ()
{
  ClearAll ();
}

TypeTable::~TypeTable ()
{  }

