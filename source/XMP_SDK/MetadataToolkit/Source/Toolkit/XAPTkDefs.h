
/* $Header: //xaptk/xaptk/XAPTkDefs.h#14 $ */
/* XAPTkDefs.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.  If you have received this file from a 
source other than Adobe, then your use, modification, or distribution of it requires the prior 
written permission of Adobe.
*/

/*
Implementation constants.
*/


#ifndef XAPTKDEFS_H
#define XAPTKDEFS_H /* as nothing */


#include "XAPConfigure.h"
#include "XAPDefs.h"
#include "XAPExcep.h"

#include "XAPObjWrapper.h"


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


/* ===== Defines ===== */

#define ISCR(escnl) ((escnl & XAP_ESCAPE_CR) != 0)
#define ISLF(escnl) ((escnl & XAP_ESCAPE_LF) != 0)
#define ISCRLF(escnl) (ISCR(escnl) && ISLF(escnl))

#if ! (XAP_DLL_Build && __MWERKS__)
	#define DECL_EXPORT /* as nothing */
#else
	#define DECL_EXPORT __declspec(export)
#endif

/* ===== Member Function Selectors ===== */

XAP_ENUM_TYPE ( XAPTk_ThrowMode ) {
    xaptk_no_throw,
    xaptk_throw
};

/* ===== Implementation Constants ===== */

// XML literals for normalized trees

extern const char *const XAPTK_NS_MXAP;
extern const char *const XAPTK_NS_INFO_XAP;

extern const char *const XAPTK_TAG_META_ROOT;
extern const char *const XAPTK_TAG_SCHEMA_ROOT;

extern const char *const XAPTK_TAG_META_PROP;
extern const char *const XAPTK_TAG_SCHEMA_DEFPROP;

extern const char *const XAPTK_TAG_META_FAKE_ALT;
extern const char *const XAPTK_TAG_META_FAKE_BAG;
extern const char *const XAPTK_TAG_META_FAKE_SEQ;

extern const char *const XAPTK_TAG_META_ALT;
extern const char *const XAPTK_TAG_META_BAG;
extern const char *const XAPTK_TAG_META_SEQ;

extern const char *const XAPTK_TAG_RDF_VALUE;
extern const char *const XAPTK_TAG_RDF_TYPE;
extern const char *const XAPTK_TAG_RDF_ID;

extern const char *const XAPTK_ATTR_NS;
extern const char *const XAPTK_ATTR_XMLNS_MXAP;

extern const char *const XAPTK_ATTR_XML_LANG;

extern const char *const XAPTK_ATTR_RDF_RESOURCE;

extern const char *const XAPTK_ATTR_META_FEATURES;

extern const char *const XAPTK_FEATURE_XML;
extern const char *const XAPTK_FEATURE_RDF_RESOURCE;
extern const char *const XAPTK_FEATURE_RDF_VALUE;

extern const char *const XAPTK_TAG_IX_IS;

extern const char *const XAPTK_QUAL_IS_XML;

extern const char *const XAPTK_XML_LANG_NOTDEF;

extern const char *const XAPTK_DEL_SCHEMATX;

// Timestamp element names

extern const char *const XAPTK_TAG_TS_CHANGES;

#ifdef XXX
extern const char *const XAPTK_TAG_TS_TS;
extern const char *const XAPTK_TAG_TS_PATH;
extern const char *const XAPTK_TAG_TS_LAST;
extern const char *const XAPTK_TAG_TS_SEQ;
extern const char *const XAPTK_TAG_TS_CODES;
#endif

// Timestamp change codes

extern const char *const XAPTK_CCODE_CREATED;
extern const char *const XAPTK_CCODE_SET;
extern const char *const XAPTK_CCODE_REMOVED;
extern const char *const XAPTK_CCODE_FORCED;
extern const char *const XAPTK_CCODE_SUSPECT;

/* ===== AWE Support Constants ===== */

extern const char *const XAPTK_TAG_AWE_PROPPARMS;

extern const char *const XAPTK_ATTR_AWE_VERSION;
extern const char *const XAPTK_ATTR_AWE_NAME;
extern const char *const XAPTK_ATTR_AWE_TYPE;
extern const char *const XAPTK_ATTR_AWE_LANG;
extern const char *const XAPTK_ATTR_AWE_REQ;
extern const char *const XAPTK_ATTR_AWE_ADD;

/* ===== Other Constants ===== */

extern const char *const XAPTK_XTAB;
extern const char *const XAPTK_DEFAULT_LANG;
extern const char *const XAPTK_METADATA_DATE;
extern const char *const XAPTK_TAG_META;
extern const char *const XAPTK_TAG_META_OLD;
extern const char *const XAPTK_ALT_DEFAULT_LANG;

/* ====== Static Initialization Support ===== */

void
XAPTk_InitConstData();

namespace XAPTk {

const size_t CSTRSIZE = 8*1024;
const size_t CBUFSIZE = 1024;

//REVISIT: These should probably go in an eval class ...
const char LBRA = '[';
const char RBRA = ']';
const char ATTR = '@';
const char WILD = '*';
const char EQLS = '=';
const char EMPT = '.';
const char SLSH = '/';
const char COLN = ':';
const char CHCR = '\015';
const char CHLF = '\012';
const char STNL = CHLF;

const char *const S_WILD = "*";
const char *const S_EMPT = ".";
const char *const S_STNL = "\012";

extern std::string * NullString;	// ! Should be const.

extern XAPTk_PairOfString * TAG_RDF;	// ! Should be const.
extern XAPTk_PairOfString * TAG_DESC;
extern XAPTk_PairOfString * TAG_ALT;
extern XAPTk_PairOfString * TAG_BAG;
extern XAPTk_PairOfString * TAG_SEQ;
extern XAPTk_PairOfString * TAG_LI;
extern XAPTk_PairOfString * TAG_VALUE;
extern XAPTk_PairOfString * TAG_META;
extern XAPTk_PairOfString * TAG_META_OLD;
extern XAPTk_PairOfString * TAG_IS;


/* ===== Need some special exceptions. ===== */

class not_implemented : public std::runtime_error {
public:
    not_implemented() : runtime_error("XAP_ERR_NOT_IMPLEMENTED") {}
    explicit not_implemented(const char* what) :
      runtime_error(what) {}
};

} // XAPTk


#if macintosh
	#pragma options align=reset
#endif


#endif // XAPTKDEFS_H

/*
$Log$
*/

