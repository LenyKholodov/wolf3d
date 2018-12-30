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
/////������ ⨯�� ��� ��� ��஢�� ��ꥪ⮢
/////////////////////////////////////////////////////////////////////////////
class   Ring;
class   TypeTable: public GameObject
{
  friend class GameSystem;
  public:
                        TypeTable  ();
             virtual    ~TypeTable ();

/////////////////////////////////////////////////////////////////////////////
/////����㧪� - ��࠭���� - ��ࠡ�⪠ ᮮ�饭��
/////////////////////////////////////////////////////////////////////////////
    virtual     void            Load (BinFile*);
    virtual     void            Save (BinFile*);
    virtual     BOOL            Exec (GameMessage*)   { return FALSE; }
                //Exec ��� ��㯯� ����⮢ �㦥� � ��⮬��

    virtual     GameObjects     type ()            const { return TYPE_TABLE; }
    virtual     void            dump (ostream&)    const;

/////////////////////////////////////////////////////////////////////////////
/////������祭�� � ⠡���
/////////////////////////////////////////////////////////////////////////////
                ID              Include (GameObjects,PoolIndex);
                void            Exclude (ID);
                void            ClearAll ();

/////////////////////////////////////////////////////////////////////////////
/////����� � ��ꥪ��
/////////////////////////////////////////////////////////////////////////////
                PoolIndex       GetObject    (ID);
                Ring*           GetRing      (GameObjects);

/////////////////////////////////////////////////////////////////////////////
/////��ࠢ����� �������
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