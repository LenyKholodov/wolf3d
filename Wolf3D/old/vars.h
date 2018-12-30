#ifndef __SEARCH_VAR__
#define __SEARCH_VAR__

#include <debug.h>
#include <strstrea.h>

class   Var
{
  friend class Debug;
  private:
           Debug::VarNode*  handler;
  public:
           Var();
           Var(VarDesc*);
           Var(const Var& var);
           operator = (const Var& var);
           ~Var();

           void far*    GetPtr() const;

    friend ostream& operator << (ostream&,Var&);
    friend istream& operator >> (istream&,Var&);
};

template <class Type>
class   SearchVar: public VarDesc
{
  protected:
             Var*       search; //interface
  public:
          SearchVar(char*,Type far*);
          virtual ~SearchVar();

    Var*  operator ->   () const { return search; }
          operator Var* () const { return search; }

   virtual  void        print(ostream& os) const;
   virtual  void        input(istream& is) const;
   virtual  void        input(char*)       const;
};

template <class Type>
inline void     SearchVar<Type>::print (ostream& os) const
{
  os<<*(Type*)addr;
}

template <class Type>
inline void     SearchVar<Type>::input (istream& is) const
{
  is>>*(Type*)addr;
}

template <class Type>
inline void     SearchVar<Type>::input (istream& is) const
{
  is>>*(Type*)addr;
}

template <class Type>
inline void     SearchVar<Type>::input(char* str) const
{
  strstreambuf buf(100);
  iostream is (&buf);
  is<<str;
  is>>*(Type*)addr;
}

template <class Type>
SearchVar<Type>::SearchVar (char* name,Type far* ptr)
                     :VarDesc(name,ptr), search(new Var(this)) { }

template <class Type>
SearchVar<Type>::~SearchVar () { delete search; }

#endif