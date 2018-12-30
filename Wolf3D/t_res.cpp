#include "t_res.h"
#include "g_types.h"

#include <sbinfile.h>

#include <string.h>
#include <debug/debug.h>

/////////////////////implementation of class ResourceHeader//////////////////

///////////////////Загрузка - восстановление ресурса оружия//////////////////

void            ResourceHeader::Refresh (GameObjects _type)
{
  id   = gameTags[_type];
  type = _type;
}

void            ResourceHeader::Save (BinFile* file,GameObjects _type)
{
  Refresh (_type);

  file->write (this,sizeof(*this));
}

GameObjects     ResourceHeader::Load (BinFile* file)
{
  file->read (this,sizeof(*this));

  if (id!=gameTags[type])
  {
    dout<<"Resource header: Wrong file format "<<endl;
    return (GameObjects)-1;
  }

  return type;
}

GameObjects     ResourceHeader::Load (const char* name,void* dest)
{
  stdBinFile    file;

  if (!file.open (name,BinFile::OPEN_RO))
  {
    dout<<"Resource header: File not found "<<name<<endl;
    return (GameObjects)-1;
  }

  GameObjects _type;

  if ((_type = Load(&file)) == -1)
     return (GameObjects)-1;

  Refresh (_type);

  file.read (dest,file.length()-sizeof(ResourceHeader));

  return type;
}

/////////////////implementation of class EnvironmentMap//////////////////////////

struct  EnvironmentMapHeader
{
  char          id [4];
  int           size;
  int           count;
};

void           EnvironmentMap::AddObject (const Exemplar& res)
{
#ifndef NDEBUG
        if (mCount == mSize-1)
           return;

#endif

  mMap [mCount++] = res;
}

const Exemplar&    EnvironmentMap::GetObject (int i) const
{
#ifndef NDEBUG
        if (i>=mSize)
           return mMap [0];
#endif
  return mMap [i];
}

void           EnvironmentMap::ClearAll   ()
{
  mCount = 0;
}

BOOL           EnvironmentMap::Load (BinFile* file)
{
  EnvironmentMapHeader      smh;

  file->read (&smh,sizeof (smh));

  if (memcmp (smh.id,"SMAP",4))
  {
    dout<<"Subject map: Wrong file format"<<endl;
    return FALSE;
  }

        //delete old

  if (mMap) delete mMap;

        //create new

  mMap    = new Exemplar [mSize = smh.size];
  mCount  = smh.count;

  file->read (mMap,sizeof(Exemplar)*mCount);

  return TRUE;
}

BOOL           EnvironmentMap::Load (const char* path)
{
  stdBinFile f;
  if (!f.open(path,BinFile::OPEN_RO))
  {
     dout<<"Subject map: File not found "<<path<<endl;
     return FALSE;
  }
  BOOL status=Load(&f);
  f.close();
  return status;
}

void           EnvironmentMap::Save (BinFile* file)      const
{
  EnvironmentMapHeader      smh;

  memcpy (smh.id,"SMAP",4);

  smh.size  = mSize;
  smh.count = mCount;

  file->write (&smh,sizeof(smh));
  file->write (mMap,sizeof(Exemplar)*mCount);
}

void           EnvironmentMap::Save (const char* path)   const
{
  stdBinFile f;
  f.open(path,BinFile::OPEN_CR);
  Save(&f);
  f.close();
}

EnvironmentMap::EnvironmentMap  (int max )
           : mMap (new Exemplar [max] ),
             mCount (0), mSize (max)
{  }

EnvironmentMap::EnvironmentMap  (BinFile* file)
           :mMap (NULL)
        { Load (file); }

EnvironmentMap::EnvironmentMap  (const char* name)
           :mMap (NULL)
        { Load (name); }

EnvironmentMap::~EnvironmentMap ()
{
  delete mMap;
}