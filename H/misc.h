#ifndef __MISC_H__
#define __MISC_H__

#include <limits.h>
#include <types.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define DEFAULT_CACHE_SIZE 32768

template <class T,class T1> inline T max(T a,T1 b) { return a>T(b)?a:T(b); }
template <class T,class T1> inline T min(T a,T1 b) { return a<T(b)?a:T(b); }
template <class T> void swap(T& x,T& y)	     	   { T t=x; x=y; y=t; }
template <class T> inline T sqr(T x) 		   { return x*x; }

inline int sign(int val)    { return val==0?0:(val>0?1:-1); }

#ifndef __VISUALC__

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))

#endif

#define lobyte(w) LOBYTE(w)
#define hibyte(w) HIBYTE(w)
#define loword(l) LOWORD(l)
#define hiword(l) HIWORD(l)

#define dword(h,l)      ((((ulong)(h)) << 16) | ((l) & 0xFFFF))
#define word(h,l)       ((((uint16)(h)) << 8) | ((l) & 0x00FF))

inline uint16 MakeWord(ulong h, ulong l) { return (h<<8) | (l&0x00FF); }
inline ulong  MakeDword(ulong h,ulong l) { return (h<<16)| (l & 0xFFFF); }

#ifdef __386__
#define   mem_char(  adr )   (*(uchar  *)(adr))
#define   mem_short( adr )   (*(ushort *)(adr))
#define   mem_long(  adr )   (*(ulong  *)(adr))
#else
#define   mem_char(  adr )   (*(uchar  far *)(adr))
#define   mem_short( adr )   (*(ushort far *)(adr))
#define   mem_long(  adr )   (*(ulong  far *)(adr))
#endif

void memsetd(void* ptr,long data,int count);
void memsetw(void* ptr,int data,int count);
void outsb(int port,void* ptr,int count);
void outsw(int port,void* ptr,int count);
void insb(int port,void* ptr,int count);
void insw(int port,void* ptr,int count);


#ifdef __WATCOMC__

long imuldiv(long,long,long);
#pragma aux imuldiv parm [eax] [edx] [ebx] value [eax] = "imul edx" "idiv ebx"
unsigned long umuldiv(unsigned long,unsigned long,unsigned long);
#pragma aux umuldiv parm [eax] [edx] [ebx] value [eax] = "mul edx" "div ebx"
long imulshr16(long,long);
#pragma aux imulshr16 parm [eax] [edx] [ebx] value [eax] = "imul edx" "shrd eax,edx,16"
long imulshr8(long,long);
#pragma aux imulshr8 parm [eax] [edx] [ebx] value [eax] = "imul edx" "shrd eax,edx,8"
unsigned long umulshr16(unsigned long,unsigned long);
#pragma aux umulshr16 parm [eax] [edx] [ebx] value [eax] = "mul edx" "shrd eax,edx,16"
unsigned long umuldivrnd(unsigned long, unsigned long, unsigned long);
#pragma aux umuldivrnd parm [eax] [edx] [ecx] modify [ebx] = "mul edx" "mov ebx,ecx" "shr ebx,1" "add eax,ebx" "adc edx,0" "div ecx"
#pragma aux memsetd parm [edi] [eax] [ecx] = "rep stosd"
#pragma aux memsetw parm [edi] [eax] [ecx] = "rep stosw"
void memsetb(void *, int, int);
#pragma aux memsetb parm [edi] [eax] [ecx] = "rep stosb"

#pragma aux outsb parm [edx] [esi] [ecx] = "rep outsb";
#pragma aux outsw parm [edx] [esi] [ecx] = "rep outsw";
void outsd(int port,void* ptr,int count);
#pragma aux outsd parm [edx] [esi] [ecx] = "rep outsd";

#pragma aux insb parm [edx] [edi] [ecx] = "rep insb";
#pragma aux insw parm [edx] [edi] [ecx] = "rep insw";
void insd(int port,void* ptr,int count);
#pragma aux insd parm [edx] [edi] [ecx] = "rep insd";

short _disableint(void);
#pragma aux _disableint value [ax] = "pushf" "pop ax" "cli"
void _restoreint(short);
#pragma aux _restoreint parm [ax] = "push ax" "popf"

#else

inline void memsetb(void* ptr,int data,int count) { memset(ptr,data,count); }

#endif

extern uint16 zxSeed;
inline uint16 zxRandom(void)  {	return (zxSeed=zxSeed*75-1)/2; }

#ifndef __BORLANDC__
inline int random(int val)  { return rand()%val; }
#endif

const ulong MagicCRC=0xEDB88320;

inline ulong CRC32(uchar data,ulong crc) 
      		  { if((((crc<<=1)>>24)^data)&0x80) crc^=MagicCRC; data<<=1; 
		    if((((crc<<=1)>>24)^data)&0x80) crc^=MagicCRC; data<<=1; 
		    if((((crc<<=1)>>24)^data)&0x80) crc^=MagicCRC; data<<=1; 
		    if((((crc<<=1)>>24)^data)&0x80) crc^=MagicCRC; data<<=1; 
		    if((((crc<<=1)>>24)^data)&0x80) crc^=MagicCRC; data<<=1; 
		    if((((crc<<=1)>>24)^data)&0x80) crc^=MagicCRC; data<<=1; 
		    if((((crc<<=1)>>24)^data)&0x80) crc^=MagicCRC; data<<=1; 
		    if((((crc<<=1)>>24)^data)&0x80) crc^=MagicCRC;
		    return crc; }

ulong CRC32(uchar* data,int size,ulong crc=0xFFFFFFFF);
char* tempname(char *dir=NULL);
BOOL MakeDir(char* dir);

long nod(long a,long b);

void killSpace(char* str,int maxsize=INT_MAX);

#endif
