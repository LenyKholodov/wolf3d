#ifndef __SINGLE_GAME__
#define __SINGLE_GAME__

#include <mscreen.h>
#include <klip.h>

#include "level.h"

#include "g_object.h"
#include "g_view.h"
#include "g_input.h"

#include "g_system.h"

#include "cons.h"

////////////////////////////////////////////////////////////////////////////
//////Одноигровка
////////////////////////////////////////////////////////////////////////////
class   SingleGameClient;
class   SingleGame
{
  public:
                        SingleGame  (VESAdevice*,Screen*);
                        SingleGame  (VESAdevice*,Screen* dest,const char* levelName);
                        SingleGame  (VESAdevice*,Screen* dest,BinFile* saveGame);
            virtual     ~SingleGame ();

                enum IFACE_ID
                {
                  IF_SFX_VOLUME = 0,
                  IF_MUSIC_VOLUME,
                  IF_MUSIC_CHANNEL
                };

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
                void            Load (const char*);
                void            Save (const char*);
    virtual     void            Load (BinFile*)    ;
    virtual     void            Save (BinFile*)    ;
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return SINGLE_GAME; }
    virtual     void            dump (ostream&) const;

                ID              id   ()         const;
                ID              mainplayer ()   const { return player; }
                GameSystem*     system     ()   const  { return server; }

///////////////////////////////////////////////////////////////////////////
///////рабочая область
///////////////////////////////////////////////////////////////////////////
                        void    PlaySound   (int);
                        void    SfxVolume   (int);
                        void    MusicVolume (int);
        virtual         void    MainLoop  ();
                        void    PauseLoop ();
                        void    ErrorLoop ();

////////////////////////////////////////////////////////////////////////////
///////TEST
////////////////////////////////////////////////////////////////////////////
                void            happy_blood ();

////////////////////////////////////////////////////////////////////////////
//////Загрузка уровней
////////////////////////////////////////////////////////////////////////////
                        void    SendLoadMessage (const char*);
                        void    SendLevelName   (const char*);
                        void    LoadLevel       (const char*,int x,int y);

  protected:
                        void    CreateMainPlayer (const char*,int = 1,int = 1);
                        void    InitSound        ();
                        void    PlayKlip         (KlipPlayer*);

                        void    InitSystem       (const char*,int,int);
                        void    ReloadLevel      (const char*,int,int);

  protected:
             MidasSoundShell*   soundShell;
             Screen*            _screen;
             VESAdevice*        vesa;
             VideoSystem*       video;
             LoadTable*         loadTable;

             InputTranslator*   input;
             GameView*          view;
             GameSystem*        server;
             SingleGameClient*  client;

             Level*             level;

             ID                 player;
             BOOL               consLine;
             int                fragCount;

             BOOL               isContinue;
             BOOL               isPause;
             BOOL               isError;

             int                iFace [IFACE_COUNT];
};

#endif