#ifndef __TYPE_TABLE__
#define __TYPE_TABLE__

#include "g_object.h"
#include "g_pool.h"

#pragma pack(1)

struct TypeTableNode
{
  PoolIndex     data;
  int16         next;
};

#pragma pack(0)

/////////////////////////////////////////////////////////////////////////////
/////Таблица типов для всех игровых объектов
/////////////////////////////////////////////////////////////////////////////
class   Ring;
class   TypeTable: public GameObject
{
  friend class GameSystem;
  public:
                        TypeTable  ();
             virtual    ~TypeTable ();

/////////////////////////////////////////////////////////////////////////////
/////Загрузка - сохранение - обработка сообщений
/////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     BOOL            Exec (GameMessage*)   { return FALSE; }
                //Exec для группы элементов нужен в потомке

    virtual     GameObjects     type ()            const { return TYPE_TABLE; }
    virtual     void            dump (ostream&)    const;

/////////////////////////////////////////////////////////////////////////////
/////Подключение к таблице
/////////////////////////////////////////////////////////////////////////////
                ID              Include (GameObjects,PoolIndex);
                void            Exclude (ID);
                void            ClearAll ();

/////////////////////////////////////////////////////////////////////////////
/////Доступ к объекту
/////////////////////////////////////////////////////////////////////////////
                PoolIndex       GetObject    (ID);
                Ring*           GetRing      (GameObjects);

/////////////////////////////////////////////////////////////////////////////
/////Управление памятью
/////////////////////////////////////////////////////////////////////////////
                void*           operator new    (size_t,GameSystem*);
                void            operator delete (void* x) { ::operator delete (x); }

  protected:
                ID              Include         (int,PoolIndex);
                int             FindClearType   ();
                int             RegisterNewType (GameObjects);
                void            SetPoolIndex    (ID,PoolIndex);

  protected:
                TypeTableNode   mTable [MAX_TABLES][MAX_SLOTS];
                int16           mSize  [MAX_TABLES];
                int16           mNext  [MAX_TABLES];
                int16           mTypes [MAX_TYPES];                
};

//extern TypeTable*        gameTypeTable;

#endif