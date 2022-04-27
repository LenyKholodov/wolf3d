#ifndef __VESA_TYPES_H__
#define __VESA_TYPES_H__

#include <grtypes.h>

#pragma pack(1);

typedef struct  
{
  //--------- VBE 1.x
        char         Signature[4];   //  0h: 4 signature bytes: must be 'VESA'
        uint16       Version;        //  4h: VESA version number
        ulong 	     OemStringPtr;       //  6h: Pointer to OEM string
        ulong        Capabilities;       // 0Ah: Capabilities of the video environment
        ulong	     VideoModePtr;       // 0Eh: list of supported Super VGA modes
        uint16       TotalVideoMem;      // 12h: total amount of video memory in 64K blocks

  //--------- VBE 2.0
        uint16       OemSoftVersion;     // 14h: OEM software version
        ulong	     VendorName;         // 16h: Pointer to vendor name
        ulong	     ProductName;        // 1Ah: Pointer to product name
	ulong	     ProductRev;      // 1Eh: Pointer to product revision string
        char         VIBfiller[222];     // 22h: reserved
        char         OemData[256]; //100h: OEM scratchpad
} VESAInfoStruct;

typedef struct 
{
	WORD    ModeAttr;
    	char    WinAAttr,
    		WinBAttr;
    	WORD    WinGranularity;         
    	WORD    WinSize;
    	WORD    WinASeg;
    	WORD    WinBSeg;
    	long    WinFuncPtr;
    	WORD    BytesPerScanLine;
    	WORD    XResolution;
    	WORD    YResolution;
    	char    XCharSize;
    	char    YCharSize;
    	char    NumberOfPlanes;
    	char    BitsPerPixel;
    	char    NumberOfBanks;          
    	char    MemoryModel;            
    	char    BankSize;               
    	char    NumberOfImagePages;
    	char    reserved1;                   
    	char    RedMaskSize;            
    	char    RedFieldPosition;       
    	char    GreenMaskSize;          
    	char    GreenFieldPosition;     
    	char    BlueMaskSize;
    	char    BlueFieldPosition;      
    	char    RsvdMaskSize;           
    	char    RsvdFieldPosition;      
    	char    DirectColorModeInfo;    
    /* VBE 2.0 extensions */
       	long    PhysBasePtr;            
        long    OffScreenMemOffset;     
        WORD    OffScreenMemSize;
        char    reserved2[206];              
} VESAModeInfoStruct;

#pragma pack()

typedef struct 
{
	int bCount,		// количество байт в строке
	    pCount,		// длина строки в точках растра
	    MaxNumber;          // максимальный номер строки растра
} VESALineWidthInfo;

#define VESA_MODE_SUPPORTED	   0x01 // видеорежим поддерживается
#define VESA_MODE_BIOS_SUPPORTED   0x04 // стандартные функции вывода поддерживаются BIOS
#define VESA_MODE_COLOR		   0x08 // цветной режим
#define VESA_MODE_GRAPHICS	   0x10 // графический режим
#define VESA_MODE_VGA_INCOMPATIBLE 0x20 // режим, несовместимый с VGA
#define VESA_MODE_WIN_DISABLE      0x40 // оконный режим, совместимый с VGA, недоступен
#define VESA_MODE_LFB		   0x80 // LFB доступен

#define VESA640x400x8		   0x0100
#define VESA640x400x8LFB	   0x4100
#define VESA640x480x8		   0x0101
#define VESA640x480x8LFB	   0x4101
#define VESA800x600x8		   0x0103
#define VESA800x600x8LFB	   0x4103
#define VESA1024x768x8		   0x0105
#define VESA1024x768x8LFB	   0x4105
#define VESA320x200x15		   0x010D
#define VESA320x200x15LFB	   0x410D
#define VESA320x200x16		   0x010E
#define VESA320x200x16LFB	   0x410E
#define VESA320x200x24		   0x010F
#define VESA320x200x24LFB	   0x410F
#define VESA640x480x15		   0x0110
#define VESA640x480x15LFB	   0x4110
#define VESA640x480x16		   0x0111
#define VESA640x480x16LFB	   0x4111
#define VESA640x480x24		   0x0112
#define VESA640x480x24LFB	   0x4112
#define VESA800x600x15		   0x0113
#define VESA800x600x15LFB	   0x4113
#define VESA800x600x16		   0x0114
#define VESA800x600x16LFB	   0x4114
#define VESA800x600x24		   0x0115
#define VESA800x600x24LFB	   0x4115
#define VESA1024x768x15		   0x0116
#define VESA1024x768x15LFB	   0x4116
#define VESA1024x768x16		   0x0117
#define VESA1024x768x16LFB	   0x4117
#define VESA1024x768x24		   0x0118
#define VESA1024x768x24LFB	   0x4118

#define VESA_WIN_MOVE	0x01
#define VESA_WIN_READ	0x02
#define VESA_WIN_WRITE	0x04

#endif