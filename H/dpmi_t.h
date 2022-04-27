#ifndef __DPMI_TYPES_H__
#define __DPMI_TYPES_H__

#include <types.h>

/**********************************************************************
* 		             Типы		                      *
**********************************************************************/

#pragma pack(1);

typedef struct
{
	WORD limit;		// предел сегмента
        WORD base_lo; 		// младшее слово 24-битового физического адреса сегмента
        BYTE base_hi; 		// старший байт 24-битового физического адреса сегмента
        BYTE access; 		// поле доступа
        BYTE reserved; 		// зарезервировано, для сегментов
                                // процессора i80286 должно быть равно нулю
	BYTE base_hi2;		// 386 only!
} DescriptorStruct;

typedef struct _FreeMemInfo
{
	ulong AvailBytes;   // largest free block, in bytes
	ulong UnlockedPgs;  // pages available if left unlocked
	ulong LockedPgs;    // pages available if locked
	ulong TotLinPgs;    // linear address space size, in pages
	ulong TotUnlkPgs;   // number of unlocked pages
	ulong TotFreePgs;   // number of free pages
	ulong TotPhysPgs;   // number of physical pages
	ulong FreeLinPgs;   // free linear address space, in pages
	ulong TotSwapPgs;   // size of page swapping file, in pages
	ulong res[3];       //  reserved
} FreeMemInfo;

typedef struct _TotalMemInfo
{
       ulong AlcPhys;     // allocated bytes of physical memory
       ulong AlcVirt;     // allocated bytes of virtual memory
       ulong AvlVirt;     // available bytes of virtual memory
       ulong AlcVirtTask; // allocd virtual mem for this virt mach
       ulong AvlVirtTask; // available virt mem for this virt mach
       ulong AlcVirtClnt; // allocated virtual mem for this client
       ulong AvlVirtClnt; // available virtual mem for this client
       ulong TotLokdClnt; // total locked bytes for this client
       ulong MaxLokdClnt; // maximum locked bytes for this client
       ulong MaxLinAddr;  // highest linear addr avail to client
       ulong LargestFree; // size of largest free block (bytes)
       ulong MinAlloc;    // minimum allocation unit (bytes)
       ulong Alignment;   // allocation unit size (bytes)
       ulong res[19];      //  reserved
} TotalMemInfo;

typedef struct _xRMREGS
{
	ulong edi;
    	ulong esi;
    	ulong ebp;
    	ulong reserved_by_system;
    	ulong ebx;
    	ulong edx;
    	ulong ecx;
    	ulong eax;
    	short cflag;
    	short es,ds,fs,gs,ip,cs,sp,ss;
} xRMREGS;

typedef struct _wRMREGS
{
	ushort di, _1;
    	ushort si, _2;
    	ushort bp, _3;
    	ulong reserved_by_system;
    	ushort bx, _4;
    	ushort dx, _5;
    	ushort cx, _6;
    	ushort ax, _7;
    	short cflag;
    	short es,ds,fs,gs,ip,cs,sp,ss;
} wRMREGS;

typedef struct _hRMREGS
{
	ushort di, _1;
    	ushort si, _2;
    	ushort bp, _3;
    	ulong reserved_by_system;
    	uchar bl, bh; ushort _4;
    	uchar dl, dh; ushort _5;
    	uchar cl, ch; ushort _6;
    	uchar al, ah; ushort _7;
    	short cflag;
    	short es,ds,fs,gs,ip,cs,sp,ss;
} hRMREGS;

typedef union _RMREGS 
{
      xRMREGS x;
      wRMREGS w;
      hRMREGS h;
} RMREGS;


typedef struct 
{
	BYTE major,
	     minor;
	WORD attr;
	BYTE cpu,
	     irq0,
	     irq8;
} DPMIVerStruct;

#pragma pack();

/**********************************************************************
* 		             Константы		                      *
**********************************************************************/
// Константы прав доступа
const unsigned 
// бит гранулярности, интерпретация поля предела
// (0 - предел в байтах, 1 - предел в страницах).    
      DPMI_PAGE_GRANULARITY=0x0080,
// Бит 6 указывает режим работы процессора
// (0 - 16-разрядный, 1 - 32-разрядный).
      DPMI_32=0x0040,
      DPMI_DESCRIPTOR_DATA=0x0010,
//    [bit5:bit4]=10b
      DPMI_DESCRIPTOR_CODE=0x0018,
//    [bit5:bit4]=11b
//    DPMI_DESCRIPTOR_SYSTEM=0x0000
//    bit4=0b
      DPMI_SEG_WRITABLE=0x02,
      DPMI_SEG_READABLE=0x02,
      DPMI_SEG_PRESENT=0x80,
      DPMI_DEFAULT=0x8000|DPMI_PAGE_GRANULARITY|
      			  DPMI_DESCRIPTOR_DATA|
			  DPMI_SEG_WRITABLE;


#endif