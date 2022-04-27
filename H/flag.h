#include <debug.h>

class   Flag
{
  friend class Debug;
  private:
           Debug::FlagNode*  handler;
  public:
           Flag();
           Flag(const char* flag_name,BOOL state=FALSE);
           Flag(const Flag& flag);
           operator = (const Flag& flag);
           ~Flag();

           operator int();

           friend ostream& operator<<(ostream&,const Flag&);
};