#ifndef __GAME_OBJECT__
#define __GAME_OBJECT__

#include <binfile.h>
#include <message.h>

#include "g_types.h"

class   GameSystem;
extern  GameSystem*      gameSystem;

enum GameObjectAttr
{
  OBJ_UNLOAD_PROTECT = 128
};

/////////////////////////////////////////////////////////////////////////////
//////Абстрактный класс для всех игровых объектов
/////////////////////////////////////////////////////////////////////////////
class   GameObject
{
  friend class GameSystem;
  friend class GameServer;
  friend class TypeTable;
  friend class GameClient;
  private:
             ID                 mID;     //Логическая ссылка на объект
  protected:
             GameSystem*        owner;     //Система - владелец
             int                mAttr;     //Атрибуты
             ID                 callBack;  //Объект - владелец (кто создал)

  public:
                                GameObject(ArgPtr = NULL);
                                GameObject(BinFile*);
                  virtual       ~GameObject() {}

         const  ID              id   ()           const { return mID;  }

////////////////////////////////////////////////////////////////////////////
//////Атрибуты объекта
////////////////////////////////////////////////////////////////////////////
         const  int             attr    ()    const { return mAttr; }
                void            SetAttr (int x)       { mAttr = x; }

////////////////////////////////////////////////////////////////////////////
//////Основные абстрактные функции для всех игровых объектов
////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     BOOL            Exec (GameMessage*)       = 0;

    virtual     GameObjects     type () const  { return NULL_OBJECT; }
    virtual     void            dump (ostream&) const;

////////////////////////////////////////////////////////////////////////////
//////Управление памятью
////////////////////////////////////////////////////////////////////////////
    void*           operator new (size_t,
                                  GameObjects,
                                  ID callBack = ID(-1,-1),
                                  GameSystem* owner = gameSystem);

    void*           operator new (size_t,
                                  ID,
                                  ID callBack = ID(-1,-1),
                                  GameSystem* owner = gameSystem);

    void            operator delete (void*);
};

#endif