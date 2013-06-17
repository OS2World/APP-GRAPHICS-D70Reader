
/* $Header: //xaptk/include/XAPAllocator.h#5 $ */
/* XAPAllocator.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/


#ifndef XAPALLOCATOR_H
#define XAPALLOCATOR_H /* as nothing */


/*
"Interface" class for implementing custom allocators.

If XAP_FORCE_NORMAL_ALLOC is defined, normal allocator functions
are used.

Otherwise, if XAP_CUSTOM_ALLOC is not defined, the default allocator
is used, as explained here.  The default allocator can be used as
an example of how to implement a custom allocator.

The default allocator is composed of the following files:
    xaptk/XAPTkAlloc.h
    xaptk/xaptkalloc.cpp
    xaptk/DefaultAllocator.h
    xaptk/DefaultAllocator.cpp

The header file XAPTkAlloc.h declares macros for the standard
C library memory routines malloc, calloc, realloc, and free.
It also declares overrides for the new and delete operators.
Each macro and override calls functions defined in xaptkalloc.cpp.
All modules that do allocation include this header file.

xaptkalloc.cpp implements the overriden allocator functions. It
uses an instance of the DefaultAllocator.h class to do the
actual allocation.

DefaultAllocator.h declares the class that implements allocations.
The DefaultAllocator class is derived from XAPAllocator.

DefaultAllocator.cpp is the implementation of that class.  By NOT
including XAPTkAlloc.h, it avoids the override macros, and calls
C library memory functions directly, even for new and delete.

There are two ways to implement a custom allocator.

1) You can leave XAP_CUSTOM_ALLOC undefined, and simply change the implementation of DefaultAllocator.cpp.

2) You can define XAP_CUSTOM_ALLOC and provide your own implementation
via the header file xap_custom_alloc.h (which is included by XAPTkAlloc.h
into every XAP module that does allocation.)  xap_custom_alloc.h should
be based on XAPTkAlloc.h, and your custom implementation should have
a structure similar to xaptkalloc.cpp and DefaultAllocator.cpp.  You
will also need to implement the global function
    void XAPTk_InitAlloc(int n);
which is called at static initialization time.

*/

#include <new>

#if defined(WITHIN_PS) && (WITHIN_PS == 0)
/*
This is unfortunate, but necessary in order to make both the Acrobat
Viewer and Distiller happy.
*/
#undef WITHIN_PS
#endif

#ifndef WITHIN_PS
#include "XAPConfigure.h"
#endif /* WITHIN_PS */


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


//MACPORT: using namespace std;

class XAPAllocator {

public:

    // Destructor
    virtual ~XAPAllocator() {};

    // Public Member Functions
    virtual void*
    xCalloc(size_t nmemb, size_t size, const char* theFile, int theLine) throw() = 0;

    virtual void*
    xMalloc(size_t size, const char* theFile, int theLine) throw() = 0;

    virtual void*
    xRealloc(void* ptr, size_t size, const char* theFile, int theLine) throw() = 0;

    virtual void
    xFree(void* ptr, const char* theFile, int theLine) throw() = 0;

    virtual void
    xDelete(void* ptr) throw() = 0;

    virtual void*
    xNew(size_t s) throw(std::bad_alloc) = 0;

    virtual void*
    xNew(size_t s, const char* file, int line) throw (std::bad_alloc) = 0;
        /*
        Debug version of new, invoked as:
            new(sizeof(C), __FILE__, __LINE__);
        */

    virtual void*
    xSafeNew(size_t s) throw() = 0;

};


#if macintosh
	#pragma options align=reset
#endif


#endif /* XAPALLOCATOR_H */

/*
$Log$
*/

