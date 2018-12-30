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
*                Создать дескриптор в таблице LDT                      *
* Для выполнения функции необходимо вызвать прерывание INT 31h        *
* Регистры на входе:                                                  *
* AX      0000h                                                       *
* CX      Количество создаваемых дескрипторов.                        *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
* AX      Базовый селектор.                                           *
* Если программа заказывала несколько селекторов,                     *
* AX содержит первый селектор из созданного массива.                  *
* Для получения остальных селекторов необходимо воспользоваться       *
* функцией 00003h.                                                    *
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
*                Освободить дескриптор в таблице LDT                   *
* Для выполнения функции необходимо вызвать прерывание INT 31h        *
* Регистры на входе                                                   *
* AX      0001h                                                       *
* BX      Селектор освобождаемого дескриптора.                        *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
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
*                Преобразование сегмента в дескриптор                  *
* преобразование сегментной компоненты адреса реального режима        *
* в дескриптор.                                                       *
* Для выполнения функции необходимо вызвать прерывание INT 31h        *
* Регистры на входе                                                   *
* AX      0002h                                                       *
* BX      Сегментный адрес реального режима.                          *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
* AX      Селектор, соответствующий дескриптору,                      *
*           созданному для адресации сегмента реального режима.         *
*                                                                     *
* Дескриптор, созданный данной функцией, не может быть освобождён,    *
* поэтому эту функцию имеет смысл использовать для адресации          *
* таких областей памяти, которые будут нужны на всём протяжении       *
* работы программы (область данных BIOS, память видеоконтроллера...). *
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
* значение инкремента для вычисления следующего селектора в массиве   *
* Если вы создали несколько дескрипторов в LDT при помощи функции 0h, *
* то этой функцией будет возвращён только первый селектор,            *
* соответствующий первому дескриптору в созданном массиве.            *
* Для вычисления остальных селекторов ваша программа должна           *
* использовать функцию 0003h.                                         *
* Эта функция возвращает значение, которое вы будете прибавлять к     *
* базовому селектору для вычисления остальных селекторов в массиве.   *
* Для выполнения функции необходимо вызвать прерывание INT 31h        *
* Регистры на входе                                                   *
* AX      0003h                                                       *
* Регистры на выходе:                                                 *
* AX      Значение для инкремента базового селектора.                 *
**********************************************************************/
int DPMIIncrementSelector(void)
{
        union REGS r;
            r.w.ax=0x0003;
            int386(0x31, &r, &r);
        return r.w.ax;
}

/**********************************************************************
*                  Получить базовый адрес сегмента                    *
* С помощью этой функции ваша программа может определить 32-разрядный *
* базовый адрес сегмента по его селектору.                            *
* Регистры на входе                                                   *
* AX      0006h                                                       *
* BX      Селектор сегмента, для которого требуется получить          *
*                        базовый адрес.                                     *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
* CX:DX   32-разрядный базовый адрес сегмента.                        *
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
*                Установить базовый адрес сегмента                     *
* Эта функция устанавливает базовый адрес сегмента в дескрипторе,     *
* соответствующем заданному селектору.                                *
* Вы можете модифицировать только те дескрипторы,                     *
* которые создала ваша программа.                                     *
* Регистры на входе                                                   *
* AX    0007h                                                         *
* BX    Селектор сегмента, для которого требуется установить          *
*                              базовый адрес.                             *
* CX:DX Устанавливаемый 32-разрядный линейный базовый адрес сегмента. *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
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
*                Установить предел сегмента                            *
* Регистры на входе                                                   *
* AX      0008h                                                       *
* BX      Селектор сегмента, для которого требуется установить новое  *
*                       значение предела.                                  *
* CX:DX   Устанавливаемое 32-разрядное значение предела сегмента.     *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
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

// более "умная" версия. При слишком большом пределе меняет атрибуты
// дескриптора. В отличие от предыдущей понимает размер только в байтах
BOOL DPMISetSelectorLimitX(uint16 sel,ulong limit)
{
        DescriptorStruct dsr;
        unsigned PageGranularity;
        if(!DPMIGetDescriptor(sel,&dsr))
                return FALSE;
        PageGranularity=dsr.reserved&DPMI_PAGE_GRANULARITY;
        if(limit>=0x100000)  // 1Mb
         // предел описывается 20ю битами, при превышении переходим
            // к страничным ограничениям. округление в большую сторону
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
*                Установить поле прав доступа в дескрипторе            *
* Регистры на входе                                                   *
* AX      0009h                                                       *
* BX      Селектор сегмента, для которого требуется установить новые  *
*                        права доступа.                                     *
* CL      Байт прав доступа.                                          *
* CH      Расширение байта прав доступа для процессора i80386/i80486, *
*                      используется только 32-разрядными реализациями DPMI. *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
*                                                                     *
* В расширении байта прав доступа старший бит                         *
*    (бит 7) - бит гранулярности, интерпретация поля предела          *
*               (0 - предел в байтах, 1 - предел в страницах).             *
*    Бит 6 указывает режим работы процессора                          *
*        (0 - 16-разрядный, 1 - 32-разрядный).                        *
*    Бит 5 и 2-3 должны быть равны 0.                                 *
*    Бит 4 может содержать любое значение.                            *
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
*                Создание алиасного дескриптора для сегмента кода      *
* Если алиас больше не нужен, он может быть уничтожен при помощи      *
*   функции 0001h.                                                    *
* Регистры на входе                                                   *
* AX      000Ah                                                       *
* BX     Селектор сегмента кода, для которого требуется создать алиас.*
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
*           1, если произошла ошибка.                                   *
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
*                Получить дескриптор указанного селектора              *
* Регистры на входе                                                   *
* AX    000Bh                                                         *
* BX    Селектор сегмента, для которого требуется получить дескриптор.*
* ES:(E)DI        Указатель на буфер размером 8 байт,                 *
*                           в который будет скопирован дескриптор.    *
                            Для 32-разрядных программ необходимо      *
                            использовать регистр EDI.                 *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
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
*                Установить дескриптор указанного селектора            *
* Регистры на входе                                                   *
* AX    000Ch                                                         *
* BX  Селектор сегмента, для которого требуется установить дескриптор.*
* ES:(E)DI        Указатель на буфер размером 8 байт,                 *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
* Программа может модифицировать только дескрипторы из своей          *
* локальной таблицы LDT, созданные там с помощью функции 0000h.       *
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
*                Выделить конкретный дескриптор в таблице LDT          *
* Регистры на входе                                                   *
* AX    000Dh                                                         *
* BX  Селектор сегмента, для которого требуется получить дескриптор.  *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
* Для этой фичи в таблице ЛДТ зарезервировано первые 16 дескрипторов  *
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
* Получить блок памяти из пула свободной памяти DOS                   *
* Для полученного блока памяти функция создаёт один или несколько     *
* дескрипторов (в зависимости от размера блока) и возвращает как      *
* сегмент блока для реального режима, так и базовый селектор для      *
* доступа к блоку в защищённом режиме.                                *
* Если размер запрошенного блока превышает 64 килобайта,              *
* функция создаст массив дескрипторов и возвратит базовый селектор.   *
* Для получения остальных селекторов вы сможете использовать значение *
* инкремента, полученное после вызова функции 0003h.                  *
* Дескрипторы, созданные этой функцией, можно освобождать только с    *
* помощью функции 0101h.                                              *
*                                                                     *
* Регистры на входе                                                   *
* AX      0100h                                                       *
* BX      Размер блока в параграфах                                   *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
* В случае ошибки регистр AX содержит код ошибки, полученный от DOS:  *
*         07h - разрушен блок MCB;                                    *
*         08h - слишком большой размер заказанного блока.             *
* В последнем случае регистр BX содержит максимально возможный размер *
* блока в параграфах.                                                 *
* AX    Сегмент реального режима полученного блока.                   *
* DX    Селектор для доступа к полученному блоку в защищённом режиме. *
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
*                 Освободить блок памяти, взятый из пула DOS            *
* Регистры на входе                                                   *
* AX      0101h                                                       *
* DX      Селектор освобождаемого блока.                              *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
* В случае ошибки регистр AX содержит код ошибки, полученный от DOS:  *
*  07h - разрушен блок MCB;                                           *
*  09h - неправильное задание сегментного адреса освобождаемого блока.*
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
*         Изменить размер блока памяти, полученного из пула DOS        *
* Регистры на входе                                                   *
* AX      0102h                                                       *
* BX      Новый размер блока памяти в параграфах.                     *
* DX      Селектор модифицируемого блока.                             *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
* В случае ошибки регистр AX содержит код ошибки, полученный от DOS:  *
*       07h - разрушен блок MCB;                                      *
*       08h - слишком большой размер заказанного блока.               *
* В этом случае регистр BX содержит максимально возможный размер      *
*    блока в параграфах.                                              *
*     09h - неправильное задание адреса модифицируемого блока.        *
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
* Получить вектор прерывания реального режима                         *
* Функция возвращает для заданного вектора прерывания адрес его       *
* обработчика в формате реального рeжима.                             *
* Регистры на входе                                                   *
* AX      0200h                                                       *
* BL      Номер прерывания.                                           *
* Регистры на выходе:                                                 *
* CARRY   0                                                           *
* CX:DX   Адрес обработчика прерывания в формате <сегмент:смещение>.  *
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
* Установить вектор прерывания реального режима                       *
* Регистры на входе                                                   *
* AX      0201h                                                       *
* BL      Номер прерывания.                                           *
* CX:DX   Адрес обработчика прерывания в формате <сегмент:смещение>.  *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                                      *
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
*         Получить адрес обработчика исключения с заданным номером      *
* Функция возвращает адрес текущего обработчика исключения с заданным *
* номером в формате защищённого режима.                                                 *
* Регистры на входе                                                   *
* AX      0202h                                                       *
* BL      Номер исключения (00h - 1Fh).                               *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
* CX:(E)DX Адрес обработчика исключения в формате <селектор:смещение>.*
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
*    Установить адрес обработчика исключения с заданным номером       *
* Регистры на входе                                                   *
* AX      0203h                                                       *
* BL      Номер исключения (00h - 1Fh).                               *
* CX:(E)DX  Адрес обработчика исключения в формате <селектор:смещение>*
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
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
*          Получить вектор прерывания защищённого режима              *
* Регистры на входе                                                   *
* AX      0204h                                                       *
* BL      Номер прерывания.                                           *
* Регистры на выходе:                                                 *
* CX:(E)DX Адрес обработчика прерывания в формате <селектор:смещение>.*
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
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
*          Установить вектор прерывания защищённого режима            *
* Регистры на входе                                                   *
* AX      0205h                                                       *
* BL      Номер прерывания.                                           *
* CX:(E)DX Адрес обработчика прерывания в формате <селектор:смещение>.*
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
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
*                 Эмуляция прерывания реального режима                  *
* Регистры на входе                                                   *
* AX      0300h                                                       *
* BL      Номер прерывания.                                           *
* BH      Байт флагов. Если установлен в единицу бит 0, выполняется с *
*            брос контроллера и линии A20. Остальные биты зарезервированы*
*           и должны быть сброшены в нуль.                             *
* CX      Количество слов, которые должны быть скопированы из стека   *
*                         защищённого режима в стек реального режима.      *
* ES:(E)DI        Адрес управляющей структуры для вызова прерывания.  *
*                                                                     *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
* ES:(E)DI        Адрес модифицированной (в результате выполнения     *
*                   обработчика прерывания) управляющей структуры       *
*                    в формате защищённого режима.                     *
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
* Вызов процедуры реального режима, заканчивающейся командой RET FAR  *
* Регистры на входе                                                   *
* AX      0301h                                                       *
* остальные полностью совпадают с вызовом прерывания                      *
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
* Вызов процедуры реального режима, заканчивающейся командой IRET FAR  *
* Регистры на входе                                                   *
* AX      0302h                                                       *
* остальные полностью совпадают с вызовом прерывания                      *
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
*                 Получить информацию о свободной памяти                *
* Регистры на входе                                                   *
* AX      0500h                                                       *
* ES:(E)DI         Адрес бyфера размером 30h байт в формате              *
*                       <селектор:смещение>.                            *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
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
*                        Получить блок памяти                          *
* Эта функция предназначена для получения программой участка линейной *
* памяти. Для полученного блока памяти не выполняется автоматического *
* создания селектора, программа должна создать селектор самостоятельно*
* Регистры на входе                                                   *
* AX      0501h                                                       *
* BX:CX   Размер блока памяти в байтах.                               *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
* BX:CX   Линейный адрес полученного блока.                           *
* SI:DI   Индекс полученного блока памяти, нужен для выполнения       *
* операций с блоком памяти (изменение его размера или освобождение).  *
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
*                        Освободить блок памяти                        *
* Регистры на входе                                                   *
* AX      0502h                                                       *
* SI:DI   Индекс освобождаемого блока памяти.                         *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
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
*                Изменить размер блока памяти                          *
* Регистры на входе                                                   *
* AX      0503h                                                       *
* BX:CX   Новый размер блока памяти в байтах.                         *
* SI:DI   Индекс изменяемого блока памяти.                            *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
* BX:CX   Новый линейный адрес полученного блока.                     *
* SI:DI   Новый индекс блока памяти.                                  *
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
*                  Зафиксировать линейную область памяти                      *
* Эта функция фиксирует в памяти область, задаваемую линейным адресом.*
* Для зафиксированной области памяти не выполняется свопинг страниц.  *
*                                                                     *
* Регистры на входе                                                   *
* AX      0600h                                                       *
* BX:CX   Начальный линейный адрес фиксируемого участка памяти.       *
* SI:DI   Размер фиксируемого блока памяти в байтах.                  *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
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
*                   Расфиксирование блока памяти                      *
* Эта функция должна быть использована для расфиксирования блока      *
* памяти, зафиксированного предыдущей функцией.                          *
*                                                                     *
* Регистры на входе                                                   *
* AX      0601h                                                       *
* BX:CX   Начальный линейный адрес расфиксируемого участка памяти.    *
* SI:DI   Размер расфиксируемого блока памяти в байтах.               *
* Регистры на выходе:                                                 *
* CARRY   0, если функция выполнилась без ошибки,                     *
* 1, если произошла ошибка.                                           *
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
*                 Преобразование физического адреса в линейный        *
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
