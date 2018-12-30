#include <misc.h>

#include <dos.h>
#include <malloc.h>

#include <keyboard.h>
#include <memory\memory.h>

#define NDEBUG
#include "debug.h"

Pool*                   Keyboard::KeyQueue::key_pool;
Pool*                   Keyboard::CharQueue::ch_pool;
Keyboard::CharQueue*    Keyboard::ch_first;
Keyboard::CharQueue*    Keyboard::ch_last;
BOOL                    Keyboard::keymap[0x80];
Keyboard::KeyQueue*     Keyboard::key_first;
Keyboard::KeyQueue*     Keyboard::key_last;
Keyboard::KeyQueue*     Keyboard::fastlist[0x80];
void __far interrupt    (*Keyboard::old9)();
KeyPress                key_press;           //Для работы с очередью сообщений

const   Simbol  SCAN2ASCII[] =
{
  0, CHAR_ESC, CHAR_1, CHAR_2, CHAR_3, CHAR_4, CHAR_5, CHAR_6, CHAR_7, CHAR_8, //9
  CHAR_9,   CHAR_0, CHAR_MINUS, CHAR_SET, CHAR_BS, CHAR_TAB, CHAR_Q, CHAR_W, //8
  CHAR_E, CHAR_R, CHAR_T, CHAR_Y, CHAR_U, CHAR_I, CHAR_O, CHAR_P, CHAR_LSCOB,  //9
  CHAR_RSCOB, CHAR_ENTER, 0, CHAR_A, CHAR_S, CHAR_D, CHAR_F, CHAR_G, CHAR_H,   //9
  CHAR_J, CHAR_K, CHAR_L, CHAR_SIMB0, CHAR_SIMB1, CHAR_SIMB2, 0, CHAR_DIV2,    //8
  CHAR_Z, CHAR_X, CHAR_C, CHAR_V, CHAR_B, CHAR_N, CHAR_M, CHAR_LOW, CHAR_HI,  //9
  CHAR_DIV, 0, 0, 0, CHAR_SPACE, 0, CHAR_F1, CHAR_F2, CHAR_F3, CHAR_F4,        //10
  CHAR_F5, CHAR_F6, CHAR_F7, CHAR_F8, CHAR_F9, CHAR_F10, 0, 0,                 //8
  CHAR_HOME, CHAR_UPARROW, CHAR_PGDN, CHAR_MINUS, CHAR_LEFTARROW, CHAR_5,      //6
  CHAR_RIGHTARROW, CHAR_PLUS, CHAR_END, CHAR_DOWNARROW, CHAR_PGDN, CHAR_INS,   //6
  CHAR_DEL                                                                     //1
};

const   uchar  ASCII2SCAN[0xFF] =
{
  0, 0, 0, 0, 0, 0, 0, 0, KEY_BS, KEY_TAB, 0, 0, 0, KEY_ENTER, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, KEY_ESC, 0, 0, 0, 0,
  KEY_SPACE, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_7, 0x28, KEY_9, KEY_0,
  KEY_8, KEY_PLUS, 0x33, KEY_MINUS, 0x34, KEY_DIV, KEY_0, KEY_1, KEY_2, KEY_3,
  KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, 0x27, 0x27, KEY_LOW, 0xD, KEY_HI,
  KEY_DIV, KEY_2, KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H,
  KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S,
  KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z, 0x1A, 0x2B, 0x1B,
  KEY_6, KEY_SPACE, 0x29, KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G,
  KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R,
  KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z, 0x1A, 0x2B,
  0x1B, 0x29, KEY_SPACE, KEY_F, KEY_LOW, KEY_D, KEY_U, KEY_L, KEY_T, 0x27,
  KEY_P, KEY_B, KEY_Q, KEY_R, KEY_K, KEY_V, KEY_Y, KEY_J, KEY_G, KEY_H,
  KEY_C, KEY_N, KEY_E, KEY_A, 0x1A, KEY_W, KEY_X, KEY_I, KEY_O, KEY_M, KEY_S,
  0x1B, 0x28, KEY_HI, KEY_Z, KEY_F, KEY_LOW, KEY_D, KEY_U, KEY_L, KEY_T, 0x27,
  KEY_P, KEY_B, KEY_Q, KEY_R, KEY_K, KEY_V, KEY_Y, KEY_J, KEY_G, KEY_H,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  KEY_C, KEY_N, KEY_E, KEY_A, 0x1A, KEY_W, KEY_X, KEY_I, KEY_O, KEY_M, KEY_S,
  0x1B, 0x28, KEY_HI, KEY_Z,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const   char*  ASCII2Name[0xFF] =
{
  "NULL" ,"", "", "", "", "", "", "", "BS", "TAB", "", "", "", "ENTER",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "ESC",
  "", "", "", "",
  "SPACE", "!", "'", "#", "$", "%", "&", "'", "(", ")",
  "*", "+", ",", "-", ".", "/", "0", "1", "2", "3",
  "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">",
  "?", "@", "A", "B", "C", "D", "E", "F", "G", "H",
  "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S",
  "T", "U", "V", "W", "X", "Y", "Z", "[", "\\", "[",
  "^", " ", "`", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l",
  "m",  "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
  "{", "|", "}", "~", "А", "Б", "В", "Г", "Д", "Е", "Ё", "Ж",
  "З", "И", "Й", "К", "Л", "М", "Н", "О", "П", "Р",
  "С", "Т", "У", "Ф", "Х", "Ц", "Ч", "Ш", "Щ", "Ь", "Ы",
  "Ъ", "Э", "Ю", "Я", "а", "б", "в", "г", "д", "е", "ё",
  "ж", "з", "и", "й", "к", "л", "м", "н", "о", "п",
  "░", "▒", "▓", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0,
  "р", "с", "т", "у", "ф", "х", "ц", "ч", "ш", "щ", "ь", "ы", "ъ", "э", "ю", "я",
  0, 0, 0, 0, 0, 0, 0, 0, 0
};

const   char*  SCAN2Name[] =
{
  "NULL","ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=",
  "BS", "TAB", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[",
  "]", "ENTER", "CTRL", "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'",
  "`", "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".",
  "/", "RShift", "Prt Scr", "Alt", "SPACE", "Caps Lock", "F1", "F2", "F3",
  "F4", "F5", "F6", "F7", "F8", "F9", "F10", "Num Lock", "Scroll Lock",
  "HOME", "Up Arrow", "PgUp", "-", "Left Arrow", "5", "Right Arrow",
  "+", "END", "Down Arrow", "PgDn", "Insert", "Delete","Sys Rq",0,
  "Macro","F11","F12",0,"PA1","Left Windows","Right Windows","Menu",
  0,0,0,0,0,"F16","F17","F18","F19","F20","F21","F22","F23","F24",0,
  "Erase EOF",0,"Copy/Play",0,0,"Create Selection","Delta","Excel",0,"Clear"
};

extern KeyboardState1 const *  kbstate1 = (KeyboardState1 const *)(0x400+0x17);
extern KeyboardState2 const *  kbstate2 = (KeyboardState2 const *)(0x400+0x18);


BOOL          Keyboard::IsPress(int key) { return key<0x80 ? keymap[key] : 0; }
Keyboard::KeyQueue*
              Keyboard::GetKey()         { return key_first;    }
BOOL          Keyboard::IsEmpty()        { return ch_first==ch_last; }
void          Keyboard::Clear()          { ch_last = ch_first; }
BOOL          Keyboard::IsPress(Simbol simb)       { return IsPress(ToSCAN(simb.full_simb.simb)); }
BOOL          Keyboard::IsPress()                  { return !IsEmpty(); }
Simbol        Keyboard::ToASCII(int key)           { return SCAN2ASCII[key]; }
int           Keyboard::ToSCAN(Simbol simb)        { return ASCII2SCAN[simb.full_simb.simb]; }
const char*   Keyboard::GetKeyName(int key)        { return key <=KEY_LAST_ELEMENT ? SCAN2Name[key]:NULL;}
const char*   Keyboard::GetKeyName(Simbol simb)    { return ASCII2Name[simb.full_simb.simb]; }
BOOL          Keyboard::IsExtented(Simbol simb)    { return simb.IsExtented(); }

void Keyboard::ClearSystem()
{
    uchar state = inp(0x61);
    outp(0x61,state | 0x80);
    outp(0x61,state);
    outp(0x20,0x20);
}

void Keyboard::AddKey(int key)
{
  if (keymap[key]) //Если клaвиша нажата - выход
     return;

  if (key_first == NULL)
     key_first = key_last = new KeyQueue(key,NULL,NULL);
  else
     key_last  = key_last->next = new KeyQueue(key,key_last,NULL);
  fastlist[key]  = key_last;

        //Пишем в глобальную очередь событий
  key_press.scan = key;
  key_press.id   = KEY_DOWN;

  message_queue.PutMessage(&key_press,sizeof(KeyPress));
}

void Keyboard::SubKey(int key)
{
  if (!keymap[key]) //Если клaвиша не нажата - выход
    return;

  KeyQueue* tmp = fastlist[key];
  if (tmp = key_first)
     if (key_first != key_last)
     {
       key_first = key_first->next;
       key_first->prev = NULL; //begin of list
     }
     else
       key_last = key_first = NULL;
  else if (tmp = key_last)
  {
    key_last = key_last->prev;
    key_last->next = NULL;     //end of list
  }
  else
    tmp->prev->next = tmp->next;
  fastlist[key] = NULL;
  delete tmp;

        //Пишем в глобальную очередь событий
  key_press.scan = key;
  key_press.id   = KEY_UP;

  message_queue.PutMessage(&key_press,sizeof(KeyPress));
}

Simbol        Keyboard::GetChar()
{
  if (ch_first == ch_last)
     return Simbol(0);
  Simbol el = ch_first->ch;
  ch_first  = ch_first->next;
  return el;
}

void Keyboard::PutChar(Simbol simb)
{
  if (ch_last->next == ch_first)
     ch_first = ch_first->next;
  ch_last->ch = simb;
  ch_last     = ch_last->next;
}

void*   Keyboard::CharQueue::operator new (size_t bytes)
{
  memblock_t*    tmp = ch_pool->Alloc();
  return tmp ? (void*)(tmp->data) : NULL;
}

void*   Keyboard::KeyQueue::operator new (size_t bytes)
{
  memblock_t*    tmp = key_pool->Alloc();
  return tmp ? (void*)(tmp->data) : NULL;
}

void   Keyboard::KeyQueue::operator delete (void* el)
{
  memblock_t*    tmp = (memblock_t*)((uchar*)el-sizeof(memblock_t)+sizeof(uchar));//!!+SIZEOF(UCHAR)
  if (tmp->id != MEM_BLOCK_ID)
     D_Error("Keyboard: Wrong delete element of key-queue");
  key_pool->Free(tmp);
}

void __far interrupt Keyboard::new9()
{
  int  scan = inp(0x60);
  if (scan<=0x80)
  {
    AddKey(scan);
    PutChar(ToASCII(scan));
    keymap[scan] = TRUE;
  }
  else
  {
    SubKey(scan-0x80);
    keymap[scan-0x80] = FALSE;
  }
  ClearSystem();
}

Keyboard::Keyboard()
{
        //Allocate memory for system pools
  D_String("Keyboard: Memory allocated...");
  KeyQueue::key_pool   = new Pool(0xF,sizeof(KeyQueue));
  CharQueue::ch_pool   = new Pool(0xFF,sizeof(CharQueue));
  if ( CharQueue::ch_pool==NULL || KeyQueue::key_pool==NULL)
     D_Error("Not enough memory for keyboard");
  D_String ("Keyboard: Memory alloc OK");

        //Init ASCII queue (circle queue)
  ch_first = ch_last = new CharQueue(0);
  for (ch_last->next = new CharQueue(0);ch_last->next;ch_last = ch_last->next)
      ch_last->next->next = new CharQueue(0);
  ch_last->next = ch_first;
  ch_last = ch_first;
  D_String("Keyboard: ASCII circle queue init OK");

        //Init SCAN-code interface
  for (int i=0;i<0x80;i++)
  {
        keymap[i]   = FALSE;
        fastlist[i] = NULL;
  }
  D_String("Keyboard: SCAN-code init OK");

        //Init interrupt interface
  old9 = _dos_getvect(0x9);
  _dos_setvect(0x9,new9);
  D_String("Keyboard: Interrupt settings OK");

  D_String("Keyboard: Construct OK");
}

Keyboard::~Keyboard()
{
  _dos_setvect(0x9,old9);
  D_String("Keyboard: Free system memory...");
  D_Check;
  delete Keyboard::KeyQueue::key_pool;
  D_String("Keyboard: SCAN-code pool free OK");
  delete Keyboard::CharQueue::ch_pool;
  D_String("Keyboard: ASCII pool free OK");
  D_String("Keyboard: Destruct OK");
}
