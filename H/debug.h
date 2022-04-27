#ifndef __DEBUG__
#define __DEBUG__

#include <iostream.h>
#include <streambu.h>
#include <process.h>
#include <malloc.h>

#include <types.h>

class  BP;              //Отслеживание доступа на адрес
class  Flag;            //Класс флаг (TRUE,FALSE)
class  Var;             //Отслеживание состояния переменных

class  tVar             //Структура для описания переменных
{
  public:
    char*         var_name;
    void  far*    addr;

    tVar() {}
    tVar (char* name,void far* a)
         :var_name(name), addr(a) {}
};

void    far _long_print    (void far*);
void    far _float_print   (void far*);
void    far _LPCTSTR_print (void far*);
void    far _ptr_print     (void far*);

/////////////////////////////////////////////////////////////////////////////
////Класс осуществляющий отладку
/////////////////////////////////////////////////////////////////////////////
class   Debug
{
  public:
          Debug();
          ~Debug();

   enum BPModes {
     EXEC=0,
     READ,
     READ_WRITE
   };

              void       far         SetExceptionName(int i,char* name);
   const char far*       far         GetExceptionName(int i);
              void       far         ResetNames();
              void       far         DeleteNames();

   static     void       far         PrintFlags();
   static     void       far         PrintVars();
   static     void       far         Print();
   static     void       far         ErrorFlush();

              void       far         BreakPoint();

  private:
        struct FlagNode;
        struct VarNode;
  protected:

///////////////////////////////Обработка исключений//////////////////////////

    static void  far     exc00h(); static  void  far     exc01h();
    static void  far     exc02h(); static  void  far     exc03h();
    static void  far     exc04h(); static  void  far     exc05h();
    static void  far     exc06h(); static  void  far     exc07h();
    static void  far     exc08h(); static  void  far     exc09h();
    static void  far     exc0Ah(); static  void  far     exc0Bh();
    static void  far     exc0Ch(); static  void  far     exc0Dh();
    static void  far     exc0Eh(); static  void  far     exc0Fh();
    static void  far     exc10h(); static  void  far     exc11h();
    static void  far     exc12h();

    void  far     InitExceptions();
    void  far     DoneExceptions();

///////////////////////////////Проверка доступа//////////////////////////////

    friend class BP;

    static int        far   AddBP(void far* bp,int mode,int size=4);
    static void       far   DeleteBP(int bp);
    static BOOL       far   GetBPState(int bp);
    static void       far   ResetBP(int bp);

///////////////////////////////Флаги/////////////////////////////////////////
    friend class Flag;

    static void       far    DeleteFlag(FlagNode* flag);
    static void       far    SetFlag(FlagNode* flag,BOOL state);
    static FlagNode*  far    SetFlag(const char* flag_name,BOOL first_state=0);
    static FlagNode*  far    AddFlag(const char* flag_name,BOOL first_state=0)
                                     { return SetFlag(flag_name,first_state);}
    static BOOL       far    GetFlagState(FlagNode* flag);
    static FlagNode*  far    GetAliasFlag(FlagNode* flag);

    static void       far    DeleteFlags();

///////////////////////////////Переменные////////////////////////////////////
    friend class  Var;

    static void       far    DeleteVar(VarNode* flag);
    static void       far    SetVar(VarNode*,tVar&);
    static VarNode*   far    SetVar(tVar&);
    static VarNode*   far    AddVar(tVar& var)
                  { return SetVar(var); }
    static void far*  far    GetVar(VarNode* var);
    static VarNode*   far    GetAliasVar(VarNode* flag);

    static void       far    DeleteVars();

  private:
/////////////////////////////Работа с флагами////////////////////////////////
           struct FlagNode
           {
             Flag*              owner;
             FlagNode*          next;
             FlagNode*          prev;
             char*              flag_name;
             int                state;

             FlagNode (const char* name,int s);
             ~FlagNode();
           };

    static   FlagNode* first_flag;

/////////////////////////////Отслеживание переменных/////////////////////////
           struct VarNode
           {
             tVar               var;
             Var*               owner;
             VarNode*           next;
             VarNode*           prev;

             VarNode (tVar& var);
             ~VarNode();
           };

    static   VarNode* first_var;

/////////////////////////////Исключения////////////////////////////////////
    static char* excNames[0x13];

    void          far* _old_exc00h; void  far* _old_exc01h;
    void          far* _old_exc02h; void  far* _old_exc03h;
    void          far* _old_exc04h; void  far* _old_exc05h;
    void          far* _old_exc06h; void  far* _old_exc07h;
    void          far* _old_exc08h; void  far* _old_exc09h;
    void          far* _old_exc0Ah; void  far* _old_exc0Bh;
    void          far* _old_exc0Ch; void  far* _old_exc0Dh;
    void          far* _old_exc0Eh; void  far* _old_exc0Fh;
    void          far* _old_exc10h; void  far* _old_exc11h;
    void          far* _old_exc12h;
};

extern iostream        dout;            //Поток для отладки

#define __FILE_INFO__

#ifndef NDEBUG
  #ifdef __FILE_INFO__
        #define D_String(x) dout<<__FILE__<<" line #"<<__LINE__<<": "<<(x)<<endl;
        #define D_Error(x)       { \
                                   dout<<"Main error! "; \
                                   D_String(x); \
                                   dout<<"Program halted."<<endl; \
                                   exit(-1); \
                                 }
  #else
        #define D_String(x) cout<<(x)<<endl;
        #define D_Error(x)       { \
                                   dout<<"Main error! "; \
                                   D_String(x); \
                                   dout<<"Program halted."<<endl; \
                                   exit(-1); \
                                 }
  #endif
        #define D_Check          { \
                _heapchk();         \
        }
        #define D_BreakPoint     Debug::BreakPoint()
#else
        #define D_String(x) { }
        #define D_BreakPoint { }
        #define D_Error(x) { }
        #define D_Check { }
#endif

#endif