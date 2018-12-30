#include <console/bkeffect.h>

template <class mScrPtr> class Simple: public ScreenEffect
{
protected:
  mScrPtr* mScr;
public:
  virtual void close(void) { mScr = NULL; }
  virtual BOOL IsValid(void) { return mScr!=NULL; }
  virtual ~Simple(void) { close();}
};

class Simple8bit: public Simple<mScreen8bit>
{
  public:
          Simple8bit (mScreen8bit* scr)  {
                     mScr = scr;
                     }
  virtual void tick(void);
};

void    Simple8bit::tick()
{
  if (mScr == NULL)
     return;
     
  mScr->cls(1);
  mScr->refresh();
}

ScreenEffect* SimpleEffect(Screen* scr)
{
  if(scr!=NULL && scr->IsValid())
    switch(scr->bpp())
    {
      case 8: return new Simple8bit((mScreen8bit*)scr);
      case 16: return NULL;
      case 24:
      case 32: return NULL;
    }
  return NULL;
}