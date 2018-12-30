#include <keyboard.h>

SystemKeyBuff::SystemKeyBuff(void)
{
  IgnoreOverload=TRUE;
  mStartPtr=(uint16 FAR*)0x0000041A;
  mEndPtr=mStartPtr+1;
  mKeyBuff=(uchar FAR*)0x00000400; 	 // 0x0040:001E
  clear();
}

int SystemKeyBuff::getchx(void)
{
  if(IsEmpty())
    return NULL;
  int value=*((uint16 FAR*)(mKeyBuff+*mStartPtr));
  next(mStartPtr);
  return value;
}

BOOL SystemKeyBuff::ungetch(int key)
{
   // проверка на переполнение
  if(IsFull())
  {
    if(!IgnoreOverload)
      return FALSE;
    prev(mEndPtr);
  }
  prev(mStartPtr);
  *((uint16 FAR*)(mKeyBuff+*mStartPtr))=key;
  return TRUE;
}

BOOL SystemKeyBuff::putch(int key)
{
  // проверка на переполнение
  if(IsFull())
  {
    if(!IgnoreOverload)
      return FALSE;
    next(mStartPtr); // извлечь "лишний" символ
  }
  *((uint16 FAR*)(mKeyBuff+*mEndPtr))=key;
  next(mEndPtr);
  return TRUE;
}
