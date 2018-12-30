#include "g_scrfc.h"
#include "t_defs.h"

#include <debug/debug.h>

int     TransformCoord (int x_proc,int width)
{
  return width*x_proc/100;
}

/////////////////implementation of Screen Face Meneger///////////////////////


FaceNode*       ScreenFace::AddFace (FaceItem* item)
{
  FaceNode* temp = new FaceNode;

  temp->next = list;
  temp->item = item;
  list       = temp;

  return list;
}

void            ScreenFace::Send    (FaceNode* handler,int value)
{
  if (handler->item)    handler->item->Send (value);
}

void            ScreenFace::Send    (FaceNode* handler,const char* string)
{
  if (handler->item)    handler->item->Send (string);
}

void            ScreenFace::DelFace (FaceNode* handler)
{
  if (!list)
     return;

  for (FaceNode* node=list;node->next && node->next->item != handler->item;node=node->next);

  if (node->next && node->next->item == handler->item)
  {
    FaceNode*    temp = node->next;
    node->next        = node->next->next;

    if (temp->item) delete temp->item;

    delete temp;
  }
}

void            ScreenFace::Draw ()
{
  for (FaceNode* node=list;node;node=node->next)
      if (node->item)
         node->item->Draw (screen);
}

ScreenFace::ScreenFace (Screen* video)
           : screen (video), list(NULL)
{     }

ScreenFace::~ScreenFace ()
{
  for (FaceNode* node=list;node;)
  {
    FaceNode* temp = node;
    node           = temp->next;

    if (temp->item)
       delete temp->item;

    delete temp;
  }
}

/////////////////////////////implementation of class FaceText////////////////


void    FaceBinFont::Draw (Screen* screen) const
{
  if (font == NULL) return;

  int   xc,yc;

  switch (xJust)
  {
    default:
    case LEFT:   xc=x; break;
    case RIGHT:  xc=x-font->widthb;break;
    case CENTER: xc=x-(font->widthb>>1);break;
  }

  switch (yJust)
  {
    default:
    case LEFT:   yc=y; break;
    case RIGHT:  yc=y-font->height;break;
    case CENTER: yc=y-(font->height>>1);break;
  }

  screen->outtext (xc,yc,message_string,color,font);
}

void    FaceBinFont::Send (int value)
{
  if (value != FACE_NO_VALUE)
  {
    char  buf [128];

    itoa (value,buf,10);

    Send (buf);
  }
}

void    FaceBinFont::Send (const char* str)
{
  int len = strlen (str);

  if (len < 128)
    strcpy (message_string,str);
  else
  {
    memcpy (message_string,str,127);
    message_string [127] = 0;
  }
}

FaceBinFont::FaceBinFont (int _x,int _y,tBinFont* _font,Justify __x,Justify __y)
         : FaceItem (_x,_y,__x,__y), font (_font), color(255)
         {     }

/////////////////////////////implementation of class FaceText////////////////


void    FaceBGIFont::Draw (Screen* screen) const
{
  if (font == NULL) return;

  int   xc,yc;
  int   width  = font->width(message_string),
        height = font->height('A');

  switch (xJust)
  {
    default:
    case LEFT:   xc=x; break;
    case RIGHT:  xc=x-width;break;
    case CENTER: xc=x-(width>>1);break;
  }

  switch (yJust)
  {
    default:
    case LEFT:   yc=y; break;
    case RIGHT:  yc=y-height;break;
    case CENTER: yc=y-(height>>1);break;
  }

  screen->outtext (xc,yc,message_string,255,font);
}

void    FaceBGIFont::Send (int value)
{
  char  buf [128];

  itoa (value,buf,10);

  Send (buf);
}

void    FaceBGIFont::Send (const char* str)
{
  int len = strlen (str);

  if (len < 128)
    strcpy (message_string,str);
  else
  {
    memcpy (message_string,str,127);
    message_string [127] = 0;
  }
}

FaceBGIFont::FaceBGIFont (int _x,int _y,tBGIFont* _font,Justify __x,Justify __y)
         : FaceItem (_x,_y,__x,__y), font (_font)
         {     }

/////////////////////implementation of class FaceSprite///////////////////////

void    FaceSprite::Draw (Screen* screen) const
{
        //optimize
  tSprite* sprite = ((SpriteListCache*)cache)->GetData(index,pos);

  if (sprite == NULL)
     return;

  int   xc,yc;

  switch (xJust)
  {
    default:
    case LEFT:   xc=x; break;
    case RIGHT:  xc=x-sprite->width;break;
    case CENTER: xc=x-(sprite->width>>1);break;
  }

  switch (yJust)
  {
    default:
    case LEFT:   yc=y; break;
    case RIGHT:  yc=y-sprite->height;break;
    case CENTER: yc=y-(sprite->height>>1);break;
  }

  screen->PutSprite (xc,yc,sprite);
}

void    FaceSprite::Send (int _pos)
{
  pos = _pos < 0 ? 0 : _pos;
}

FaceSprite::FaceSprite (int _x,int _y,SpriteListCache* _cache,int _index,Justify __x,Justify __y)
          : FaceCache (_x,_y,_cache,_index,__x,__y), pos (0)
          {   }
