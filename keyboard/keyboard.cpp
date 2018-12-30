#include <debug.h>
#include <keyboard.h>
#include <i86.h>
#include <stdlib.h>
#include <dos.h>

SystemKeyBuff Keyboard::SystemQueue;

BOOL Keyboard::mKeymap[0x80];
BOOL Keyboard::mFirstRun=TRUE;
BOOL Keyboard::IgnoreRepeatMap[0x80];

const char* Keyboard::mScan2ChrTable=Scan2ChrTableEn[0];

void __far interrupt (*Keyboard::old9)()=NULL;

SysKeyboardState  & Keyboard::kbState  =*(SysKeyboardState*)  0x00000417;
SysKeyboardStateEx& Keyboard::kbStateEx=*(SysKeyboardStateEx*)0x00000418;

BOOL Keyboard::WaitClear(void)           // Wait until keyboard is cleared
{
  for(int timeout=0xFFFF;timeout--;)
    if((inp(0x64)&2)==0)
      return TRUE;
  return FALSE;
}

BOOL Keyboard::WaitForACK(void)          // Port 0x60 wait for ACK
{
  for(int timeout=0xFFFF;timeout--;)
    if(inp(0x64)&1)
      return inp(0x60)==0xFA;
  return FALSE;
}

BOOL Keyboard::KeyboardCommand(char Command, char Data) // Send command to keyboard  
{
  if(WaitClear()) 
    return FALSE; // Wait til ready
  _disable(); // We don't want to be interupted
  outp(0x60,Command);         // Send command
  WaitForACK();
  outp(0x60,Data);            // Send Command Data
  WaitForACK(); 		  // and wait for ACK
  _enable(); // Interrupts ok.
  return TRUE;
}

void Keyboard::ClearSystem(void)
{
    uchar state=inp(0x61);
    outp(0x61,state|0x80);
    outp(0x61,state&0x7F);
    outp(0x20,0x20);
}

BOOL Keyboard::advancedKey=FALSE;

void __far interrupt Keyboard::new9()
{
  int scan=inp(0x60);  
  if(scan==0xE0) // advanced mode
  {
    advancedKey=TRUE;
    ClearSystem();
    return;
  }
  BOOL keyDown=scan&0x80?FALSE:TRUE;
  scan&=0x7F;

  if(advancedKey)
  {
    advancedKey=FALSE;
    switch(scan)
    {
      case KEY_ALT:  scan=KEY_RALT;  break;
      case KEY_CTRL: scan=KEY_RCTRL; break;

      case KEY_INS:  scan=KEY_CINS;  break;
      case KEY_DEL:  scan=KEY_CDEL;  break;
      case KEY_HOME: scan=KEY_CHOME; break;
      case KEY_END:  scan=KEY_CEND;  break;
      case KEY_PGUP: scan=KEY_CPGUP; break;
      case KEY_PGDN: scan=KEY_CPGDN; break;

      case KEY_LEFTARROW: scan=KEY_CLEFT; break;
      case KEY_RIGHTARROW:scan=KEY_CRIGHT;break;
      case KEY_DOWNARROW: scan=KEY_CDOWN; break;
      case KEY_UPARROW:   scan=KEY_CUP;   break;

      case KEY_ENTER:scan=KEY_NENTER;break;
    }
  }

  if(keyDown)
    KeyDown(scan);
  else
    KeyUp(scan);
  ClearSystem();
}

void Keyboard::KeyDown(int scan)
{
  // Если кнопка из игнорируемых, и уже нажата - пропустить
  if(IgnoreRepeatMap[scan] && mKeymap[scan])
    return;
  // иначе: отметить на карте
  mKeymap[scan]=TRUE;
  // проверить статус клавиатуры
  switch(scan)
  {
    case KEY_LSHIFT: kbState.LShift=1; break;
    case KEY_RSHIFT: kbState.RShift=1; break;
    case KEY_CTRL:   kbState.Ctrl=1;	kbStateEx.LCtrl=1; break;
    case KEY_RCTRL:  kbState.Ctrl=1;   		   break;
    case KEY_ALT:    kbState.Alt=1;	kbStateEx.LAlt=1;  break;
    case KEY_RALT:   kbState.Alt=1;   		   break;
    case KEY_SCROLL: if(!kbStateEx.ScrollDown)
    		{
    		  kbStateEx.ScrollDown=1;
		  kbState.ScrollLock=!kbState.ScrollLock;
		}
					     	   break;
    case KEY_NUM:    if(!kbStateEx.NumDown)
    		{
    		  kbStateEx.NumDown=1;
		  kbState.NumLock=!kbState.NumLock;
		}
						   break;
    case KEY_CAPS:   if(!kbStateEx.CapsLockDown)
    		{
    		  kbStateEx.CapsLockDown=1;
		  kbState.CapsLock=!kbState.CapsLock;
		}
						   break;
    case KEY_INS:
    case KEY_CINS:   if(!kbStateEx.InsDown)
    		{
    		  kbStateEx.InsDown=1;
		  kbState.Insert=!kbState.Insert;
		}
						   break;
//    default:    memset((void*)0xB8000,7,4000); break;
  }
  // статик - это пошло, но выхода нет (На случай DS!=SS).
  static KeyPress key_press;
  // добавить в очередь сообщений
  key_press.scan=scan;
  key_press.chr =
  	mScan2ChrTable[scan+(kbState.NumLock?256:0)+(kbState.IsUpper()?128:0)];
  key_press.id  =KEY_DOWN;

  message_queue.PutMessage(&key_press,sizeof(KeyPress));
  // добавить в системный буфер
  SystemQueue.putch(word(key_press.scan,key_press.chr));
}

void Keyboard::KeyUp(int scan)
{
  // отметить на карте
  mKeymap[scan]=FALSE;
  // проверить статус клавиатуры
  // проверить статус клавиатуры
  switch(scan)
  {
    case KEY_LSHIFT: kbState.LShift=0; break;
    case KEY_RSHIFT: kbState.RShift=0; break;
    case KEY_CTRL:   if(!mKeymap[KEY_RCTRL]) 
    		  kbState.Ctrl=0;	
		kbStateEx.LCtrl=0; 
		break;
    case KEY_RCTRL:  if(!mKeymap[KEY_CTRL]) 
    		  kbState.Ctrl=0; 
		break;
    case KEY_ALT:    if(!mKeymap[KEY_RALT]) 
    		  kbState.Alt=0;	
		kbStateEx.LAlt=0; 
		break;
    case KEY_RALT:   if(!mKeymap[KEY_ALT]) 
    		  kbState.Alt=0; 
		break;
    case KEY_SCROLL: kbStateEx.ScrollDown=0; break;
    case KEY_NUM:    kbStateEx.NumDown=0;    break;
    case KEY_CAPS:   kbStateEx.CapsLockDown=0;    break;
    case KEY_INS:
    case KEY_CINS:   if(mKeymap[KEY_INS]==FALSE && mKeymap[KEY_CINS]==FALSE)
    		  kbStateEx.InsDown=0;
   	        break;
  }
  // статик - это пошло, но выхода нет (На случай DS!=SS).
  static KeyPress key_press;
  // добавить в очередь сообщений
  key_press.scan=scan;
  key_press.chr =
  	mScan2ChrTable[scan+(kbState.NumLock?256:0)+(kbState.IsUpper()?128:0)];
  key_press.id  =KEY_UP;

  message_queue.PutMessage(&key_press,sizeof(KeyPress));
}

BOOL Keyboard::CapsLock(BOOL val)
{
  int KeybStatus=(kbState.NumLock?2:0)|(kbState.ScrollLock?1:0);
  if(val)
  {
    KeybStatus|=4;
    kbState.CapsLock=1;
  }
  else
    kbState.CapsLock=0;
  KeyboardCommand(0xED, KeybStatus);
  return TRUE;
}

BOOL Keyboard::ScrollLock(BOOL val)
{
  int KeybStatus=(kbState.NumLock?2:0)|(kbState.CapsLock?4:0);
  if(val)
  {
    KeybStatus|=1;
    kbState.ScrollLock=1;
  }
  else
    kbState.ScrollLock=0;
  KeyboardCommand(0xED,KeybStatus);
  return TRUE;
}

BOOL Keyboard::NumLock(BOOL val)
{
  int KeybStatus=(kbState.ScrollLock?1:0)|(kbState.CapsLock?4:0);
  if(val)
  {
    KeybStatus|=2;
    kbState.NumLock=1;
  }
  else
    kbState.NumLock=0;
  KeyboardCommand(0xED,KeybStatus);
  return TRUE;
}

BOOL Keyboard::open(void)
{
  if(old9!=NULL)
    return TRUE;
  if(mFirstRun)
  {
    mFirstRun=FALSE;
    atexit(close);
  }
  close();
  old9 = _dos_getvect(0x9);
  _dos_setvect(0x9,new9);
  debug("Keyboard: Interrupt settings OK\n");
  // init tables
  for(int i=0;i<0x80;i++)
    IgnoreRepeatMap[i]=mKeymap[i]=FALSE;
  IgnoreRepeatMap[KEY_CTRL]=TRUE;
  IgnoreRepeatMap[KEY_RCTRL]=TRUE;
  IgnoreRepeatMap[KEY_ALT]=TRUE;
  IgnoreRepeatMap[KEY_RALT]=TRUE;
  IgnoreRepeatMap[KEY_LSHIFT]=TRUE;
  IgnoreRepeatMap[KEY_RSHIFT]=TRUE;
  IgnoreRepeatMap[KEY_HOME]=TRUE;
  IgnoreRepeatMap[KEY_CHOME]=TRUE;
  IgnoreRepeatMap[KEY_END]=TRUE;
  IgnoreRepeatMap[KEY_CEND]=TRUE;
  return TRUE;
}

void Keyboard::close(void)
{
  if(old9!=NULL)
  {
    _dos_setvect(0x9,old9);
    old9=NULL;
  }
  SystemQueue.clear();
}

Keyboard::~Keyboard()
{
  close();
}