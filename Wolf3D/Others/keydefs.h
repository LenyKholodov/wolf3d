#ifndef __KEY_DEFS__
#define __KEY_DEFS__

#include <types.h>
#include <iostream.h>

typedef struct
{
  uchar  simb;
  uchar  extented;
}SimbolDef;

union Simbol
{
  SimbolDef     full_simb;
  uint16        ch;
  operator      = (int v) { ch = v; }
                Simbol( int v ) { ch = v; }
                Simbol(uchar v) { full_simb.simb = v; full_simb.extented = 1; }
  BOOL          IsExtented() { return full_simb.extented; }
                operator char() { return full_simb.simb; }
};

union  KeyboardState1
{
  uchar  RShift          : 1;
  uchar  LShift          : 1;
  uchar  Ctrl            : 1;
  uchar  Alt             : 1;
  uchar  ScrollLock      : 1;
  uchar  NumLock         : 1;
  uchar  CapsLock        : 1;
  uchar  Insert          : 1;
  friend ostream& operator<<(ostream&,const KeyboardState1&);
};

union  KeyboardState2
{
  uchar  LCtrl           : 1;
  uchar  LAlt            : 1;
  uchar  RCtrl           : 1;
  uchar  RAlt            : 1;
  uchar  ScrollLock      : 1;
  uchar  NumLock         : 1;
  uchar  CapsLock        : 1;
  uchar  SysRq           : 1;
  friend ostream& operator<<(ostream&,const KeyboardState2&);
};

extern KeyboardState1 const *  kbstate1;
extern KeyboardState2 const *  kbstate2;

const   Simbol  CHAR_ESC        = 0x101B;
const   Simbol  CHAR_ENTER      = 0x100D;
const   Simbol  CHAR_LEFTARROW  = 75;
const   Simbol  CHAR_RIGHTARROW = 77;
const   Simbol  CHAR_DOWNARROW  = 80;
const   Simbol  CHAR_UPARROW    = 72;
const   Simbol  CHAR_END        = 79;
const   Simbol  CHAR_HOME       = 71;
const   Simbol  CHAR_PGUP       = 81;
const   Simbol  CHAR_PGDN       = 73;
const   Simbol  CHAR_INS        = 82;
const   Simbol  CHAR_DEL        = 83;

const   Simbol  CHAR_F1          = 59;
const   Simbol  CHAR_F2          = 60;
const   Simbol  CHAR_F3          = 61;
const   Simbol  CHAR_F4          = 62;
const   Simbol  CHAR_F5          = 63;
const   Simbol  CHAR_F6          = 64;
const   Simbol  CHAR_F7          = 65;
const   Simbol  CHAR_F8          = 66;
const   Simbol  CHAR_F9          = 67;
const   Simbol  CHAR_F10         = 68;
const   Simbol  CHAR_F11         = 133;
const   Simbol  CHAR_F12         = 134;

const   Simbol  CHAR_0          = 0x1030;
const   Simbol  CHAR_1          = 0x1031;
const   Simbol  CHAR_2          = 0x1032;
const   Simbol  CHAR_3          = 0x1033;
const   Simbol  CHAR_4          = 0x1034;
const   Simbol  CHAR_5          = 0x1035;
const   Simbol  CHAR_6          = 0x1036;
const   Simbol  CHAR_7          = 0x1037;
const   Simbol  CHAR_8          = 0x1038;
const   Simbol  CHAR_9          = 0x1039;

const   Simbol  CHAR_A          = 0x1041;
const   Simbol  CHAR_B          = 0x1042;
const   Simbol  CHAR_C          = 0x1043;
const   Simbol  CHAR_D          = 0x1044;
const   Simbol  CHAR_E          = 0x1045;
const   Simbol  CHAR_F          = 0x1046;
const   Simbol  CHAR_G          = 0x1047;
const   Simbol  CHAR_H          = 0x1048;
const   Simbol  CHAR_I          = 0x1049;
const   Simbol  CHAR_J          = 0x104A;
const   Simbol  CHAR_K          = 0x104B;
const   Simbol  CHAR_L          = 0x104C;
const   Simbol  CHAR_M          = 0x104D;
const   Simbol  CHAR_N          = 0x104E;
const   Simbol  CHAR_O          = 0x104F;
const   Simbol  CHAR_P          = 0x1050;
const   Simbol  CHAR_Q          = 0x1051;
const   Simbol  CHAR_R          = 0x1052;
const   Simbol  CHAR_S          = 0x1053;
const   Simbol  CHAR_T          = 0x1054;
const   Simbol  CHAR_U          = 0x1055;
const   Simbol  CHAR_V          = 0x1056;
const   Simbol  CHAR_W          = 0x1057;
const   Simbol  CHAR_X          = 0x1058;
const   Simbol  CHAR_Y          = 0x1059;
const   Simbol  CHAR_Z          = 0x105A;

const   Simbol  CHAR_MINUS      = 0x102D;
const   Simbol  CHAR_PLUS       = 0x102B;
const   Simbol  CHAR_MUL        = 0x102A;
const   Simbol  CHAR_DIV        = 0x102F;
const   Simbol  CHAR_DIV1       = 0x102F;
const   Simbol  CHAR_DIV2       = 0x105C;
const   Simbol  CHAR_SET        = 0x103D;
const   Simbol  CHAR_BS         = 0x1008;
const   Simbol  CHAR_SPACE      = 0x1020;
const   Simbol  CHAR_TAB        = 0x1009;
const   Simbol  CHAR_LSCOB      = 0x105B;
const   Simbol  CHAR_RSCOB      = 0x105D;
const   Simbol  CHAR_SIMB0      = 0x103B;
const   Simbol  CHAR_SIMB1      = 0x1027;
const   Simbol  CHAR_SIMB2      = 0x1060;
const   Simbol  CHAR_LOW        = 0x102C;
const   Simbol  CHAR_HI         = 0x102E;

enum    KeyDefs
{
  NO_PRESS=0,
  KEY_ESC=0x1,
  KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,
  KEY_MINUS,    KEY_PLUS,     KEY_BS,       KEY_TAB,
  KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P,
  KEY_LSCOB,    KEY_RSCOB,
  KEY_ENTER,    KEY_CTRL,
  KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L,
  KEY_LSHIFT=0x2A,
  KEY_Z=0x2C,   KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M,
  KEY_LOW,      KEY_HI,KEY_DIV,
  KEY_RSHIFT=0x36,     KEY_PRNTSCR,  KEY_ALT,      KEY_SPACE, KEY_CAPS,
  KEY_F1,       KEY_F2,KEY_F3,KEY_F4,KEY_F5,KEY_F6,
  KEY_F7,       KEY_F8,KEY_F9,KEY_F10,KEY_NUM, KEY_SCROLL,
  KEY_HOME,     KEY_UPARROW,  KEY_PGUP, KEY_GMINUS, KEY_LEFTARROW,
  KEY_G5,       KEY_RIGHTARROW,         KEY_GPLUS,  KEY_END,  KEY_DOWNARROW,
  KEY_PGDN,     KEY_INS,      KEY_DEL,  KEY_SYSRQ,  KEY_MACRO = 0x56,
  KEY_F11,      KEY_F12,      KEY_PA1 = 0x5A,  KEY_LWIN,   KEY_RWIN,
  KEY_MENU,     KEY_F16=0x63,      KEY_F17,  KEY_F18,    KEY_F19,
  KEY_F20,      KEY_F21,      KEY_F22,  KEY_F23,    KEY_F24,
  KEY_ERASEEOF=0x6D, KEY_COPY=0x6F,     KEY_CRSEL=0x72,            KEY_DELTA,
  KEY_EXCEL,    KEY_CLEAR = 0x76,
  KEY_LAST_ELEMENT

};

extern const   Simbol  SCAN2ASCII[];
extern const   uchar  ASCII2SCAN[0xFF];
extern const   char*  ASCII2Name[0xFF];
extern const   char*  SCAN2Name[];

inline ostream& operator<<(ostream& os,const KeyboardState1& ks)
{
  const char* names[] = {"Right Shift","Left Shift","Ctrl\t","Alt\t",
                         "Scroll Lock","Num Lock","Caps Lock","Insert\t"};
  uchar* state = (uchar*)&ks;
  for (int i = 0;i<8;i++)
  {
      int       mask = 1l<<i;
      if (*state & mask)
            cout<<names[i]<<" \t\t ON"<<endl;
      else
            cout<<names[i]<<" \t\t OFF"<<endl;
  }
  return os;
}

inline ostream& operator<<(ostream& os,const KeyboardState2& ks)
{
  const char* names[] = {"Left Ctrl","Left Alt","Right Ctrl","Right Alt",
                         "Scroll Lock","Num Lock","Caps Lock","Sys Rq\t"};
  uchar* state = (uchar*)&ks;
  for (int i = 0;i<8;i++)
  {
      int       mask = 1l<<i;
      if (*state & mask)
            cout<<names[i]<<" \t\t ON"<<endl;
      else
            cout<<names[i]<<" \t\t OFF"<<endl;
  }
  return os;
}

#endif