#ifndef __MULTI_CLIENT__
#define __MULTI_CLIENT__

#include "g_system.h"
#include "g_view.h"
#include "g_input.h"
#include "g_netmap.h"
#include "net.h"

/////////////////////////////////////////////////////////////////////////////
////Ретранслятор сообщений клиенту
/////////////////////////////////////////////////////////////////////////////
class     FarClient: public GameObject
{
  public:
                FarClient (ArgPtr);
                FarClient (BinFile* file) { Load (file); }
     virtual    ~FarClient ();

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*) {}
    virtual     void            Save (BinFile*) {}
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return MULTIPLAYER; }
    virtual     void            dump (ostream&) const;

                ID              player () const {return mainPlayer;}
                void            player (ID x) { mainPlayer = x;}

/////////////////////////////////////////////////////////////////////////////
///Happy Blood!
/////////////////////////////////////////////////////////////////////////////
                void            happy_blood ();

  protected:
     ID                 mainPlayer;
     Net*               net;
     NetMapIndex        netMap;
     clock_t            last;
     int                disconnectPos;
     clock_t            writeAnswer, readAnswer;
};

////////////////////////////////////////////////////////////////////////////
/////Игровая система клиента
////////////////////////////////////////////////////////////////////////////
class   MultiGame
{
  public:
                        MultiGame  (VESAdevice*,Screen*);
                        MultiGame  (addrnode_t&,ID farClient,ID player,VESAdevice*,Screen* dest,const char* levelName);
            virtual     ~MultiGame ();

                enum IFACE_ID
                {
                  IF_SFX_VOLUME = 0,
                  IF_MUSIC_VOLUME,
                  IF_MUSIC_CHANNEL
                };

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*)     {}
    virtual     void            Save (BinFile*)     {}
    virtual     BOOL            Exec (GameMessage*);

    virtual     GameObjects     type () const  { return MULTI_GAME; }
    virtual     void            dump (ostream&) const;

                ID              id   ()         const { return farClient; }
                ID              mainplayer ()   const { return player; }
                AbstractSystem* system     ()   const { return server; }

                void            MainLoop  ();

                void            Send      (GameMessage* mes,ID _id) { net->Send (mes,_id); }

////////////////////////////////////////////////////////////////////////////
//////Загрузка уровней
////////////////////////////////////////////////////////////////////////////
                        void    SendLoadMessage (const char*);
                        void    SendLevelName   (const char*);
                        void    LoadLevel       (const char*,int x,int y);

////////////////////////////////////////////////////////////////////////////
//////Работа со звуком
////////////////////////////////////////////////////////////////////////////
                        void    PlaySound   (int);
                        void    SfxVolume   (int);
                        void    MusicVolume (int);

  protected:
                        void    InitSound   ();
                        void    OutNet      ();
                        void    InNet       (addrnode_t,ID,ID,const char*);

  protected:
             MidasSoundShell*   soundShell;
             Screen*            _screen;
             VideoSystem*       video;
             Console*           console;
             LoadTable*         loadTable;

             InputTranslator*   input;
             GameView*          view;
             AbstractSystem*    server;
             Net*               net;

             Level*             level;

             ID                 player;
             ID                 farClient;
             BOOL               consLine;

             BOOL               isContinue;
             BOOL               isNet;
             BOOL               isDisconnect;

             int                iFace [IFACE_COUNT];
             clock_t            writeAnswer, readAnswer;
};

#endif