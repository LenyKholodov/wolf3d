#ifndef __FINALLY__
#define __FINALLY__

#include <types.h>
#include <malloc.h>     //for NULL

////////////////////////////////////////////////////////////////////////////
////������ �����, �� ��⮬�� ���ண� ��ࠡ��뢠���� ��� �����樠�������
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
        FinClient*      next;   //᫥���騩 ��ꥪ�
        Finall*         owner;  //��������
};

////////////////////////////////////////////////////////////////////////////
////����� �������� ��� ��� �����樠������
////////////////////////////////////////////////////////////////////////////
class   Finall
{
  public:
                        Finall();
          virtual       ~Finall();

        void            AddClient (FinClient*);
        void            DelClient (FinClient*);

  protected:
        FinClient*      finList;        //���᮪ ��ꥪ⮢ �� 㤠�����
        FinClient*      curPos;         //����饥 ������ �� 㤠�����

        BOOL            isDelete;       //���� (�ந�������� 㤠�����)
};

extern  Finall*          fin;    //�������� �����樠������

#endif