
/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/

#ifndef __XMPInitTerm__
#define __XMPInitTerm__


#ifdef __cplusplus	/* Only declare these routines for C++ compiles, the include chain chokes plain C compilers. */

#include "XAPTkAlloc.h"

// Listed in initialization order, for convenience.

extern void XAPTk_InitAlloc  ( XAPAllocator * xapAlloc );

extern void XAPTk_InitDOM();
extern void XAPTk_InitConstData();
extern void XAPTk_InitDOMGlue();

extern void XAPTk_InitMetaXAP();
extern void XAPTk_InitNormTree();
extern void XAPTk_InitNormTreeSerializer();
extern void XAPTk_InitPaths();
extern void XAPTk_InitRDFToNormTrees();

extern void XAPTk_InitUtilityXAP();
extern void XAPTk_InitData();
extern void XAPTk_InitFuncs();

extern bool	gXMP_IsInitialized;
#define XMP_CheckToolkitInit	if ( ! gXMP_IsInitialized ) XAPTk_GeneralInit

extern void XAPTk_KillAlloc();

extern void XAPTk_KillDOM();
extern void XAPTk_KillConstData();
extern void XAPTk_KillDOMGlue();

extern void XAPTk_KillMetaXAP();
extern void XAPTk_KillNormTree();
extern void XAPTk_KillNormTreeSerializer();
extern void XAPTk_KillPaths();
extern void XAPTk_KillRDFToNormTrees();

extern void XAPTk_KillUtilityXAP();
extern void XAPTk_KillData();
extern void XAPTk_KillFuncs();

#endif	/* C++ only declarations. */


#endif	/* __XMPInitTerm__ */
