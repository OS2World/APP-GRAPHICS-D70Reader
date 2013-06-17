/* $Header: //xaptk/xaptk/xaptkalloc.cpp#8 $ */
/* xaptkalloc.cpp */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/

#ifndef XAP_CUSTOM_ALLOC

#include "XAPTkAlloc.h"
#include "XAPConfigure.h"
#include "XAPAllocator.h"
#include "XAPTkAlloc.h"
#include "XMPInitTerm.h"

#include <new>

XAPAllocator *	g_xapAllocator	= NULL;
bool			g_usingDefaultAllocator	= false;

#ifndef XAP_FORCE_NORMAL_ALLOC

#ifdef __cplusplus
    #ifdef _MSL_USING_NAMESPACE
        namespace std {
    #endif
    extern "C" {
#endif

void
free_override(void* ptr, const char* theFile, int theLine) {
    if ( g_xapAllocator == NULL ) XAPTk_InitDefaultAllocator();
    g_xapAllocator->xFree(ptr, theFile, theLine);
}

void*
calloc_override(size_t nmemb, size_t size, const char* theFile, int theLine) {
    if ( g_xapAllocator == NULL ) XAPTk_InitDefaultAllocator();
    return g_xapAllocator->xCalloc(nmemb, size, theFile, theLine);
}

void*
malloc_override(size_t size, const char* theFile, int theLine) {
    if ( g_xapAllocator == NULL ) XAPTk_InitDefaultAllocator();
    return g_xapAllocator->xMalloc(size, theFile, theLine);
}

void*
realloc_override(void* ptr, size_t size, const char* theFile, int theLine) {
    if ( g_xapAllocator == NULL ) XAPTk_InitDefaultAllocator();
    return g_xapAllocator->xRealloc(ptr, size, theFile, theLine);
}


#ifdef __cplusplus
    }
    #ifdef _MSL_USING_NAMESPACE
        }
    #endif
#endif

static void* DoNewThrow ( size_t s ) throw ( std::bad_alloc ) {
    if ( g_xapAllocator == NULL ) XAPTk_InitDefaultAllocator();
    if (s < 1) s = 1;
    while (true) {
        void* p = NULL;
        try {
            p = g_xapAllocator->xNew(s);
        } catch (std::bad_alloc&) {
            p = NULL;
        }
        if (p != NULL) return(p);
        std::new_handler globalHandler = std::set_new_handler(0);
        std::set_new_handler(globalHandler);

        if (globalHandler) {
            (*globalHandler)();
        } else {
            throw new std::bad_alloc;
        }
    }
}

static void* DoNewNoThrow ( size_t s, const std::nothrow_t& no ) throw () {
    if ( g_xapAllocator == NULL ) XAPTk_InitDefaultAllocator();
    if ( s < 1 ) s = 1;
    
    while (true) {
        void* p = g_xapAllocator->xSafeNew (s);
        if (p != NULL) return(p);
        std::new_handler globalHandler = std::set_new_handler(0);
        std::set_new_handler(globalHandler);
		try {
        	if ( globalHandler != NULL ) (*globalHandler)();
        } catch (...) {
        	return NULL;
        }
    }
}

void*
::operator new ( size_t s ) throw ( std::bad_alloc ) {
	return DoNewThrow ( s );
}

void*
::operator new[] ( size_t s ) throw ( std::bad_alloc ) {
	return DoNewThrow ( s );
}

void*
::operator new ( size_t s, const std::nothrow_t& no ) throw () {
	return DoNewNoThrow ( s, no );
}

void*
::operator new[] ( size_t s, const std::nothrow_t& no ) throw () {
	return DoNewNoThrow ( s, no );
}

void
::operator delete ( void* ptr ) throw () {
    if (ptr != NULL) g_xapAllocator->xDelete ( ptr );
}

void
::operator delete[] ( void* ptr ) throw () {
    if (ptr != NULL) g_xapAllocator->xDelete ( ptr );
}

void
::operator delete ( void* ptr, const std::nothrow_t& no ) throw () {
    if (ptr != NULL) g_xapAllocator->xDelete ( ptr );
}

void
::operator delete[] ( void* ptr, const std::nothrow_t& no ) throw () {
    if (ptr != NULL) g_xapAllocator->xDelete ( ptr );
}

#if defined(_DEBUG) && defined(WIN_ENV)
void*
operator new(size_t s, const char* file, int line) {
    if ( g_xapAllocator == NULL ) XAPTk_InitDefaultAllocator();
    return g_xapAllocator->xNew(s, file, line);
}
#endif /* _DEBUG && WIN_ENV */

#endif /* XAP_FORCE_NORMAL_ALLOC */

#endif /* XAP_CUSTOM_ALLOC */

void
XAPTk_InitAlloc ( XAPAllocator * xapAlloc ) {
	if ( g_xapAllocator == 0 ) {
		if ( xapAlloc != 0 ) {
			g_xapAllocator = xapAlloc;
		} else {
			XAPTk_InitDefaultAllocator();
		}
	}
}

void
XAPTk_KillAlloc() {
	if ( g_usingDefaultAllocator ) XAPTk_KillDefaultAllocator();
    g_xapAllocator = 0;
}


/*
$Log$
*/
