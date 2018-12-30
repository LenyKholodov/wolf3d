#ifndef __RESOURCE_HEADER__
#define __RESOURCE_HEADER__

#include "t_obj.h"

#include <binfile.h>

const   int     RES_NAME_LEN         = 128;
const   int     MAX_RES_PARAM_LEN    = 128;

////////////////Описание абстрактной схемы ресурса///////////////////////////

struct  ResourceHeader
{
  ulong         id ;
  GameObjects   type;

     void        Refresh (GameObjects);

     GameObjects Load (const char*,void* dest); //размер >= размер ресурса
     GameObjects Load (BinFile*);
     void        Save (BinFile*,GameObjects);
};

/////////////////////////////////////////////////////////////////////////////
////Описание экземпляра в карте окружения
/////////////////////////////////////////////////////////////////////////////
struct  Exemplar
{
  char           resName       [RES_NAME_LEN];      //имя файла содержащего ресурс
  char           resourceParam [MAX_RES_PARAM_LEN]; //описание экземпляра
};

/////////////////////////////////////////////////////////////////////////////
////Схема создания объекта (по ресурсу)
/////////////////////////////////////////////////////////////////////////////
      //Добавить union
template <class Resource,class Param>
class   CreateExemplar
{
  public:
          Param&  param()  { return *((Param*)paramData); }

  public:
    char           paramData    [MAX_RES_PARAM_LEN];
    Resource       resourceClass   ;    
};

////////////////////////////////////////////////////////////////////////////
/////Карта экземпляров объектов
////////////////////////////////////////////////////////////////////////////
class   EnvironmentMap
{
  public:
                        EnvironmentMap  (int max = 512);
                        EnvironmentMap  (BinFile* file);
                        EnvironmentMap  (const char* name);
         virtual        ~EnvironmentMap ();

/////////////////////////////////////////////////////////////////////////////
/////Работа с картой
/////////////////////////////////////////////////////////////////////////////
         void              AddObject (const Exemplar&);
 const   Exemplar&         GetObject (int) const;
         void              ClearAll  ();

 const   int               count   () const { return mCount; }

/////////////////////////////////////////////////////////////////////////////
/////Загрузка - Сохранение
/////////////////////////////////////////////////////////////////////////////
         BOOL           Load (BinFile*);
         BOOL           Load (const char*);
         void           Save (BinFile*)      const;
         void           Save (const char*)   const;

  protected:
             Exemplar*          mMap;         //карта экземпляров объектов
             int                mSize;        //размер карты
             int                mCount;       //позиция в стеке
};

#endif