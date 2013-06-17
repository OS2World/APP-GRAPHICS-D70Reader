
/* $Header: //xaptk/include/XAPDefs.h#15 $ */
/* XAPDefs.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/


#ifndef XAPDEFS_H
#define XAPDEFS_H /* as nothing */


/*
Types, enumerations, constants.
*/

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


/* ===== VERSION INFO ===== */

extern const char *const XAP_LIB_VERSION_TEXT;
extern const unsigned short XAP_LIB_VERSION_MAJOR;
extern const unsigned short XAP_LIB_VERSION_MINOR;
extern const unsigned short XAP_LIB_VERSION_BUILD;

/* ===== TYPES ===== */

typedef struct {
  short sec;          // seconds after the minute - [0,59]
  short min;          // minutes after the hour - [0,59]
  short hour;         // hours since midnight - [0,23]
  short mday;         // day of the month - [1,31]
  short month;        // month of the year - [1,12]
  short year;         // year A.D. (can be negative!)
  short tzHour;       // hours +ahead/-behind UTC - [-12,12]
  short tzMin;        // minutes offset of UTC - [0,59]
  long nano;          // nanoseconds after second (if supported)
  long seq;           // sequence number (if nano not supported)
} XAPDateTime;

XAP_ENUM_TYPE ( XAPFormatType ) {
  xap_format_pretty,
  xap_format_compact
};

XAP_ENUM_TYPE ( XAPMergePolicy ) {
  xap_policy_dont_merge = 0,
  xap_policy_a,
  xap_policy_b,
  xap_policy_newest,
  xap_policy_oldest,
  xap_policy_ask_user
};

XAP_ENUM_TYPE ( XAPStructContainerType ) {
  xap_sct_unknown,
  xap_alt,
  xap_bag,
  xap_seq
};

XAP_ENUM_TYPE ( XAPTimeRelOp ) {
  xap_before,
  xap_at,
  xap_after,
  xap_noTime,
  xap_notDef
};

XAP_ENUM_TYPE ( XAPValForm ) {
  xap_simple,
  xap_description,
  xap_container,
  xap_unknown
};


/* ===== CONSTANTS ===== */


// Boolean String Constant

extern const char *const XAP_BOOL_TRUE;
extern const char *const XAP_BOOL_FALSE;

// Change Bits
typedef long int XAPChangeBits;
enum {
	XAP_CHANGE_NONE     = 0,
	XAP_CHANGE_CREATED  = 1 << 0,
	XAP_CHANGE_SET      = 1 << 1,
	XAP_CHANGE_REMOVED  = 1 << 2,
	XAP_CHANGE_FORCED   = 1 << 3,
	XAP_CHANGE_SUSPECT  = 1 << 4,
	XAP_CHANGE_MASK     = XAP_CHANGE_CREATED	|
						  XAP_CHANGE_SET		|
						  XAP_CHANGE_REMOVED	|
						  XAP_CHANGE_FORCED		|
						  XAP_CHANGE_SUSPECT
};


// Newline Escape Char Bits
enum {
	XAP_ESCAPE_CR = 1 << 0,
	XAP_ESCAPE_LF = 1 << 1
};

// String Feature Constants
typedef long int XAPFeatures;
enum {
	XAP_FEATURE_NONE			= 0,
	XAP_FEATURE_XML				= 1 << 0,
	XAP_FEATURE_RDF_RESOURCE	= 1 << 1,
	XAP_FEATURE_RDF_VALUE		= 1 << 2,
	XAP_FEATURE_DEFAULT			= XAP_FEATURE_NONE
};

// String Value Constants

extern const char *const XAP_NO_VALUE;

extern const char *const XAP_ALT_SEL_DEFAULT;
extern const char *const XAP_ALT_SEL_FIRST;

// Namespace String Constants

#ifdef __cplusplus
extern "C" {	// Needed to allow access through the plain C interface.
#endif

extern const char *const XAP_NS_XAP;
extern const char *const XAP_NS_XAP_G;
extern const char *const XAP_NS_XAP_G_IMG;
extern const char *const XAP_NS_XAP_DYN;
extern const char *const XAP_NS_XAP_DYN_A;
extern const char *const XAP_NS_XAP_DYN_V;
extern const char *const XAP_NS_XAP_T;
extern const char *const XAP_NS_XAP_T_PG;
extern const char *const XAP_NS_XAP_RIGHTS;
extern const char *const XAP_NS_XAP_MM;
extern const char *const XAP_NS_XAP_S;
extern const char *const XAP_NS_XAP_BJ;

extern const char *const XAP_NS_PDF;
extern const char *const XAP_NS_PHOTOSHOP;
extern const char *const XAP_NS_EXIF;
extern const char *const XAP_NS_TIFF;

extern const char *const XAP_NS_DC;
extern const char *const XAP_NS_RDF;

extern const char *const XAP_NS_META;

extern const char *const XAP_NS_ST_DIMENSIONS;
extern const char *const XAP_NS_ST_RESOLUTION;
extern const char *const XAP_NS_ST_TRACK_DESC;
extern const char *const XAP_NS_ST_FONT;
extern const char *const XAP_NS_ST_RESOURCE_REF;
extern const char *const XAP_NS_ST_VERSION;
extern const char *const XAP_NS_ST_RESOURCE_EVENT;
extern const char *const XAP_NS_ST_FILE_DISPOSITION;
extern const char *const XAP_NS_ST_JOB;
extern const char *const XAP_NS_ST_RIGHT;

#ifdef __cplusplus
}
#endif

extern const char *const XAP_PREFIX_INFO_XAP;
extern const char *const XAP_PREFIX_RDF;

// Setup String Constants

extern const char *const XAP_SETUP_VENDOR;
extern const char *const XAP_SETUP_APP;
extern const char *const XAP_SETUP_VERSION;
extern const char *const XAP_SETUP_PLATFORM;


#if macintosh
	#pragma options align=reset
#endif


#endif /* XAPDEFS_H */

/*
$Log$
*/

