#ifndef __GAME_SYSTEM__
#define __GAME_SYSTEM__

#include "g_types.h"
#include "g_object.h"
#include "g_pool.h"
#include "g_ttable.h"
#include "g_alist.h"
#include "g_mq.h"
#include "g_map.h"

#include "level.h"

class   AbstractSystem
{
  public:
       virtual      void          PutMessage  (GameMessage*,ID,int size,ID = ID(-1,-1)) = 0;
       virtual      void          PutMessage  (GameMessage* x,int size,ID cb = ID(-1,-1)) = 0;
       virtual      Map*          GetMap      () const = 0;
       virtual      BOOL          IsCross     (uint)   = 0;
       virtual      void          SetBlock    (uint,const Block&) = 0;
       virtual      Block*        GetBlock    (uint) = 0;
       virtual      void*         CreateObject (GameObjects) = 0;
       virtual      void*         CreateObject (ID) = 0;
       virtual      void          DisposeObject (ID) = 0;
       virtual      void          AddActive       (ID,int prioriry = 15) = 0;
       virtual      void          DelActive       (ID) = 0;
       virtual      void          ChangeLevel     (Level*) = 0;
};

/////////////////////////////////////////////////////////////////////////////
//////�����窠 ��ꥤ������ �� �᭮��� ����䥩�� �ࠢ����� ��⥬��
/////////////////////////////////////////////////////////////////////////////
class   GameServer;
class   GameSystem: public AbstractSystem
{
  public:
                                  GameSystem(Level* lev    = NULL,
                                             int poolSize  = MAX_OBJECTS,
                                             int queueSize = QUEUE_SIZE);
                                  GameSystem(Level* lev,BinFile* saveGame,
                                             int poolSize  = MAX_OBJECTS,
                                             int queueSize = QUEUE_SIZE);
               virtual            ~GameSystem ();

/////////////////////////////////////////////////////////////////////////////
//////���࠭���� � ����㧪� �ࢥ�
/////////////////////////////////////////////////////////////////////////////
    virtual         void          Load   (BinFile*,Level*);
    virtual         void          Save   (BinFile*);

/////////////////////////////////////////////////////////////////////////////
//////�������� � 㤠����� ��ꥪ⮢
/////////////////////////////////////////////////////////////////////////////
///////�㭪�� ��᮪��஢�����
/////////////////////////////////////////////////////////////////////////////
                    ID            CreateObject (GameObjects,ArgPtr,
                                                ID callBack = ID(-1,-1));
                    void          DeleteObject (ID);

/////////////////////////////////////////////////////////////////////////////
///////������஢���� �㭪樨 �ࠢ����� ��⥬��
/////////////////////////////////////////////////////////////////////////////
///////��������! ����㠫��� ⠡���� �㭪樨 �� ����������� (��⮬� � void*)
/////////////////////////////////////////////////////////////////////////////
///////make protected

      virtual       void*         CreateObject (GameObjects);
      virtual       void*         CreateObject (ID);
      virtual       void          DisposeObject (ID);

/////////////////////////////////////////////////////////////////////////////
//////����� � ��ꥪ��
/////////////////////////////////////////////////////////////////////////////
                    GameObject*   GetObject (ID);
                    ID            GetObject (uint mapIndex);
                    void          SetObject (uint,ID);
      virtual       GameObjects   GetType   (uint);
                    Ring*         GetRing   (GameObjects);

                    int           GetAttr   (ID);
                    void          SetAttr   (ID,int);
                    void          AddAttr   (ID,int);

/////////////////////////////////////////////////////////////////////////////
//////��㦥���
/////////////////////////////////////////////////////////////////////////////
                    ID            GetDispatch    ();

/////////////////////////////////////////////////////////////////////////////
//////����� � ���� (��஢�� ����)
/////////////////////////////////////////////////////////////////////////////
      virtual       void          SendMessage (uint index,GameMessage*,int size);

/////////////////////////////////////////////////////////////////////////////
//////�������� ���㦥���
/////////////////////////////////////////////////////////////////////////////
      virtual       void          CreateEnvironment ();

/////////////////////////////////////////////////////////////////////////////
//////����� � ���� (७��� ����)
/////////////////////////////////////////////////////////////////////////////
       virtual      BOOL           IsCross     (uint);
       virtual      void           SetBlock    (uint,const Block&);
       virtual      Block*         GetBlock    (uint);
       virtual      void           SetMap      (Map* map);
       virtual      Map*           GetMap      () const { return gameLevel->GetMap(); }
       virtual      LightMap*      GetLightMap () const { return gameLevel->GetLightMap(); }

/////////////////////////////////////////////////////////////////////////////
//////����� � ����ᠬ�
/////////////////////////////////////////////////////////////////////////////
       virtual      ResourceTable* GetResTable () const { return gameLevel->GetResourceTable(); }
                    int            GetRes      (const char* name)
                    { return GetResTable ()->GetIndex(name); }
                    NameCache*     GetCache    (int i) { return gameLevel->GetCache(i); }

                    ID             LoadObject  (int cacheId,int idRes,ArgPtr,int _size,ID owner);

/////////////////////////////////////////////////////////////////////////////
//////����� � ��⥬�� ᮮ�饭��
/////////////////////////////////////////////////////////////////////////////
       virtual      void          DispatchMessage (GameMessage*);
       virtual      void          DispatchAll();
       virtual      void          PutMessage  (GameMessage*,ID,int size,ID = ID(-1,-1));
       virtual      void          PutMessage  (GameMessage* x,int size,ID cb = ID(-1,-1))
                                  { PutMessage (x,GetDispatch(),size,cb); }

/////////////////////////////////////////////////////////////////////////////
//////����� � ��⥬�� �६����� �����⮢
/////////////////////////////////////////////////////////////////////////////
                    void          NextTick        ();   //��⨢�� �����
                    void          TickAll         ();
     virtual        void          AddActive       (ID,int prioriry = 15);
     virtual        void          DelActive       (ID);
                    void          ClearActive     ();

/////////////////////////////////////////////////////////////////////////////
//////����� � �����⠬�
/////////////////////////////////////////////////////////////////////////////
                    BOOL          AddClient       (ID);
                    void          DelClient       (ID);
                    void          Sequence        (GameMessage*,int size);
                    int           GetClientsCount () const { return clientCount; }

/////////////////////////////////////////////////////////////////////////////
//////������࠭���� ��㪠 � ��
/////////////////////////////////////////////////////////////////////////////
                    void          PutEcho         (int x,int y,int volume,ID obj);

/////////////////////////////////////////////////////////////////////////////
//////����㧪� ������ �஢�� � ���㧪�� ��ண�
/////////////////////////////////////////////////////////////////////////////
     virtual        void          ChangeLevel     (Level*);
                    void          UnloadLevel     ();

                        //out
            const   Level*        GetLevel        () const { return gameLevel;}

/////////////////////////////////////////////////////////////////////////////
//////������������ ࠡ�� ��஢�� ��⥬� � ��ਮ���᪮� ०���
/////////////////////////////////////////////////////////////////////////////
    virtual         void          tick ();
    virtual         void          Pause (ID,BOOL);

/////////////////////////////////////////////////////////////////////////////
//////Multiplayer
/////////////////////////////////////////////////////////////////////////////
                    void          CreateClient (const addrnode_t&);

  protected:
                    void          Close       ();

  protected:
                    GameServer*       gameServer;
                    GamePool*         gamePool;
                    TypeTable*        gameTypeTable;
                    ActiveList*       gameActiveList;
                    MainQueue*        gameMessageQueue;
                    GameMap*          gameMap;

                    Level*            gameLevel;

                    ID                clientMap  [MAX_CLIENTS];
                    int               clientCount;

                    BOOL              pauseState;
                    ID                reTrans;
};

/////////////////////////////////////////////////////////////////////////////
//////�᭮���� ����� �ࠢ����� ��ꥪ⠬� ᨤ�騩 �� ᮮ�饭��� (�� ���� �ࢥ�)
/////////////////////////////////////////////////////////////////////////////
class  GameServer: public GameObject
{
  public:
                                GameServer  (ArgPtr = NULL);
                                GameServer  (BinFile*);
            virtual             ~GameServer ();

////////////////////////////////////////////////////////////////////////////
//////�᭮��� ����ࠪ�� �㭪樨 ��� ��� ��஢�� ��ꥪ⮢
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     BOOL            Exec (GameMessage*);
    virtual     BOOL            ExecNet (NetPacket*);

    virtual     GameObjects     type () const  { return GAME_SERVER; }
    virtual     void            dump (ostream&) const;

};

/////////////////////////////////////////////////////////////////////////////
//////������� ���⨯����
/////////////////////////////////////////////////////////////////////////////
class   MultiGame;
class   GameClient: public AbstractSystem
{
  public:
                                  GameClient(MultiGame*,
                                             addrnode_t,
                                             Level* lev    = NULL,
                                             int poolSize  = MAX_OBJECTS);
       virtual                    ~GameClient ();

       virtual      void          PutMessage  (GameMessage*,ID,int size,ID = ID(-1,-1));
       virtual      void          PutMessage  (GameMessage* x,int size,ID cb = ID(-1,-1));

       virtual      Map*          GetMap      () const;
       virtual      BOOL          IsCross     (uint);
       virtual      void          SetBlock    (uint,const Block&);
       virtual      Block*        GetBlock    (uint);

                    ID            GetDispatch () { return clientId; }

       virtual      void*         CreateObject (GameObjects) ;
       virtual      void*         CreateObject (ID) ;
       virtual      void          DisposeObject (ID) ;

       virtual      void          ChangeLevel     (Level*) ;

       virtual      void          AddActive       (ID,int)  {}
       virtual      void          DelActive       (ID)  {}

  protected:
       addrnode_t    server;

       ID            clientId;
       Level*        gameLevel;
       GamePool*     gamePool;
       MultiGame*    game;
};

#endif