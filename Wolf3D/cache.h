#ifndef __CACHE__
#define __CACHE__

#include <stdlib.h>      //for NULL

#include <memory/memory.h>
#include <binfile.h>

/////////////////////////////////////////////////////////////////////////////
////Кэш-блок (плавающий размер)
/////////////////////////////////////////////////////////////////////////////
class   Cache;
struct  CacheList
{
  private:
        friend class    Cache;
        long            id;
        CacheList       *prev,
                        *next;

  public:
        uchar           data[1];  //Данные
};

/////////////////////////////////////////////////////////////////////////////
////Класс осуществляющий кэширование информации (загрузка виртуальна)
/////////////////////////////////////////////////////////////////////////////
////Данные хранятся за описанием кеш-блока
/////////////////////////////////////////////////////////////////////////////
class   Cache
{
  public:
                       Cache  ();
                       Cache  (long id_num,long cache_size,long cl_size = 0);
          virtual      ~Cache ();

          virtual void     Close  ();
          virtual void     Open   (long id_num,long cache_size,long cl_size);

/////////////////////////////////////////////////////////////////////////////
////Доступ к данным
/////////////////////////////////////////////////////////////////////////////
          void*        GetData(long id);

/////////////////////////////////////////////////////////////////////////////
////Служебные
/////////////////////////////////////////////////////////////////////////////
          long         GetNum()         const { return id_size; }
          long         GetCacheSize()   const { return cache_size; }
          long         GetClasterSize() const { return cl_size;}

  protected:
    virtual  void         LoadData(long id,CacheList* dest) = 0;
             void         ChangeReq(CacheList* block);

  protected:
////////////////////////////Работа с системой////////////////////////////////
           Pool*        queue_pool;     //Пул кластеров
           CacheList**  fast;           //Таблица быстрого доступа к кэшу

           CacheList    *first,         //Для вставки и взятия из кэша
                        *last;

//////////////////////////////Служебная информацмя///////////////////////////
           long         cache_size,
                        id_size,
                        cl_size;

//////////////////////////////Функции////////////////////////////////////////
           CacheList*   Alloc(CacheList* prev=NULL,CacheList* next=NULL);


};

////////////////////////////////////////////////////////////////////////////
////Описание файла Name Cache
////////////////////////////////////////////////////////////////////////////
#pragma pack (1)
struct  NameCacheHeader
{
          uchar         id[4];          //"NCCH"
          long          num;            //Колличество кэшируемых элементов
          long          cl_size;        //Размер кластера
          long          names_size;     //Размер пула с именами
          long          names_num;      //Колличестов имен
};
#pragma pack(0)

/////////////////////////////////////////////////////////////////////////////
///////Кэш имён (например файлов)
/////////////////////////////////////////////////////////////////////////////
class     NameCache: public Cache
{
  public:
                  NameCache  (char* names[],
                              int  names_num,
                              long id_num,
                              long cache_sz,
                              long cl_sz);
                  NameCache  (BinFile* file,long cache_sz=1);
                  NameCache  (const char* name,long cache_sz=1);
    virtual       ~NameCache ()                               { Close(); }

    virtual void            Close();

            int             GetNamesNum () const { return namesNum; }

/////////////////////////////////////////////////////////////////////////////
/////Имя элемента по индексу
/////////////////////////////////////////////////////////////////////////////
            char*           GetName (int) const;

/////////////////////////////////////////////////////////////////////////////
/////Сохранение - загрузка кэша имён
/////////////////////////////////////////////////////////////////////////////
    virtual void            Save (BinFile* file);
            void            Save (const char* name);
    virtual BOOL            Load (BinFile* file,int size);
            BOOL            Load (const char* name,int size);

  protected:
            char*          pool;        //Массив имён и смещений
            ulong*         files;       //Массив смещений имён
            ulong          pool_size;   //Размер пула с именами
            int            namesNum;    //Количество имён

  protected:
//////////////////////Служебные функции//////////////////////////////////////
                                //загрузка на потомках
    virtual void            LoadData (long,CacheList* x) { x->data[0] = -1; }

};

#endif