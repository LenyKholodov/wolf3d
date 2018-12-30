#include "lscreen.h"

#include <debug/debug.h>
#include <string.h>

void      LoadTable::SetMessage (int i,const char* name,const char* file,int color)
{
  char  dest [256];

  strcpy (dest,name);
  strcat (dest,file);

  SetMessage (i,dest,color);
}

void      LoadTable::SetMessage (int item,const char* message,int color)
{
  Flag flag ("SET_MESSAGE");

  if (item > MAX_LOAD_MESSAGES || item < 0)
     return;

  if (bkImage [item])
  {
    int width     = font->width(message);
    int fntHeight = FONT_STRING_HEIGHT;

    screen->PutImage (0,point[item].y,bkImage [item]);
    screen->outtext  (point[item].x-(width>>1),point[item].y+fntHeight/1.5,
                      message,color,font);
  }
  else
  {
    int width  = font->width(message);

    int fntHeight  = FONT_STRING_HEIGHT;
    bkImage [item] = (tImage*)malloc (tImage::size (screen->width(),fntHeight,screen->bpp()));

    screen->GetImage (0,point[item].y,screen->width(),fntHeight,bkImage [item]);
    screen->outtext  (point[item].x-(width>>1),point[item].y+fntHeight/1.5,
                      message,color,font);

    bkImage[item]->width   = screen->width();
    bkImage[item]->height  = fntHeight;
  }
}

void      LoadTable::freeze  ()
{
  if (freezeImage)
     free (freezeImage);

  freezeImage = (tImage*)malloc (tImage::size (screen->width(),screen->height(),screen->bpp()));

  if (freezeImage)
  {
    screen->GetImage (0,0,screen->width(),screen->height(),freezeImage);

    freezeImage->width   = screen->width();
    freezeImage->height  = screen->height();
  }
}

void      LoadTable::refresh (Screen* scr,tPoint* p)
{
  if (scr == NULL)
     return;

  screen = scr;

  float   xscale = (float)screen->width()/100.0,
          yscale = (float)screen->height()/100.0;

  for (int i=0;i<MAX_LOAD_MESSAGES;i++)
  {
    point   [i].x  = p [i].x * xscale;
    point   [i].y  = p [i].y * yscale;
  }

  if (freezeImage)
  {
     int x = (screen->width()-freezeImage->width)/2,
         y = (screen->height()-freezeImage->height)/2;

     screen->PutImage (x,y,freezeImage);
  }
}

LoadTable::LoadTable (Screen* scr,tPoint* p,const char* fontName,const char* bkName)
          : font (NULL), screen(scr), freezeImage (NULL)
{
  if (p == NULL || screen == NULL)
  {
    dout<<"Load table: Error create load table"<<endl;
    return;
  }

  if (fontName)
     font = tBGIFont::load (fontName);

  if (bkName)
  {
    cPicture picture;

    if (picture.load (bkName))
    {
      tImage* image = picture.GetImage ();

      if (image && screen)
      {
        int x = (screen->width()-image->width)/2,
            y = (screen->height()-image->height)/2;

        screen->PutImage (x,y,image);
      }
    }
    else
      dout<<"Load table: File not found '"<<bkName<<"'"<<endl;
  }

  float   xscale = (float)screen->width()/100.0,
          yscale = (float)screen->height()/100.0;

  for (int i=0;i<MAX_LOAD_MESSAGES;i++)
  {
    bkImage [i]    = NULL;
    point   [i].x  = p [i].x * xscale;
    point   [i].y  = p [i].y * yscale;
  }
}

LoadTable::~LoadTable ()
{
  if (font) free (font);
  if (freezeImage) free (freezeImage);

  for (int i=0;i<MAX_LOAD_MESSAGES;i++)
      if (bkImage [i])
         free (bkImage [i]);
}