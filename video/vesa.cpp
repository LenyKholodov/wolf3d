#include <vesa.h>
#include <iomanip.h>
#include <vga.h>

VESAInfoStruct 	   VESAdevice::mInfo;
VESAModeInfoStruct VESAdevice::mModeInfo;
BOOL	       VESAdevice::mFirstRun=TRUE;
int 	       VESAdevice::mFlags=0;
int 	       VESAdevice::mOldMode,
	       VESAdevice::mCurrentMode,
	       VESAdevice::mDACWidth;
int 	       VESAdevice::mBank[2];

#ifdef __386__
void __far*    VESAdevice::DosMemoryBufferPtr;
uint16 	       VESAdevice::DosMemoryBufferRealSeg;
int	       VESAdevice::DosMemoryBufferSize;
void*          VESAdevice::mBankPtr[2];
#else
void far*      VESAdevice::mBankPtr[2];
#endif

int 		VESAdevice::mReadBankIndex, 
    		VESAdevice::mWriteBankIndex;
Screen* 	VESAdevice::mCurrentModeScreen=NULL;

BOOL VESAdevice::CheckVideoModeList(void) const
{
#ifdef __386__
	uint16* ModeList=(uint16*)RM_PTR(mInfo.VideoModePtr);
#else
	uint16 far* ModeList=(uint16 far*)mInfo.VideoModePtr;
#endif
        for(int i=0;ModeList[i]!=0xFFFF;i++)
                if(ModeList[i]<0x100 || ModeList[i]>0x1FF || i>10000)
			return FALSE;
	return TRUE;
}

int VESAdevice::ReadVar(const char* val) const
{
	for(int flags=0;val[0];val++)
		if(val[0]=='-' || val[0]=='+')
		{
			int bm;
			switch(val[1])
			{
				case '2': 	    bm=ENABLE_2X; 	   break;
				case 'l': case 'L': bm=ENABLE_LFB; 	   break;
				case 'r': case 'R': bm=ENABLE_RESTORE; 	   break;
				case 'c': case 'C': bm=ENABLE_DIRECT_CALL; break;
				default:       	    continue;
			}
			if(val[0]=='-')
				flags&=~bm;
			else
				flags|=bm;
			val++;
		}	
	return flags;
}

BOOL VESAdevice::ReadDosVar(const char* var)
{
	char* val=getenv(var);
	if(val==NULL || val[0]=='\0')
		return FALSE;
	mFlags=ReadVar(val);
	return TRUE;
}

int VESAdevice::SaveAdapter(void)
{
	mOldMode=::GetMode();
	return OK;
}

int VESAdevice::RestoreAdapter(void)
{
	if(mOldMode!=mCurrentMode)
	{
		::SetMode(mOldMode);
	}
	mCurrentMode=mOldMode;
	return OK;
}

int VESAdevice::GetModeInfo(VESAModeInfoStruct* ptr,int mode)
{
	if(mFlags&ENABLE_INIT)
	{
		if(!
#ifdef __386__
	    	VESALOModeInfo(MakeDword(DosMemoryBufferRealSeg,0),mode)
#else
	    	VESALOModeInfo(ptr,mode)
#endif
		)
		return MODE_UNAVAILABLE;
#ifdef __386__
		_fmemcpy(ptr,DosMemoryBufferPtr,sizeof(VESAModeInfoStruct));
#endif	
		return OK;

	}
	return NO_INIT;
}

int VESAdevice::open(int mode)
{	
        if(mFirstRun)
	{
          mFirstRun=FALSE;
	  atexit(close);
	}
	close();
#ifdef __386__
	if((DosMemoryBufferPtr=DPMIDOSAlloc(DosMemoryBufferSize=1024,NULL,&DosMemoryBufferRealSeg))==NULL)
		return TOO_FEW_DOS_MEMORY;
	_fstrcpy((char __far*)DosMemoryBufferPtr,"VBE2");
	if(!VESALOinfo(MakeDword(DosMemoryBufferRealSeg,0)))
	{
		DPMIDOSFree(DosMemoryBufferPtr);
		return NOT_DETECTED;
	}	
	_fmemcpy(&mInfo,DosMemoryBufferPtr,sizeof(VESAInfoStruct));
#else
	strcpy(mInfo.Signature,"VBE2");
	if(!VESALOinfo(&mInfo))
		return NOT_DETECTED;
#endif
	if(strncmp(mInfo.Signature,"VESA",4)==0)
	{
		if((mFlags=mode)&ENABLE_DOS_VAR)
			ReadDosVar();
		if(CheckVideoModeList())
			mFlags&=~BUG_MODE_LIST;
		else
			mFlags|=BUG_MODE_LIST;
		mFlags|=ENABLE_INIT;
		SaveAdapter();
		mCurrentMode=mOldMode;
		return OK;
	}
#ifdef __386__
	DPMIDOSFree(DosMemoryBufferPtr);
#endif
	return NOT_DETECTED;
}

void VESAdevice::close(void)
{
	if(mFlags)
	{
		RestoreAdapter();
		mFlags=0;
		if(mCurrentModeScreen!=NULL)
			delete mCurrentModeScreen;
		mCurrentModeScreen=NULL;
#ifdef __386__
	DPMIDOSFree(DosMemoryBufferPtr);
#endif
	}	
}

ostream& VESAdevice::adapterInfo(ostream& os)
{
	if(!mFlags)
	{
		os<<"VESA VBE not detected or not initialized"<<endl;
		return os;
	}
	
	os<<"VESA VBE version "<<int(hibyte(mInfo.Version))
		      	      <<"."<<int(lobyte(mInfo.Version));
	if(!(mFlags&ENABLE_2X))
		os<<"(force 1.xx)";
	os<<endl<<STRING_PTR(mInfo.OemStringPtr)<<endl
	  <<((mInfo.Capabilities&0x01)?"DAC switch":"DAC fixed")<<endl
	  <<((mInfo.Capabilities&0x02)?"VGA incompatible":"VGA compatible")<<endl
	  <<((mInfo.Capabilities&0x04)?"DAC fucked":"DAC cool")<<endl
	  <<"Total memory: "<<mInfo.TotalVideoMem*64<<"Kb\n";
	if(mInfo.Version>=0x200 && (mFlags&ENABLE_2X))
	  	os<<"Vendor name: "<<STRING_PTR(mInfo.VendorName)<<endl
	  	  <<"Product name: "<<STRING_PTR(mInfo.ProductName)<<endl
 	  	  <<"Product version: : "<<STRING_PTR(mInfo.ProductRev)<<endl
	  	  <<"Vendor info: : "<<mInfo.OemData<<endl;
       if(mFlags&BUG_MODE_LIST)
       		os<<"VESA bug of modelist detected\n";
       return os;
}

ostream& VESAdevice::modeInfo(ostream& os,int mode)
{
	VESAModeInfoStruct mi;
	if(GetModeInfo(&mi,mode)!=OK)
		return os;
	if((mi.ModeAttr&VESA_MODE_SUPPORTED)==0)
		return os;
	os<<hex<<setw(3)<<setfill('0')<<mode
	  <<(mi.ModeAttr&VESA_MODE_GRAPHICS?" graphics ":" text     ")
	  <<setw(4)<<setfill(' ')<<dec
	  <<mi.XResolution<<"x"<<setw(4)<<mi.YResolution
	  <<setw(4)<<int(mi.BitsPerPixel)<<" ";
	if(mInfo.Version>=0x200)
	  	os<<hex<<setfill('0')<<setw(8)<<mi.PhysBasePtr<<" ";
	os<<setw(22)<<setfill(' ')<<ColorInfo(mi.BitsPerPixel)
	  <<" Granul: "<<setw(5)<<dec<<mi.WinGranularity
 	  <<" WSize: "<<mi.WinSize<<endl;
	return os;
}

ostream& VESAdevice::about(ostream& os)
{
	adapterInfo(os);
	if(mFlags)
	{
		int i;
#ifdef __386__
		uint16* ModeList=(uint16*)RM_PTR(mInfo.VideoModePtr);
#else
		uint16 far* ModeList=(uint16 far*)mInfo.VideoModePtr;
#endif
		os<<"Available VESA modes:\n";
		if(mFlags&BUG_MODE_LIST)
			for(i=0x100;i<0x200;i++)
				modeInfo(os,i);
		else
                	for(i=0;ModeList[i]!=0xFFFF;i++)
				modeInfo(os,ModeList[i]);
	}
	return os;	
}

int VESAdevice::DACWidth(int w)
{
	if(w!=mDACWidth)
		mDACWidth=VESALOSetDAC(w);
	return mDACWidth;
}

int VESAdevice::SetWriteBank(int bank)
{
	if(bank==mBank[mWriteBankIndex])
		return OK;
	if(VESALOSetBank(bank,mWriteBankIndex))
	{
		mBank[mWriteBankIndex]=bank;
		return OK;
	}
	return LO_ERROR;
}

int VESAdevice::SetReadBank(int bank)
{
	if(bank==mBank[mReadBankIndex])
		return OK;
	if(VESALOSetBank(bank,mReadBankIndex))
	{
		mBank[mReadBankIndex]=bank;
		return OK;
	}
	return LO_ERROR;
}

int VESAdevice::SetMode(int mode,int rw,int rh)
{
// определяем текущие характеристики
	VESAModeInfoStruct vmi;
	int err=GetModeInfo(&vmi,mode);
	if(err!=OK)
	{
		debug("Error mode info:"<<hex<<mode<<endl);
		if(mode==0x13)
		{
		  vmi.ModeAttr=VESA_MODE_LFB|0x005D;
		  vmi.BitsPerPixel=8;
		  vmi.YResolution=200;
		  vmi.BytesPerScanLine=vmi.XResolution=320;
		  vmi.WinFuncPtr=NULL;
		  vmi.WinASeg=0xA000;
		  vmi.WinBSeg=0xA000;
		  vmi.WinGranularity=64;
		  vmi.WinSize=64;
		  vmi.NumberOfPlanes=1;
		  vmi.NumberOfBanks=1;
		  vmi.MemoryModel=4;
		  vmi.PhysBasePtr=NULL;
		  vmi.BankSize=0;
		  vmi.XCharSize=8;
		  vmi.YCharSize=8;
		  vmi.NumberOfImagePages=1;
		  //DirectColorModeInfo;    
		  //OffScreenMemOffset;
        	  //OffScreenMemSize;
		}
		else
		  return err;
	}
#ifndef __BORLANDC__
	if(mFlags&ENABLE_2X && vmi.ModeAttr&VESA_MODE_LFB)
		mode|=0x4000;
	else
#endif
		mode &=0x3FFF;
	if(vmi.BitsPerPixel!=8  &&
	   vmi.BitsPerPixel!=15 &&
	   vmi.BitsPerPixel!=16 &&
	   vmi.BitsPerPixel!=32)
	   	return MODE_UNAVAILABLE;
// переключаемся...
        if((mode&0x3FFF)!=0x13)
	  if(!VESALOSetMode(mode))
	    return LO_ERROR;
// настраиваем параметры...
	memcpy(&mModeInfo,&vmi,sizeof(VESAModeInfoStruct));
	if(mCurrentModeScreen!=NULL)
		delete mCurrentModeScreen;

	if(rw<=0)
		rw=vmi.XResolution;
	else
	{
		VESALineWidthInfo lwi;
		VESALOLineWidth(rw,0,&lwi);
		rw=lwi.pCount;
	}

	if(rh==0)
		rh=vmi.YResolution;
	else
		if(rh<0)
			rh=TotalMemory()/((rw*(vmi.BitsPerPixel+1)/4));		

	mCurrentModeScreen=NULL;
	if((mode&0x3FFF)==0x13)
	{
          mCurrentModeScreen=new vga13hScreen;
	  if(!((vga13hScreen*)mCurrentModeScreen)->open())
	  {
	    delete mCurrentModeScreen;
	    mCurrentModeScreen=NULL;
	    return MODE_UNAVAILABLE;
	  }
	  return OK;
	}
#ifdef __386__
	if((mCurrentMode=mode)&0x4000 && vmi.PhysBasePtr) // LFB-mode
	{
		void* lfb=(void*)DPMILinAddr(vmi.PhysBasePtr,TotalMemory());
		switch(vmi.BitsPerPixel)
		{
			case 8:  mDACWidth=VESALOGetDAC();
			     	 mCurrentModeScreen=new vfScreen8bit(rw,rh,lfb,this);
				 break;
			case 15:
			case 16:
			     	 mCurrentModeScreen=new vfScreen16bit(rw,rh,lfb,this);
				 break;
			case 32:
			     	 mCurrentModeScreen=new vfScreen32bit(rw,rh,lfb,this);
				 break;
		}	
	}
	else	// banked-mode
#endif
	{
		mBankPtr[0]=(void*)(long(vmi.WinASeg)<<4);
		mBankPtr[1]=(void*)(long(vmi.WinBSeg)<<4);
		mBank[0]=VESALOGetBank(0);
		mBank[1]=VESALOGetBank(1);
		mReadBankIndex=(vmi.WinAAttr&VESA_WIN_READ)?0:1;
		mWriteBankIndex=(vmi.WinAAttr&VESA_WIN_WRITE)?0:1;;
		switch(vmi.BitsPerPixel)
		{
			case 8:  mDACWidth=VESALOGetDAC();
			     	 mCurrentModeScreen=new vbScreen8bit(rw,rh,this);
				 break;
			case 15:
			case 16:
			     	 mCurrentModeScreen=new vbScreen16bit(rw,rh,this);
				 break;
			case 32:
			     	 mCurrentModeScreen=new vbScreen32bit(rw,rh,this);
				 break;
		}		
	}
	return mCurrentModeScreen==NULL?MODE_UNAVAILABLE:OK;
}

//////////////////////////////////////////////////////////////////////////////
// VESAScreen's
//////////////////////////////////////////////////////////////////////////////

template <class T> void vesaFlatScreen<T>::close(void)
{
	T::close();
}

template <class T> vesaFlatScreen<T>::~vesaFlatScreen(void)
{
	close();
}

template <class T> int vesaFlatScreen<T>::GetType(void) const
{
	return ID_VFSCREEN;
}

void vfScreen8bit::_PutPalette(int index,int count,const RGBStruct* palette)
{
	::SetPalette(index,count,palette);
}

void vfScreen8bit::_PutPalette(int index,int count,const RGBQUAD* palette)
{
	for(;count--;index++)
		::SetPalette(index,((long*)palette)[index]);
}

void vfScreen8bit::_GetPalette(int index,int count,RGBStruct* palette)
{
	::GetPalette(index,count,palette);
}

void vfScreen8bit::_GetPalette(int index,int count,RGBQUAD* palette)
{
	for(;count--;index++)
		((long*)palette)[index]=::GetPalette(index);
}

long vfScreen16bit::rgb(uchar red,uchar green,uchar blue)
{
	if(!IsValid())
		return 0;
	const VESAModeInfoStruct* mi=mDevice->GetModeInfo();
	red  >>=8-mi->RedMaskSize;
	green>>=8-mi->GreenMaskSize;
	blue >>=8-mi->BlueMaskSize;
	return (long(red)<<mi->RedFieldPosition)|
	       (long(green)<<mi->GreenFieldPosition) |
	       (long(blue)<<mi->BlueFieldPosition);
}

void vfScreen16bit::rgb(long color,uchar* red,uchar* green,uchar* blue)
{
	if(IsValid())
	{
		const VESAModeInfoStruct* mi=mDevice->GetModeInfo();
		*red=color>>mi->RedFieldPosition;
		*green=color>>mi->GreenFieldPosition;
		*blue=color>>mi->BlueFieldPosition;
		*red  <<=8-mi->RedMaskSize;
		*green<<=8-mi->GreenMaskSize;
		*blue <<=8-mi->BlueMaskSize;
	}
}

//////////////////////////////////////////////////////////////////////////////
// VESA banked Screen's

template <class T> BOOL vesaBankedScreen<T>::open(int w,int h,VESAdevice* dev)
{
	mDevice=dev;
	mbpSize=65536L/sizeof(T);
	mrBank=(T*)dev->GetReadBankAddr();
	mwBank=(T*)dev->GetWriteBankAddr();
	return Screen::open(w,h,sizeof(T)*8);
}

template <class T> void vesaBankedScreen<T>::close(void)
{
	Screen::close();
}

template <class T> vesaBankedScreen<T>::~vesaBankedScreen(void)
{
	close();
}
//// Работа с характеристиками экрана //////////////////////
template <class T> int vesaBankedScreen<T>::GetType(void) const
{
	return ID_VBSCREEN;
}
/// Графические примитивы /////////////////////////////////
template <class T> void vesaBankedScreen<T>::_PutPixel(int x,int y,long color)
{
	int addr=x+y*mWidth;
	mDevice->SetWriteBank(addr/mbpSize);
	mwBank[addr%mbpSize]=color;
}

template <class T> long vesaBankedScreen<T>::_GetPixel(int x,int y)
{
	int addr=x+y*mWidth;
	mDevice->SetReadBank(addr/mbpSize);
	return mwBank[addr%mbpSize];
}

template <class T> void vesaBankedScreen<T>::_vline(int x,int y,int h,long color)
{
	int addr=x+y*mWidth,bank;	
	mDevice->SetWriteBank(bank=addr/mbpSize);
	for(addr%=mbpSize;h--;)
	{
		mwBank[addr]=color;
		if((addr+=mWidth)>=mbpSize)
		{
			mDevice->SetWriteBank(++bank);
			addr-=mbpSize;
		}
	}
}

/// Графические примитивы ////////////////////////////////
template <class T> void vesaBankedScreen<T>::_PutImage(int x,int y,const tImage* image,const cRect* rect)
{
 	const T* data=(const T*)image->data;
 	int h=image->height,w=image->width,wd=w;
	if(y<rect->top)
 	{
  		if((h-=(rect->top-y))<=0)
    			return;
  		data+=(rect->top-y)*wd;
   		y=rect->top;
 	}
 	if(y+h>rect->bottom+1)
   	h=rect->bottom-y+1;
 	if(x+w>rect->right+1)
  	if((w=rect->right-x+1)<=0)
   		return;
 	int i,stx;
 	if(x<rect->left)
  		data+=(stx=rect->left-x);
 	else
  		stx=0;
 	int addr=y*mWidth+x+stx,bank;
	mDevice->SetWriteBank(bank=addr/mbpSize);
	addr%=mbpSize;
 	int  dx=w-stx,dxl=dx*sizeof(T);
	for(i=0;i++<h;y++,data+=wd)
 	{
  		const T* ldata=data;
		if(addr+dx<=mbpSize)
		{
   			memcpy((mwBank+addr),ldata,dxl);
			if((addr+=mWidth)>=mbpSize)
			{
				addr-=mbpSize;
				mDevice->SetWriteBank(++bank);	
			}
		}
		else
		{	
			int d1=mbpSize-addr,d2=d1*sizeof(T);
			memcpy((mwBank+addr),ldata,d2);
			mDevice->SetWriteBank(++bank);
			addr+=mWidth-mbpSize;
			memcpy(mwBank,(ldata+d1),dxl-d2);
  		}
 	}
}

template <class T> void vesaBankedScreen<T>::_GetImage(int x,int y,int w,int h,tImage* image,const cRect* rect)
{
 	T* data=(T*)image->data;
 	image->height=h;
	int wd=image->width=w;
	if(y<rect->top)
 	{
  		if((h-=(rect->top-y))<=0)
    			return;
  		data+=(rect->top-y)*wd;
   		y=rect->top;
 	}
 	if(y+h>rect->bottom+1)
   	h=rect->bottom-y+1;
 	if(x+w>rect->right+1)
  	if((w=rect->right-x+1)<=0)
   		return;
 	int i,stx;
 	if(x<rect->left)
  		data+=(stx=rect->left-x);
 	else
  		stx=0;
 	int addr=y*mWidth+x+stx,bank;
	mDevice->SetReadBank(bank=addr/mbpSize);
	addr%=mbpSize;
 	int  dx=w-stx,dxl=dx*sizeof(T);
	for(i=0;i++<h;y++,data+=wd)
 	{
  		T* ldata=data;
		if(addr+dx<=mbpSize)
		{
   			memcpy(ldata,(mrBank+addr),dxl);
			if((addr+=mWidth)>=mbpSize)
			{
				addr-=mbpSize;
				mDevice->SetReadBank(++bank);	
			}
		}
		else
		{	
			int d1=mbpSize-addr,d2=d1*sizeof(T);
			memcpy(ldata,(mrBank+addr),d2);
			mDevice->SetReadBank(++bank);
			addr+=mWidth-mbpSize;
			memcpy((ldata+d1),mrBank,dxl-d2);
  		}
 	}
}

void vbScreen8bit::_PutPalette(int index,int count,const RGBStruct* palette)
{
	::SetPalette(index,count,palette);
}

void vbScreen8bit::_PutPalette(int index,int count,const RGBQUAD* palette)
{
	for(;count--;index++)
		::SetPalette(index,((long*)palette)[index]);
}

void vbScreen8bit::_GetPalette(int index,int count,RGBStruct* palette)
{
	::GetPalette(index,count,palette);
}

void vbScreen8bit::_GetPalette(int index,int count,RGBQUAD* palette)
{
	for(;count--;index++)
		((long*)palette)[index]=::GetPalette(index);
}

// other
long vbScreen8bit::rgb(uchar red,uchar green,uchar blue)
{
#define COLORLEN(c,r,g,b)	(sqr(long(c.red)-long(r))+sqr(long(c.green)-long(g))+sqr(long(c.blue)-long(b)))
	RGBStruct rgb;
	int imin=0;
	GetPalette(0,&rgb);
	long lmin=COLORLEN(rgb,red,green,blue),l;
	for(int i=1;i<256;i++)
	{
		GetPalette(i,&rgb);
		if((l=COLORLEN(rgb,red,green,blue))<lmin)
		{
			lmin=l;
			imin=i;
		}
	}
#undef COLORLEN
	return imin;
}

void vbScreen8bit::rgb(long color,uchar* red,uchar* green,uchar* blue)
{
	RGBStruct rgb;
	GetPalette(color,&rgb);
	*red=rgb.red;
	*green=rgb.green;
	*blue=rgb.blue;
}

void vbScreen8bit::_hline(int x,int y,int w,long color)
{
	int addr=x+y*mWidth,bank;	
	mDevice->SetWriteBank(bank=addr/mbpSize);
	if(w+(addr%=mbpSize)<=mbpSize)
   		memset((mwBank+addr),color,w);
 	else
	{
  		int dx=mbpSize-addr;
		memset((mwBank+addr),color,dx);
		mDevice->SetWriteBank(bank+1);
  		memset(mwBank,color,w-dx);
	}
}


void vbScreen16bit::_hline(int x,int y,int w,long color)
{
	int addr=x+y*mWidth,bank;	
	mDevice->SetWriteBank(bank=addr/mbpSize);
	if(w+(addr%=mbpSize)<=mbpSize)
   		memsetw((mwBank+addr),color,w);
 	else
	{
  		int dx=mbpSize-addr;
		memsetw((mwBank+addr),color,dx);
		mDevice->SetWriteBank(bank+1);
  		memsetw(mwBank,color,w-dx);
	}
}

long vbScreen16bit::rgb(uchar red,uchar green,uchar blue)
{
	if(!IsValid())
		return 0;
	const VESAModeInfoStruct* mi=mDevice->GetModeInfo();
	red  >>=8-mi->RedMaskSize;
	green>>=8-mi->GreenMaskSize;
	blue >>=8-mi->BlueMaskSize;
	return (long(red)<<mi->RedFieldPosition)|
	       (long(green)<<mi->GreenFieldPosition) |
	       (long(blue)<<mi->BlueFieldPosition);
}

void vbScreen16bit::rgb(long color,uchar* red,uchar* green,uchar* blue)
{
	if(IsValid())
	{
		const VESAModeInfoStruct* mi=mDevice->GetModeInfo();
		*red=color>>mi->RedFieldPosition;
		*green=color>>mi->GreenFieldPosition;
		*blue=color>>mi->BlueFieldPosition;
		*red  <<=8-mi->RedMaskSize;
		*green<<=8-mi->GreenMaskSize;
		*blue <<=8-mi->BlueMaskSize;
	}
}

void vbScreen32bit::_hline(int x,int y,int w,long color)
{
	int addr=x+y*mWidth,bank;	
	mDevice->SetWriteBank(bank=addr/mbpSize);
	if(w+(addr%=mbpSize)<=mbpSize)
   		memsetd((mwBank+addr),color,w);
 	else
	{
  		int dx=mbpSize-addr;
		memsetd((mwBank+addr),color,dx);
		mDevice->SetWriteBank(bank+1);
  		memsetd(mwBank,color,w-dx);
	}
}

long vbScreen32bit::rgb(uchar red,uchar green,uchar blue)
{
	return ulong(blue)|(ulong(green)<<8)|(ulong(red)<<16);
}

void vbScreen32bit::rgb(long color,uchar* red,uchar* green,uchar* blue)
{
	*red=color>>16;
	*green=color>>8;
	*blue=color;
}
