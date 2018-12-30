#include <console\console.h>

void grConsole::StartInit(void)
{
  mFnt=NULL;
  mFntWidth=0;
  mSe=NULL;
  mEScreen=NULL;
  mDrawScreen=NULL;
  mConsole=NULL;
  mESOwner=mFntOwner=FALSE;
  eLink=FALSE;
  mCursorBlinkValue=0;
  textColor=0xFF;
  etColor=0xFF;
  bkColor=0;
  ShowSpeed=1;
  CursorBlinkSpeed=0x8800000;
  mVisible=FALSE;
}

BOOL grConsole::open(Screen* dS,tBinFont* font,Screen* e)
{
  close();
  if(dS==NULL || font==NULL || !dS->IsValid())
    return FALSE;
  mFnt=font;
  mDrawScreen=dS;
  for(int i=mFnt->start;i<=mFnt->end;i++)
    if(mFntWidth<mFnt->width(i))
      mFntWidth=mFnt->width(i);
  if((mConsole=tConsole::create(mDrawScreen->width()/mFntWidth,
                                 mDrawScreen->height()/mFnt->height))==NULL)
  {
    close();
    return FALSE;
  }
  if(e==NULL || !e->IsValid())
  {
    e=GimmeMirror(dS);
    mESOwner=TRUE;
  }
    //effects (blood)
  if((mSe=SimpleEffect(mEScreen=e))!=NULL)
  {
    eLink=TRUE;
    if(dS->bpp()==8)
      dS->PutPalette(192,64,((mScreen8bit*)e)->GetPalette()+192);
  }
  mVisible=FALSE;
  mCurrentHeight=0;
  MaxHeight=dS->height();
  return TRUE;
}

void grConsole::putc(int c)
{
  if(mConsole!=NULL)
    mConsole->putc(c);

  if (c==10)
    tick();
}

void grConsole::clear(void)
{
  if(mConsole!=NULL)
    mConsole->clear();
}

void grConsole::tick(void)
{
  if(mVisible)
  {
    mCurrentHeight+=ShowSpeed;
    if(mCurrentHeight>=MaxHeight)
      mCurrentHeight=MaxHeight;
  }
  else
  {
    mCurrentHeight-=ShowSpeed;
    if(mCurrentHeight<=0)
    {
      mCurrentHeight=0;
      return;
    }
  }
  mCursorBlinkValue+=CursorBlinkSpeed;
  if(mSe!=NULL)
  {
    mEScreen->SetRefreshPoint(0,mCurrentHeight-mEScreen->height());
    mSe->tick();
    if(eLink)
      drawText(mEScreen,etColor);
  }
  else
    mDrawScreen->bar(0,0,mDrawScreen->width(),mCurrentHeight,bkColor);
  drawText(mDrawScreen,textColor);

  if (mDrawScreen->GetType() == ID_MSCREEN)
     mDrawScreen->refresh();
}

void grConsole::close(void)
{
  if(mESOwner)
    delete mEScreen;
  if(mFntOwner)
    free(mFnt);
  if(mConsole!=NULL)
    free(mConsole);
  if(mSe!=NULL)
    delete mSe;
  mFnt=NULL;
  mFntWidth=0;
  mDrawScreen=NULL;
  mConsole=NULL;
  mFntOwner=FALSE;
  mESOwner=FALSE;
  mEScreen=NULL;
  mSe=NULL;
  mCurrentHeight=0;
  mVisible=NULL;
}

void grConsole::drawText(Screen* scr,int color)
{
  if(mConsole==NULL || scr==NULL || mFnt==NULL)
    return;
  int y=scr==mDrawScreen?mCurrentHeight:scr->height();
  if(mCursorBlinkValue&0x80000000)
    scr->_outchar(mConsole->pos*mFntWidth,y-mFnt->height,'█',color,mFnt,&scr->GetViewPort());
  for(int i=mConsole->height;i && y>=0;i--)
  {
    y-=mFnt->height;
    char* cstr=mConsole->data+((mConsole->cline+i)%mConsole->height)*mConsole->width;
    int x=0;
    for(int j=0;j<mConsole->width;x+=mFntWidth)
      scr->_outchar(x,y,cstr[j++],color,mFnt,&scr->GetViewPort());
  }
}


int consoleBuf::overflow(int d)
{
  mGrc->putc(d);
  return d;
}

int consoleBuf::underflow(void)  // поведение по переполнению буфера чтения
{
  return EOF;
}

consoleBuf::~consoleBuf(void)
{
  streambuf::~streambuf();
}
