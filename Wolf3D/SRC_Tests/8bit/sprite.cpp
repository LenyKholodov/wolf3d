#include "sprite.h"
#include "render.h"

#include <debug\debug.h>

#include <math.h>
#include <tables.h>
#include <string.h>

#include <sbinfile.h>

/////////////////////////Make-functions for all objects//////////////////////

SimpleSprite*    makeSimpleSprite (tImage* image,int z,int bpp)
{
  return    new (SimpleSprite::size(image->width,image->height,bpp))
            SimpleSprite (image,z);
}

Sprite3D*        makeSprite3D  ( tImageList* list,int z )
{
  return    new (Sprite3D::size(list))
            Sprite3D (list,z);
}

Sprite3D*        makeSprite3D     ( SimpleSprite** sprite,int num,int bpp)
{
  return    new (Sprite3D::size(sprite,num,bpp))
            Sprite3D (sprite,num,bpp);
}

GameSprite3D*    makeGameSprite3D ( Sprite3D** sprite,int num)
{
  return    new (GameSprite3D::size(sprite,num))
            GameSprite3D (sprite,num);
}

GameSprite3D*    makeGameSprite3D ( BinFile* file )
{
  file->reset();

  WSFHeader wsf;

  file->read(&wsf,sizeof(WSFHeader));

  if (memcmp(&wsf.id,"WSF ",4))
  {
     dout<<"Game sprite 3D: Wrong file format"<<endl;
     return NULL;
  }

  GameSprite3D*  sprite = new (wsf.total_size)
                          GameSprite3D (file,wsf.total_size);

  return sprite;
}

GameSprite3D*    makeGameSprite3D (const char* path)
{
  stdBinFile    file;

  if (!file.open(path,BinFile::OPEN_RO))
  {
     dout<<"Game sprite 3D: Error open file"<<endl;
     return NULL;
  }

  GameSprite3D* sprite = makeGameSprite3D (&file);

  file.close();

  return sprite;
}

/////////////////////Cache make-functions////////////////////////////////////

        //Предролагается, что  member NUM первый в списке переменных класса
Sprite3D*       makeSprite3D (BinFile* file,int face,void* dest)
{
  file->reset();

//  WSFHeader wsf;

//  file->read(&wsf,sizeof(WSFHeader));

//  if (memcmp(&wsf.id,"WSF ",4))
//  {
//     dout<<"Game sprite 3D: Wrong file format"<<endl;
//     return NULL;
//  }

  file->seek(sizeof(WSFHeader));

  ulong          num;
  ulong          offs [MAX_SPRITE_FACES];

  file->read (&num,sizeof(int));

  if (face >= num) face = num-1;

  file->read (offs,sizeof(ulong)*num);
  file->seek (sizeof(WSFHeader)+sizeof(GameSprite3D)-sizeof(uchar)+
              offs[face]);

  offs[num]         = file->length();

  Sprite3D*  sprite = (Sprite3D*)dest;

//  if (dest == NULL)   dest  = new uchar [offs[face+1]-offs[face]];

  file->read (dest,offs[face+1]-offs[face]);

  return sprite;
}

Sprite3D*       makeSprite3D (const char* name,int face,void* dest)
{
  stdBinFile    file;

  if (!file.open(name,BinFile::OPEN_RO))
  {
     dout<<"Game sprite 3D: Error open file"<<endl;
     return NULL;
  }

  Sprite3D* sprite = makeSprite3D (&file,face,dest);

  file.close();

  return sprite;
}

/////////////////////implementation of class SimpleSprite////////////////////

SimpleSprite::SimpleSprite(tImage* image,int z):
   width(image->width),height(image->height),z_color(z)
{
  ulong*        offsets = (ulong*)data;

  for (int x=0;x<width;x++)
  {
      uchar* filePos = x+image->data;
      int pos        = offsets[x] = x*height+width*sizeof(ulong);

      for (int y=0;y<height;y++,filePos+=width)
          data[pos++] = *filePos;
  }
}

long   SimpleSprite::size (int bpp) const
{
  return width*height*bpp+sizeof(SimpleSprite)-sizeof(uchar)+
         width*sizeof(ulong);
}

long   SimpleSprite::size (int width,int height,int bpp)
{
  return width*height*bpp+sizeof(SimpleSprite)-sizeof(uchar)+
         width*sizeof(ulong);
}

void*   SimpleSprite::operator new (size_t,size_t bytes)
{
  return ::operator new (bytes);
}


//////////////////implementation of class Sprite3D///////////////////////////
        //no test
Sprite3D::Sprite3D (tImageList* list,int z):num(list->count),bpp(list->bpp/8)
{
  ulong*        offset = (ulong*)data;
  ulong         pos    = num*sizeof(ulong);
  SimpleSprite* sprite;

  for (int i=0;i<num;i++)
  {
    sprite    = new(data+pos) SimpleSprite(list->GetImage(i),z);
    pos      += sprite->size(bpp);
    offset[i] = pos;
  }
}

Sprite3D::Sprite3D (SimpleSprite** sprite,int n,int _bpp):num(n),bpp(_bpp)
{
  ulong*        offset = (ulong*)data;
  ulong         pos    = num*sizeof(ulong);

  for (int i=0;i<num;pos+=sprite[i]->size(bpp),i++)
  {
    memcpy(data+pos,sprite[i],sprite[i]->size(bpp)); //sprite[i]->data
    offset[i] = pos;
  }
}

long  Sprite3D::size (tImageList* list)
{
  ulong sz = sizeof(Sprite3D)-sizeof(uchar)+list->count*sizeof(ulong);

  for (int i=0;i<list->count;i++)
      sz += SimpleSprite::size(list->GetImage(i)->width,
                               list->GetImage(i)->height,
                               list->bpp/8);

  return sz;
}

long  Sprite3D::size (SimpleSprite** list,int num,int bpp)
{
  ulong sz = sizeof(Sprite3D)-sizeof(uchar)+num*sizeof(ulong);

  for (int i=0;i<num;sz+=list[i]->size(bpp),i++);

  return sz;
}

long  Sprite3D::size () const
{
  ulong sz = sizeof(Sprite3D)-sizeof(uchar)+num*sizeof(ulong);

  SimpleSprite*   ss     = (*this)[0];

  for (int i=0;i<num;i++,sz+=ss->size(bpp))
      ss = (*this)[i];

  return sz;
}

void*   Sprite3D::operator new (size_t,size_t bytes)
{
  return ::operator new (bytes);
}

//////////////////implementation of class GameSprite3D///////////////////////

GameSprite3D::GameSprite3D (Sprite3D** sprite,int n): num(n)
{
  ulong*        offset = (ulong*)data;
  ulong         pos    = num*sizeof(ulong);

  for (int i=0;i<num;pos+=sprite[i]->size(),i++)
  {
    memcpy(data+pos,sprite[i],sprite[i]->size());
    offset[i] = pos;
  }
}

GameSprite3D::GameSprite3D (BinFile* file,long size)
                           { file->read(&num,size); }

long  GameSprite3D::size () const
{
  ulong sz = sizeof(GameSprite3D)-sizeof(uchar)+num*sizeof(ulong);

  Sprite3D*       s3D    = (*this)[0];

  for (int i=0;i<num;i++,sz+=s3D->size())
      s3D  = (*this)[i];

  return sz;
}

long   GameSprite3D::size (Sprite3D** list,int num)
{
  ulong sz = sizeof(GameSprite3D)-sizeof(uchar)+num*sizeof(ulong);

  for (int i=0;i<num;sz+=list[i]->size(),i++);

  return sz;
}

void*   GameSprite3D::operator new (size_t,size_t bytes)
{
  return ::operator new (bytes);
}

void    GameSprite3D::Save (BinFile* file)
{
  file->reset();

  WSFHeader     wsf;

  memcpy(&wsf.id,"WSF ",4);

  wsf.num             = num;
  wsf.total_size      = size();

  file->write (&wsf,sizeof(WSFHeader));
  file->write (&num,wsf.total_size);
}

void    GameSprite3D::Save (const char* path)
{
  stdBinFile    file;

  if (!file.open(path,BinFile::OPEN_CR))
     return;

  Save (&file);

  file.close();
}

//////////////////implementation of class RenderSprite///////////////////////

        //Максимальное расстояние > 64, но < 128
RenderSprite::RenderSprite (Screen* scr,SpriteCache* _cache,Fixed* zbuf):
       zbuffer(zbuf),screenWidth(scr->width()),screenHeight(scr->height()),
       xscale(NULL), yscale(NULL), distMask(0xFFFF), distShift(14), //8
       screen(scr), cache(_cache)
{
  SetParam(FOCUS,NORM_FOCUS);
  SetParam(VIEW_ANGLE,NORM_VIEW_ANGLE);
  SetParam(WALL_HEIGHT,NORM_WALL_HEIGHT);

  RenderSprite::Reset();
}

RenderSprite::~RenderSprite()
{
  if (xscale) delete xscale;
  if (yscale) delete yscale;
}

void    RenderSprite::DrawSprites (SpriteList* list)
{
        Flag    flag ("DRAW_SPRIES");

        VisSprite*  sprite = NULL;

        while (list->GetCount())
        {
            sprite = list->GetSprite ();

            if (sprite->block)
               sprite->block->id.sprite.already = 0;
            if (sprite->dist < FX_HALF)
               continue;

            DrawSprite (*sprite);
        }
}

void    RenderSprite::Reset()
{
  if (xscale) delete xscale;
  if (yscale) delete yscale;

  int tableSize = pow(2,32-distShift);
  distMask      = tableSize - 1;

  xscale        = new Fixed [tableSize];
  yscale        = new Fixed [tableSize];

  Fixed      dist;
  double     d;

  for (long ij=0;ij<tableSize;ij++)
  {
   dist = ij<<distShift;
   d   = fixed2float(dist);

   double   dy    = d/(focus*(float)screenHeight);
   yscale   [ij]  = float2fixed(dy);

   double   dx    = d/(viewAngle*(float)screenWidth);
   xscale   [ij]  = float2fixed(dx);
  }
}

void    RenderSprite::SetParam(Param parm,double value)
{
  switch(parm)
  {
    case FOCUS:      focus = value;     break;
    case VIEW_ANGLE: viewAngle = value; break;
    case WALL_HEIGHT:wallHeight = value; break;
    default:                            break;
  }
}

double    RenderSprite::GetParam(Param parm)
{
  switch (parm)
  {
    case FOCUS:         return focus;
    case VIEW_ANGLE:    return viewAngle;
    case WALL_HEIGHT:   return wallHeight;
    default:            return -1;
  }
}

//////////////////implementation of class RenderSprite8bit///////////////////

RenderSprite8bit::RenderSprite8bit(fScreen8bit* scr,SpriteCache* _cache,Fixed* zbuf,int y):
      RenderSprite(scr,_cache,zbuf), frame(scr->GetPtr()), centerY(y)
{
  if (scr->bpp()!=8)
     D_Error ("Render sprite 8-bit: Wrong video mode")

  if (centerY == -1)
     centerY = scr->height()/2;
}

void    RenderSprite8bit::Reset()
{
  RenderSprite::Reset();

  centerY = (float)screenHeight/wallHeight;
}

#pragma pack (1)
union FxInt
{
  Fixed         _fx;
  struct        _Int
  {
    uint16      frac;
    uint16      integer;
  }_int;
};
#pragma pack (0)

void    RenderSprite8bit::DrawSprite (VisSprite& sprite)
{
        SimpleSprite* ssprite = (*cache->GetData(sprite.id,
                                                sprite.face))
                                                [sprite.angle];

#ifndef NDEBUG
        if (ssprite == NULL || sprite.block == NULL)
           return ;
#endif
                //real sprite width & height
        uFixed  spr_width  = int2fixed(ssprite->width),
                spr_height = int2fixed(ssprite->height);

                //calc sprite x & y steps
        uFixed  xstep  = xscale[(sprite.dist>>distShift) & distMask],
                ystep  = yscale[(sprite.dist>>distShift) & distMask];

                //scale them
        xstep          = mul(xstep,spr_width);
        ystep          = mul(ystep,spr_height);

                //sprite scale coords
        FxInt     sprX, sprY;

        sprX._fx       = 0;
        sprY._fx       = 0;

        uFixed  startY = 0;

        Fixed   dist   = sprite.dist;

                //screen params
        int     width, height;

        width  = fixed2int( div((Fixed)spr_width,xstep+FX_VERY_LOW) );
        height = fixed2int( div((Fixed)spr_height,ystep+FX_VERY_LOW) );

        static Fixed correctTable [3] = {FX_ONE<<1,FX_ONE,float2fixed(0.4)};

        int     x      = sprite.xstart-(width>>1),
                y      = centerY-fixed2int(mul(fixed2int(height>>1),correctTable [sprite.block->id.sprite.z]));
        int     z_color= ssprite->z_color;

        if (x<0)
        {
           if (x+width<0)
              return ;

           Fixed fxX = -int2fixed(x);
           sprX._fx  = mul(fxX,xstep);
           width    += x;
           x         = 0;
        }
        if (y<0)
        {
           if (y+height<0)
              return ;

           Fixed fxY = -int2fixed(y);
           startY    = mul(fxY,ystep);
           height   += y;
           y         = 0;
        }
        if (y+height>screenHeight)
           height = screenHeight - y;
        if (x+width>screenWidth)
           width = screenWidth - x;

                //sprite setings
        ulong*  offset = ssprite->offs();
        uchar*  image  = ssprite->data;
        uchar*  buffer = frame + y*screenWidth+x;
        uchar   *data,
                *dest;
        int     color;

        for (int i=0;i<width;i++,sprX._fx+=xstep,buffer++,x++)
        {
          if (zbuffer[x]<dist)
             continue;

                //Вынести стековые операции
          data  = image  + offset [sprX._int.integer];
          dest  = buffer;

          sprY._fx        = startY;

          for (int j=0;j<height;j++,sprY._fx+=ystep,dest+=screenWidth)
            if ((color = data[sprY._int.integer]) != z_color)
              *dest = color;
        }
}

/////////////////implementation of class SpriteList//////////////////////////

void    SpriteList::SortSprites ()
{
  qsort(node,count+1,sizeof(VisSprite*),CmpSprites);
}

int     SpriteList::CmpSprites ( const void* v1, const void* v2)
{
  VisSprite* s1     = *((VisSprite**)v1);
  VisSprite* s2     = *((VisSprite**)v2);

  if      (s1->dist < s2->dist)   return  -1;
  else if (s1->dist > s2->dist)   return   1;
  else                            return   0;
}

void    SpriteList::SetMaxCount(int c)
{
  if (node) delete node;
  node  = new VisSprite* [maxCount=c];
  count = -1;
}

BOOL    SpriteList::PutSprite (VisSprite* x)
{
#ifndef NDEBUG
  if (count==maxCount || x==NULL)
     return FALSE;
#endif

  node[++count] = x;

  return TRUE;
}

VisSprite*      SpriteList::GetSprite ()
{
#ifndef NDEBUG
  if (count<0)
     return NULL;
#endif

  return node [count--];
}

SpriteList::SpriteList(int max_count): node (new VisSprite* [maxCount=max_count]), count(-1)
                           {   }

SpriteList::~SpriteList()
{
  delete node;
}

////////////////////////implementation of class SpriteSystem/////////////////

VisSprite*     SpriteSystem::ProjectSprite (Fixed spritex,Fixed spritey,Block* block) const
{
  Flag  flag ("PROJECT_SPRITE");

  Fixed  dx   = spritex - viewx;
  Fixed  dy   = spritey - viewy;

  Fixed locY  = mul(dx,viewcos)+mul(dy,viewsin);
  Fixed locX  = mul(dx,viewsin)-mul(dy,viewcos);        //focus
        locY  = abs(locY) > FX_VERY_LOW ? locY : mul(FX_VERY_LOW<<1,arsign(locY));

  Fixed xscale = mul(FX_ONE<<1,div(proj_x,locY+FX_VERY_LOW));
  int   xstart = fixed2int( proj_x-div (mul(locX,xscale),vangleFx) );

  Fixed  dist  = locY;

  VisSprite*    vSprite = new(&sprites[list->GetCount()]) VisSprite;

  vSprite->block        = block;
  vSprite->dist         = dist;
  vSprite->xstart       = xstart;

  return vSprite;
}

void    SpriteSystem::ProcessSprite (Fixed x,Fixed y,Angle cur,Block* block)
{
  Flag  flag ("PROCESS_SPRITE");
#ifndef NDEBUG
  if (block )
  {
#endif
      if (block->id.sprite.id == NOSPRITE || block->id.sprite.face==NOFACE)
         return;

      Fixed spritex = x + (block->id.sprite.xfrac<<12);
      Fixed spritey = y + (block->id.sprite.yfrac<<12);

      int   alpha   = (-3 - block->id.sprite.alpha + (cur >> 13)) & 7;

      VisSprite*   sprite = ProjectSprite (spritex,spritey,block);

      sprite->id          = block->id.sprite.id;
      sprite->face        = block->id.sprite.face;
      sprite->angle       = alpha;

      list->PutSprite (sprite);
#ifndef NDEBUG
  }
#endif
}

void    SpriteSystem::SetCache (SpriteCache* x)
{
  if (render)
     render->SetCache (x);
}

void    SpriteSystem::Reset()
{
  RenderPtr()->Reset();

  if (sprites)   delete sprites;

  sprites  = new VisSprite [list->GetMaxCount()];
  proj_x   = int2fixed( RenderPtr()->ScreenPtr()->width()/2 );
  vangleFx = float2fixed(RenderPtr()->GetParam(RenderSprite::VIEW_ANGLE));
}

void    SpriteSystem::Reset (Fixed x,Fixed y,Angle angle)
{
  viewcos    = cos (angle);
  viewsin    = sin (angle);
  viewx      = x;
  viewy      = y;

  list->Reset();
}

void    SpriteSystem::DrawView ()
{
  list->SortSprites();
  render->DrawView(list);
}

SpriteSystem::SpriteSystem (RenderSprite* rend,SpriteList* sprList,BOOL del)
       :deleteAccess (del), render(rend), list(sprList),
        sprites(new VisSprite [list->GetMaxCount()]),
        proj_x ( int2fixed( rend->ScreenPtr()->width()/2 ) ),
        vangleFx (float2fixed(RenderPtr()->GetParam(RenderSprite::VIEW_ANGLE)))
{ Reset(0,0,0);  }

SpriteSystem::~SpriteSystem()
{
  dout<<"Sprite system: Destruct system";
  delete sprites;
  if (deleteAccess)
  {
    delete render;
    delete list;
  }
  dout<<"OK"<<endl;
}

////////////////////implementation of class SpriteCache /////////////////////

void          SpriteCache::LoadData(long id,CacheList* dest)
{
  int _id   = index[id];
  int _face = id-idOffs[_id];

  makeSprite3D (GetName (_id),_face,(void*)dest->data);
}

int    SpriteCache::Research (const char* _name)
{
  stdBinFile*    file = new stdBinFile (_name,BinFile::OPEN_RO);

  file->reset();

  WSFHeader wsf;

  file->read(&wsf,sizeof(WSFHeader));

  if (memcmp(&wsf.id,"WSF ",4))
  {
     dout<<"Sprite cache: Wrong file format "<<_name<<endl;
     return 0;
  }

  ulong          num;

  file->read (&num,sizeof(int));
  delete file;

  return num;
}

void              SpriteCache::Recalc ()
{
  if (idOffs)   delete idOffs;
  if (index)    delete index;

  if (GetNamesNum() > 8192 )
  {
    dout<<"Sprite cache: Too large!"<<endl;
    return;
  }

  idOffs = new int [GetNamesNum ()];

  int   len [8192];     //max posible
  int   pos    = 0;

  for (int i=0;i<GetNamesNum();i++)
  {
      idOffs[i] = pos;
      len[i]    = Research(GetName(i));
      pos      += len [i];
  }

  Cache::Open (pos,GetCacheSize(),GetClasterSize());

  index  = new int [GetNum()];

  for (i=0,pos=0;i<GetNamesNum();pos+=len[i],i++)
      for (int j=0;j<len[i];index[pos+j] = i,j++);
}

void              SpriteCache::Close       ()
{
  NameCache::Close();

  if (idOffs)   delete idOffs;
  if (index)    delete index;

  idOffs = NULL;
  index  = NULL;
}

Sprite3D*         SpriteCache::GetData (int id,int face)
{
  return (Sprite3D*)Cache::GetData (idOffs[id]+face);
}

SpriteCache::SpriteCache (char* name[],long id_num,long cache_sz,long cl_sz)
            : idOffs(NULL), index(NULL),
              NameCache(name,id_num,id_num,cache_sz,cl_sz)
{     }

SpriteCache::SpriteCache (BinFile* file,long cache_sz)
            : idOffs(NULL), index(NULL),
              NameCache (file,cache_sz)
{
  Recalc ();
}

SpriteCache::SpriteCache (const char* name,long cache_sz)
             : idOffs(NULL), index(NULL),
              NameCache (name,cache_sz)
{
  Recalc ();
}

