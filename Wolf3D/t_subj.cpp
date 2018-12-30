#include "t_subj.h"
#include "t_res.h"

#include <sbinfile.h>

#include <string.h>
#include <debug/debug.h>

///////////////////Загрузка - восстановление ресурса оружия//////////////////

void            SubjectRes::Save (BinFile* file)
{
  ResourceHeader::Save (file,SUBJECT);

  file->write (this,sizeof(SubjectRes));
}

void            SubjectRes::Save (const char* name)
{
  stdBinFile    file;

  if (!file.open (name,BinFile::OPEN_CR))
  {
    dout<<"Subject resource: Cannot create file "<<name<<endl;
    return ;
  }

  Save(&file);

  file.close();
}

BOOL            SubjectRes::Load (BinFile* file)
{
  file->seek (sizeof(ResourceHeader));
  file->read (this,sizeof(SubjectRes));

  return TRUE;
}

BOOL            SubjectRes::Load (const char* name)
{
  stdBinFile    file;

  if (!file.open (name,BinFile::OPEN_RO))
  {
    dout<<"Subject resource: File not found "<<name<<endl;
    return FALSE;
  }

  BOOL  state = Load (&file);

  file.close();

  return state;
}
