
/* $Header: //xaptk/xaptk/DefaultAllocator.h#3 $ */
/* DefaultAllocator.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.  If you have received this file from a 
source other than Adobe, then your use, modification, or distribution of it requires the prior 
written permission of Adobe.
*/


#ifndef DEFAULTALLOCATOR_H
#define DEFAULTALLOCATOR_H /* as nothing */


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


#ifndef XAP_CUSTOM_ALLOC

#include "XAPAllocator.h"

namespace XAPTk {

class DefaultAllocator : public XAPAllocator {
public:
    // Static (Class) Functions
    static XAPAllocator*
    New();

    // Public Member Functions
    virtual void*
    xCalloc(size_t nmemb, size_t size, const char* theFile, int theLine) throw();

    virtual void*
    xMalloc(size_t size, const char* theFile, int theLine) throw();

    virtual void*
    xRealloc(void* ptr, size_t size, const char* theFile, int theLine) throw();

    virtual void
    xFree(void* ptr, const char* theFile, int theLine) throw();

    virtual void
    xDelete(void* ptr) throw();

    virtual void*
    xNew(size_t s) throw(std::bad_alloc);

    virtual void*
    xNew(size_t s, const char* file, int line) throw (std::bad_alloc);

    virtual void*
    xSafeNew(size_t s) throw();
}; // DefaultAllocator

} // XAPTk

#endif /* XAP_CUSTOM_ALLOC */


#if macintosh
	#pragma options align=reset
#endif


#endif /* DEFAULTALLOCATOR_H */

/*
$Log$
*/

