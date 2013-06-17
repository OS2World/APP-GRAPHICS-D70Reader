/* $Header: //xaptk/include/xapi.h#5 $ */
/* xapi.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/

/* Everything you need to include to use the XAP toolkit. */

#ifndef XAPI_H
#define XAPI_H /* as nothing */

#if defined(WITHIN_PS) && (WITHIN_PS == 0)
/*
This is unfortunate, but necessary in order to make both the Acrobat
Viewer and Distiller happy.
*/
#undef WITHIN_PS
#endif

#ifndef WITHIN_PS

#include "XAPDefs.h"
#include "XAPExcep.h"
#include "XAPPathTree.h"
#include "MetaXAP.h"
#include "XAPStart.h"

#endif /* WITHIN_PS */

#endif /* XAPI_H */
/*
$Log$
*/
