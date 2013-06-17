
/* $Header:$ */
/* XAPStart.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/


#ifndef XAPSTART_H
#define XAPSTART_H /* as nothing */


#include "XAPConfigure.h"
#include "XAPAllocator.h"


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


extern void XAPTk_GeneralInit ( XAPAllocator * xapAlloc = NULL );
extern void XAPTk_Terminate ();


#if macintosh
	#pragma options align=reset
#endif


#endif /* XAPSTART_H */

/*
$Log$
*/

