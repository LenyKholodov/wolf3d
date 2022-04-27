#ifndef __KEYBOARD__
#define __KEYBOARD__

#include <keydefs.h>
#include <message.h>
#include <memory\memory.h>

class Keyboard
{
  private:
////////////////////////////////////////////////////////////////
////Очередь нажатых клавиш (ASCII)
////////////////////////////////////////////////////////////////
           struct  CharQueue
           {
             static  Pool*        ch_pool;
             CharQueue*           next;
             Simbol               ch;
                                CharQueue(Simbol v): next(NULL), ch(v) {}
             void*              operator new (size_t bytes);
           };
           static  CharQueue*   ch_first;
           static  CharQueue*   ch_last;
////////////////////////////////////////////////////////////////
////Карта нажатых клавиш (SCAN-code) и массив нажатий
////////////////////////////////////////////////////////////////
           struct  KeyQueue
           {
             private:
               friend class Keyboard;
               static Pool*       key_pool;
               KeyQueue           *next,
                                  *prev;
               int                key;
             public:
                                KeyQueue(int v,KeyQueue* pr=NULL,KeyQueue* nx=NULL):
                                  prev(pr), next(nx), key(v) {}
             KeyQueue*          Next() { return next; }
             KeyQueue*          Prev() { return prev; }
             int                Key()  { return key; }
             void*              operator new (size_t bytes);
             void               operator delete (void* el);
           };
           static  BOOL                 keymap[0x80];
           static  KeyQueue*            fastlist[0x80];
           static  KeyQueue*            key_first;
           static  KeyQueue*            key_last;
////////////////////////////////////////////////////////////////
////Интерфейс перехвата прерываний от клавиатуры
////////////////////////////////////////////////////////////////
           static void __far interrupt new9();
           static void __far interrupt (*old9)();
           static BOOL       wait;
  protected:
           static void ClearSystem(); //clear system key-buffer
           static void AddKey(int key);
           static void SubKey(int key);
  public:
          Keyboard();
          ~Keyboard();

          BOOL          IsPress();

////////////////////////////////////////////////////////////////
////Scan-code interface
////////////////////////////////////////////////////////////////
          BOOL          IsPress(int key);
          KeyQueue*     GetKey();

////////////////////////////////////////////////////////////////
////ASCII char interface
////////////////////////////////////////////////////////////////
          Simbol        GetChar();
  static  void          PutChar(Simbol simb); //analog "C" ungetch()
          BOOL          IsEmpty();
          void          Clear();    //clear simbol buffer
          BOOL          IsPress(Simbol simb); //is this simbol press

////////////////////////////////////////////////////////////////
////Конверторы ASCII - SCAN-code и SCAN-code - ASCII
////////////////////////////////////////////////////////////////
  static  Simbol        ToASCII(int key);
  static  int           ToSCAN(Simbol simb);

////////////////////////////////////////////////////////////////
////Интерфейс работы с характеристиками кодов клавиш
////////////////////////////////////////////////////////////////
          const char*   GetKeyName(int key);
          const char*   GetKeyName(Simbol simb);
          BOOL          IsExtented(Simbol simb);
};

struct  KeyPress: public Message
{
  int   scan;
};

struct  KeyDown: public KeyPress {};
struct  KeyUp:   public KeyPress {};

#endif