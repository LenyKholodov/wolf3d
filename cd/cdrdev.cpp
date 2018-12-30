#include <cd.h>

void cdromDevice::device_request(long daddr) const
{
  DECLARE_RMR(regs);
  W_REG(regs).ax=0x1510;
  W_REG(regs).cx=mCurrentDrive;
  W_REG(regs).bx=loword(daddr);
  regs.x.es=hiword(daddr);
  RMInt(0x2F,&regs);
}

void cdromDevice::red_book(ulong value,uint *min,uint *sec,uint *frame)
{
  *frame=value&0x000000ff;
  *sec  =(value&0x0000ff00)>>8;  // & 0x0000ff00) >> 8;
  *min  =(value&0x00ff0000)>>16; // & 0x00ff0000) >> 16;
}

ulong cdromDevice::hsg(ulong value)
{
  uint min,sec,frame;
  red_book(value,&min,&sec,&frame);
  return min*4500L+sec*75L+frame-150L;
}

cdromDevice::cdromDevice(void)
{
  mDrivesCount=0;
  mFirstDrive=mCurrentDrive=0;
}

char cdromDevice::CurrentDrive(char d)
{
  if((d&=0xDF)<'A' || d>'Z')
    return mCurrentDrive;
  d-='A';
  uchar oldDrive=mCurrentDrive;
  mCurrentDrive=d;
  if(!get_audio_info())
    mCurrentDrive=oldDrive;
  return mCurrentDrive+'A';
}

BOOL cdromDevice::get_audio_info(void)
{
#pragma pack(1);
  typedef struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
    unsigned char media;
    long address;
    short bytes;
    short sector;
    long volid;
  } ioctli;

  typedef struct {
    unsigned char mode;
    unsigned char lowest;
    unsigned char highest;
    ulong address;
  } track_data;
#pragma pack();
  DosMirrorBuff<ioctli>     ii;
  DosMirrorBuff<track_data> td;
  if(ii.link()==NULL || td.link()==NULL)
    return FALSE;

  ii.link()->length =sizeof (ioctli);
  ii.link()->subunit=0;
  ii.link()->comcode=3;
  ii.link()->media  =0;
  ii.link()->sector =0;
  ii.link()->volid  =0;
  ii.link()->address=td.dosaddr();
  ii.link()->bytes  =sizeof(track_data);
  td.link()->mode   =0x0A;
  device_request(ii.dosaddr());
  mLowAudio=td.link()->lowest;
  mHiAudio =td.link()->highest;
  red_book(td.link()->address,&mDiskLengthMin,&mDiskLengthSec,&mDiskLengthFrames);
  mEndOfDisk=hsg(td.link()->address);
  mError=ii.link()->status;
  return TRUE;
}

BOOL cdromDevice::set_track(int track)
{
#pragma pack(1);
  typedef struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
    unsigned char media;
    unsigned long address;
    unsigned short bytes;
    unsigned short sector;
    unsigned long  volid;
  } tray_request;

  typedef struct {
    unsigned char mode;
    unsigned char track;
    ulong address;
    unsigned char control;
  } track_data;
#pragma pack();
  DosMirrorBuff<tray_request> tr;
  DosMirrorBuff<track_data>   td;
  if(tr.link()==NULL || td.link()==NULL)
    return FALSE;

  tr.link()->length =sizeof(tray_request);
  tr.link()->subunit=0;
  tr.link()->comcode=3;
  tr.link()->media  =0;
  tr.link()->sector =0;
  tr.link()->volid  =0;
  tr.link()->address=td.dosaddr();
  tr.link()->bytes  =sizeof(track_data);
  td.link()->mode   =0x0b;
  td.link()->track  =track;
  device_request(tr.dosaddr());

  mError=tr.link()->status;
//  ulong addr=(td.link()->address);
  mCurrentTrackAddr=hsg((td.link()->address));
  mCurrentTrack    =track;
  mCurrentTrackType=td.link()->control & TRACK_MASK;
  return TRUE;
}

BOOL cdromDevice::seek(ulong location)
{
#pragma pack(1);
  typedef struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
    unsigned char addressmode;
    unsigned long transfer;
    unsigned short sectors;
    unsigned long seekpos;
  } play_request;
#pragma pack();
  DosMirrorBuff<play_request> pr;
  if(pr.link()==NULL)
    return FALSE;

  pr.link()->length =sizeof(play_request);
  pr.link()->subunit=0;
  pr.link()->comcode=131;
  pr.link()->addressmode=0;
  pr.link()->transfer=0;
  pr.link()->sectors =0;
  pr.link()->seekpos=location;
  device_request(pr.dosaddr());
  mError =pr.link()->status;
  return TRUE;
}

BOOL cdromDevice::play(ulong begin,ulong end)
{
#pragma pack(1);
  typedef struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
    unsigned char addressmode;
    unsigned long start;
    unsigned long playlength;
  } play_request;
#pragma pack();
  DosMirrorBuff<play_request> pr;
  if(pr.link()==NULL)
    return FALSE;
  pr.link()->length =sizeof (play_request);
  pr.link()->subunit=0;
  pr.link()->comcode=132;
  pr.link()->addressmode=0;
  pr.link()->start=begin;
  pr.link()->playlength=end-begin;
  device_request(pr.dosaddr());
  mError=pr.link()->status;
  return TRUE;
}

BOOL cdromDevice::play(int track)
{
  if(track<mLowAudio || track>mHiAudio)
    return FALSE;
  set_track(track);

  if(mCurrentTrackType==CD_DATA_TRACK)
    return FALSE;
  int startpos=mCurrentTrackAddr,endpos;
  if(track+1>mHiAudio)
    endpos=mEndOfDisk;
  else
  {
    set_track(track+1);
    endpos=mCurrentTrackAddr;
    set_track(track);
  }

//  seek(mCurrentTrackAddr);
//  delay(400);
  return play(startpos,endpos);
}

BOOL cdromDevice::installed(void)
{
  union REGS regs;
  regs.w.ax=0x1500;
  regs.w.bx=0;
  int386(0x2f,&regs,&regs);
  if(regs.w.bx)
  {
    mFirstDrive=mCurrentDrive=regs.w.cx;
    mDrivesCount=regs.w.bx;
    get_audio_info();
    return TRUE;
  }
  return FALSE;
}

BOOL cdromDevice::lock(BOOL mode)
{
#pragma pack(1);
  typedef struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
    unsigned char media;
    unsigned long address;
    unsigned short bytes;
    unsigned char unused[4];
  } tray_request;

  typedef struct {
    unsigned char mode;
    unsigned char media;
  } cd_data;
#pragma pack();

  DosMirrorBuff<tray_request> tr;
  DosMirrorBuff<cd_data>      c_d;

  if(tr.link()==NULL || c_d.link()==NULL)
    return FALSE;
  tr.link()->length =sizeof(tray_request);
  tr.link()->subunit=0;
  tr.link()->comcode=12;
  tr.link()->media  =0;
  tr.link()->address=c_d.dosaddr();
  tr.link()->bytes  =sizeof(cd_data);
  c_d.link()->mode  =1;
  c_d.link()->media =mode?1:0;
  device_request(tr.dosaddr());
  mError=tr.link()->status;
  return TRUE;
}

BOOL cdromDevice::cmd(uchar mode)
{
#pragma pack(1);
  typedef struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
    unsigned char media;
    unsigned long address;
    unsigned short bytes;
    unsigned char unused[4];
  } tray_request;

  typedef struct {
    unsigned char mode;
    unsigned char media;
  } cd_data;
#pragma pack();

  DosMirrorBuff<tray_request> tr;
  DosMirrorBuff<cd_data>      c_d;

  if(tr.link()==NULL || c_d.link()==NULL)
    return FALSE;
  tr.link()->length =sizeof(tray_request);
  tr.link()->subunit=0;
  tr.link()->comcode=12;
  tr.link()->media  =0;
  tr.link()->address=c_d.dosaddr();
  tr.link()->bytes  =sizeof(1);
  c_d.link()->mode  =mode;
  device_request(tr.dosaddr());
  mError=tr.link()->status;
  return TRUE;
}

BOOL cdromDevice::stop_resume(uchar mode)
{
#pragma pack(1);
  typedef struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
  } stop_request;
#pragma pack();
  DosMirrorBuff<stop_request> sr;
  if(sr.link()==NULL)
    return FALSE;
  sr.link()->length =sizeof(stop_request);
  sr.link()->subunit=0;
  sr.link()->comcode=mode;
  device_request(sr.dosaddr());
  mError=sr.link()->status;
  return TRUE;
}

