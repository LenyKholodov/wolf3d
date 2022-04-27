//  WATCOM C   DPMI memory service
#ifndef __DPMI_H__
#define __DPMI_H__

#include <dpmi_t.h>
#include <error.h>
#include <misc.h>
#include <i86.h>

#if !defined(__WATCOMC__) || !defined(__386__)
#error This library worked only with Watcom C++!
#endif

const int DPMI_ERROR_ID=ERROR_ID+0x0100;

#define DpmiRealModeInt DPMIRMInt

class DPMIError: public ErrorPro
{
public:
        int fnNo;
        WORD Selector,
             Attr,
             DosError;
        ulong LinearAddr,
              Limit;
        int intrNo;
        ulong RmAddr;
        void __far* pmPtr;
        DPMIError(void):ErrorPro(0) { IdCode=DPMI_ERROR_ID; }
        virtual void dump(ostream&) const;
};

enum
{
        DPMI_ALLOC_SELECTOR=0x0000,
        DPMI_FREE_SELECTOR,
        DPMI_RM_SELECTOR,
        DPMI_INC_SELECTOR,
        DPMI_GET_LINEAR_ADDR=0x0006,
        DPMI_SET_LINEAR_ADDR,
        DPMI_SET_LIMIT,
        DPMI_SET_ACCESS,
        DPMI_CREATE_ALIAS_CS,
        DPMI_GET_DT_RECORD,
        DPMI_SET_DT_RECORD,
        DPMI_ALLOC_RESERVED_SELECTOR,
        DPMI_DOS_ALLOC=0x0100,
        DPMI_DOS_FREE,
        DPMI_DOS_REALLOC,
        DPMI_GET_RM_INT=0x0200,
        DPMI_SET_RM_INT,
        DPMI_GET_EXCEPTION,
        DPMI_SET_EXCEPTION,
        DPMI_GET_PM_INT,
        DPMI_SET_PM_INT,
        DPMI_CALL_RM_INT=0x0300,
        DPMI_CALL_RM_FPROC,
        DPMI_CALL_RM_IPROC,
        DPMI_MEM_INFO=0x500,
        DPMI_MEM_ALLOC,
        DPMI_MEM_FREE,
        DPMI_MEM_REALLOC,
        DPMI_PAGE_LOCK=0x600,
        DPMI_PAGE_UNLOCK
};

// only for dos4gw extender model
inline void __near* RM_PTR(ulong ptr)
                   { return (void __near*)(((ptr>>12)&0xFFFF0)+(ptr&0xFFFF)); }

/**********************************************************************
*          Local�Descriptor�Table�Management�Services (00xx)              *
**********************************************************************/

//                ��⠭����� ���� �ࠢ ����㯠 � ���ਯ��
// � ���७�� ���� �ࠢ ����㯠 ���訩 ���
//    (��� 7) - ��� �࠭��୮��, �������� ���� �।���
//               (0 - �।�� � �����, 1 - �।�� � ��࠭���).
//    ��� 6 㪠�뢠�� ०�� ࠡ��� ������
//        (0 - 16-ࠧ�來�, 1 - 32-ࠧ�來�).
//    ��� 5 � 2-3 ������ ���� ࠢ�� 0.
//    ��� 4 ����� ᮤ�ঠ�� �� ���祭��.
BOOL DPMISetSelectorAccess(uint16 sel,uint16 attr);

// ������� ���ਯ�� � ⠡��� LDT
BOOL DPMIAllocSelector(uint16 *sel,int count=1);
inline BOOL DpmiAllocSelector(uint16 *sel)
       {    if(DPMIAllocSelector(sel)) return DPMISetSelectorAccess(*sel,DPMI_DEFAULT); return FALSE; }

// �᢮������ ���ਯ�� � ⠡��� LDT
BOOL DPMIFreeSelector(uint16 sel);
inline BOOL DpmiFreeSelector(uint16 sel)
       { return DPMIFreeSelector(sel); }

// �८�ࠧ������ ᥣ���� ���� ॠ�쭮�� ०��� � ���ਯ��.
// ���ਯ��, ᮧ����� ������ �㭪樥�, �� ����� ���� �᢮�����.
BOOL DPMIMakeRMSelector(uint16 seg,uint16 *sel);

// �᫨ �� ᮧ���� ��᪮�쪮 ���ਯ�஢ � LDT �� ����� �㭪樨 0h,
// � �⮩ �㭪樥� �㤥� ������� ⮫쪮 ���� ᥫ����,
// ᮮ⢥�����騩 ��ࢮ�� ���ਯ��� � ᮧ������ ���ᨢ�.
// �� �㭪�� �����頥� ���祭��, ���஥ �� �㤥� �ਡ������ �
// �������� ᥫ����� ��� ���᫥��� ��⠫��� ᥫ���஢ � ���ᨢ�.
int DPMIIncrementSelector(void);

// ������� ������ ���� ᥣ����
BOOL DPMIGetSelectorBase(uint16 sel,ulong *addr);
inline ulong DpmiGetSelectorBase(uint16 sel)
                    { ulong addr; return DPMIGetSelectorBase(sel,&addr)?addr:0; }

// ��⠭����� ������ ���� ᥣ����. �� ����� ������஢��� ⮫쪮 �
// ���ਯ���, ����� ᮧ���� ��� �ணࠬ��.                                     *
BOOL DPMISetSelectorBase(uint16 sel,ulong linAddr);
#define DpmiSetSelectorBase(x,y) DPMISetSelectorBase(x,y)

// ��⠭����� �।�� ᥣ����
BOOL DPMISetSelectorLimit(uint16 sel,ulong limit);
// ����� "㬭��" �����. �� ᫨誮� ����讬 �।��� ����� ��ਡ���
// ���ਯ��. � �⫨稥 �� �।��饩 �������� ࠧ��� ⮫쪮 � �����
BOOL DPMISetSelectorLimitX(uint16 sel,ulong limit);
#define DpmiSetSelectorLimit(x,y) DPMISetSelectorLimitX(x,y*4096)

// �������� ����᭮�� ���ਯ�� ��� ᥣ���� ����
BOOL DPMIMakeSelectorAlias(uint16 sel,uint16 *asel);

// ������� ���ਯ�� 㪠������� ᥫ����
BOOL DPMIGetDescriptor(uint16 sel,DescriptorStruct __far* dsr);

// ��⠭����� ���ਯ�� 㪠������� ᥫ����
BOOL DPMISetDescriptor(uint16 sel,DescriptorStruct __far* dsr);

// �뤥���� ������� ���ਯ�� � ⠡��� LDT
// ��� �⮩ �� � ⠡��� ��� ��१�ࢨ஢��� ���� 16 ���ਯ�஢
BOOL DPMIAllocDescriptor(uint16 sel);


/**********************************************************************
*             DOS�Memory�Management�Services (01xx)                      *
**********************************************************************/
// ������� ���� ����� �� �㫠 ᢮������ ����� DOS. �᫨ ࠧ���
// ����襭���� ����� �ॢ�蠥� 64 ��������, �㭪�� ᮧ���� ���ᨢ
// ���ਯ�஢ � ������� ������ ᥫ����.
void __far* DPMIDOSAlloc(ulong size,uint16 *sel,uint16 *rseg);

inline void __far* DPMIDOSAlloc(ulong size,uint16 *rseg)
       { return DPMIDOSAlloc(size,NULL,rseg); }

#define DpmiAllocRealSeg(x,y) DPMIDOSAlloc(x,y)

// �᢮������ ���� �����, ����� �� �㫠 DOS
BOOL DPMIDOSFree(uint16 sel);
#define DpmiFreeRealSeg(x) DPMIDOSFree(x)
inline void DpmiFreeRealSeg(void __far* ptr) { if(ptr!=NULL) DPMIDOSFree(FP_SEG(ptr)); }

// �������� ࠧ��� ����� �����, ����祭���� �� �㫠 DOS
BOOL DPMIDOSRealloc(uint16 sel,ulong newsize);

/**********************************************************************
*                         Interrupt Services (02xx)                      *
**********************************************************************/

// ������� ����� ���뢠��� ॠ�쭮�� ०���
ulong DPMIRMGetInterrupt(unsigned i);

// ��⠭����� ����� ���뢠��� ॠ�쭮�� ०���
BOOL DPMIRMSetInterrupt(unsigned i,ulong addr);

// ������� ���� ��ࠡ��稪� �᪫�祭�� � ������� ����஬
void __far* DPMIGetExceptionHandler(unsigned i);

// ��⠭���� ���� ��ࠡ��稪� �᪫�祭�� � ������� ����஬
BOOL DPMISetExceptionHandler(unsigned i,void __far* ptr);

// ������� ����� ���뢠��� ����񭭮�� ०���
InterruptHandlerPtr DPMIGetInterrupt(unsigned n);

// ��⠭����� ����� ���뢠��� ����񭭮�� ०���
BOOL DPMISetInterrupt(unsigned i,InterruptHandlerPtr ptr);

/**********************************************************************
*                         Translation�Services (03xx)                      *
**********************************************************************/

// ������ ���뢠��� ॠ�쭮�� ०���
BOOL DPMIRMInt(unsigned i,RMREGS *rmregs,RMREGS __far** rgout=NULL);

// �맮� ��楤��� ॠ�쭮�� ०���, �����稢��饩�� �������� RET FAR
BOOL DPMIRMCall(RMREGS *rmregs);

// �맮� ��楤��� ॠ�쭮�� ०���, �����稢��饩�� �������� IRET FAR
BOOL DPMIRMCalliret(RMREGS *rmregs);


/**********************************************************************
*                        Memory Management Services                    *
**********************************************************************/

// ������� ���ଠ�� � ᢮������ �����
BOOL DPMIFreeMemInfo(FreeMemInfo *meminfo);
void DPMIDumpFreeMemInfo(ostream& os,FreeMemInfo* mi);
inline void DPMIDumpFreeMemInfo(ostream& os)
       {    FreeMemInfo mi; DPMIFreeMemInfo(&mi); DPMIDumpFreeMemInfo(os,&mi); }

// ����� �����
BOOL DPMILinearAlloc(ulong size,ulong* addr,ulong* handle);
void __far* DPMIalloc(ulong size,ulong *handle);
inline void __far* DpmiMemAlloc(ulong size,ulong *handle)
       { return DPMIalloc(size,handle);                 }

// �᢮��������
BOOL DPMILinearFree(ulong handle);
inline BOOL DPMIfree(void __far *ptr,ulong handle)
       { return DPMIFreeSelector(FP_SEG(ptr)) && DPMILinearFree(handle); }
#define DpmiMemFree(x,y) DPMIfree(x,y)

// ��������� ࠧ���
BOOL DPMILinearRealloc(ulong size,ulong* addr,ulong* handle);
inline BOOL DPMIrealloc(ulong size,void __far* ptr,ulong *handle)
       {
                ulong laddr;
                if(!DPMILinearRealloc(size=(size+0xFFF)&0xFFFFF000,&laddr,handle))
                        return FALSE;
                if(!DPMISetSelectorBase(FP_SEG(ptr),laddr))
                        return FALSE;
                return DPMISetSelectorLimitX(FP_SEG(ptr),size-1);
        }

// ������ ���ଠ�� � �����
void DPMITotalMemInfo(TotalMemInfo *meminfo);

/**********************************************************************
*                Page Locking Services ( 06xx)                              *
**********************************************************************/

// ��䨪�஢��� �������� ������� �����
BOOL DPMIlock(ulong addr,ulong size);

// ���䨪�஢���� ����� �����
BOOL DPMIunlock(ulong addr,ulong size);

/**********************************************************************
*                �८�ࠧ������ 䨧��᪮�� ���� � �������          *
**********************************************************************/
ulong DPMILinAddr(ulong addr,ulong size);

/*inline BOOL  DPMIMapPhysical2Linear(ulong addr,ulong size,uint16 sel)
{
        if(DPMISetSelectorBase(sel,DPMILinAddr(addr,size)))
                if(DPMISetSelectorAccess(sel,0xCF93))
                        return DPMISetSelectorLimitX(sel,size);
        return FALSE;
}

inline void __far* DPMIMapPhysical2Linear(ulong addr,ulong size)
{
        uint16 sel;
        if(DPMIAllocSelector(&sel))
        {
                if(DPMIMapPhysical2Linear(addr,size,sel))
                        return MK_FP(sel,0);
                DPMIFreeSelector(sel);
        }
        return NULL;
}*/


#ifdef __3S__
#pragma library ("vm3s.lib")
#endif

#ifdef __3R__
#pragma library ("vm3r.lib")
#endif

#endif