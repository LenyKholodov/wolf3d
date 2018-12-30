#ifndef __LOAD_SCREEN__
#define __LOAD_SCREEN__

#include <video.h>

const int               MAX_LOAD_MESSAGES  = 16;
const int               FONT_STRING_HEIGHT = 45;

////////////////////////////////////////////////////////////////////////////
///Загрузочная таблица
////////////////////////////////////////////////////////////////////////////
class   LoadTable
{
  public:
                LoadTable (Screen*,tPoint*,const char* font = NULL,const char* bkName = NULL);
      virtual   ~LoadTable ();

                void      SetMessage (int,const char*,int color = 255);
                void      SetMessage (int,const char*,const char*,int color = 255);

                void      freeze  ();
                void      refresh (Screen*,tPoint*);

  protected:
             Screen*            screen;
             tPoint             point   [MAX_LOAD_MESSAGES] ;
             tImage*            bkImage [MAX_LOAD_MESSAGES];
             tImage*            freezeImage;
             tBGIFont*          font;
};

#endif