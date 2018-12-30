#include <console\command.h>

uint VarPool::insert(const char* name,long crcn,uint owner)
{
  uint i=AllocUnit();
  if(i<mCount)
  {
    mPool[i].owner=owner;
    mPool[i].crc=crcn;
    strncpy(mPool[i].name,name,NAME_SIZE);
    mPool[i].name[NAME_SIZE-1]='\0';
    if(owner<mCount)
    {
      if(mPool[owner].crc>=crcn)
      {
        mPool[i].left=mPool[owner].left;
    	mPool[i].right=NOTHING;
	mPool[owner].left=i;
      }      
      else
      {
        mPool[i].right=mPool[owner].right;
    	mPool[i].left=NOTHING;
	mPool[owner].right=i;
      }
    }
    else
    {
      mFirstVar=i;
      mPool[i].left=mPool[i].right=NOTHING;
    }
  }
  return i;
}

long VarPool::crc(const char* s) const
{
  long c=0;
  for(int i=0;i<5 && s[i];c=(c<<6)|(s[i++]&0x0000003F));
  return c;
}

void VarPool::restoreFreeChunks(void)
{
  if(mCount==0)
    return;
  uint last=NOTHING;
  for(uint i=mCount;i--;)
    if(mPool[i].type==VT_FREE)
    {
      if(last<mCount)
	mPool[last].owner=i;
      mPool[i].left=last;
      last=i;
    }
  if((mFirstFree=last)<mCount)
    mPool[mFirstFree].owner=NOTHING;
}

int VarPool::sort_function(const void *a,const void *b)
{
   return ((VarUnit*)a)->crc-((VarUnit*)b)->crc;
}

uint VarPool::makethree(uint start,uint end,uint owner)
{
  if(end<start || end>=mCount)
    return NOTHING;
  uint i=(end+start)/2;
  mPool[i].owner=owner;
  mPool[i].left=makethree(start,i-1,i);
  mPool[i].right=makethree(i+1,end,i);
  return i;
}

void VarPool::pack(void)
{
  // "Перегоняем" все елементы в начало
  uint fi=0,vi=mCount;
  for(;;)
  {
    for(;mPool[fi].type!=VT_FREE && fi<vi;fi++);
    if(fi==vi)
      break;
    for(;mPool[--vi].type==VT_FREE && fi<vi;);
    if(fi==vi)
      break;
    memcpy(mPool+fi,mPool+vi,unitsize());
    mPool[vi].type=VT_FREE;
  }
  // упорядочиваем цепь свободных элементов
  restoreFreeChunks();
  // сортировка массива переменных
  qsort(mPool,varcount(),unitsize(),sort_function);
  // создание дерева
  mFirstVar=makethree(0,varcount()-1,NOTHING);
}

uint VarPool::AllocUnit(void)
{
  if(mFirstFree<mCount)
  {
    uint i=mFirstFree;
    if((mFirstFree=mPool[i].left)<mCount)
      mPool[mFirstFree].owner=NOTHING;
    mVarCount++;
    return i;
  }
  return NOTHING;
}


BOOL VarPool::FreeUnit(uint i)
{
  if(i<mCount)
  {
    if(mPool[i].type!=VT_FREE)
    {
      if(mFirstFree<mCount)
      {
	mPool[mFirstFree].owner=i;
	mPool[i].left=mFirstFree;
      }
      else
        mPool[i].left=NOTHING;
      mPool[i].owner=NOTHING;
      mPool[i].type=VT_FREE;
      mFirstFree=i;
      mVarCount--;
    }
    return TRUE;
  }
  return FALSE;
}

void VarPool::clear(void)
{
  for(uint i=0;i<mCount;mPool[i++].type=VT_FREE);
  restoreFreeChunks();
  mFirstVar=NOTHING;
  mVarCount=0;
}

void VarPool::close(void)
{
  if(mCount)
  {
    ::free(mPool);
    mPool=NULL; 
    mCount=mVarCount=0;
    mFirstFree=mFirstVar=NOTHING;
  }
}

BOOL VarPool::resize(uint count)
{
  if(mCount)
  {
    VarUnit* ptr;
    if(count<mCount)
    {
      if(count<varcount())
        return FALSE;
      pack();
    }
    if((ptr=(VarUnit*)realloc(mPool,count*unitsize()))==NULL)
      return FALSE;
    mPool=ptr;
    for(uint i=mCount;i<count;i++)
      mPool[i].type=VT_FREE;
    mCount=count;
    restoreFreeChunks();    
  }  
  else
  {
    if((mPool=(VarUnit*)malloc(unitsize()*count))==NULL)
      return FALSE;
    mCount=count;
    clear();
  }
  return TRUE;
}

void VarPool::dump(ostream& os)
{
#define OUT_INDEX(i) if(i==NOTHING) os<<"NULL"; else os<<i
  os<<"VarPool. Full Info:"<<endl
    <<"Count(used)="<<mCount<<'('<<mVarCount
    <<"); unitsize="<<unitsize()
    <<"; size="<<mCount*unitsize()<<endl;
  os<<"Pool addr: "<<mPool<<endl;
  os<<"First var block: ";
  OUT_INDEX(mFirstVar);
  os<<"; First free block: ";
  OUT_INDEX(mFirstFree);
  os<<endl<<"Block map:"<<endl;
  for(uint i=0;i<mCount;i++)
  {
    os<<'#'<<i<<'\t';
    OUT_INDEX(mPool[i].owner);
    os<<'\t';
    OUT_INDEX(mPool[i].left);
    os<<'\t';
    if(mPool[i].type!=VT_FREE)
      OUT_INDEX(mPool[i].right);
    else  
      os<<'-';
    os<<'\t'<<hex<<setw(8)<<setfill('0')<<mPool[i].crc<<dec<<'\t';
    switch(mPool[i].type)
    {
      case VT_FREE:  os<<"[FREE]"; break;
      case VT_INT:   os<<"   int["<<mPool[i].name<<"]="<<mPool[i].value.i;    break;
      case VT_FLOAT: os<<" float["<<mPool[i].name<<"]="<<mPool[i].value.f;    break;
      case VT_STRING:os<<"string["<<mPool[i].name<<"]="<<mPool[i].value.s;    break;
      case VT_PINT:  os<<"  *int["<<mPool[i].name<<'='<<*(mPool[i].value.pi); break;
      case VT_PFLOAT:os<<"*float["<<mPool[i].name<<'='<<*(mPool[i].value.pf); break;
      case VT_MACRO: os<<" macro["<<mPool[i].name<<"]="<<mPool[i].value.m;    break;
      case VT_FUNC:  os<<"  func["<<mPool[i].name<<"]="<<mPool[i].value.func; break;
      default:       os<<"!undefined "<<mPool[i].name;       		      break;
    }
    os<<endl;
  }
#undef OUT_INDEX
}

void VarPool::listChain(uint i,ostream& os,ViewAttr attr)
{
  if(i<mCount)
  {
    if(mPool[i].type==VT_FREE)
    {
      os<<"ERROR! Variable pool is corrupted!"<<endl;
      return;
    }
    listChain(mPool[i].left,os,attr);
    switch(mPool[i].type)
    {
      case VT_INT:   if(attr.var) os<<mPool[i].name<<'='<<mPool[i].value.i<<endl; break;
      case VT_FLOAT: if(attr.var) os<<mPool[i].name<<'='<<mPool[i].value.f<<endl; break;
      case VT_STRING:if(attr.var) os<<mPool[i].name<<"=\""<<mPool[i].value.s<<'"'<<endl; break;
      case VT_PINT:  if(attr.var) os<<mPool[i].name<<'='<<*(mPool[i].value.pi)<<endl; break;
      case VT_PFLOAT:if(attr.var) os<<mPool[i].name<<'='<<*(mPool[i].value.pf)<<endl; break;
      case VT_MACRO: if(attr.macro) os<<"macro{"<<mPool[i].name<<"}=\""<<mPool[i].value.m<<'"'<<endl; break;
      case VT_FUNC: if(attr.func) os<<"func{"<<mPool[i].name<<"}=\""<<mPool[i].value.func<<'"'<<endl; break;    
      default:       os<<mPool[i].name<<"=?"<<endl;
      		     break;
    }
    listChain(mPool[i].right,os,attr);
  }
}

void VarPool::list(ostream& os,ViewAttr attr)
{
  os<<"Variable list:"<<endl;
  listChain(mFirstVar,os,attr);
  os<<".end"<<endl;
}

void VarPool::listChain2(uint i,ostream& os)
{
  if(i<mCount)
  {
    char s[VALUE_SIZE+10];
    listChain2(mPool[i].left,os);
    switch(mPool[i].type)
    {
      case VT_INT:   os<<"set "<<mPool[i].name<<' '<<mPool[i].value.i<<endl; break;
      case VT_FLOAT: os<<"set "<<mPool[i].name<<' '<<mPool[i].value.f<<endl; break;
      case VT_STRING:os<<"set "<<mPool[i].name<<" \"";
      	             str2cstr(s,mPool[i].value.s,sizeof(s));
      	   	     os<<s<<'"'<<endl; break;
      case VT_PINT:  os<<"set "<<mPool[i].name<<' '<<*(mPool[i].value.pi)<<endl; break;
      case VT_PFLOAT:os<<"set "<<mPool[i].name<<' '<<*(mPool[i].value.pf)<<endl; break;
      case VT_MACRO: os<<"alias "<<mPool[i].name<<" \""; 
      	             str2cstr(s,mPool[i].value.m,sizeof(s));
      	   	     os<<s<<'"'<<endl; break;
    }
    listChain2(mPool[i].right,os);
  }
}


BOOL VarPool::find(const char* name,long& crcn,uint& i)
{
  crcn=crc(name);
  for(i=mFirstVar;i<mCount;)
  {
    if(crcn==mPool[i].crc)
    {
      if(strcmp(name,mPool[i].name)==0)
        return TRUE;
    }
    if(crcn<=mPool[i].crc)
    {
      if(mPool[i].left<mCount)
      	i=mPool[i].left;
      else
	return FALSE;	
    }
    else
      if(crcn>mPool[i].crc)
      {
        if(mPool[i].right<mCount)
      	  i=mPool[i].right;
        else
	  return FALSE;	
      }
  }
  return FALSE;
}


BOOL VarPool::set(const char* name,const char* value)
{
  uint i;
  long crcn;
  if(find(name,crcn,i))
    switch(mPool[i].type)
    {
      case VT_INT:   mPool[i].value.i=atol(value); 	  break;
      case VT_FLOAT: mPool[i].value.f=atof(value); 	  break;
      case VT_STRING:strncpy(mPool[i].value.s,value,VALUE_SIZE);
		     mPool[i].value.s[VALUE_SIZE-1]='\0';
		     break;
      case VT_PINT:  *(mPool[i].value.pi)=atol(value); 	  break;
      case VT_PFLOAT:*(mPool[i].value.pf)=atof(value); 	  break;
      case VT_MACRO: strncpy(mPool[i].value.m,value,VALUE_SIZE);
		     mPool[i].value.m[VALUE_SIZE-1]='\0';
		     break;
      default: 	     return FALSE;
    }
  else
  {
    uint j=insert(name,crcn,i);
    if(j>=mCount)
      return FALSE;
    mPool[j].type=VT_STRING;
    strncpy(mPool[j].value.s,value,VALUE_SIZE);
    mPool[j].value.s[VALUE_SIZE-1]='\0';
  }
  return TRUE;
}

BOOL VarPool::set(const char* name,long value)
{
  uint i;
  long crcn;
  if(find(name,crcn,i))
    switch(mPool[i].type)
    {
      case VT_INT:   mPool[i].value.i=value; 	      break;
      case VT_FLOAT: mPool[i].value.f=value; 	      break;
      case VT_STRING:ltoa(value,mPool[i].value.s,10); break;
      case VT_PINT:  *(mPool[i].value.pi)=value;      break;
      case VT_PFLOAT:*(mPool[i].value.pf)=value;      break;
      default: 	     return FALSE;
    }
  else
  {
    uint j=insert(name,crcn,i);
    if(j>=mCount)
      return FALSE;
    mPool[j].type=VT_INT;
    mPool[j].value.i=value;
  }
  return TRUE;
}

BOOL VarPool::set(const char* name,double value)
{
  uint i;
  long crcn;
  if(find(name,crcn,i))
    switch(mPool[i].type)
    {
      case VT_INT:   mPool[i].value.i=value; 	      break;
      case VT_FLOAT: mPool[i].value.f=value; 	      break;
      case VT_STRING:gcvt(value,8,mPool[i].value.s);  break;
      case VT_PINT:  *(mPool[i].value.pi)=value;      break;
      case VT_PFLOAT:*(mPool[i].value.pf)=value;      break;
      default: 	     return FALSE;
    }
  else
  {
    uint j=insert(name,crcn,i);
    if(j>=mCount)
      return FALSE;
    mPool[j].type=VT_FLOAT;
    mPool[j].value.f=value;
  }
  return TRUE;
}

BOOL VarPool::setm(const char* name,const char* value)
{
  uint i;
  long crcn;
  if(find(name,crcn,i))
    switch(mPool[i].type)
    {
      case VT_STRING:strncpy(mPool[i].value.s,value,VALUE_SIZE);
		     mPool[i].value.s[VALUE_SIZE-1]='\0';
		     break;
      case VT_MACRO: strncpy(mPool[i].value.m,value,VALUE_SIZE);
		     mPool[i].value.m[VALUE_SIZE-1]='\0';
		     break;
      default: 	     return FALSE;
    }
  else
  {
    uint j=insert(name,crcn,i);
    if(j>=mCount)
      return FALSE;
    mPool[j].type=VT_MACRO;
    strncpy(mPool[j].value.m,value,VALUE_SIZE);
    mPool[j].value.m[VALUE_SIZE-1]='\0';
  }
  return TRUE;
}

BOOL VarPool::link(const char* name,long* value)
{
  uint i;
  long crcn;
  if(find(name,crcn,i))
  {
    mPool[i].type=VT_PINT;
    mPool[i].value.pi=value;
  }
  else
  {
    uint j=insert(name,crcn,i);
    if(j>=mCount)
      return FALSE;
    mPool[j].type=VT_PINT;
    mPool[j].value.pi=value;
  }
  return TRUE;
}

BOOL VarPool::link(const char* name,double* value)
{
  uint i;
  long crcn;
  if(find(name,crcn,i))
  {
    mPool[i].type=VT_PFLOAT;
    mPool[i].value.pf=value;
  }
  else
  {
    uint j=insert(name,crcn,i);
    if(j>=mCount)
      return FALSE;
    mPool[j].type=VT_PFLOAT;
    mPool[j].value.pf=value;
  }
  return TRUE;
}

BOOL VarPool::kill(const char* name)
{
  uint i;
  long crcn;
  if(find(name,crcn,i))
  {
    // "цепляем" левую ветвь к крайнему левому в правой цепи
    uint newi;
    if(mPool[i].right<mCount)
    {
      newi=mPool[i].right;
      for(uint j=mPool[i].right;mPool[j].left<mCount;j++);
      mPool[j].left=mPool[i].left;
    }
    else
      newi=mPool[i].left;
    uint owner=mPool[i].owner;
    FreeUnit(i);
    if(owner<mCount)
    {
      if(mPool[owner].left==i)
        mPool[owner].left=newi;
      else
	mPool[owner].right=newi;	
    }
    else
      mFirstVar=newi;
    mPool[newi].owner=owner;
    return TRUE;
  }
  return FALSE;
}

BOOL VarPool::gets(const char* name,char* value)
{
  uint i;
  long crcn;
  if(find(name,crcn,i))
  {
    switch(mPool[i].type)
    {
      case VT_INT:   ltoa(mPool[i].value.i,value,10); 	  break;
      case VT_FLOAT: gcvt(mPool[i].value.f,8,value);  	  break;
      case VT_STRING:strcpy(value,mPool[i].value.s);  	  break;
      case VT_PINT:  ltoa(*(mPool[i].value.pi),value,10); break;
      case VT_PFLOAT:gcvt(*(mPool[i].value.pf),8,value);  break;
      case VT_MACRO: strcpy(value,mPool[i].value.m);  	  break;
      default: 	     return FALSE;
    }
    return TRUE;
  }
  return FALSE;
}

BOOL VarPool::getm(const char* name,char* value)
{
  uint i;
  long crcn;
  if(find(name,crcn,i) && mPool[i].type==VT_MACRO)
  {
    strcpy(value,mPool[i].value.m);
    return TRUE;
  }
  return FALSE;
}

BOOL VarPool::geti(const char* name,long* value)
{
  uint i;
  long crcn;
  if(find(name,crcn,i))
  {
    switch(mPool[i].type)
    {
      case VT_INT:   *value=mPool[i].value.i; 	   break;
      case VT_FLOAT: *value=mPool[i].value.f;  	   break;
      case VT_STRING:*value=atoi(mPool[i].value.s);break;
      case VT_PINT:  *value=*(mPool[i].value.pi);  break;
      case VT_PFLOAT:*value=*(mPool[i].value.pf);  break;
      default: 	     return FALSE;
    }
    return TRUE;
  }
  return FALSE;
}

BOOL VarPool::getf(const char* name,double* value)
{
  uint i;
  long crcn;
  if(find(name,crcn,i))
  {
    switch(mPool[i].type)
    {
      case VT_INT:   *value=mPool[i].value.i; 	   break;
      case VT_FLOAT: *value=mPool[i].value.f;  	   break;
      case VT_STRING:*value=atof(mPool[i].value.s);break;
      case VT_PINT:  *value=*(mPool[i].value.pi);  break;
      case VT_PFLOAT:*value=*(mPool[i].value.pf);  break;
      default: 	     return FALSE;
    }
    return TRUE;
  }
  return FALSE;
}

BOOL VarPool::setf(const char* name,tCmdFn fn)
{
  uint i;
  long crcn;
  if(find(name,crcn,i))
  {
    if(mPool[i].type!=VT_FUNC)
      return FALSE;
  }
  else
  {
    uint j=insert(name,crcn,i);
    if(j>=mCount)
      return FALSE;
    mPool[j].type=VT_FUNC;
    i=j;
  }
  mPool[i].value.func=fn;
  return TRUE;
}