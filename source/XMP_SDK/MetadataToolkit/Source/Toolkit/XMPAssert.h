/* $Header: //xaptk/xaptk/XMPAssert.h $ */
/* XMPAssert.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.  If you have received this file from a 
source other than Adobe, then your use, modification, or distribution of it requires the prior 
written permission of Adobe.
*/

/* Stuff for the internal implementation of the XAP library. */

#ifndef __XMPAssert__
#define __XMPAssert__	1

#if XAP_DEBUG_BUILD

	#include <cassert>

#else

	// Asserts ought to do nothing in release builds.  But at the moment there are known cases
	// of unsupported, but not unlikely RDF, where removing the assert leads to a crash when
	// the assert would have failed.  So for now we will have the assert throw an exception.
	
	#ifdef assert
		#undef assert
	#endif
	
	extern void XMPAssertFailure();
	
	#define assert(cond)	if ( ! (cond) ) XMPAssertFailure()

#endif

#endif // __XMPAssert__

/*
$Log$
*/
