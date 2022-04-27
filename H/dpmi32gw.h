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
*          Local Descriptor Table Management Services (00xx)              *
**********************************************************************/

//                Установить поле прав доступа в дескрипторе
// В расширении байта прав доступа старший бит
//    (бит 7) - бит гранулярности, интерпретация поля предела
//               (0 - предел в байтах, 1 - предел в страницах).
//    Бит 6 указывает режим работы процессора
//        (0 - 16-разрядный, 1 - 32-разрядный).
//    Бит 5 и 2-3 должны быть равны 0.
//    Бит 4 может содержать любое значение.
BOOL DPMISetSelectorAccess(uint16 sel,uint16 attr);

// Создать дескриптор в таблице LDT
BOOL DPMIAllocSelector(uint16 *sel,int count=1);
inline BOOL DpmiAllocSelector(uint16 *sel)
       {    if(DPMIAllocSelector(sel)) return DPMISetSelectorAccess(*sel,DPMI_DEFAULT); return FALSE; }

// Освободить дескриптор в таблице LDT
BOOL DPMIFreeSelector(uint16 sel);
inline BOOL DpmiFreeSelector(uint16 sel)
       { return DPMIFreeSelector(sel); }

// Преобразование сегмента адреса реального режима в дескриптор.
// Дескриптор, созданный данной функцией, не может быть освобождён.
BOOL DPMIMakeRMSelector(uint16 seg,uint16 *sel);

// Если вы создали несколько дескрипторов в LDT при помощи функции 0h,
// то этой функцией будет возвращён только первый селектор,
// соответствующий первому дескриптору в созданном массиве.
// Эта функция возвращает значение, которое вы будете прибавлять к
// базовому селектору для вычисления остальных селекторов в массиве.
int DPMIIncrementSelector(void);

// Получить базовый адрес сегмента
BOOL DPMIGetSelectorBase(uint16 sel,ulong *addr);
inline ulong DpmiGetSelectorBase(uint16 sel)
                    { ulong addr; return DPMIGetSelectorBase(sel,&addr)?addr:0; }

// Установить базовый адрес сегмента. Вы можете модифицировать только те
// дескрипторы, которые создала ваша программа.                                     *
BOOL DPMISetSelectorBase(uint16 sel,ulong linAddr);
#define DpmiSetSelectorBase(x,y) DPMISetSelectorBase(x,y)

// Установить предел сегмента
BOOL DPMISetSelectorLimit(uint16 sel,ulong limit);
// более "умная" версия. При слишком большом пределе меняет атрибуты
// дескриптора. В отличие от предыдущей понимает размер только в байтах
BOOL DPMISetSelectorLimitX(uint16 sel,ulong limit);
#define DpmiSetSelectorLimit(x,y) DPMISetSelectorLimitX(x,y*4096)

// Создание алиасного дескриптора для сегмента кода
BOOL DPMIMakeSelectorAlias(uint16 sel,uint16 *asel);

// Получить дескриптор указанного селектора
BOOL DPMIGetDescriptor(uint16 sel,DescriptorStruct __far* dsr);

// Установить дескриптор указанного селектора
BOOL DPMISetDescriptor(uint16 sel,DescriptorStruct __far* dsr);

// Выделить конкретный дескриптор в таблице LDT
// Для этой фичи в таблице ЛДТ зарезервировано первые 16 дескрипторов
BOOL DPMIAllocDescriptor(uint16 sel);


/**********************************************************************
*             DOS Memory Management Services (01xx)                      *
**********************************************************************/
// Получить блок памяти из пула свободной памяти DOS. Если размер
// запрошенного блока превышает 64 килобайта, функция создаст массив
// дескрипторов и возвратит базовый селектор.
void __far* DPMIDOSAlloc(ulong size,uint16 *sel,uint16 *rseg);

inline void __far* DPMIDOSAlloc(ulong size,uint16 *rseg)
       { return DPMIDOSAlloc(size,NULL,rseg); }

#define DpmiAllocRealSeg(x,y) DPMIDOSAlloc(x,y)

// Освободить блок памяти, взятый из пула DOS
BOOL DPMIDOSFree(uint16 sel);
#define DpmiFreeRealSeg(x) DPMIDOSFree(x)
inline void DpmiFreeRealSeg(void __far* ptr) { if(ptr!=NULL) DPMIDOSFree(FP_SEG(ptr)); }

// Изменить размер блока памяти, полученного из пула DOS
BOOL DPMIDOSRealloc(uint16 sel,ulong newsize);

/**********************************************************************
*                         Interrupt Services (02xx)                      *
**********************************************************************/

// Получить вектор прерывания реального режима
ulong DPMIRMGetInterrupt(unsigned i);

// Установить вектор прерывания реального режима
BOOL DPMIRMSetInterrupt(unsigned i,ulong addr);

// Получить адрес обработчика исключения с заданным номером
void __far* DPMIGetExceptionHandler(unsigned i);

// Устанвить адрес обработчика исключения с заданным номером
BOOL DPMISetExceptionHandler(unsigned i,void __far* ptr);

// Получить вектор прерывания защищённого режима
InterruptHandlerPtr DPMIGetInterrupt(unsigned n);

// Установить вектор прерывания защищённого режима
BOOL DPMISetInterrupt(unsigned i,InterruptHandlerPtr ptr);

/**********************************************************************
*                         Translation Services (03xx)                      *
**********************************************************************/

// Эмуляция прерывания реального режима
BOOL DPMIRMInt(unsigned i,RMREGS *rmregs,RMREGS __far** rgout=NULL);

// Вызов процедуры реального режима, заканчивающейся командой RET FAR
BOOL DPMIRMCall(RMREGS *rmregs);

// Вызов процедуры реального режима, заканчивающейся командой IRET FAR
BOOL DPMIRMCalliret(RMREGS *rmregs);


/**********************************************************************
*                        Memory Management Services                    *
**********************************************************************/

// Получить информацию о свободной памяти
BOOL DPMIFreeMemInfo(FreeMemInfo *meminfo);
void DPMIDumpFreeMemInfo(ostream& os,FreeMemInfo* mi);
inline void DPMIDumpFreeMemInfo(ostream& os)
       {    FreeMemInfo mi; DPMIFreeMemInfo(&mi); DPMIDumpFreeMemInfo(os,&mi); }

// Заказ памяти
BOOL DPMILinearAlloc(ulong size,ulong* addr,ulong* handle);
void __far* DPMIalloc(ulong size,ulong *handle);
inline void __far* DpmiMemAlloc(ulong size,ulong *handle)
       { return DPMIalloc(size,handle);                 }

// Освобождение
BOOL DPMILinearFree(ulong handle);
inline BOOL DPMIfree(void __far *ptr,ulong handle)
       { return DPMIFreeSelector(FP_SEG(ptr)) && DPMILinearFree(handle); }
#define DpmiMemFree(x,y) DPMIfree(x,y)

// Изменение размера
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

// Полная информация о памяти
void DPMITotalMemInfo(TotalMemInfo *meminfo);

/**********************************************************************
*                Page Locking Services ( 06xx)                              *
**********************************************************************/

// Зафиксировать линейную область памяти
BOOL DPMIlock(ulong addr,ulong size);

// Расфиксирование блока памяти
BOOL DPMIunlock(ulong addr,ulong size);

/**********************************************************************
*                Преобразование физического адреса в линейный          *
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