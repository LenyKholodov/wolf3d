#include <debug.h>

class   Var
{
  friend class Debug;
  private:
           Debug::VarNode*  handler;
  public:
           Var();
           Var(tVar&);
           Var(const Var& var);
           operator = (const Var& var);
           ~Var();

           void far*    GetPtr() const;
};