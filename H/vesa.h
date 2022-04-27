#ifndef __VESA_H__
#define __VESA_H__

#include <fscreen.h>
#include <vesalo.h>

#ifdef __386__
        #include <dpmi32gw.h>
        #define STRING_PTR(x)        ((char*)RM_PTR(x))
#else
        #include <dos.h>
        #define STRING_PTR(x)        ((char far*)CHPTR(x))
#endif

class VESAdevice
{
protected:
      static VESAInfoStruct mInfo;
      static VESAModeInfoStruct mModeInfo;
      static int mFlags;
      static int mOldMode,mCurrentMode,mDACWidth;
#ifdef __386__
      static void __far*  DosMemoryBufferPtr;
      static uint16           DosMemoryBufferRealSeg;
      static int          DosMemoryBufferSize;
#endif
      static int mBank[2];
#ifdef __386__
      static void* mBankPtr[2];
#else
      static void far* mBankPtr[2];
#endif
      static int mReadBankIndex, mWriteBankIndex;
      static Screen* mCurrentModeScreen;

      BOOL CheckVideoModeList(void) const;
      int ReadVar(const char*) const;
      BOOL ReadDosVar(const char* var="VESA");
      int SaveAdapter(void);
      int RestoreAdapter(void);
public:
      enum {         ENABLE_2X                =0x0001,
                ENABLE_DOS_VAR                =0x0002,
                ENABLE_LFB                =0x0004,
                ENABLE_INIT                =0x0008,
                ENABLE_RESTORE                =0x0010,
                ENABLE_DIRECT_CALL             =0x0020,
                BUG_MODE_LIST                =0x0040,
                DEFAULT                        =ENABLE_2X|ENABLE_DOS_VAR|ENABLE_LFB
           };
       enum {   OK=0,
                           TOO_FEW_DOS_MEMORY,
                NOT_DETECTED,
                NO_INIT,
                DEVICE_LOCKED,
                MODE_UNAVAILABLE,
                LO_ERROR
            };
      int GetModeInfo(VESAModeInfoStruct* ptr,int mode);
      const VESAModeInfoStruct* GetModeInfo(void) const { return &mModeInfo; }
      VESAdevice(int mode)                { open(mode);        }
      VESAdevice(const char* mode)        { open(mode);        }
      ~VESAdevice(void)                        { close(); }
      VESAdevice(void)                        { }
      int open(int mode=DEFAULT);
      int open(const char* mode)        { return open(ReadVar(mode)); }
      void close(void);

      VESAInfoStruct* info(void)        { return mFlags?&mInfo:NULL; }
      int version(void) const                 { return (mFlags&ENABLE_2X)?mInfo.Version:0x100; }
      const CHPTR OemStr(void) const
                  {          return mFlags?STRING_PTR(mInfo.OemStringPtr):NULL; }
      long TotalMemory(void) const
                  {          return mFlags?mInfo.TotalVideoMem*65536L:0;        }
      ostream& adapterInfo(ostream&);
      ostream& modeInfo(ostream&,int);
      ostream& about(ostream& os);
/////////////////////////////////////////////////////////////////////////
      int SetMode(int mode,int rw=0,int rh=0);
      Screen* CurrentMode(void)          { return mCurrentModeScreen; }
      Screen* CurrentScreen(void)          { return mCurrentModeScreen; }
      int SetWriteBank(int);
      int SetReadBank(int);
      int GetWriteBank(void) const { return mBank[mWriteBankIndex]; }
      int GetReadBank(void)  const { return mBank[mReadBankIndex]; }
#ifdef __386__
      void* GetWriteBankAddr(void) { return mBankPtr[mWriteBankIndex]; }
      void* GetReadBankAddr(void)  { return mBankPtr[mReadBankIndex]; }
#else
      void far* GetWriteBankAddr(void) { return mBankPtr[mWriteBankIndex]; }
      void far* GetReadBankAddr(void)  { return mBankPtr[mReadBankIndex]; }
#endif
      int SetViewPos(int x,int y,BOOL wr=FALSE)  { return VESALOSetWinPos(x,y,wr)==0?OK:LO_ERROR; }
      void GetViewPos(int* x,int* y)                   { VESALOGetWinPos(x,y); }
      int GetViewWidth(void)  const                 { return mModeInfo.XResolution; }
      int GetViewHeight(void) const                 { return mModeInfo.YResolution; }
      int DACWidth(int);
      int DACWidth(void) const                          { return mDACWidth; }
};

//////////////////////////////////////////////////////////////////////////////
// VESAScreen's
//////////////////////////////////////////////////////////////////////////////
class vesaScreenPro
{
protected:
        VESAdevice* mDevice;
public:
        VESAdevice* GetDevice(void)         { return mDevice; }
};

template <class T> class vesaFlatScreen:public T,public vesaScreenPro
{
public:
        vesaFlatScreen(void)        {}
        vesaFlatScreen(int w,int h,void* lfb,VESAdevice* dev) { open(w,h,lfb,dev); }
        virtual void close(void);
        virtual ~vesaFlatScreen(void);
          virtual int  GetType(void)    const;
        BOOL open(int w,int h,void* lfb,VESAdevice* dev)
                                                   { mDevice=dev; return T::open(w,h,lfb); }
};

class vfScreen8bit:public vesaFlatScreen<fScreen8bit>
{
public:
        vfScreen8bit(int w,int h,void* lfb,VESAdevice* dev) { open(w,h,lfb,dev); }
/// Палитра //////////////////////////////////////////////
        virtual void _PutPalette(int index,int count,const RGBStruct* palette);
        virtual void _PutPalette(int index,int count,const RGBQUAD* palette);
        virtual void _GetPalette(int index,int count,RGBStruct* palette);
        virtual void _GetPalette(int index,int count,RGBQUAD* palette);
};

class vfScreen16bit:public vesaFlatScreen<fScreen16bit>
{
public:
        vfScreen16bit(int w,int h,void* lfb,VESAdevice* dev) { open(w,h,lfb,dev); }
// other
           virtual long rgb(uchar red,uchar green,uchar blue);
          virtual void rgb(long color,uchar* red,uchar* green,uchar* blue);
};

typedef vesaFlatScreen<fScreen32bit> vfScreen32bit;

template <class T> class vesaBankedScreen:public Screen,public vesaScreenPro
{
protected:
        T* mrBank;
        T* mwBank;
        int mbpSize;
public:
  BOOL open(int w,int h,VESAdevice*);
  virtual void close(void);

  vesaBankedScreen(void)                        { }
  vesaBankedScreen(int w,int h,VESAdevice* dev) { open(w,h,dev);   }
  virtual ~vesaBankedScreen(void);
//// Работа с характеристиками экрана //////////////////////
  virtual int  GetType(void)    const;
/// Графические примитивы /////////////////////////////////
  virtual void _PutPixel(int x,int y,long color);
  virtual long _GetPixel(int x,int y);
  virtual void _vline(int x,int y,int h,long color);
/// Графические примитивы ////////////////////////////////
  virtual void _PutImage(int x,int y,const tImage* image,const cRect* rect);
  virtual void _GetImage(int x,int y,int w,int h,tImage* image,const cRect* rect);
};

class vbScreen8bit: public vesaBankedScreen<uchar>
{
public:
  vbScreen8bit(int w,int h,VESAdevice* dev) { open(w,h,dev);   }
/// Графические примитивы /////////////////////////////////
  virtual void _hline(int x,int y,int w,long color);
/// Палитра //////////////////////////////////////////////
  virtual void _PutPalette(int index,int count,const RGBStruct* palette);
  virtual void _PutPalette(int index,int count,const RGBQUAD* palette);
  virtual void _GetPalette(int index,int count,RGBStruct* palette);
  virtual void _GetPalette(int index,int count,RGBQUAD* palette);
// other
           virtual long rgb(uchar red,uchar green,uchar blue);
          virtual void rgb(long color,uchar* red,uchar* green,uchar* blue);
};

class vbScreen16bit: public vesaBankedScreen<uint16>
{
public:
  vbScreen16bit(int w,int h,VESAdevice* dev) { open(w,h,dev);   }
/// Графические примитивы /////////////////////////////////
  virtual void _hline(int x,int y,int w,long color);
// other
           virtual long rgb(uchar red,uchar green,uchar blue);
          virtual void rgb(long color,uchar* red,uchar* green,uchar* blue);
};

class vbScreen32bit: public vesaBankedScreen<ulong>
{
public:
  vbScreen32bit(int w,int h,VESAdevice* dev) { open(w,h,dev);   }
/// Графические примитивы /////////////////////////////////
  virtual void _hline(int x,int y,int w,long color);
// other
           virtual long rgb(uchar red,uchar green,uchar blue);
          virtual void rgb(long color,uchar* red,uchar* green,uchar* blue);
};

#endif
