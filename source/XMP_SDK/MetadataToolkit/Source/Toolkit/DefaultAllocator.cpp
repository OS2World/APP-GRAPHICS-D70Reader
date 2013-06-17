/* $Header: //xaptk/xaptk/DefaultAllocator.cpp#7 $ */
/* DefaultAllocator.cpp */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/

#ifndef XAP_CUSTOM_ALLOC

#include <stdlib.h>
#include "XAPConfigure.h"
#include "XAPAllocator.h"
#include "XAPTkAlloc.h"
#include "XMPInitTerm.h"
#include "DefaultAllocator.h"

#ifdef XAP_WIN_CRTDBG
	#include <crtdbg.h>
#endif

#ifdef XAP_DEBUG_MEMLOG
	#include <fstream.h>
	#include "XMPAssert.h"
#endif

#define DECL_VIRTUAL /* as nothing */
#define DECL_STATIC /* as nothing */

#ifdef XAP_DEBUG_MEMLOG
	//static ofstream flog("memlog.txt");
	static ofstream* flog = NULL;
#endif

#ifdef XAP_DEBUG_MEMLOG
	static size_t nAllocs = 0;
	static size_t nReallocs = 0;
	static size_t nNews = 0;
#endif

#ifdef XAP_DEBUG_MEMLOG

	typedef struct {
	    size_t loGuard;
	    char allocType[4];
	    size_t bytes;
	    const char* theFile;
	    long int theLine;
	    size_t hiGuard;
	} XAPTk_MemInfo;

	static const size_t SZ_MEMINFO = sizeof(XAPTk_MemInfo);

	static const char AT_CALLOC = 'C';
	static const char AT_MALLOC = 'M';
	static const char AT_REALLOC = 'R';
	static const char AT_NEW = 'N';

	static const size_t HI_GUARD = 0xEBDAEBDA;
	static const size_t LO_GUARD = 0xADBEADBE;

	static void*
	setMemInfo(void* block, size_t bytes, char allocType, const char* theFile, const int theLine) {
	    XAPTk_MemInfo *info = (XAPTk_MemInfo*)block;
	    info->loGuard = LO_GUARD;
	    info->allocType[0] = allocType;
	    info->allocType[1] = 0;
	    info->bytes = bytes;
	    info->theFile = theFile;
	    info->theLine = theLine;
	    info->hiGuard = HI_GUARD;
	    return(((char*)block) + SZ_MEMINFO);
	}

#endif /* XAP_DEBUG_MEMLOG */

namespace XAPTk {

DECL_STATIC XAPAllocator*
DefaultAllocator::New() {
	#ifndef XAP_DEBUG_MEMLOG
	    void* here = calloc(1, sizeof(class DefaultAllocator));
	#else
	    const size_t szd = sizeof(class DefaultAllocator);
	    size_t fullSize = szd + SZ_MEMINFO;
		#ifdef XAP_WIN_CRTDBG
		    void* here = _calloc_dbg(1, fullSize, _NORMAL_BLOCK, __FILE__, __LINE__);
		#else
		    void* here = calloc(1, fullSize);
		#endif
	    here = setMemInfo(here, szd, AT_NEW, __FILE__, __LINE__);
	    nNews += szd;
	#endif
    return new(here) DefaultAllocator();
}

DECL_VIRTUAL void*
DefaultAllocator::xCalloc(size_t nmemb, size_t size, const char* /* theFile */, int /* theLine */ ) throw() {
	#ifndef XAP_DEBUG_MEMLOG
	    return(calloc(nmemb, size));
	#else
	    size_t sz = nmemb * size;
		#ifdef XAP_WIN_CRTDBG
		    void* p = _calloc_dbg(1, sz + SZ_MEMINFO, _NORMAL_BLOCK, theFile, theLine);
		#else
		    void* p = calloc(1, sz + SZ_MEMINFO);
		#endif
	    p = setMemInfo(p, sz, AT_CALLOC, theFile, theLine);
	    nAllocs += sz;
	    return(p);
	#endif
}

DECL_VIRTUAL void*
DefaultAllocator::xMalloc(size_t size, const char* /* theFile */, int /* theLine */ ) throw() {
	#ifndef XAP_DEBUG_MEMLOG
	    return(malloc(size));
	#else
		#ifdef XAP_WIN_CRTDBG
		    void* p = _malloc_dbg(size + SZ_MEMINFO, _NORMAL_BLOCK, theFile, theLine);
		#else
		    void* p = malloc(size + SZ_MEMINFO);
		#endif
	    p = setMemInfo(p, size, AT_MALLOC, theFile, theLine);
	    nAllocs += size;
	    return(p);
	#endif
}

DECL_VIRTUAL void*
DefaultAllocator::xRealloc(void* ptr, size_t size, const char* /* theFile */, int /* theLine */ ) throw() {
	#ifndef XAP_DEBUG_MEMLOG
	    return(realloc(ptr, size));
	#else
	    XAPTk_MemInfo* info = (XAPTk_MemInfo*)((char*)ptr - SZ_MEMINFO);
	    assert(info->loGuard == LO_GUARD);
	    assert(info->hiGuard == HI_GUARD);
	    assert(info->allocType[0] != AT_NEW);
	    nAllocs -= info->bytes;
		#ifdef XAP_WIN_CRTDBG
		    void* p = _realloc_dbg(info, size + SZ_MEMINFO, _NORMAL_BLOCK, theFile, theLine);
		#else
		    void* p = realloc(info, size + SZ_MEMINFO);
		#endif
	    p = setMemInfo(p, size, AT_REALLOC, theFile, theLine);
	    nAllocs += size;
	    ++nReallocs;
	    return(p);
	#endif
}

DECL_VIRTUAL void
DefaultAllocator::xFree(void* ptr, const char* /* theFile */, int /* theLine */ ) throw() {
    void* p = ptr;
    if (p == NULL) return;
	#ifdef XAP_DEBUG_MEMLOG
	    XAPTk_MemInfo* info = (XAPTk_MemInfo*)((char*)ptr - SZ_MEMINFO);
	    assert(info->loGuard == LO_GUARD);
	    assert(info->hiGuard == HI_GUARD);
	    assert(info->allocType[0] != AT_NEW);
	    nAllocs -= info->bytes;
	    if (flog)
	    *flog << "F[C:" << nAllocs << ", N:" << nNews << ", R:" << nReallocs << "]" << endl;
	    p = (void*)info;
	#endif
    free(p);
}

DECL_VIRTUAL void
DefaultAllocator::xDelete(void* ptr) throw() {
    void* p = ptr;
    if (p == NULL) return;
	#ifdef XAP_DEBUG_MEMLOG
	    XAPTk_MemInfo* info = (XAPTk_MemInfo*)((char*)ptr - SZ_MEMINFO);
	    assert(info->loGuard == LO_GUARD);
	    assert(info->hiGuard == HI_GUARD);
	    assert(info->allocType[0] == AT_NEW);
	    nNews -= info->bytes;
	    if (flog)
	    *flog << "D[C:" << nAllocs << ", N:" << nNews << ", R:" << nReallocs << "]" << endl;
	    p = (void*)info;
	#endif
    free(p);
}

DECL_VIRTUAL void*
DefaultAllocator::xNew(size_t s) throw(std::bad_alloc) {
	#ifndef XAP_DEBUG_MEMLOG
	    return calloc(1, s);
	#else
		#ifdef XAP_WIN_CRTDBG
		    void* p = _calloc_dbg(1, s + SZ_MEMINFO, _NORMAL_BLOCK, __FILE__, __LINE__);
		#else
		    void* p = calloc(1, s + SZ_MEMINFO);
		#endif
	    p = setMemInfo(p, s, AT_NEW, __FILE__, __LINE__);
	    nNews += s;
	    return p;
	#endif
}

DECL_VIRTUAL void*
DefaultAllocator::xNew(size_t s, const char* /* theFile */, int /* theLine */ ) throw (std::bad_alloc) {
	#ifndef XAP_DEBUG_MEMLOG
	    return calloc(1, s);
	#else
		#ifdef XAP_WIN_CRTDBG
		    void* p = operator new(s + SZ_MEMINFO, _NORMAL_BLOCK, file, line);
		#else
		    void* p = calloc(1, s + SZ_MEMINFO);
		#endif
	    p = setMemInfo(p, s, AT_NEW, file, line);
	    nNews += s;
	    return p;
	#endif
}

DECL_VIRTUAL void*
DefaultAllocator::xSafeNew(size_t s) throw() {
	#ifndef XAP_DEBUG_MEMLOG
	    return calloc(1, s);
	#else
		#ifdef XAP_WIN_CRTDBG
    		void* p = _calloc_dbg(1, s + SZ_MEMINFO, _NORMAL_BLOCK, __FILE__, __LINE__);
		#else
    		void* p = calloc(1, s + SZ_MEMINFO);
		#endif
	    p = setMemInfo(p, s, AT_NEW, __FILE__, __LINE__);
	    nNews += s;
	    return p;
	#endif
}

} // XAPTk

void
XAPTk_InitDefaultAllocator() {
    if ( g_xapAllocator == NULL ) {
    	g_xapAllocator = XAPTk::DefaultAllocator::New();
	    g_usingDefaultAllocator = true;
		#ifdef XAP_DEBUG_MEMLOG
		    if ( flog == NULL ) flog = new ofstream ( "memlog.txt" );
		#endif
	}
}

void
XAPTk_KillDefaultAllocator() {
	if ( ! g_usingDefaultAllocator ) return;
	#ifndef XAP_DEBUG_MEMLOG
	    free ( g_xapAllocator );
	#else
		XAPTk_MemInfo* info = NULL;
		if ( g_xapAllocator != NULL ) {
			info = (XAPTk_MemInfo*) ((char*)g_xapAllocator - SZ_MEMINFO);
			assert ( info->loGuard == LO_GUARD );
			assert ( info->hiGuard == HI_GUARD );
			assert ( info->allocType[0] == AT_NEW );
			nNews -= info->bytes;
			if ( flog ) {
				XAPTk_MemInfo *in = (XAPTk_MemInfo*) ((char*)flog - SZ_MEMINFO);
				assert ( info->loGuard == LO_GUARD );
				assert ( info->hiGuard == HI_GUARD );
				nNews -= in->bytes;
				*flog << "Z[C:" << nAllocs << ", N:" << nNews << ", R:" << nReallocs << "]" << endl;
			}
	    }
	    ofstream* tmp = flog;
	    flog = NULL;
	    delete tmp; // calls g_xapAllocator->xDelete
		if ( info != NULL ) free ( info );
	#endif
    g_xapAllocator = NULL;
    g_usingDefaultAllocator = false;
}


#endif /* XAP_CUSTOM_ALLOC */

/*
$Log$
*/
