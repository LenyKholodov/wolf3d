#include <console\command.h>

const char CH_DELIMITER=';';

Command command;

int Command::get_token(char** p,int* size)
{
  char* pos=*p;
  for(*size=0;isspace(*pos);pos++);
  *p=pos;
  const char* tmp;
  switch(*pos)
  {
    case '\0': return T_EOL;
    case ';' : *size=1; return T_DELIMITER;
    case '"' : // string
                   for(tmp=pos+1;(tmp=strchr(tmp+1,'"'))!=NULL;)
                        if(tmp[-1]!='\\')
                 {
                   *size=tmp-pos+1;
                   return T_STR;
                 }
                   // if broken string
               *size=strlen(pos);
               return T_BROKEN_STR;
    case '0' : if(pos[1]!='x' && pos[1]!='X') // hex digit
                     break;
                   for(*size=2;isxdigit(pos[*size]);(*size)++);
               return T_HNUM;
    case '+':
    case '-':  *size=1;
  }
  if(isalpha(pos[*size]) || pos[*size]=='_') // w0rd
  {
    for((*size)++;isalnum(pos[*size]) || pos[*size]=='_';(*size)++);
    return T_WORD;
  }
  // number, fnumber
  if(isdigit(pos[*size]))
  {
    (*size)++;
    for(int fflag=0;;(*size)++)
      if(!isdigit(pos[*size]))
      {
              if(pos[*size]=='.')
             fflag=1;
        else
          break;
      }
    return fflag?T_FNUM:T_NUM;
  }
  return T_ERR;
}

void Command::operator()(char* cmd)
{
  enum {COMMAND_MODE,PARM_MODE,LOCK_MODE};

  char* CmdStack[MAX_STACK_EQUATION],*endptr;
  VarUnit* ptr;
  char c;
  int size,
      CmdStackPos,
      CmdMode=COMMAND_MODE;                 // начинаем работать с поиска команды
  tCmdFn CmdFn;
  for(CmdStack[CmdStackPos=0]=cmd;;)
    switch(CmdMode)
    {
      case COMMAND_MODE:
        switch(get_token(CmdStack+CmdStackPos,&size))
        {
          case T_DELIMITER:
                             CmdStack[CmdStackPos]+=size; break;
          case T_EOL:
                             if(CmdStackPos)
                        CmdStackPos--;
                      else
                        return;
                      break;
          case T_WORD:
                             c=CmdStack[CmdStackPos][size];
                      CmdStack[CmdStackPos][size]='\0';
                      ptr=vpool.find(CmdStack[CmdStackPos]);
                      CmdStack[CmdStackPos][size]=c;
                      if(ptr==NULL)
                      {
                        (*mOs)<<"Command or variable not found"<<endl;
                        return;
                      }
                      CmdStack[CmdStackPos]+=size;
                      switch(ptr->type)
                      {
                        case VT_MACRO: if(CmdStackPos==MAX_STACK_EQUATION-1)
                                            {
                                          (*mOs)<<"Equations stack overflow"<<endl;
                                         return;
                                       }
                                       CmdStack[++CmdStackPos]=ptr->value.m;
                                       break;
                        case VT_FUNC:  CmdFn=ptr->value.func;
                                                pstack.clear();
                                         CmdMode=PARM_MODE;
                                         break;
                        default:       (*mOs)<<ptr->name<<" is ";
                                           switch(ptr->type)
                                         {
                                           case VT_INT:
                                             (*mOs)<<ptr->value.i<<endl;break;
                                           case VT_PINT:
                                             (*mOs)<<*(ptr->value.pi)<<endl;break;
                                           case VT_FLOAT:
                                             (*mOs)<<ptr->value.f<<endl;break;
                                           case VT_PFLOAT:
                                             (*mOs)<<*(ptr->value.pf)<<endl;break;
                                           case VT_STRING:
                                             {
                                               char s[VALUE_SIZE+10];
                                               str2cstr(s,ptr->value.s,sizeof(s));
                                               (*mOs)<<'"'<<s<<'"'<<endl;
                                             }
                                             break;
                                           default:
                                             (*mOs)<<'?'<<endl;
                                         }
                                         CmdMode=LOCK_MODE;
                      }
                      break;
          default:
                        (*mOs)<<"Error in command line"<<endl;
                      return;
        }
        break;
      case PARM_MODE:
        switch(get_token(CmdStack+CmdStackPos,&size))
        {
          case T_DELIMITER:
                             CmdStack[CmdStackPos]+=size;
                      (*CmdFn)(vpool,pstack,*mOs);
                      CmdMode=COMMAND_MODE;
                      break;
          case T_EOL:
                             if(CmdStackPos)
                        CmdStackPos--;
                      else
                      {
                        (*CmdFn)(vpool,pstack,*mOs);
                        return;
                      }
                      break;
          case T_NUM:
          case T_HNUM:
                             c=CmdStack[CmdStackPos][size];
                      CmdStack[CmdStackPos][size]='\0';
                      if(!pstack.append())
                      {
                              (*mOs)<<"Error: p-stack overflow."<<endl;
                        break;
                      }
                      pstack.top().ptype=PT_INT;
                      pstack.top().value.i=strtol(CmdStack[CmdStackPos],&endptr,0);
                      CmdStack[CmdStackPos][size]=c;
                      CmdStack[CmdStackPos]+=size;
                      break;
          case T_FNUM:
                             c=CmdStack[CmdStackPos][size];
                      CmdStack[CmdStackPos][size]='\0';
                      if(!pstack.append())
                      {
                              (*mOs)<<"Error: p-stack overflow."<<endl;
                        break;
                      }
                      pstack.top().ptype=PT_FLOAT;
                      pstack.top().value.f=strtod(CmdStack[CmdStackPos],&endptr);
                      CmdStack[CmdStackPos][size]=c;
                      CmdStack[CmdStackPos]+=size;
                      break;
          case T_WORD:
                      if(!pstack.append())
                      {
                              (*mOs)<<"Error: p-stack overflow."<<endl;
                        break;
                      }
                      pstack.top().ptype=PT_WORD;
                      pstack.top().value.w.data=CmdStack[CmdStackPos];
                      pstack.top().value.w.length=size;
                      CmdStack[CmdStackPos]+=size;
                      break;
          case T_STR:
                      if(!pstack.append())
                      {
                              (*mOs)<<"Error: p-stack overflow."<<endl;
                        break;
                      }
                      pstack.top().ptype=PT_STRING;
                      pstack.top().value.s.data=CmdStack[CmdStackPos];
                      pstack.top().value.s.length=size;
                      CmdStack[CmdStackPos]+=size;
                      break;
          default:
                        (*mOs)<<"Error in command line"<<endl;
                      return;
        }
        break;
      default: //case LOCK_MODE:
        switch(get_token(CmdStack+CmdStackPos,&size))
        {
          case T_DELIMITER:
                             CmdStack[CmdStackPos]+=size;
                      CmdMode=COMMAND_MODE;
                      break;
          case T_EOL:
                             if(CmdStackPos)
                        CmdStackPos--;
                      else
                        return;
                      break;
          case T_NUM:
          case T_HNUM:
          case T_FNUM:
          case T_WORD:
          case T_STR:
                      CmdStack[CmdStackPos]+=size;
                      break;
          default:
                        (*mOs)<<"Error in command line"<<endl;
                      return;
        }
        break;
    }
}
