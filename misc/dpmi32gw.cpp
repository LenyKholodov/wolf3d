#include <dpmi32gw.h>
#include <iomanip.h>
#include <debug.h>

DPMIError dpmiError;

void DPMIError::dump(ostream& os) const
{
#define HEXFILL(x)        hex<<setfill('0')<<setw(x)
        IOS_FMTFLAGS sflags=os.setf(ios::uppercase);
        os<<"DPMI error (fn #"<<fnNo<<"): ";
        switch(fnNo)
        {
                case DPMI_ALLOC_SELECTOR: os<<"Can't allocate selector"; break;
                case DPMI_FREE_SELECTOR:  os<<"Can't free selector "
                                                 <<HEXFILL(4)<<Selector; break;
                case DPMI_RM_SELECTOR:          os<<"Can't allocate selector for rm-seg "
                                                 <<HEXFILL(4)<<Selector; break;
                case DPMI_GET_LINEAR_ADDR:os<<"GetLinearAddr: Invalid selector "
                                                 <<HEXFILL(4)<<Selector; break;
                case DPMI_SET_LINEAR_ADDR:os<<"SetLinearAddr: Invalid selector("
                                                 <<HEXFILL(4)<<Selector
                                            <<") or LinearAddr ("
                                            <<HEXFILL(8)<<LinearAddr<<")."; break;
                case DPMI_SET_LIMIT:          os<<"SetLimit: Invalid selector("
                                                 <<HEXFILL(4)<<Selector
                                            <<") or limit ("
                                            <<HEXFILL(8)<<Limit<<")."; break;
                case DPMI_SET_ACCESS:          os<<"SetAccess: Invalid selector("
                                                 <<HEXFILL(4)<<Selector
                                            <<") or attr ("
                                            <<HEXFILL(4)<<Attr<<")."; break;
                case DPMI_CREATE_ALIAS_CS:os<<"CreateAlias: Selector "
                                                 <<HEXFILL(4)<<Selector
                                            <<" is not a valid code segment."; break;
                case DPMI_GET_DT_RECORD:  os<<"GetDtRecord: Selector "
                                                 <<HEXFILL(4)<<Selector
                                            <<" is invalid or unallocated."; break;
                case DPMI_SET_DT_RECORD:  os<<"SetDtRecord: Selector "
                                                 <<HEXFILL(4)<<Selector
                                            <<" is invalid."; break;
                case DPMI_ALLOC_RESERVED_SELECTOR:
                                               os<<"AllocReservedSelector: Selector "
                                                 <<HEXFILL(4)<<Selector
                                            <<" is already in use or not a valid LDT descriptor."; break;
                case DPMI_DOS_ALLOC:          os<<"DosAlloc: "<<DosErrStr(DosError)
                                                 <<". Selector: "<<HEXFILL(4)<<Selector; break;
                case DPMI_DOS_FREE:          os<<"DosFree: "<<DosErrStr(DosError)
                                                 <<". Selector: "<<HEXFILL(4)<<Selector; break;
                case DPMI_DOS_REALLOC:          os<<"DosRealloc: "<<DosErrStr(DosError)
                                                 <<". Selector: "<<HEXFILL(4)<<Selector; break;
                case DPMI_SET_RM_INT:     os<<"SetRMInt: vector "<<HEXFILL(2)<<intrNo
                                                 <<" addr "<<HEXFILL(4)<<hiword(RmAddr)
                                            <<':'<<HEXFILL(4)<<loword(RmAddr); break;
                case DPMI_GET_EXCEPTION:  os<<"GetException: exception "
                                                 <<HEXFILL(2)<<intrNo; break;
                case DPMI_SET_EXCEPTION:  os<<"SetException: exception "
                                                 <<HEXFILL(2)<<intrNo<<" addr "
                                            <<HEXFILL(4)<<FP_SEG(pmPtr)<<':'
                                            <<HEXFILL(8)<<FP_OFF(pmPtr); break;
                case DPMI_SET_PM_INT:          os<<"SetPMInt: vector "
                                                 <<HEXFILL(2)<<intrNo<<" addr "
                                            <<HEXFILL(4)<<FP_SEG(pmPtr)<<':'
                                            <<HEXFILL(8)<<FP_OFF(pmPtr); break;
                case DPMI_CALL_RM_INT:          os<<"CallRMInt: vector "
                                                 <<HEXFILL(2)<<intrNo; break;
                case DPMI_CALL_RM_FPROC:  os<<"Failed calling to RM-procedure"; break;
                case DPMI_CALL_RM_IPROC:  os<<"Failed calling to RM-iret-procedure"; break;
                case DPMI_MEM_INFO:       os<<"Failed query about free memory"; break;
                case DPMI_MEM_ALLOC:          os<<"DPMIAlloc: can't alloc "<<Limit<<" bytes memory"; break;
                case DPMI_MEM_FREE:       os<<"DPMIFree: invalid handle "<<HEXFILL(8)<<LinearAddr; break;
                case DPMI_MEM_REALLOC:          os<<"DPMIRealloc: invalid handle "
                                                 <<HEXFILL(8)<<LinearAddr<<" or not enough memory"; break;
                case DPMI_PAGE_LOCK:      os<<"Can't lock pages at "
                                                 <<HEXFILL(8)<<LinearAddr<<" addr, "
                                            <<HEXFILL(8)<<Limit<<" size"; break;
                case DPMI_PAGE_UNLOCK:    os<<"Can't unlock pages at "
                                                 <<HEXFILL(8)<<LinearAddr<<" addr, "
                                            <<HEXFILL(8)<<Limit<<" size"; break;
        }
        os.flags(sflags);
}

/**********************************************************************
*                ������� ���ਯ�� � ⠡��� LDT                      *
* ��� �믮������ �㭪樨 ����室��� �맢��� ���뢠��� INT 31h        *
* �������� �� �室�:                                                  *
* AX      0000h                                                       *
* CX      ������⢮ ᮧ�������� ���ਯ�஢.                        *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
* AX      ������ ᥫ����.                                           *
* �᫨ �ணࠬ�� �����뢠�� ��᪮�쪮 ᥫ���஢,                     *
* AX ᮤ�ন� ���� ᥫ���� �� ᮧ������� ���ᨢ�.                  *
* ��� ����祭�� ��⠫��� ᥫ���஢ ����室��� ��ᯮ�짮������       *
* �㭪樥� 00003h.                                                    *
**********************************************************************/
BOOL DPMIAllocSelector(uint16 *sel,int count)
{
        union REGS r;
            r.w.ax=0x0000;
            r.w.cx=count;
            int386(0x31, &r, &r);
        if(r.x.cflag)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_ALLOC_SELECTOR;
                return FALSE;
        }
        *sel=r.w.ax;
        return TRUE;
}

/**********************************************************************
*                �᢮������ ���ਯ�� � ⠡��� LDT                   *
* ��� �믮������ �㭪樨 ����室��� �맢��� ���뢠��� INT 31h        *
* �������� �� �室�                                                   *
* AX      0001h                                                       *
* BX      ������� �᢮���������� ���ਯ��.                        *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
**********************************************************************/
BOOL DPMIFreeSelector(uint16 sel)
{
        union REGS r;
            r.x.eax=0x0001;
            r.x.ebx=sel;
            int386(0x31,&r,&r);
        if(r.x.cflag)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_FREE_SELECTOR;
                dpmiError.Selector=sel;
                return FALSE;
        }
            return TRUE;
}

/**********************************************************************
*                �८�ࠧ������ ᥣ���� � ���ਯ��                  *
* �८�ࠧ������ ᥣ���⭮� ���������� ���� ॠ�쭮�� ०���        *
* � ���ਯ��.                                                       *
* ��� �믮������ �㭪樨 ����室��� �맢��� ���뢠��� INT 31h        *
* �������� �� �室�                                                   *
* AX      0002h                                                       *
* BX      �������� ���� ॠ�쭮�� ०���.                          *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
* AX      �������, ᮮ⢥�����騩 ���ਯ���,                      *
*           ᮧ������� ��� ����樨 ᥣ���� ॠ�쭮�� ०���.         *
*                                                                     *
* ���ਯ��, ᮧ����� ������ �㭪樥�, �� ����� ���� �᢮�����,    *
* ���⮬� ��� �㭪�� ����� ��� �ᯮ�짮���� ��� ����樨          *
* ⠪�� �����⥩ �����, ����� ���� �㦭� �� ��� ���殮���       *
* ࠡ��� �ணࠬ�� (������� ������ BIOS, ������ ���������஫���...). *
**********************************************************************/
BOOL DPMIMakeRMSelector(uint16 seg,uint16 *sel)
{
        union REGS r;
            r.w.ax=0x0002;
            r.w.bx=seg;
            int386(0x31, &r, &r);
        if(r.x.cflag)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_RM_SELECTOR;
                dpmiError.Selector=seg;
                return FALSE;
        }
        *sel=r.w.ax;
            return TRUE;
}

/**********************************************************************
* ���祭�� ���६��� ��� ���᫥��� ᫥���饣� ᥫ���� � ���ᨢ�   *
* �᫨ �� ᮧ���� ��᪮�쪮 ���ਯ�஢ � LDT �� ����� �㭪樨 0h, *
* � �⮩ �㭪樥� �㤥� ������� ⮫쪮 ���� ᥫ����,            *
* ᮮ⢥�����騩 ��ࢮ�� ���ਯ��� � ᮧ������ ���ᨢ�.            *
* ��� ���᫥��� ��⠫��� ᥫ���஢ ��� �ணࠬ�� ������           *
* �ᯮ�짮���� �㭪�� 0003h.                                         *
* �� �㭪�� �����頥� ���祭��, ���஥ �� �㤥� �ਡ������ �     *
* �������� ᥫ����� ��� ���᫥��� ��⠫��� ᥫ���஢ � ���ᨢ�.   *
* ��� �믮������ �㭪樨 ����室��� �맢��� ���뢠��� INT 31h        *
* �������� �� �室�                                                   *
* AX      0003h                                                       *
* �������� �� ��室�:                                                 *
* AX      ���祭�� ��� ���६��� �������� ᥫ����.                 *
**********************************************************************/
int DPMIIncrementSelector(void)
{
        union REGS r;
            r.w.ax=0x0003;
            int386(0x31, &r, &r);
        return r.w.ax;
}

/**********************************************************************
*                  ������� ������ ���� ᥣ����                    *
* � ������� �⮩ �㭪樨 ��� �ணࠬ�� ����� ��।����� 32-ࠧ�來� *
* ������ ���� ᥣ���� �� ��� ᥫ�����.                            *
* �������� �� �室�                                                   *
* AX      0006h                                                       *
* BX      ������� ᥣ����, ��� ���ண� �ॡ���� �������          *
*                        ������ ����.                                     *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
* CX:DX   32-ࠧ�來� ������ ���� ᥣ����.                        *
**********************************************************************/
BOOL DPMIGetSelectorBase(uint16 sel,ulong *addr)
{
        union REGS r;
        r.w.ax=0x0006;
            r.w.bx=sel;
        int386(0x31, &r, &r);
        if(r.x.cflag)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_GET_LINEAR_ADDR;
                dpmiError.Selector=sel;
                return FALSE;
        }
        *addr=dword(r.w.cx,r.w.dx);
        return TRUE;
}

/**********************************************************************
*                ��⠭����� ������ ���� ᥣ����                     *
* �� �㭪�� ��⠭�������� ������ ���� ᥣ���� � ���ਯ��,     *
* ᮮ⢥�����饬 ��������� ᥫ�����.                                *
* �� ����� ������஢��� ⮫쪮 � ���ਯ���,                     *
* ����� ᮧ���� ��� �ணࠬ��.                                     *
* �������� �� �室�                                                   *
* AX    0007h                                                         *
* BX    ������� ᥣ����, ��� ���ண� �ॡ���� ��⠭�����          *
*                              ������ ����.                             *
* CX:DX ��⠭��������� 32-ࠧ�來� ������� ������ ���� ᥣ����. *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
**********************************************************************/
BOOL DPMISetSelectorBase(uint16 sel,ulong linAddr)
{
        union REGS r;
        r.w.ax=0x0007;
            r.w.bx=sel;
            r.w.cx=hiword(linAddr);
        r.w.dx=loword(linAddr);
        int386(0x31,&r,&r);
        if(r.x.cflag)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_SET_LINEAR_ADDR;
                dpmiError.Selector=sel;
                dpmiError.LinearAddr=linAddr;
                return FALSE;
        }
        return TRUE;
}

/**********************************************************************
*                ��⠭����� �।�� ᥣ����                            *
* �������� �� �室�                                                   *
* AX      0008h                                                       *
* BX      ������� ᥣ����, ��� ���ண� �ॡ���� ��⠭����� �����  *
*                       ���祭�� �।���.                                  *
* CX:DX   ��⠭���������� 32-ࠧ�來�� ���祭�� �।��� ᥣ����.     *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
**********************************************************************/
BOOL DPMISetSelectorLimit(uint16 sel,ulong limit)
{
        union REGS r;
        r.w.ax=0x0008;
            r.w.bx=sel;
            r.w.cx=hiword(limit);
            r.w.dx=loword(limit);
        int386(0x31, &r, &r);
        if(r.x.cflag)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_SET_LIMIT;
                dpmiError.Selector=sel;
                dpmiError.Limit=limit;
                return FALSE;
        }
        return TRUE;
}

// ����� "㬭��" �����. �� ᫨誮� ����讬 �।��� ����� ��ਡ���
// ���ਯ��. � �⫨稥 �� �।��饩 �������� ࠧ��� ⮫쪮 � �����
BOOL DPMISetSelectorLimitX(uint16 sel,ulong limit)
{
        DescriptorStruct dsr;
        unsigned PageGranularity;
        if(!DPMIGetDescriptor(sel,&dsr))
                return FALSE;
        PageGranularity=dsr.reserved&DPMI_PAGE_GRANULARITY;
        if(limit>=0x100000)  // 1Mb
         // �।�� ����뢠���� 20� ��⠬�, �� �ॢ�襭�� ���室��
            // � ��࠭��� ��࠭�祭��. ���㣫���� � ������� ��஭�
                       if(!PageGranularity)
                {
                        dsr.reserved|=PageGranularity=DPMI_PAGE_GRANULARITY;
                        if(!DPMISetSelectorAccess(sel,word(dsr.reserved,dsr.access)))
                                return FALSE;
                }
        if(PageGranularity)
                limit=(limit+0xFFF)>>12;
        return DPMISetSelectorLimit(sel,limit);
}

/**********************************************************************
*                ��⠭����� ���� �ࠢ ����㯠 � ���ਯ��            *
* �������� �� �室�                                                   *
* AX      0009h                                                       *
* BX      ������� ᥣ����, ��� ���ண� �ॡ���� ��⠭����� ����  *
*                        �ࠢ� ����㯠.                                     *
* CL      ���� �ࠢ ����㯠.                                          *
* CH      ����७�� ���� �ࠢ ����㯠 ��� ������ i80386/i80486, *
*                      �ᯮ������ ⮫쪮 32-ࠧ�來묨 ॠ�����ﬨ DPMI. *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
*                                                                     *
* � ���७�� ���� �ࠢ ����㯠 ���訩 ���                         *
*    (��� 7) - ��� �࠭��୮��, �������� ���� �।���          *
*               (0 - �।�� � �����, 1 - �।�� � ��࠭���).             *
*    ��� 6 㪠�뢠�� ०�� ࠡ��� ������                          *
*        (0 - 16-ࠧ�來�, 1 - 32-ࠧ�來�).                        *
*    ��� 5 � 2-3 ������ ���� ࠢ�� 0.                                 *
*    ��� 4 ����� ᮤ�ঠ�� �� ���祭��.                            *
**********************************************************************/
BOOL DPMISetSelectorAccess(uint16 sel,uint16 attr)
{
        union REGS r;
        r.w.ax=0x0009;
            r.w.bx=sel;
            r.w.cx=attr;
        int386(0x31, &r, &r);
        if(r.x.cflag)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_SET_ACCESS;
                dpmiError.Selector=sel;
                dpmiError.Attr=attr;
                return FALSE;
        }
        return TRUE;
}

/**********************************************************************
*                �������� ����᭮�� ���ਯ�� ��� ᥣ���� ����      *
* �᫨ ����� ����� �� �㦥�, �� ����� ���� 㭨�⮦�� �� �����      *
*   �㭪樨 0001h.                                                    *
* �������� �� �室�                                                   *
* AX      000Ah                                                       *
* BX     ������� ᥣ���� ����, ��� ���ண� �ॡ���� ᮧ���� �����.*
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
*           1, �᫨ �ந��諠 �訡��.                                   *
* AX      alias                                                       *
**********************************************************************/
BOOL DPMIMakeSelectorAlias(uint16 sel,uint16 *asel)
{
        union REGS r;
            r.w.ax=0x000A;
            r.w.bx=sel;
            int386(0x31, &r, &r);
        if(r.x.cflag)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_CREATE_ALIAS_CS;
                dpmiError.Selector=sel;
                return FALSE;
        }
        *asel=r.w.ax;
        return TRUE;
}

/**********************************************************************
*                ������� ���ਯ�� 㪠������� ᥫ����              *
* �������� �� �室�                                                   *
* AX    000Bh                                                         *
* BX    ������� ᥣ����, ��� ���ண� �ॡ���� ������� ���ਯ��.*
* ES:(E)DI        �����⥫� �� ���� ࠧ��஬ 8 ����,                 *
*                           � ����� �㤥� ᪮��஢�� ���ਯ��.    *
                            ��� 32-ࠧ�來�� �ணࠬ� ����室���      *
                            �ᯮ�짮���� ॣ���� EDI.                 *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
**********************************************************************/
BOOL DPMIGetDescriptor(uint16 sel,DescriptorStruct __far* dsr)
{
        union REGS r;
        struct SREGS sregs;
        memset(&sregs,0,sizeof(sregs));
            r.w.ax=0x000B;
            r.w.bx=sel;
        sregs.es=FP_SEG(dsr);
        r.x.edi=FP_OFF(dsr);
            int386x(0x31, &r, &r,&sregs);
        if(r.x.cflag)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_GET_DT_RECORD;
                dpmiError.Selector=sel;
                return FALSE;
        }
        return TRUE;
}

/**********************************************************************
*                ��⠭����� ���ਯ�� 㪠������� ᥫ����            *
* �������� �� �室�                                                   *
* AX    000Ch                                                         *
* BX  ������� ᥣ����, ��� ���ண� �ॡ���� ��⠭����� ���ਯ��.*
* ES:(E)DI        �����⥫� �� ���� ࠧ��஬ 8 ����,                 *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
* �ணࠬ�� ����� ������஢��� ⮫쪮 ���ਯ��� �� ᢮��          *
* �����쭮� ⠡���� LDT, ᮧ����� ⠬ � ������� �㭪樨 0000h.       *
**********************************************************************/
BOOL DPMISetDescriptor(uint16 sel,DescriptorStruct __far* dsr)
{
        union REGS r;
        struct SREGS sregs;
        memset(&sregs,0,sizeof(sregs));
            r.w.ax=0x000C;
            r.w.bx=sel;
        sregs.es=FP_SEG(dsr);
        r.x.edi=FP_OFF(dsr);
            int386x(0x31, &r, &r,&sregs);
        if(r.x.cflag)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_SET_DT_RECORD;
                dpmiError.Selector=sel;
                return FALSE;
        }
        return TRUE;
}

/**********************************************************************
*                �뤥���� ������� ���ਯ�� � ⠡��� LDT          *
* �������� �� �室�                                                   *
* AX    000Dh                                                         *
* BX  ������� ᥣ����, ��� ���ண� �ॡ���� ������� ���ਯ��.  *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
* ��� �⮩ �� � ⠡��� ��� ��१�ࢨ஢��� ���� 16 ���ਯ�஢  *
**********************************************************************/
BOOL DPMIAllocDescriptor(uint16 sel)
{
        union REGS r;
            r.w.ax=0x000D;
            r.w.bx=sel;
            int386(0x31, &r, &r);
        if(r.x.cflag)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_ALLOC_RESERVED_SELECTOR;
                dpmiError.Selector=sel;
                return FALSE;
        }
        return TRUE;
}

/**********************************************************************
* ������� ���� ����� �� �㫠 ᢮������ ����� DOS                   *
* ��� ����祭���� ����� ����� �㭪�� ᮧ���� ���� ��� ��᪮�쪮     *
* ���ਯ�஢ (� ����ᨬ��� �� ࠧ��� �����) � �����頥� ���      *
* ᥣ���� ����� ��� ॠ�쭮�� ०���, ⠪ � ������ ᥫ���� ���      *
* ����㯠 � ����� � ����񭭮� ०���.                                *
* �᫨ ࠧ��� ����襭���� ����� �ॢ�蠥� 64 ��������,              *
* �㭪�� ᮧ���� ���ᨢ ���ਯ�஢ � ������� ������ ᥫ����.   *
* ��� ����祭�� ��⠫��� ᥫ���஢ �� ᬮ��� �ᯮ�짮���� ���祭�� *
* ���६���, ����祭��� ��᫥ �맮�� �㭪樨 0003h.                  *
* ���ਯ���, ᮧ����� �⮩ �㭪樥�, ����� �᢮������� ⮫쪮 �    *
* ������� �㭪樨 0101h.                                              *
*                                                                     *
* �������� �� �室�                                                   *
* AX      0100h                                                       *
* BX      ������ ����� � ��ࠣ���                                   *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
* � ��砥 �訡�� ॣ���� AX ᮤ�ন� ��� �訡��, ����祭�� �� DOS:  *
*         07h - ࠧ��襭 ���� MCB;                                    *
*         08h - ᫨誮� ����让 ࠧ��� ����������� �����.             *
* � ��᫥���� ��砥 ॣ���� BX ᮤ�ন� ���ᨬ��쭮 �������� ࠧ��� *
* ����� � ��ࠣ���.                                                 *
* AX    ������� ॠ�쭮�� ०��� ����祭���� �����.                   *
* DX    ������� ��� ����㯠 � ����祭���� ����� � ����񭭮� ०���. *
**********************************************************************/
void __far* DPMIDOSAlloc(ulong size,uint16 *sel,uint16 *rseg)
{
        union REGS r;
        r.w.ax=0x100;
        r.w.bx=(size+0xF)>>4;
            int386(0x31,&r,&r);
        if(r.w.cflag)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_DOS_ALLOC;
                dpmiError.DosError=r.w.ax;
                dpmiError.Limit=r.w.bx<<4;
                return NULL;
        }
        if(sel!=NULL)
                *sel=r.w.dx;
        *rseg=r.w.ax;
        return MK_FP(*sel,0x0000);
}

/**********************************************************************
*                 �᢮������ ���� �����, ����� �� �㫠 DOS            *
* �������� �� �室�                                                   *
* AX      0101h                                                       *
* DX      ������� �᢮���������� �����.                              *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
* � ��砥 �訡�� ॣ���� AX ᮤ�ন� ��� �訡��, ����祭�� �� DOS:  *
*  07h - ࠧ��襭 ���� MCB;                                           *
*  09h - ���ࠢ��쭮� ������� ᥣ���⭮�� ���� �᢮���������� �����.*
**********************************************************************/
BOOL DPMIDOSFree(uint16 sel)
{
        union REGS r;
            r.w.ax=0x101;
            r.w.dx=sel;
            int386(0x31,&r,&r);
        if(r.w.cflag)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_DOS_FREE;
                dpmiError.DosError=r.w.ax;
                dpmiError.Selector=sel;
                return FALSE;
        }
        return TRUE;
}

/**********************************************************************
*         �������� ࠧ��� ����� �����, ����祭���� �� �㫠 DOS        *
* �������� �� �室�                                                   *
* AX      0102h                                                       *
* BX      ���� ࠧ��� ����� ����� � ��ࠣ���.                     *
* DX      ������� �������㥬��� �����.                             *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
* � ��砥 �訡�� ॣ���� AX ᮤ�ন� ��� �訡��, ����祭�� �� DOS:  *
*       07h - ࠧ��襭 ���� MCB;                                      *
*       08h - ᫨誮� ����让 ࠧ��� ����������� �����.               *
* � �⮬ ��砥 ॣ���� BX ᮤ�ন� ���ᨬ��쭮 �������� ࠧ���      *
*    ����� � ��ࠣ���.                                              *
*     09h - ���ࠢ��쭮� ������� ���� �������㥬��� �����.        *
**********************************************************************/
BOOL DPMIDOSRealloc(uint16 sel,ulong newsize)
{
        union REGS r;
            r.w.ax=0x102;
        r.w.bx=(newsize+0x000F)>>4;
            r.w.dx=sel;
            int386(0x31,&r,&r);
        if(r.w.cflag)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_DOS_REALLOC;
                dpmiError.DosError=r.w.ax;
                dpmiError.Limit=r.w.bx<<4;
                dpmiError.Selector=sel;
                return FALSE;
        }
        return TRUE;
}

/**********************************************************************
* ������� ����� ���뢠��� ॠ�쭮�� ०���                         *
* �㭪�� �����頥� ��� ��������� ����� ���뢠��� ���� ���       *
* ��ࠡ��稪� � �ଠ� ॠ�쭮�� �e����.                             *
* �������� �� �室�                                                   *
* AX      0200h                                                       *
* BL      ����� ���뢠���.                                           *
* �������� �� ��室�:                                                 *
* CARRY   0                                                           *
* CX:DX   ���� ��ࠡ��稪� ���뢠��� � �ଠ� <ᥣ����:ᬥ饭��>.  *
**********************************************************************/
ulong DPMIRMGetInterrupt(unsigned i)
{
        union REGS r;
            r.w.ax=0x200;
        r.h.bl=i;
            int386(0x31,&r,&r);
        return dword(r.w.cx,r.w.dx);
}

/**********************************************************************
* ��⠭����� ����� ���뢠��� ॠ�쭮�� ०���                       *
* �������� �� �室�                                                   *
* AX      0201h                                                       *
* BL      ����� ���뢠���.                                           *
* CX:DX   ���� ��ࠡ��稪� ���뢠��� � �ଠ� <ᥣ����:ᬥ饭��>.  *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                                      *
**********************************************************************/
BOOL DPMIRMSetInterrupt(unsigned i,ulong addr)
{
        union REGS r;
            r.w.ax=0x201;
        r.h.bl=i;
        r.w.cx=hiword(addr);
        r.w.dx=loword(addr);
            int386(0x31,&r,&r);
        if(r.w.cflag)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_SET_RM_INT;
                dpmiError.intrNo=i;
                dpmiError.RmAddr=addr;
                return FALSE;
        }
        return TRUE;
}

/**********************************************************************
*         ������� ���� ��ࠡ��稪� �᪫�祭�� � ������� ����஬      *
* �㭪�� �����頥� ���� ⥪�饣� ��ࠡ��稪� �᪫�祭�� � ������� *
* ����஬ � �ଠ� ����񭭮�� ०���.                                                 *
* �������� �� �室�                                                   *
* AX      0202h                                                       *
* BL      ����� �᪫�祭�� (00h - 1Fh).                               *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
* CX:(E)DX ���� ��ࠡ��稪� �᪫�祭�� � �ଠ� <ᥫ����:ᬥ饭��>.*
**********************************************************************/
void __far* DPMIGetExceptionHandler(unsigned i)
{
        union REGS r;
            r.w.ax=0x202;
        r.h.bl=i;
            int386(0x31,&r,&r);
        if(r.w.cflag)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_GET_EXCEPTION;
                dpmiError.intrNo=i;
                return NULL;
        }
        return MK_FP(r.w.cx,r.x.edx);
}

/**********************************************************************
*    ��⠭����� ���� ��ࠡ��稪� �᪫�祭�� � ������� ����஬       *
* �������� �� �室�                                                   *
* AX      0203h                                                       *
* BL      ����� �᪫�祭�� (00h - 1Fh).                               *
* CX:(E)DX  ���� ��ࠡ��稪� �᪫�祭�� � �ଠ� <ᥫ����:ᬥ饭��>*
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
**********************************************************************/
BOOL DPMISetExceptionHandler(unsigned i,void __far* ptr)
{
        union REGS r;
            r.w.ax=0x203;
        r.h.bl=i;
        r.w.cx=FP_SEG(ptr);
        r.x.edx=FP_OFF(ptr);
            int386(0x31,&r,&r);
        if(r.w.cflag)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_SET_EXCEPTION;
                dpmiError.intrNo=i;
                dpmiError.pmPtr=ptr;
                return FALSE;
        }
        return TRUE;
}

/**********************************************************************
*          ������� ����� ���뢠��� ����񭭮�� ०���              *
* �������� �� �室�                                                   *
* AX      0204h                                                       *
* BL      ����� ���뢠���.                                           *
* �������� �� ��室�:                                                 *
* CX:(E)DX ���� ��ࠡ��稪� ���뢠��� � �ଠ� <ᥫ����:ᬥ饭��>.*
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
**********************************************************************/
InterruptHandlerPtr DPMIGetInterrupt(unsigned n)
{
        union REGS r;
            r.w.ax=0x204;
        r.h.bl=n;
            int386(0x31,&r,&r);
        return (InterruptHandlerPtr)MK_FP(r.w.cx,r.x.edx);
}

/**********************************************************************
*          ��⠭����� ����� ���뢠��� ����񭭮�� ०���            *
* �������� �� �室�                                                   *
* AX      0205h                                                       *
* BL      ����� ���뢠���.                                           *
* CX:(E)DX ���� ��ࠡ��稪� ���뢠��� � �ଠ� <ᥫ����:ᬥ饭��>.*
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
**********************************************************************/
BOOL DPMISetInterrupt(unsigned i,InterruptHandlerPtr ptr)
{
        union REGS r;
            r.w.ax=0x205;
        r.h.bl=i;
        r.w.cx=FP_SEG(ptr);
        r.x.edx=FP_OFF(ptr);
            int386(0x31,&r,&r);
        if(r.w.cflag)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_SET_PM_INT;
                dpmiError.intrNo=i;
                dpmiError.pmPtr=ptr;
                return FALSE;
        }
        return TRUE;
}

/**********************************************************************
*                 ������ ���뢠��� ॠ�쭮�� ०���                  *
* �������� �� �室�                                                   *
* AX      0300h                                                       *
* BL      ����� ���뢠���.                                           *
* BH      ���� 䫠���. �᫨ ��⠭����� � ������� ��� 0, �믮������ � *
*            ��� ����஫��� � ����� A20. ��⠫�� ���� ��१�ࢨ஢���*
*           � ������ ���� ��襭� � ���.                             *
* CX      ������⢮ ᫮�, ����� ������ ���� ᪮��஢��� �� �⥪�   *
*                         ����񭭮�� ०��� � �⥪ ॠ�쭮�� ०���.      *
* ES:(E)DI        ���� �ࠢ���饩 �������� ��� �맮�� ���뢠���.  *
*                                                                     *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
* ES:(E)DI        ���� ������஢����� (� १���� �믮������     *
*                   ��ࠡ��稪� ���뢠���) �ࠢ���饩 ��������       *
*                    � �ଠ� ����񭭮�� ०���.                     *
**********************************************************************/
BOOL DPMIRMInt(unsigned i,RMREGS *rmregs,RMREGS __far** rgout)
{
        union REGS regs;
        struct SREGS sregs;
        memset(&sregs,0,sizeof(sregs));
        regs.w.ax=0x300;
        regs.h.bl=i;
        regs.h.bh=0;
        regs.w.cx=0;
        sregs.es=FP_SEG(rmregs);
        regs.x.edi=FP_OFF(rmregs);
        int386x(0x31,&regs,&regs,&sregs);
        if(regs.w.cflag & INTR_CF)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_CALL_RM_INT;
                dpmiError.intrNo=i;
                return FALSE;
        }
        if(rgout!=NULL)
                *rgout=(RMREGS __far*)MK_FP(sregs.es,regs.x.edi);
        return TRUE;
}

/**********************************************************************
* �맮� ��楤��� ॠ�쭮�� ०���, �����稢��饩�� �������� RET FAR  *
* �������� �� �室�                                                   *
* AX      0301h                                                       *
* ��⠫�� ��������� ᮢ������ � �맮��� ���뢠���                      *
**********************************************************************/
BOOL DPMIRMCall(RMREGS *rmregs)
{
// DOS4GW not support DPMI function 301h
//union REGS regs;
//struct SREGS sregs;
//memset( &sregs, 0, sizeof(sregs) );
//regs.x.eax = 0x301;
//regs.h.bh = 0;
//regs.x.ecx = 0;
//sregs.es = FP_SEG(rmregs);
//regs.x.edi = FP_OFF(rmregs);
//int386x( 0x31, &regs, &regs, &sregs );
//if( regs.w.cflag & INTR_CF ){
//  set_error_code( 0x800 | regs.h.al );
//  return( -1 );
//}
//return( 0 );

//ulong _far *vectable = NULL;
  ulong oldvec4;
  ulong oldvec6;
  ulong oldvec7;
  int flag;

//if( (vectable = (ulong _far *)dpmi_map_memory(0, 0xFFFFF)) != NULL ){
//  return( -1 );
//}
//oldvec4 = mem_long(vectable+4*4);
//oldvec6 = mem_long(vectable+4*6);
//oldvec7 = mem_long(vectable+4*7);

  oldvec4=mem_long(4*4);
  oldvec6=mem_long(4*6);
  oldvec7=mem_long(4*7);

  mem_long(4*4) = 6*4+1; // Address of INT6 vector.
  mem_long(4*6) = (long)0x9A00 + ((long)(rmregs->w.ip)<<16);   // FAR CALL [seg:off]
  mem_long(4*7) = (long)(rmregs->w.cs) + ((long)0xCF<<16);  // + IRET

  flag=DPMIRMInt(4,rmregs);
  mem_long(4*4)=oldvec4;
  mem_long(4*6)=oldvec6;
  mem_long(4*7)=oldvec7;
//  if( dpmi_descriptor_free( FP_SEG(vectable) ) || flag )  return( -1 );
  if(!flag)
  {
        CurrentError=&dpmiError;
        dpmiError.fnNo=DPMI_CALL_RM_FPROC;
          return FALSE;
  }
  return TRUE;
}

/**********************************************************************
* �맮� ��楤��� ॠ�쭮�� ०���, �����稢��饩�� �������� IRET FAR  *
* �������� �� �室�                                                   *
* AX      0302h                                                       *
* ��⠫�� ��������� ᮢ������ � �맮��� ���뢠���                      *
**********************************************************************/
BOOL DPMIRMCalliret(RMREGS *rmregs)
{
// DOS4GW not support DPMI function 302h
  ulong oldvec4;
  int flag;

  oldvec4 = mem_long(4*4);

  mem_long(4*4) = dword(rmregs->w.cs,rmregs->w.ip);

  flag=DPMIRMInt(4,rmregs);
  mem_long(4*4)=oldvec4;
  if(!flag)
  {
        CurrentError=&dpmiError;
        dpmiError.fnNo=DPMI_CALL_RM_IPROC;
          return FALSE;
  }
  return TRUE;
}

/**********************************************************************
*                 ������� ���ଠ�� � ᢮������ �����                *
* �������� �� �室�                                                   *
* AX      0500h                                                       *
* ES:(E)DI         ���� �y�� ࠧ��஬ 30h ���� � �ଠ�              *
*                       <ᥫ����:ᬥ饭��>.                            *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
**********************************************************************/
BOOL DPMIFreeMemInfo(FreeMemInfo *meminfo)
{
        union REGS regs;
        struct SREGS sregs;
        memset(&sregs,0,sizeof(sregs));
         regs.w.ax=0x500;
        regs.x.edi=FP_OFF(meminfo);
          sregs.es=FP_SEG(meminfo);
          int386x(0x31,&regs,&regs,&sregs);
        if(regs.x.cflag)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_MEM_INFO;
                return FALSE;
        }
        return TRUE;
}

void DPMIDumpFreeMemInfo(ostream& os,FreeMemInfo* mi)
{
        if(mi->AvailBytes==0xFFFFFFFF)
                os<<"Virtual memory not supported"<<endl;
        else
                os<<"Largest free block: "<<mi->AvailBytes<<"bytes\n"
                  <<"Pages available if left unlocked: "<<mi->UnlockedPgs<<endl
                    <<"Pages available if locked: "<<mi->LockedPgs<<endl
                    <<"Linear address space size: "<<mi->TotLinPgs<<" pages\n"
                    <<"Unlocked pages: "<<mi->TotUnlkPgs<<endl
                    <<"Free pages: "<<mi->TotFreePgs<<endl
                    <<"Physical pages: "<<mi->TotPhysPgs<<endl
                    <<"Free linear address space: "<<mi->FreeLinPgs<<" pages\n"
                    <<"Size of page swapping file: "<<mi->TotSwapPgs<<" pages\n";
}

/**********************************************************************
*                        ������� ���� �����                          *
* �� �㭪�� �।�����祭� ��� ����祭�� �ணࠬ��� ���⪠ �������� *
* �����. ��� ����祭���� ����� ����� �� �믮������ ��⮬���᪮�� *
* ᮧ����� ᥫ����, �ணࠬ�� ������ ᮧ���� ᥫ���� ᠬ����⥫쭮*
* �������� �� �室�                                                   *
* AX      0501h                                                       *
* BX:CX   ������ ����� ����� � �����.                               *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
* BX:CX   ������� ���� ����祭���� �����.                           *
* SI:DI   ������ ����祭���� ����� �����, �㦥� ��� �믮������       *
* ����権 � ������ ����� (��������� ��� ࠧ��� ��� �᢮��������).  *
**********************************************************************/
BOOL DPMILinearAlloc(ulong size,ulong* addr,ulong* handle)
{
        union REGS regs;
        regs.x.eax = 0x501;                        // allocate memory
        regs.x.ebx=hiword(size);
        regs.x.ecx=loword(size);
        int386(0x31,&regs,&regs);
        if(regs.w.cflag&INTR_CF)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_MEM_ALLOC;
                dpmiError.Limit=size;
                return FALSE;
        }
        *handle=dword(regs.w.si,regs.w.di);
        *addr=dword(regs.w.bx,regs.w.cx);
        return TRUE;
}

void __far* DPMIalloc(ulong size,ulong *handle)
{
        uint16 sel;
        ulong laddr;
        if(!DPMIAllocSelector(&sel))
                return NULL;
        size=(size+0xFFF)&0xFFFFF000;
        if(!DPMILinearAlloc(size,&laddr,handle))
        {
                DpmiFreeSelector(sel);
                return NULL;
        }
        if(DPMISetSelectorBase(sel,laddr) && DPMISetSelectorLimitX(sel,size-1))
                   return MK_FP(sel,0);
        DpmiFreeSelector(sel);
        DPMILinearFree(*handle);
        return NULL;
}

/**********************************************************************
*                        �᢮������ ���� �����                        *
* �������� �� �室�                                                   *
* AX      0502h                                                       *
* SI:DI   ������ �᢮���������� ����� �����.                         *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
**********************************************************************/
BOOL DPMILinearFree(ulong handle)
{
        union REGS regs;
        regs.x.eax=0x0502;          // free memory block
        regs.x.esi=hiword(handle);
        regs.x.edi=loword(handle);
        int386(0x31,&regs,&regs);
        if(regs.w.cflag&INTR_CF)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_MEM_FREE;
                dpmiError.LinearAddr=handle;
                return FALSE;
        }
        return TRUE;
}

/**********************************************************************
*                �������� ࠧ��� ����� �����                          *
* �������� �� �室�                                                   *
* AX      0503h                                                       *
* BX:CX   ���� ࠧ��� ����� ����� � �����.                         *
* SI:DI   ������ �����塞��� ����� �����.                            *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
* BX:CX   ���� ������� ���� ����祭���� �����.                     *
* SI:DI   ���� ������ ����� �����.                                  *
**********************************************************************/
BOOL DPMILinearRealloc(ulong size,ulong* addr,ulong* handle)
{
        union REGS regs;
        regs.x.eax = 0x503;                        // allocate memory
        regs.x.ebx=hiword(size);
        regs.x.ecx=loword(size);
        regs.x.esi=hiword(*handle);
        regs.x.edi=loword(*handle);
        int386(0x31,&regs,&regs);
        if(regs.w.cflag&INTR_CF)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_MEM_REALLOC;
                dpmiError.LinearAddr=*handle;
                dpmiError.Limit=size;
                return FALSE;
        }
        *handle=dword(regs.w.si,regs.w.di);
        *addr=dword(regs.w.bx,regs.w.cx);
        return TRUE;
}

/**********************************************************************
*                no commens                                              *
**********************************************************************/
void DPMITotalMemInfo(TotalMemInfo *meminfo)
{
        union REGS regs;
        struct SREGS sregs;
        memset(&sregs,0,sizeof(sregs));
        regs.x.eax=0x050B;
        regs.x.edi=FP_OFF(meminfo);
        sregs.es=FP_SEG(meminfo);
        int386x(0x31,&regs,&regs,&sregs);
}

/**********************************************************************
*                  ��䨪�஢��� �������� ������� �����                      *
* �� �㭪�� 䨪���� � ����� �������, ���������� ������� ���ᮬ.*
* ��� ��䨪�஢����� ������ ����� �� �믮������ ᢮���� ��࠭��.  *
*                                                                     *
* �������� �� �室�                                                   *
* AX      0600h                                                       *
* BX:CX   ��砫�� ������� ���� 䨪��㥬��� ���⪠ �����.       *
* SI:DI   ������ 䨪��㥬��� ����� ����� � �����.                  *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
**********************************************************************/
BOOL DPMIlock(ulong addr,ulong size)
{
        union REGS r;
        r.x.eax=0x0600;
        r.x.ebx=hiword(addr);
        r.x.ecx=loword(addr);
        r.x.esi=hiword(size);
        r.x.edi=loword(size);
        int386(0x31,&r,&r);
        if(r.x.cflag)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_PAGE_LOCK;
                dpmiError.LinearAddr=addr;
                dpmiError.Limit=size;
                return FALSE;
        }
        return TRUE;
}

/**********************************************************************
*                   ���䨪�஢���� ����� �����                      *
* �� �㭪�� ������ ���� �ᯮ�짮���� ��� ��䨪�஢���� �����      *
* �����, ��䨪�஢������ �।��饩 �㭪樥�.                          *
*                                                                     *
* �������� �� �室�                                                   *
* AX      0601h                                                       *
* BX:CX   ��砫�� ������� ���� ��䨪��㥬��� ���⪠ �����.    *
* SI:DI   ������ ��䨪��㥬��� ����� ����� � �����.               *
* �������� �� ��室�:                                                 *
* CARRY   0, �᫨ �㭪�� �믮������� ��� �訡��,                     *
* 1, �᫨ �ந��諠 �訡��.                                           *
**********************************************************************/
BOOL DPMIunlock(ulong addr,ulong size)
{
        union REGS r;
        r.x.eax=0x0601;
        r.x.ebx=hiword(addr);
        r.x.ecx=loword(addr);
        r.x.esi=hiword(size);
        r.x.edi=loword(size);
        int386(0x31, &r, &r);
        if(r.x.cflag)
        {
                CurrentError=&dpmiError;
                dpmiError.fnNo=DPMI_PAGE_UNLOCK;
                dpmiError.LinearAddr=addr;
                dpmiError.Limit=size;
                return FALSE;
        }
        return TRUE;
}

/**********************************************************************
*                 �८�ࠧ������ 䨧��᪮�� ���� � �������        *
**********************************************************************/
ulong DPMILinAddr(ulong addr,ulong size)
{
        union REGS r;
        r.x.eax=0x0800;
        r.x.ebx=hiword(addr);
        r.x.ecx=loword(addr);
        r.x.esi=hiword(size);
        r.x.edi=loword(size);
        int386(0x31, &r, &r);
        return MakeDword(r.x.ebx,r.x.ecx);
}
