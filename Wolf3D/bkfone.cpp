#include "bkeffect.h"
#include <picture.h>
#include <debug/debug.h>

template <class mScrPtr> class BkFone: public ScreenEffect
{
protected:
  mScrPtr* mScr;
public:
  virtual void close(void) { mScr = NULL; }
  virtual BOOL IsValid(void) { return mScr!=NULL; }
  virtual ~BkFone(void) { close();}
};

class BkFone8bit: public BkFone<mScreen8bit>
{
  public:
          BkFone8bit (mScreen8bit* scr,const char* _name)  {
                     mScr = scr;
                       if (mScr) mScr->cls(1);

                     if (!pict.load(_name))
                     {
                       dout<<"Error open file '"<<_name<<"'"<<endl;
                       return;
                     }

                     image = pict.GetImage();
                     }
  virtual void tick(void);

  protected:
             cPicture   pict;
             tImage*    image;
};

void    BkFone8bit::tick()
{
  if (mScr == NULL)
     return;

  if (image)
     mScr->PutImage (0,mScr->height()-image->height,image);

  mScr->refresh();
}

ScreenEffect* BkFoneEffect(Screen* scr,const char* _name)
{
  if(scr!=NULL && scr->IsValid())
    switch(scr->bpp())
    {
      case 8: return new BkFone8bit((mScreen8bit*)scr,_name);
      case 16: return NULL;
      case 24:
      case 32: return NULL;
    }
  return NULL;
}