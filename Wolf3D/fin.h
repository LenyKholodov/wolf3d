#ifndef __FINALLY__
#define __FINALLY__

#include <types.h>
#include <malloc.h>     //for NULL

////////////////////////////////////////////////////////////////////////////
////Базовый класс, все потомки которого обрабатываются как деинициализаторы
////////////////////////////////////////////////////////////////////////////
class   FinClient
{
  friend class Finall;
  public:
                FinClient  (): next(NULL) {}
        virtual ~FinClient ();

              Finall*           Owner() const { return owner; }
              FinClient*        Next()  const { return next; }

  protected:
        FinClient*      next;   //следующий объект
        Finall*         owner;  //владелец
};

////////////////////////////////////////////////////////////////////////////
////Класс менеджер для всех деинициализатор
////////////////////////////////////////////////////////////////////////////
class   Finall
{
  public:
                        Finall();
          virtual       ~Finall();

        void            AddClient (FinClient*);
        void            DelClient (FinClient*);

  protected:
        FinClient*      finList;        //Список объектов на удаление
        FinClient*      curPos;         //Текущее позиция на удаление

        BOOL            isDelete;       //Флаг (производится удаление)
};

extern  Finall*          fin;    //Глобальный деинициализатор

#endif