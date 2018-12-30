#ifndef __GAME_VIEW__
#define __GAME_VIEW__

#include "g_types.h"
#include "g_object.h"

#include "render.h"
#include "level.h"
#include "g_scrfc.h"

#include "cons.h"
#include "fin.h"

#include <time.h>

/////////////////////////////////////////////////////////////////////////////
////Видео система
/////////////////////////////////////////////////////////////////////////////
class   VideoSystem
{
  public:
                    VideoSystem (Level*,Screen*);
          virtual   ~VideoSystem ();

/////////////////////////////////////////////////////////////////////////////
///Обновление параметров
/////////////////////////////////////////////////////////////////////////////
          void      refresh (Level*);
          void      refresh (Screen*);
          void      picmip  (int);

          void      Send    (int,int);
          void      Send    (int,const char*);
          void      Reload  (int,int);
          void      MoveTo  (int,int,int);
          void      MoveRel (int,int,int);
          void      MoveStart(int);

          void      putc        (char x,int key)  { console->putc(x,key); }
          void      OnlyConsole (BOOL);

/////////////////////////////////////////////////////////////////////////////
///Отрисовка
/////////////////////////////////////////////////////////////////////////////
          void      DrawView       (Fixed,Fixed,Angle);

  protected:
          void      InitFaceSystem (Level*);

  protected:
                Screen*           screen;         //double buffer
                Render*           render;
                Console*          console;        //console
                tBinFont*         consoleFont;

                Level*            level;          //ptr for Reload

                ScreenFace*       face;
                FaceNode*         items[MAX_ITEMS];

                int               mPicMip;
                BOOL              mConsole;
};

/////////////////////////////////////////////////////////////////////////////
///////Виртуальный просмотровщик
/////////////////////////////////////////////////////////////////////////////
class     GameView: public GameObject
{
  public:
                                GameView (VideoSystem*,ID);
                                GameView (ArgPtr);
                                GameView (BinFile*);
              virtual           ~GameView ();

                int             GetFPS () volatile;

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*) ;
    virtual     void            Save (BinFile*) ;
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return GAME_VIEW; }
    virtual     void            dump (ostream&) const;

                ID              player () const { return viewPlayer; }
                void            player (ID x)   { viewPlayer = x;}

    virtual     void            Redraw();
                Fixed           getx    () const  { return state.x; }
                Fixed           gety    () const  { return state.y; }
                Angle           getangle() const  { return state.angle; }

  protected:
                VideoSystem*      video;

                int               curPrior;
                ID                viewPlayer;     //Игрок с котрым связан рендер
                PlayerInfoMessage state;
};

#endif