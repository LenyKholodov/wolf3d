#ifndef __TYPE_RING__
#define __TYPE_RING__

/////////////////////////////////////////////////////////////////////////////
/////��� ��� ��࠭���� �����
/////////////////////////////////////////////////////////////////////////////
struct    RingTag
{
  int                mFirst;
  int                mFirstFree;
  int                mCurrent;
  int                mReadSize;
       //����� ���� ⠡��� ࠧ��஬ mReadSize
};

#endif