
/* $Header: //xaptk/xaptk/XAPTkAlloc.h#4 $ */
/* XAPTkAlloc.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.  If you have received this file from a 
source other than Adobe, then your use, modification, or distribution of it requires the prior 
written permission of Adobe.
*/


#ifndef XAPTKALLOC_H
#define XAPTKALLOC_H /* as nothing */

#ifdef XAP_CUSTOM_ALLOC
#include "XAP_Custom_Alloc.h"
#else /* XAP_CUSTOM_ALLOC */

/*
If you implement xap_custom_alloc.h, model it after the contents of this
header file.  The objective is to define overrides for calloc, malloc,
realloc, free, and operators new and delete.
*/

#include "XAPConfigure.h"
#include "XAPAllocator.h"

#include <stdlib.h>


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


extern XAPAllocator *	g_xapAllocator;
extern bool				g_usingDefaultAllocator;

extern void XAPTk_InitDefaultAllocator();
extern void XAPTk_KillDefaultAllocator();

#ifdef __cplusplus
    // #ifdef _MSL_USING_NAMESPACE
    //     namespace std {
    // #endif
    extern "C" {
#endif

#undef CALLOC
#undef MALLOC
#undef REALLOC
#undef FREE

#define CALLOC(x,y) calloc_override ( x, y, __FILE__, __LINE__ )
#define MALLOC(x) malloc_override ( x, __FILE__, __LINE__ )
#define REALLOC(x,y) realloc_override ( x, y, __FILE__, __LINE__ )
#define FREE(x) free_override ( x, __FILE__, __LINE__ )

void*   calloc_override ( size_t nmemb, size_t size, const char* theFile, int theLine );
void*   malloc_override ( size_t size, const char* theFile, int theLine );
void*   realloc_override ( void* ptr, size_t size, const char* theFile, int theLine );
void    free_override ( void* ptr, const char* theFile, int theLine );

#ifdef __cplusplus      /* hh  971206 */
    }
    // #ifdef _MSL_USING_NAMESPACE
    //     }
    // #endif
#endif

/* Operators */

#ifndef XAP_FORCE_NORMAL_ALLOC

#ifdef __cplusplus

#include <new>

void* ::operator new ( size_t s ) throw ( std::bad_alloc );
void* ::operator new[] ( size_t s ) throw ( std::bad_alloc );
void* ::operator new ( size_t s, const std::nothrow_t& no ) throw ();
void* ::operator new[] ( size_t s, const std::nothrow_t& no ) throw ();

void ::operator delete ( void* ptr ) throw ();
void ::operator delete[] ( void* ptr ) throw ();
void ::operator delete ( void* ptr, const std::nothrow_t& no ) throw ();
void ::operator delete[] ( void* ptr, const std::nothrow_t& no ) throw ();

#if defined(_DEBUG) && defined(WIN_ENV)
void* operator new ( size_t s, const char* file, int line );
#endif /* _DEBUG && WIN_ENV */

#endif /* __cplusplus */

#endif /* XAP_FORCE_NORMAL_ALLOC */

#endif /* XAP_CUSTOM_ALLOC */


#if macintosh
	#pragma options align=reset
#endif


#endif /* XAPTKALLOC_H */


/*
$Log$
*/

