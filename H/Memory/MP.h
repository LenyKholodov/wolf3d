#ifndef __MASTER_PTR__
#define __MASTER_PTR__

//Класс ведущий указатель. Нужен для корректного доступа к объекту
//static int Current() возвращает количество объектов
//Master Pointer

template <class T>
class mPtr
{
        private:
                 T* object;
                 static int     current; //Подсчёт ссылок
        public:
                mPtr(); //Создаёт новый объект
                mPtr(const &mPtr<T>&);
                mPtr<T>& operator = (const mPtr<T>&);
                virtual ~mPtr();
                T*  operator -> () const;
                static int Current() { return current; }
};

//Класс Master Pointer Read-Only
//Даёт возможность создавать не изменяемые объекты

template <class T>
class MPRO:public mPtr<T>
{
  private:
  public:
      const T*  operator -> () const;
};

#endif