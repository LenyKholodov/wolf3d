#ifndef __TYPE_RING__
#define __TYPE_RING__

/////////////////////////////////////////////////////////////////////////////
/////Таг для сохранения колец
/////////////////////////////////////////////////////////////////////////////
struct    RingTag
{
  int                mFirst;
  int                mFirstFree;
  int                mCurrent;
  int                mReadSize;
       //Дальше идёт таблица размером mReadSize
};

#endif