#ifndef __MASTER_PTR__
#define __MASTER_PTR__

//����� ����騩 㪠��⥫�. �㦥� ��� ���४⭮�� ����㯠 � ��ꥪ��
//static int Current() �����頥� ������⢮ ��ꥪ⮢
//Master Pointer

template <class T>
class mPtr
{
        private:
                 T* object;
                 static int     current; //������� ��뫮�
        public:
                mPtr(); //������� ���� ��ꥪ�
                mPtr(const &mPtr<T>&);
                mPtr<T>& operator = (const mPtr<T>&);
                virtual ~mPtr();
                T*  operator -> () const;
                static int Current() { return current; }
};

//����� Master Pointer Read-Only
//���� ����������� ᮧ������ �� �����塞� ��ꥪ��

template <class T>
class MPRO:public mPtr<T>
{
  private:
  public:
      const T*  operator -> () const;
};

#endif