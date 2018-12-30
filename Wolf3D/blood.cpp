#include <console\bkeffect.h>

typedef struct
{
  int x,y,vx,vy,color;
} tMovePoint;

#define NUM_POINTS 1000

 const int Scale=10,
          MaxSpeed=Scale;

template <class mScrPtr> class Blood: public ScreenEffect
{
protected:
  tMovePoint p[NUM_POINTS];
  mScrPtr* mScr;
public:
  virtual void close(void);
  virtual BOOL IsValid(void);
  virtual ~Blood(void);
};

template <class mScrPtr> Blood<mScrPtr>::~Blood(void)
{
  close();
}

template <class mScrPtr> void Blood<mScrPtr>::close(void)
{
  mScr=NULL;
}

template <class mScrPtr> BOOL Blood<mScrPtr>::IsValid(void)
{
  return mScr!=NULL;
}

class Blood8bit: public Blood<mScreen8bit>
{
public:
  Blood8bit(mScreen8bit* scr);
  virtual void tick(void);
};

Blood8bit::Blood8bit(mScreen8bit* scr)
{
   if(scr!=NULL && scr->IsValid() && scr->bpp()==8)
   {
     (mScr=scr)->cls(192);
     int w=mScr->width(),
         h=mScr->height();
     for(int i=0;i<NUM_POINTS;i++)
     {
       p[i].x=random(w*Scale);
       p[i].y=random(h*Scale);
       do
       {
       	 p[i].vx=0;//MaxSpeed-random(2*MaxSpeed);
         p[i].vy=random(MaxSpeed);
       }
       while(p[i].vx==0 && p[i].vy==0);
       p[i].color=192+63;//128+random(128);
     }
     for(i=0;i<64;i++)
       mScr->PutPalette(192+i,i,0,0);
   }
   else
     mScr=NULL;    
}

void Blood8bit::tick(void)
{
  if(mScr==NULL)
    return;
  int w=mScr->width()*Scale,
      h=mScr->height()*Scale;
  for(int i=0;i<NUM_POINTS;i++)
  {
    int x=(p[i].x+=p[i].vx)/Scale,
        y=(p[i].y+=p[i].vy)/Scale;
    mScr->bar(x,y,x+2,y+2,p[i].color);
    if(p[i].x>w || p[i].y>h || p[i].x<0 || p[i].y<0)
    {
      p[i].x=random(w);
      p[i].y=random(h);
      do
      {
        p[i].vx=0;//MaxSpeed-random(2*MaxSpeed);
        p[i].vy=random(MaxSpeed);//MaxSpeed-random(2*MaxSpeed);
      }
      while(p[i].vx==0 && p[i].vy==0);
      p[i].color=192+63;
    }
  }  
  uchar* lfb=mScr->GetPtr();
  int ws=mScr->width();
  for(i=0;i<ws;i++)
    if(lfb[i]>192) lfb[i]--;
  for(int s=ws*(mScr->height()-1);i<s;i++)
    lfb[i]=long((lfb[i-ws]+lfb[i]+lfb[i-1]+lfb[i+1]))/4;
  mScr->refresh();
}


class Blood16bit: public Blood<mScreen16bit>
{
public:
  Blood16bit(mScreen16bit* scr);
  virtual void tick(void);
};

Blood16bit::Blood16bit(mScreen16bit* scr)
{
   if(scr!=NULL && scr->IsValid() && (scr->bpp()==16 || scr->bpp()==15))
   {
     mScr=scr;
     int w=mScr->width(),
         h=mScr->height();
     for(int i=0;i<NUM_POINTS;i++)
     {
       p[i].x=random(w*Scale);
       p[i].y=random(h*Scale);
       do
       {
       	 p[i].vx=0;//MaxSpeed-random(2*MaxSpeed);
         p[i].vy=random(MaxSpeed);
       }
       while(p[i].vx==0 && p[i].vy==0);
       p[i].color=mScr->rgb(255,0,0);
     }
   }
   else
     mScr=NULL;    
}

void Blood16bit::tick(void)
{
  if(mScr==NULL)
    return;
  int w=mScr->width()*Scale,
      h=mScr->height()*Scale;
  for(int i=0;i<NUM_POINTS;i++)
  {
    int x=(p[i].x+=p[i].vx)/Scale,
        y=(p[i].y+=p[i].vy)/Scale;
    mScr->bar(x,y,x+2,y+2,p[i].color);
    if(p[i].x>w || p[i].y>h || p[i].x<0 || p[i].y<0)
    {
      p[i].x=random(w);
      p[i].y=random(h);
      do
      {
        p[i].vx=0;//MaxSpeed-random(2*MaxSpeed);
        p[i].vy=random(MaxSpeed);//MaxSpeed-random(2*MaxSpeed);
      }
      while(p[i].vx==0 && p[i].vy==0);
      p[i].color=mScr->rgb(255,0,0);
    }
  }  
  uint16* lfb=mScr->GetPtr();
  int ws=mScr->width();
  for(i=0;i<ws;i++)
    if(lfb[i]&=0xF800) lfb[i]-=0x0100;
    for(int s=ws*(mScr->height()-1);i<s;i++)
      lfb[i]=long((lfb[i+ws]+lfb[i]+lfb[i+1]+lfb[i-1])>>2)&0xF800;
  mScr->refresh();
}

class Blood32bit: public Blood<mScreen32bit>
{
public:
  Blood32bit(mScreen32bit* scr);
  virtual void tick(void);
};

Blood32bit::Blood32bit(mScreen32bit* scr)
{
   if(scr!=NULL && scr->IsValid() && scr->bpp()==32)
   {
     mScr=scr;
     int w=mScr->width(),
         h=mScr->height();
     for(int i=0;i<NUM_POINTS;i++)
     {
       p[i].x=random(w*Scale);
       p[i].y=random(h*Scale);
       do
       {
       	 p[i].vx=0;//MaxSpeed-random(2*MaxSpeed);
         p[i].vy=random(MaxSpeed);
       }
       while(p[i].vx==0 && p[i].vy==0);
       p[i].color=mScr->rgb(255,0,0);
     }
   }
   else
     mScr=NULL;    
}

void Blood32bit::tick(void)
{
  if(mScr==NULL)
    return;
  int w=mScr->width()*Scale,
      h=mScr->height()*Scale;
  for(int i=0;i<NUM_POINTS;i++)
  {
    int x=(p[i].x+=p[i].vx)/Scale,
        y=(p[i].y+=p[i].vy)/Scale;
    mScr->bar(x,y,x+2,y+2,p[i].color);
    if(p[i].x>w || p[i].y>h || p[i].x<0 || p[i].y<0)
    {
      p[i].x=random(w);
      p[i].y=random(h);
      do
      {
        p[i].vx=0;//MaxSpeed-random(2*MaxSpeed);
        p[i].vy=random(MaxSpeed);//MaxSpeed-random(2*MaxSpeed);
      }
      while(p[i].vx==0 && p[i].vy==0);
      p[i].color=mScr->rgb(255,0,0);
    }
  }  
  ulong* lfb=mScr->GetPtr();
  int ws=mScr->width();
  for(i=0;i<ws;i++)
    if(lfb[i]&=0xFF0000) lfb[i]-=0x010000;
    for(int s=ws*(mScr->height()-1);i<s;i++)
      lfb[i]=((lfb[i-640]+lfb[i+640]+lfb[i+1]+lfb[i-1])>>2)&0xFF0000;
  mScr->refresh();
}

ScreenEffect* BloodEffect(Screen* scr)
{
  if(scr!=NULL && scr->IsValid())
    switch(scr->bpp())
    {
      case 8: return new Blood8bit((mScreen8bit*)scr);
      case 16: return new Blood16bit((mScreen16bit*)scr);
      case 24:
      case 32: return new Blood32bit((mScreen32bit*)scr);
    }
  return NULL;
}
