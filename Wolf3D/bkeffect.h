#ifndef __SCREEN_EFFECTS__
#define __SCREEN_EFFECTS__

#include <mscreen.h>
#include <iomanip.h>
#include <vesa.h>

class ScreenEffect
{
protected:
public:
  virtual void tick(void)=0;
  virtual void close(void);
  virtual BOOL IsValid(void)=0;
  virtual ~ScreenEffect(void);
};

ScreenEffect* BloodEffect(Screen* scr);
ScreenEffect* SimpleEffect(Screen*);
ScreenEffect* BkFoneEffect(Screen*,const char*);

#endif
