
/* $Header: //xaptk/include/XAPConfigure.h#6 $ */
/* XAPConfigure.h */
/* OS specific definitions */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/


#ifndef __XAPConfigure__
#define __XAPConfigure__	1


/*======================== All Environments ========================*/


/* define size_t */
#include <stddef.h>

#ifndef XAP_ENUM_TYPE
	#if ! FriendlyEnumDebugging
		// This avoids possible problems with compiler settings that affect enum size.
		#define XAP_ENUM_TYPE(name)		typedef unsigned char name;   enum 
	#else
		// This let some debuggers show enum value symbols.
		#define XAP_ENUM_TYPE(name)		enum name 
		#if __MWERKS__ && __option ( enumsalwaysint )
			#error "The debugging form of XAP_ENUM_TYPE requires minumum sized enums."
		#endif
	#endif
#endif

#ifndef NULL
	#define NULL	0
#endif


/*======================== WIN_ENV ========================*/

#if !defined(WIN_ENV) && defined(WIN32)
#define WIN_ENV	1
#endif

#if WIN_ENV

#define ENV_DEFINED	1

#ifndef XAP_DLL_Build
	#define XAP_DLL_Build	0
#endif

#define WSTRING_SUPPORTED 1

#pragma warning(disable:4291) /* Win: new exception warning */
#pragma warning(disable:4503) /* Win: decorated name too long */
#pragma warning(disable:4786) /* Win: truncated browse info symbol */

#ifdef XAP_BUILT_IN
 #define XAP_API /* as nothing */
#else
 #ifndef XAP_API
 #define XAP_API __declspec(dllimport)
 #endif /* XAP_API */
#endif /* XAP_BUILT_IN */

#define XAP_SAY_WHAT(s) XAP_WHAT_SAY(s,__LINE__)
#define XAP_QUOTE_ME(s) #s
#define XAP_WHAT_SAY(s,l) s " in " __FILE__ ":" XAP_QUOTE_ME(l)

#ifdef XAP_FORCE_NORMAL_ALLOC
 #if _DEBUG
  #include <crtdbg.h>
  #define XMP_Debug_new /* (_NORMAL_BLOCK, __FILE__, __LINE__) */
 #else
  #define XMP_Debug_new /* as nothing */
 #endif /* _DEBUG */
#else
 #if _DEBUG
  #define XAP_WIN_CRTDBG 1
  #define XMP_Debug_new /* (__FILE__, __LINE__) */
 #else
  #define XMP_Debug_new /* as nothing */
 #endif /* _DEBUG */
#endif /* XAP_FORCE_NORMAL_DEBUG */

#endif /* WIN_ENV */

/*======================== MAC_ENV ========================*/

#if macintosh	// Both MrC and Code Warrior define this automatically.
	#ifndef MAC_ENV
		#define MAC_ENV	1
	#endif
#endif

#if MAC_ENV

#define ENV_DEFINED 1

#define WSTRING_SUPPORTED 1

#ifndef XAP_DLL_Build
	#define XAP_DLL_Build	0
#endif

#ifdef XAP_BUILT_IN
 #define XAP_API /* as nothing */
#else
 #ifndef XAP_API
 #define XAP_API __declspec(dllimport)
 #endif /* XAP_API */
#endif /* XAP_BUILT_IN */

#define XAP_SAY_WHAT(s) XAP_WHAT_SAY(s,__LINE__)
#define XAP_QUOTE_ME(s) #s
#define XAP_WHAT_SAY(s,l) s " in " __FILE__ ":" XAP_QUOTE_ME(l)

#ifdef XAP_FORCE_NORMAL_ALLOC
 #if _DEBUG
  #include <crtdbg.h>
  	#define XMP_Debug_new /* (_NORMAL_BLOCK, __FILE__, __LINE__) */
 #else
  #define XMP_Debug_new /* as nothing */
 #endif /* _DEBUG */
#else
 #if _DEBUG
  #define XAP_MAC_CRTDBG /* as nothing */
  #define XMP_Debug_new /* (__FILE__, __LINE__) */
 #else
  #define XMP_Debug_new /* as nothing */
 #endif /* _DEBUG */
#endif /* XAP_FORCE_NORMAL_DEBUG */

#endif /* MAC_ENV */

/*======================== UNIX_ENV ========================*/

#if UNIX_ENV

#define ENV_DEFINED 1

#ifndef XAP_DLL_Build
	#define XAP_DLL_Build	0
#endif

/* WSTRING_SUPPORTED set in compiler options, set to 0 for GCC */

#ifdef XAP_BUILT_IN
 #define XAP_API /* as nothing */
#else
 #ifndef XAP_API
 #define XAP_API DONT_KNOW_WHAT_TO_DO_FORCE_COMPILE_ERROR
 #endif /* XAP_API */
#endif /* XAP_BUILT_IN */

#define XAP_SAY_WHAT(s) XAP_WHAT_SAY(s,__LINE__)
#define XAP_QUOTE_ME(s) #s
#define XAP_WHAT_SAY(s,l) s " in " __FILE__ ":" XAP_QUOTE_ME(l)

#define XMP_Debug_new /* as nothing */

#endif /* UNIX_ENV */

/*======================== NO ENV DEFINED ========================*/

#if ! ENV_DEFINED

#define	XAP_DLL_Build	0

#ifndef WSTRING_SUPPORTED
#define WSTRING_SUPPORTED 1
#endif

#define XAP_API /* as nothing */
#define XAP_SAY_WHAT(s) s

#define XMP_Debug_new /* as nothing */

#endif /* ENV_DEFINED */


#endif /* __XAPConfigure__ */

/*
$Log$
*/

