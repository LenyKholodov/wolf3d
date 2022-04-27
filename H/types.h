/****************************************************************************
*    Библиотека типов.                                                	    *
*    Версия 1.00                                                            *
*    Используется для Borland C++ 3.1, Watcom C++ 10.0, Visual C++ 6.0      *
*    Перечислены общие типы чисел и указателей.                             *
****************************************************************************/
#ifndef __WDT_TYPES_H__
#define __WDT_TYPES_H__

#ifdef __BORLANDC__
        #include <_defs.h>
        #define IOS_FMTFLAGS long
#else
        #define IOS_FMTFLAGS ios::fmtflags
	#ifdef  __cplusplus
		#define _PTRDEF(name) typedef name* P##name;
		#define _REFDEF(name) typedef name& R##name;
	#endif
#endif

#ifndef __WATCOMC__
	#if _MSC_VER>1000
		#define __VISUALC__
		#define __386__
		#define __WINDOWS__
	#else
		#if defined(__BORLANDC__) && defined(_WINDOWS)
			#define __WINDOWS__
		#endif
		#if sizeof(int)==4
			#define __386__
		#endif
	#endif
#else
	#ifdef __386__
	       	#ifdef __3S__
	       	      #pragma library ("vm3s.lib")
		#endif

		#ifdef __3R__
		      #pragma library ("vm3r.lib")
		#endif
	#endif
#endif

#ifdef __386__
	typedef short	int16;
	typedef unsigned short	uint16;
	#define INT_HEX_DIGIT_WIDTH	8
#else
	typedef short	int16;
	typedef unsigned int uint16;
	#define INT_HEX_DIGIT_WIDTH	4
#endif

typedef unsigned char uchar;
typedef unsigned long ulong;

typedef long	 int32;
typedef unsigned long	 uint32;

typedef unsigned int uint;
typedef unsigned short ushort;

#ifdef __cplusplus
	#define _LPTRDEF(name) 	typedef name far* LP##name;
	#define _LREFDEF(name) 	typedef name far& LR##name;
	#define _LPTRCDEF(name) typedef const name far* LPC##name;
	#define _LREFCDEF(name) typedef const name far& LRC##name;
	#define _PTRCDEF(name) 	typedef const name* PC##name;
	#define _REFCDEF(name) 	typedef const name& RC##name;
	#define _CDEF(name)     typedef const name C##name;
	#define TDEF(name)     _PTRDEF(name) \
			       _REFDEF(name) \
			       _LPTRDEF(name) \
			       _LREFDEF(name) \
			       _LPTRCDEF(name) \
			       _LREFCDEF(name) \
			       _PTRCDEF(name) \
			       _REFCDEF(name) \
			       _CDEF(name)
	#define CLASSDEF(name) class name; TDEF(name)
#endif

#ifdef __VISUALC__
       #include <wtypes.h>
#else
	#ifndef BOOL
		typedef int BOOL;
		#define TRUE 	1
		#define FALSE 	0
	#endif

	typedef long	 	INT32;
	typedef unsigned long	UINT32;

	typedef void		VOID;
	typedef unsigned char 	BYTE;
	typedef unsigned long 	DWORD;
	typedef long 	 	LONG;
	typedef uint16	      	WORD;
	typedef char far*     	LPSTR;
	typedef const char far* LPCSTR;
	#ifdef __cplusplus
	       TDEF(BOOL);
	       TDEF(BYTE);
	       TDEF(WORD);
	       TDEF(DWORD);
	       _PTRDEF(VOID)
	       _LPTRDEF(VOID);
	       _LPTRCDEF(VOID);
	       _PTRCDEF(VOID);
	#endif
#endif

typedef uint16	 	UINT16;
typedef int16		INT16;

typedef char far*      	CHPTR;
typedef const char far*	CHCPTR;
typedef char far* 	     chptr;
typedef unsigned int far*    uintptr;
typedef int   far*  	     intptr;
typedef ulong far* 	     ulongptr;
typedef long  far* 	     longptr;

typedef void (__interrupt __far* InterruptHandlerPtr)();

#endif
