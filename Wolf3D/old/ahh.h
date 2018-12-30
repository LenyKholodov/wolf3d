#include <types.h>
#include <iostream.h>

ulong    GET_SS();
ulong    GET_DS();
ulong    GET_CS();

ulong    GET_ESP();
ulong    GET_EIP();
ulong    GET_EFLAGS();

ulong    SET_SS(ulong _ss);
ulong    SET_DS(ulong _ds);
ulong    SET_CS(ulong _cs);

ulong    SET_ESP(ulong _esp);
ulong    SET_EIP(ulong _eip);
ulong    SET_EFLAGS(ulong _eflags);

struct  EFLAGS
{
 public:
  ulong         CF      : 1;
  ulong         reserv1 : 1;
  ulong         PF      : 1;
  ulong         reserv2 : 1;
  ulong         AF      : 1;
  ulong         reserv3 : 1;
  ulong         ZF      : 1;
  ulong         SF      : 1;
  ulong         TF      : 1;
  ulong         IF      : 1;
  ulong         DF      : 1;
  ulong         OF      : 1;
  ulong         IOPL    : 2;
  ulong         NT      : 1;
  ulong         reserv4 : 1;
  ulong         RF      : 1;
  ulong         VM      : 1;
  ulong         AC      : 1;
  ulong         VIF     : 1;
  ulong         VIP     : 1;
  ulong         ID      : 1;
  ulong         reserv5 : 10;

                EFLAGS();
                EFLAGS(const ulong);

                EFLAGS&       operator =  (const ulong&);
       friend   ostream&      operator << (ostream&,EFLAGS&);

  void          load();
  void          set();
  void          reset();
};

/////////////////////////////////////////////////////////////////////////////
///Класс сохраняющий текущее состояние стека
/////////////////////////////////////////////////////////////////////////////
class   StackFrame
{
  public:
           StackFrame();

     void  Restore() const;
  private:
           ulong        ss;
           ulong        esp;
};

/////////////////////////////////////////////////////////////////////////////
///Просто стек
/////////////////////////////////////////////////////////////////////////////
class   Stack
{
  public:
          Stack(size_t size = 0xFF);
          ~Stack();

          BOOL          Push (void*,size_t);
          void          Pop  (void*,size_t);

          void*         GetTop() const;
  private:
          uchar*        buf;
          uchar*        pos;
          size_t        size;
};

/////////////////////////////////////////////////////////////////////////////
///Класс временно (в текущем периметре) заменяющий системный стек
/////////////////////////////////////////////////////////////////////////////

class   TempStack: public Stack
{
  public:
          TempStack(size_t sz = 0xFFFF);
          ~TempStack();
  private:
          StackFrame*   sf;
          Stack*        temp;
};


#pragma aux GET_SS value [EAX] = "xor eax,eax" "mov ax,ss";
#pragma aux GET_DS value [EAX] = "xor eax,eax" "mov ax,ds";
#pragma aux GET_CS value [EAX] = "xor eax,eax" "mov ax,cs";

#pragma aux GET_ESP    value [EAX] = "mov eax,esp";
#pragma aux GET_EFLAGS value [EAX] = "pushfd" "pop eax";

#pragma aux SET_CS parm  [EAX] = "mov cs,ax";
#pragma aux SET_DS parm  [EAX] = "mov ds,ax";
#pragma aux SET_SS parm  [EAX] = "mov ss,ax";

#pragma aux SET_ESP parm [EAX] = "mov esp,eax";
#pragma aux SET_EFLAGS parm  [EAX] = "push eax" "popfd";
