#include "g_slist.h"
#include "g_system.h"

struct  SubjectNode
{
  ID            object;
  SubjectNode   *next,
                *prev;
};

void    SubjectList::AddSubject (ID obj)
{
  if (list==NULL)
  {
     list       = new SubjectNode ;
     list->next = list->prev = NULL;
  }
  else
  {
     list->next       = new SubjectNode;
     list->next->prev = list;
     list->next->next = NULL;
     list             = list->next;
  }

  list->object = obj;
}

void    SubjectList::DelSubject (ID obj)
{
  for (SubjectNode* tmp=list;tmp;tmp=tmp->next)
      if (tmp->object == obj)
      {
        if (tmp->prev) tmp->prev->next = tmp->next;
        if (tmp->next) tmp->next->prev = tmp->prev;

        delete tmp;
      }
}

void    SubjectList::ClearAll   ()
{
  SubjectNode* del;

  for (SubjectNode* tmp= list;tmp;del=tmp,tmp=tmp->next)
      delete del;

  list = NULL;
}

BOOL    SubjectList::IsPresent (GameObjects type,GameSystem* owner) const
{
  for (SubjectNode* tmp=list;tmp;tmp=tmp->next)
      if (owner->GetObject(tmp->object)->type() == type)
         return TRUE;

  return FALSE;
}

void    SubjectList::Load (BinFile* file)
{

  ID    subMap [4096];
  int   count;

  file->read (&count,sizeof(int));
  file->read (subMap,sizeof(ID)*(count & 4095)); //???

  ClearAll ();

  for (int i=0;i<count;i++)
      if (subMap[i] != ID(-1,-1))
         AddSubject (subMap[i]);
}

void    SubjectList::Save (BinFile* file)
{
  ID    subMap [4096];
  int   count  = 0;

  for (SubjectNode* tmp=list;tmp;tmp=tmp->next,count++)
      subMap[count] = tmp->object;

  file->write (&count,sizeof(int));
  file->write (subMap,sizeof(ID)*count);
}

SubjectList::SubjectList  ()
            : list (NULL)
      {        }

SubjectList::~SubjectList ()
{
  ClearAll ();
}