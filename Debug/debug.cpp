#include <debug/debug.h>
#include <debug/ahh.h>

#include <dpmi32gw.h>
#include <string.h>
#include <fstream.h>

//////////////////////////Статические члены/////////////////////////////////

Debug::FlagNode*                  Debug::first_flag;
Debug::VarNode*                   Debug::first_var;
void far*                         Debug::old_handlers[0x13];

ostream                           dout = cout;

///////////////////////////////Флаги////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
///Реализация класса-оболочки Flag & FlagNode
////////////////////////////////////////////////////////////////////////////

Debug::FlagNode::FlagNode (const char* name,int s)
              : next(NULL), prev(NULL), state(s), owner (NULL)
{
  flag_name = new char[strlen(name)+1];
  strcpy(flag_name,name);
}

Debug::FlagNode::~FlagNode() { delete flag_name; }

Flag::Flag(): handler(NULL) {}

Flag::~Flag() { Debug::DeleteFlag(handler); }

Flag::Flag(const char* flag_name,BOOL state)
{
  handler        = Debug::AddFlag(flag_name,state);
  handler->owner = this;
}

Flag::Flag(const Flag& flag)
{
  handler         = Debug::GetAliasFlag(flag.handler);
  handler->owner  = this;
}

Flag::operator = (const Flag& flag)
{
  Debug::DeleteFlag(handler);
  handler         = Debug::GetAliasFlag(flag.handler);
  handler->owner  = this;
}

Flag::operator = (const int state)
{
  Debug::SetFlag(handler,state);
}

Flag::operator int() { return Debug::GetFlagState(handler); }

ostream& operator<<(ostream& os,const Flag& flag)
{
   if (flag.handler)
   {
     os<<flag.handler->flag_name<<"\t";
     flag.handler->state ?
       os<<"TRUE" :
       os<<"FALSE";
   }
   return os;
}

////////////////////////////////////////////////////////////////////////////
///Создаёт новый флаг и регестрирует его в системе
////////////////////////////////////////////////////////////////////////////

Debug::FlagNode*  _loadds far Debug::SetFlag(const char* flag_name,int state)
{
  FlagNode* flag = new FlagNode(flag_name,state);
  flag->next     = first_flag;
  if (first_flag)
     first_flag->prev = flag;
  first_flag          = flag;

  return flag;
}

////////////////////////////////////////////////////////////////////////////
///Удаляет флаг
////////////////////////////////////////////////////////////////////////////

void       _loadds far    Debug::DeleteFlag(Debug::FlagNode* flag)
{
  if (flag == NULL)
     return;

 if (flag->next)        //last element
     flag->next->prev = flag->prev;
 if (flag->prev)        //first element
 {
     flag->prev->next = flag->next;
 }
 else
     first_flag = flag->next;

  flag->owner->handler = NULL;

  delete flag;
}

////////////////////////////////////////////////////////////////////////////
///Изменяет состояние флага
////////////////////////////////////////////////////////////////////////////

void _loadds far   Debug::SetFlag(Debug::FlagNode* flag,int state)
{
  if (flag == NULL)
     return;

  flag->state = state;
}

////////////////////////////////////////////////////////////////////////////
///Возвращает состояние флага
////////////////////////////////////////////////////////////////////////////

BOOL  _loadds far  Debug::GetFlagState(Debug::FlagNode* flag)
{
  if (flag == NULL)
     return FALSE;

  return flag->state;
}

////////////////////////////////////////////////////////////////////////////
///Создаёт алиасный флаг
////////////////////////////////////////////////////////////////////////////

Debug::FlagNode* _loadds far  Debug::GetAliasFlag(Debug::FlagNode* flag)
{
  if (flag == NULL)
     return NULL;
  else
     return AddFlag(flag->flag_name,flag->state);
}

////////////////////////////////////////////////////////////////////////////
///Дамп флагов
////////////////////////////////////////////////////////////////////////////

static void _loadds far  Debug::PrintFlags()
{
  dout<<"Debug: Flag information"<<endl;
  for (FlagNode* node = first_flag;node;node=node->next)
  {
    dout<<"     ";
    dout<<node->flag_name;
    dout<<"\t\t";
    if (node->state)
       dout<<"TRUE";
    else
       dout<<"FALSE";
    dout<<endl;
  }
}

////////////////////////////////////////////////////////////////////////////
///Удаляет все флаги
////////////////////////////////////////////////////////////////////////////

void  far _loadds  Debug::DeleteFlags()
{
  for (FlagNode* node = first_flag;node;)
  {
            FlagNode* del = node;
            node = node->next;

            DeleteFlag(del);
  }

}

/////////////////////////////Работа с переменными///////////////////////////

////////////////////////////////////////////////////////////////////////////
///Реализация оболочек
////////////////////////////////////////////////////////////////////////////

Debug::VarNode::VarNode (VarDesc* v)
              : next(NULL), prev(NULL), owner(NULL), var(v)
{ }

Debug::VarNode::~VarNode() { }

Var::Var(): handler(NULL) { }

Var::~Var() { Debug::DeleteVar(handler); }

Var::Var(VarDesc* var)
{
  handler        = Debug::AddVar(var);
  handler->owner = this;
}

Var::Var(const Var& var)
{
  handler         = Debug::GetAliasVar(var.handler);
  handler->owner  = this;
}

Var::operator = (const Var& var)
{
  Debug::DeleteVar(handler);
  handler         = Debug::GetAliasVar(var.handler);
  handler->owner  = this;
}

void far* Var::GetPtr () const
{
  return Debug::GetVar(handler);
}

VarDesc::VarDesc() : var_name(NULL), addr(NULL) { }

VarDesc::VarDesc(char* name,void far* ptr)
                       : addr(ptr)
{
  var_name = new char[strlen(name)+1];
  strcpy(var_name,name);
}

VarDesc::VarDesc (const VarDesc& var)
                 : addr (var.addr)
{
  var_name = new char[strlen(var.var_name)+1];
  strcpy(var_name,var.var_name);
}

VarDesc&    VarDesc::operator = (const VarDesc& var)
{
  var_name = new char[strlen(var.var_name)+1];
  strcpy(var_name,var.var_name);

  addr = var.addr;

  return *this;
}

VarDesc::~VarDesc() { delete var_name; }

void    VarDesc::print(ostream& os) const { }
void    VarDesc::input(istream& is) const { }
void    VarDesc::input(char*) const { }

ostream& operator << (ostream& os,Var& v)
{
  v.handler->var->print(os);
  return os;
}

istream& operator >> (istream& is,Var& v)
{
  v.handler->var->input(is);
  return is;
}

////////////////////////////////////////////////////////////////////////////
///Создаёт новую переменную и регестрирует её в системе
////////////////////////////////////////////////////////////////////////////

Debug::VarNode*  _loadds far Debug::SetVar(VarDesc* v)
{
  VarNode* var = new VarNode(v);
  var->next    = first_var;
  if (first_var)
     first_var->prev = var;
  first_var          = var;

  return var;
}

////////////////////////////////////////////////////////////////////////////
///Удаляет переменную
////////////////////////////////////////////////////////////////////////////

void       _loadds far    Debug::DeleteVar(Debug::VarNode* var)
{
  if (var == NULL)
     return;

 if (var->next)        //last element
     var->next->prev = var->prev;
 if (var->prev)        //first element
 {
     var->prev->next = var->next;
 }
 else
     first_var = var->next;

  var->owner->handler = NULL;

  delete var;
}

////////////////////////////////////////////////////////////////////////////
///Изменяет дескриптор переменной
////////////////////////////////////////////////////////////////////////////

void _loadds far   Debug::SetVar(Debug::VarNode* var,VarDesc* v)
{
  if (var == NULL)
     return;

  delete var;

  var = SetVar(v);
}

////////////////////////////////////////////////////////////////////////////
///Возвращает адрес отслеживаемой переменной
////////////////////////////////////////////////////////////////////////////

void far*  _loadds far  Debug::GetVar(Debug::VarNode* var)
{
  if (var == NULL)
     return NULL;

  return var->var->addr;
}

////////////////////////////////////////////////////////////////////////////
///Создаёт алиасную переменную
////////////////////////////////////////////////////////////////////////////

Debug::VarNode* _loadds far  Debug::GetAliasVar(Debug::VarNode* var)
{
  if (var == NULL)
     return NULL;
  else
     return AddVar(var->var);
}

////////////////////////////////////////////////////////////////////////////
///Дамп переменных
////////////////////////////////////////////////////////////////////////////

static void _loadds far  Debug::PrintVars()
{
  dout<<"Debug: Dump of vars"<<endl;
  for (VarNode* node = first_var;node;node=node->next)
  {
    dout<<"     ";
    dout<<node->var->var_name;
    dout<<"\t\t";
    node->var->print(dout);
    dout<<endl;
  }
}

////////////////////////////////////////////////////////////////////////////
///Удаление всех переменных
////////////////////////////////////////////////////////////////////////////

void  far _loadds  Debug::DeleteVars()
{
  for (VarNode* node = first_var;node;)
  {
            VarNode* del = node;
            node         = node->next;

            DeleteVar(del);
  }
}

/////////////////////////////////////Исключения//////////////////////////////

char*   ExcName[0x13] =
{
  "Exception #DE: Деление на ноль (00h)",
  "Exception #DB: Отладочное прерывание (01h)",
  "Exception    : Прерывание NMI (02h)",
  "Exception #BP: Точка останова (03h)",
  "Exception #OF: Переполнение (04h)",
  "Exception #BR: Переполнение при BOUND (05h)",
  "Exception #UD: Недопустимая операция (06h)",
  "Exception #NM: Сопроцессор отсутствует (07h)",
  "Exception #DF: Двойная ошибка (08h)",
  "Exception    : Reserved (09h)",
  "Exception #TS: Ошибочный TSS (0Ah)",
  "Exception #NP: Сегмент недоступен (0B0h)",
  "Exception #SS: Ошибка стека (0Ch)",
  "Exception #GP: Общая ошибка защиты (0Dh)",
  "Exception #PF: Ошибка страничной адресации (0Eh)",
  "Exception    : Reserved (0Fh)",
  "Exception #MF: Ошибка сопроцессора (10h)",
  "Exception #AC: Ошибка выравнивания (11h)",
  "Exception #MC: Машинно-зависимая ошибка (12h)"
};



void  _loadds far exc00h() { Debug::Print(0x00); exit (-1); }
void  _loadds far exc01h() { Debug::Print(0x01); exit (-1); }
void  _loadds far exc02h() { Debug::Print(0x02); exit (-1); }
void  _loadds far exc03h() { Debug::Print(0x03); exit (-1); }
void  _loadds far exc04h() { Debug::Print(0x04); exit (-1); }
void  _loadds far exc05h() { Debug::Print(0x05); exit (-1); }
void  _loadds far exc06h() { Debug::Print(0x06); exit (-1); }
void  _loadds far exc07h() { Debug::Print(0x07); exit (-1); }
void  _loadds far exc08h() { Debug::Print(0x08); exit (-1); }
void  _loadds far exc09h() { Debug::Print(0x09); exit (-1); }
void  _loadds far exc0Ah() { Debug::Print(0x0A); exit (-1); }
void  _loadds far exc0Bh() { Debug::Print(0x0B); exit (-1); }
void  _loadds far exc0Ch() { Debug::Print(0x0C); exit (-1); }
void  _loadds far exc0Dh() { Debug::Print(0x0D); exit (-1); }
void  _loadds far exc0Eh() { Debug::Print(0x0E); exit (-1); }
void  _loadds far exc0Fh() { Debug::Print(0x0F); exit (-1); }
void  _loadds far exc10h() { Debug::Print(0x10); exit (-1); }
void  _loadds far exc11h() { Debug::Print(0x11); exit (-1); }
void  _loadds far exc12h() { Debug::Print(0x12); exit (-1); }

void  far Debug::InitExceptions()
{
  for (int i = 0;i<0x13;i++)
      old_handlers[i] = DPMIGetExceptionHandler(i);

  DPMISetExceptionHandler(0x00,exc00h);
  DPMISetExceptionHandler(0x01,exc01h);
  DPMISetExceptionHandler(0x02,exc02h);
  DPMISetExceptionHandler(0x03,exc03h);
  DPMISetExceptionHandler(0x04,exc04h);
  DPMISetExceptionHandler(0x05,exc05h);
  DPMISetExceptionHandler(0x06,exc06h);
  DPMISetExceptionHandler(0x07,exc07h);
  DPMISetExceptionHandler(0x08,exc08h);
  DPMISetExceptionHandler(0x09,exc09h);
  DPMISetExceptionHandler(0x0A,exc0Ah);
  DPMISetExceptionHandler(0x0B,exc0Bh);
  DPMISetExceptionHandler(0x0C,exc0Ch);
  DPMISetExceptionHandler(0x0D,exc0Dh);
  DPMISetExceptionHandler(0x0E,exc0Eh);
  DPMISetExceptionHandler(0x0F,exc0Fh);
  DPMISetExceptionHandler(0x10,exc10h);
  DPMISetExceptionHandler(0x11,exc11h);
  DPMISetExceptionHandler(0x12,exc12h);
}

void far Debug::DoneExceptions()
{
  for (int i = 0;i<0x13;i++)
      DPMISetExceptionHandler(i,old_handlers[i]);
}

void    _break_point();
#pragma aux _break_point = "int 03h";

void    far Debug::BreakPoint() { _break_point(); }

int     far Debug::AddBP(void far* bp,int mode,int size)
{
  REGS  regs;

  regs.w.ax = 0xB00;
  regs.w.bx = hiword(bp);
  regs.w.cx = loword(bp);
  regs.h.dh = mode;
  regs.h.dl = size;

  int386(0x31,&regs,&regs);

  return regs.w.cflag & 1 ? -1 : regs.w.bx;

}

void    far Debug::DeleteBP(int bp)
{
  REGS  regs;

  regs.w.ax = 0xB01;
  regs.w.bx = bp;

  int386(0x31,&regs,&regs);
}

BOOL    far Debug::GetBPState(int bp)
{
  REGS  regs;

  regs.w.ax = 0xB02;
  regs.w.bx = bp;

  int386(0x31,&regs,&regs);

  return regs.w.cflag & 1 ? FALSE : regs.w.ax & 1;
}

void    far Debug::ResetBP(int bp)
{
  REGS  regs;

  regs.w.ax = 0xB03;
  regs.w.bx = bp;

  int386(0x31,&regs,&regs);
}

BP::BP(): handler(-1) {}

BP::BP(void far* addr,int mode,int op_size)
{
  handler = Debug::AddBP(addr,mode,op_size);
}

BP::BP(const BP& bp): handler (bp.handler) {}

BP::operator = (const BP& bp)
{
  Debug::DeleteBP(handler);
  handler = bp.handler;
}

BP::~BP()   { Debug::DeleteBP(handler); }

void       BP::Reset() const { Debug::ResetBP(handler); }

BP::operator int() { return Debug::GetBPState(handler); }

void  far _loadds Debug::Print()
{
  PrintFlags();
  PrintVars();
}

/////////////////////////////////////////////////////////////////////////////
///На время работы обработчика меняем стек
/////////////////////////////////////////////////////////////////////////////
StackFrame*     frame;
char            data[0x1000];

void  far _loadds Debug::Print(int i)
{
  _disable();

//  frame = new StackFrame();

  SET_SS(GET_DS());
  SET_ESP((int)data+0x1000);

  dout<<"Exception: #"<<hex<<i<<dec<<". Program halted"<<endl;
  dout<<ExcName[i]<<endl;

  PrintFlags();
  PrintVars();

//  frame->Restore();
//  delete frame;

  _enable();
}

////////////////////////////////////////////////////////////////////////////
///Управление отладкой
////////////////////////////////////////////////////////////////////////////

void  Debug::CmdExit(VarPool&,ParmStack&,ostream&)
{
  exit(0);
}

void  Debug::CmdList(VarPool& vp,ParmStack& p,ostream& os)
{
  Debug::PrintVars();
}

void  Debug::CmdDump(VarPool& vp,ParmStack& p,ostream& os)
{
  Debug::PrintFlags();
  Debug::PrintVars();
}

void  Debug::CmdHelp(VarPool&,ParmStack&,ostream& os)
{
  os<<"Fuck off!"<<endl;
}

void Debug::CmdCls(VarPool&,ParmStack&,ostream& os)
{
  for(int i=0;i<53;i++)
    os<<endl;
}

void  Debug::CmdSet(VarPool& v,ParmStack& p,ostream& os)
{
  if(p.size()<2)
  {
    os<<"Error: Too few parameters"<<endl;
    return;
  }
  if(p.size()>2)
  {
    os<<"Error: Too many parameters"<<endl;
    return;
  }

  char             buf[0xFF];
  strstreambuf     temp(100);
  iostream         io(&temp);

  for (VarNode* node=first_var;node;node=node->next)
  {
        io.write(p[0].value.w.data,p[0].value.w.length);
        io>>buf;

//      strcpy(buf,strupr( buf ));

      if (!strcmp(node->var->var_name,buf))
      {
        switch (p[1].ptype)
        {
          case PT_INT:   io<<p[1].value.i;      break;
          case PT_FLOAT: io<<p[1].value.f;      break;
          case PT_WORD:  io<<p[1].value.w.data; break;
          default:                              continue;
        }
        io>>buf;
        node->var->input(buf);
        os<<node->var->var_name<<":\t";
        node->var->print(os);
        os<<endl;
        return;
      }
  }
  if (node == NULL)
        os<<"Error: Variable not found"<<endl;
}

void  Debug::CmdGo(VarPool& v,ParmStack& p,ostream& os)
{
  os<<"Program go on"<<endl;
}

void Debug::InitCommand()
{
  if(!command.vpool.resize(256))
    return;
  if(command.pstack.maxsize(100)!=100)
    return;
  command.vpool.setf("exit",CmdExit);
  command.vpool.setf("help",CmdHelp);
  command.vpool.setf("list",CmdList);
  command.vpool.setf("cls",CmdCls);
  command.vpool.setf("set",CmdSet);
  command.vpool.setf("dump",CmdDump);
  command.vpool.setf("go",CmdGo);
}

////////////////////////////////////////////////////////////////////////////
///Изменение состояния переменных
////////////////////////////////////////////////////////////////////////////

void _loadds far Debug::Input()
{
  dout<<"................................DEBUG SYSTEM...................................."<<endl;
  PrintFlags();
  PrintVars();

  char buffer[128];

  while(strcmp(buffer,"go"))
  {
    dout<<"]";
    cin.getline(buffer,sizeof(buffer));
    command(buffer);
  }
}

Debug::Debug()
{
          //Init flag system
  first_flag = NULL;

          //Init var system
  first_var  = NULL;

          //Init exception system
  InitExceptions();
  InitCommand();
}

Debug::~Debug()
{
  DeleteFlags();
  DeleteVars();
  DoneExceptions();
}

///////////////////////////////Стеки/////////////////////////////////////////

StackFrame::StackFrame()
{
  ss  = GET_SS();
  esp = GET_ESP();
}

void    StackFrame::Restore() const
{
  SET_SS(ss);
  SET_ESP(esp);
}

Stack::Stack(size_t sz)
{
  buf = new uchar [size=sz];
  pos = buf + size;
}

Stack::~Stack() { delete buf; }

BOOL    Stack::Push(void* ptr,size_t sz)
{
  if (pos-sz<buf)
     return FALSE;
  else
  {
    pos -= sz;
    memcpy(pos,ptr,sz);
    return TRUE;
  }
}

void    Stack::Pop(void* dest,size_t sz)
{
  if (pos+sz>=buf+size)
     return;
  else
  {
    memcpy(dest,pos,sz);
    pos += sz;
  }
}

void*   Stack::GetTop() const { return buf+size; }

///////////////////////Класс EFLAGS//////////////////////////////////////////

EFLAGS::EFLAGS() { load(); }
EFLAGS::EFLAGS(const ulong image) { *(ulong*)this = image; }

ostream&  operator << (ostream& os,EFLAGS& eflags)
{
  const char* names[22] = {"CF",0,"PF",0,"AF",0,"ZF","SF","TF","IF",
                           "DF","OF","IOPL",0,"NT",0,"RF","VM","AC",
                           "VIF","VIP","ID"};

  ulong* state;
  *state = *(ulong*)&eflags;
  for (int i=0;i<22;i++)
  {
      if (names[i] == NULL)
         continue;
      if (i != 12)
      {
        ulong  mask = 1l<<i;
        if (*state & mask)
              os<<names[i]<<":\t TRUE"<<endl;
        else
              os<<names[i]<<":\t FALSE"<<endl;
      }
      else
        os<<names[i]<<":\t "<<((*state & 0x3000)>>12)<<endl;
  }
  return os;
}

EFLAGS& EFLAGS::operator = (const ulong& fl) { return *this = fl; }

void   EFLAGS::load() { *(ulong*)this = GET_EFLAGS(); }
void   EFLAGS::set()  { SET_EFLAGS(*(ulong*)this); }
void   EFLAGS::reset() { set(); load(); }