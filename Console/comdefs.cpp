#include <console/comdefs.h>
#include <fin.h>

void CmdExit(VarPool&,ParmStack&,ostream&)
{
  delete fin;
}

void CmdDump(VarPool& vp,ParmStack& p,ostream& os)
{
  vp.dump(os);
}

void CmdHelp(VarPool&,ParmStack&,ostream& os)
{
  os<<"Fuck off!"<<endl;
  os<<"Call: (06232) 5-96-24 or (06232) 22-19-65"<<endl;
}

void CmdCls(VarPool&,ParmStack&,ostream& os)
{
  for(int i=0;i<25;i++)
    os<<endl;
}

void CmdList(VarPool& vp,ParmStack& p,ostream& os)
{
  vp.list(os);
}

void CmdSet(VarPool& v,ParmStack& p,ostream& os)
{
  if(p.size()<2)
  {
    os<<"Error: too few parameters"<<endl;
    return;
  }
  if(p.size()>2)
  {
    os<<"Error: too many parameters"<<endl;
    return;
  }
  if(p[0].ptype!=PT_WORD)
  {
    os<<"Error: incorrect data type"<<endl;
    return;
  }
  char t,t1,buff[200];
  t=p[0].value.w.data[p[0].value.w.length];
  p[0].value.w.data[p[0].value.w.length]='\0';
  switch(p[1].ptype)
  {
    case PT_STRING:  t1=p[1].value.s.data[p[1].value.s.length-1];
                          p[1].value.s.data[p[1].value.s.length-1]='\0';
                     cstr2str(buff,p[1].value.s.data+1,sizeof(buff));
                     p[1].value.s.data[p[1].value.s.length-1]=t1;
                     v.set(p[0].value.w.data,buff);
                     break;
    case PT_ZSTRING: v.set(p[0].value.w.data,p[1].value.zs);
                          break;
    case PT_WORD:    t1=p[1].value.w.data[p[1].value.w.length];
                          p[1].value.w.data[p[1].value.w.length]='\0';
                     v.set(p[0].value.w.data,p[1].value.w.data);
                     p[1].value.w.data[p[1].value.w.length]=t1;
                     break;
    case PT_INT:     v.set(p[0].value.w.data,p[1].value.i);
                          break;
    case PT_FLOAT:   v.set(p[0].value.w.data,p[1].value.f);
                          break;
    default:         os<<"Error: incorrect data type"<<endl;
  }
  p[0].value.w.data[p[0].value.w.length]=t;
}

void CmdAlias(VarPool& v,ParmStack& p,ostream& os)
{
  if(p.size()<2)
  {
    os<<"Error: too few parameters"<<endl;
    return;
  }
  if(p.size()>2)
  {
    os<<"Error: too many parameters"<<endl;
    return;
  }
  if(p[0].ptype!=PT_WORD)
  {
    os<<"Error: incorrect data type"<<endl;
    return;
  }
  char t,t1,buff[200];
  t=p[0].value.w.data[p[0].value.w.length];
  p[0].value.w.data[p[0].value.w.length]='\0';
  switch(p[1].ptype)
  {
    case PT_STRING:  t1=p[1].value.s.data[p[1].value.s.length-1];
                          p[1].value.s.data[p[1].value.s.length-1]='\0';
                     cstr2str(buff,p[1].value.s.data+1,sizeof(buff));
                     p[1].value.s.data[p[1].value.s.length-1]=t1;
                     v.setm(p[0].value.w.data,buff);
                     break;
    case PT_ZSTRING: v.setm(p[0].value.w.data,p[1].value.zs);
                          break;
    case PT_WORD:    t1=p[1].value.w.data[p[1].value.w.length];
                          p[1].value.w.data[p[1].value.w.length]='\0';
                     v.setm(p[0].value.w.data,p[1].value.w.data);
                     p[1].value.w.data[p[1].value.w.length]=t1;
                     break;
    default:         os<<"Error: incorrect data type"<<endl;
  }
  p[0].value.w.data[p[0].value.w.length]=t;
}

void CmdKill(VarPool& v,ParmStack& p,ostream& os)
{
  if(p.size()<1)
  {
    os<<"Error: too few parameters"<<endl;
    return;
  }
  if(p.size()>1)
  {
    os<<"Error: too many parameters"<<endl;
    return;
  }
  if(p[0].ptype!=PT_WORD)
  {
    os<<"Error: incorrect data type"<<endl;
    return;
  }
  char t;
  t=p[0].value.w.data[p[0].value.w.length];
  p[0].value.w.data[p[0].value.w.length]='\0';
  if(!v.kill(p[0].value.w.data))
    os<<"Variable not found or not accessable"<<endl;
  p[0].value.w.data[p[0].value.w.length]=t;
}