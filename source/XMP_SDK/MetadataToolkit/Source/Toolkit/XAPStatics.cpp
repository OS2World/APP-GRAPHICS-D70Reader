/* $Header: //xaptk/xaptk/xapstatics.cpp#21 $ */
/* xapstatics.cpp */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/

#ifndef XAP_FORCE_NORMAL_ALLOC
#include "XAPTkAlloc.h"
#endif

/*
Static contants and the static initialization start code.
*/

#include <string.h>

#include "XAPToolkit.h"
#include "XMPInitTerm.h"
#include "XAPTkDefs.h"
#include "ConvCodes.h"


namespace XAPTk {

	std::string * NullString	= NULL; // CLING_FREE

	XAPTk_PairOfString * TAG_RDF	= NULL;
	XAPTk_PairOfString * TAG_DESC	= NULL;
	XAPTk_PairOfString * TAG_ALT	= NULL;
	XAPTk_PairOfString * TAG_BAG	= NULL;
	XAPTk_PairOfString * TAG_SEQ	= NULL;
	XAPTk_PairOfString * TAG_LI	= NULL;
	XAPTk_PairOfString * TAG_VALUE	= NULL;
	XAPTk_PairOfString * TAG_META	= NULL;
	XAPTk_PairOfString * TAG_META_OLD	= NULL;
	XAPTk_PairOfString * TAG_IS	= NULL;
	
}


static bool	doneConstDataInit	= false;

void XAPTk_InitConstData() {

	if ( ! doneConstDataInit ) {
	
		XAPTk::NullString = new std::string ( "" );

		XAPTk::TAG_RDF	= new XAPTk_PairOfString ( "RDF", XAP_NS_RDF );
		XAPTk::TAG_DESC	= new XAPTk_PairOfString ( "Description", XAP_NS_RDF );
		XAPTk::TAG_ALT	= new XAPTk_PairOfString ( "Alt", XAP_NS_RDF );
		XAPTk::TAG_BAG	= new XAPTk_PairOfString ( "Bag", XAP_NS_RDF );
		XAPTk::TAG_SEQ	= new XAPTk_PairOfString ( "Seq", XAP_NS_RDF );
		XAPTk::TAG_LI	= new XAPTk_PairOfString ( "li", XAP_NS_RDF );
		XAPTk::TAG_VALUE	= new XAPTk_PairOfString ( "value", XAP_NS_RDF );
		XAPTk::TAG_META	= new XAPTk_PairOfString ( XAPTK_TAG_META, XAP_NS_META );
		XAPTk::TAG_META_OLD	= new XAPTk_PairOfString ( XAPTK_TAG_META_OLD, XAP_NS_META );
		XAPTk::TAG_IS	= new XAPTk_PairOfString ( XAPTK_TAG_IX_IS, XAPTK_NS_INFO_XAP );
	
		doneConstDataInit = true;
	
	}

}

void XAPTk_KillConstData() {

	if ( doneConstDataInit ) {
	
		delete XAPTk::NullString;
		
		delete XAPTk::TAG_RDF;
		delete XAPTk::TAG_DESC;
		delete XAPTk::TAG_ALT;
		delete XAPTk::TAG_BAG;
		delete XAPTk::TAG_SEQ;
		delete XAPTk::TAG_LI;
		delete XAPTk::TAG_VALUE;
		delete XAPTk::TAG_META;
		delete XAPTk::TAG_META_OLD;
		delete XAPTk::TAG_IS;
		
		XAPTk::NullString = 0;
		
		XAPTk::TAG_RDF = 0;
		XAPTk::TAG_DESC = 0;
		XAPTk::TAG_ALT = 0;
		XAPTk::TAG_BAG = 0;
		XAPTk::TAG_SEQ = 0;
		XAPTk::TAG_LI = 0;
		XAPTk::TAG_VALUE = 0;
		XAPTk::TAG_META = 0;
		XAPTk::TAG_META_OLD = 0;
		XAPTk::TAG_IS = 0;

		doneConstDataInit = false;
		
	}

}


bool	gXMP_IsInitialized	= false;

DECL_EXPORT void XAPTk_GeneralInit ( XAPAllocator * xapAlloc ) {

    if ( ! gXMP_IsInitialized ) {
		try {
		
	    	XAPTk_InitAlloc ( xapAlloc );

	        // XAPTk_InitExpat();
	        XAPTk_InitDOM();
	        XAPTk_InitConstData();
	        XAPTk_InitDOMGlue();				// Empty?

	        XAPTk_InitMetaXAP();
	        XAPTk_InitNormTree();				// Empty?
	        XAPTk_InitNormTreeSerializer();		// Empty?
	        XAPTk_InitPaths();					// Empty?
	        XAPTk_InitRDFToNormTrees();

	        XAPTk_InitUtilityXAP();				// Empty?
	        XAPTk_InitData();					// Empty?
	        XAPTk_InitFuncs();					// Empty?
    
    		gXMP_IsInitialized = true;
        
        } catch ( ... ) {
 
        	gXMP_IsInitialized = false;
        	throw;
 
        }
    }

}

DECL_EXPORT void XAPTk_Terminate() {

	gXMP_IsInitialized = false;

	// Go ahead and try to clean up storage after initialization failures.
	
    XAPTk_KillFuncs();
    XAPTk_KillData();
    XAPTk_KillUtilityXAP();

    XAPTk_KillRDFToNormTrees();
    XAPTk_KillPaths();
    XAPTk_KillNormTreeSerializer();
    XAPTk_KillNormTree();
    XAPTk_KillMetaXAP();

    XAPTk_KillDOMGlue();
    XAPTk_KillConstData();
    XAPTk_KillDOM();
    // XAPTk_KillExpat();

    XAPTk_KillAlloc();

}

/* ===== Global (const) Data ===== */

/* +++++ Version Info +++++ */

#ifndef XAPVCOPY
	#define XAPVCOPY "XMP Library: Copyright 2000-2002 Adobe Systems Incorporated. All Rights Reserved."
#endif

static const char *const XAP_LIB_COPYRIGHT = XAPVCOPY;

#ifndef XAPVMAJOR
	#define XAPVMAJOR 2
#endif

#ifndef XAPVMINOR
	#define XAPVMINOR 9
#endif

#ifndef XAPVBUILD				// ! Change the string too!
	#define XAPVBUILD 9
#endif

#ifndef XAPVMESG
	#define XAPVMESG "XMP toolkit 2.9-9, framework 1.6"
#endif

const char *const XAP_LIB_VERSION_TEXT = XAPVMESG;
const unsigned short XAP_LIB_VERSION_MAJOR = XAPVMAJOR;
const unsigned short XAP_LIB_VERSION_MINOR = XAPVMINOR;
const unsigned short XAP_LIB_VERSION_BUILD = XAPVBUILD;

/* +++++ String Value Constants +++++ */

const char *const XAP_NO_VALUE = "";

/* +++++ Namespace Names +++++ */

#ifdef __cplusplus
extern "C" {	// Need to allow access through the plain C interface.
#endif

const char *const XAP_NS_XAP        =
                    "http://ns.adobe.com/xap/1.0/";
const char *const XAP_NS_XAP_G      =
                    "http://ns.adobe.com/xap/1.0/g/";
const char *const XAP_NS_XAP_G_IMG  =
                    "http://ns.adobe.com/xap/1.0/g/img/";
const char *const XAP_NS_XAP_DYN    =
                    "http://ns.adobe.com/xap/1.0/dyn/";
const char *const XAP_NS_XAP_DYN_A  =
                    "http://ns.adobe.com/xap/1.0/dyn/a/";
const char *const XAP_NS_XAP_DYN_V  =
                    "http://ns.adobe.com/xap/1.0/dyn/v/";
const char *const XAP_NS_XAP_T      =
                    "http://ns.adobe.com/xap/1.0/t/";
const char *const XAP_NS_XAP_T_PG   =
                    "http://ns.adobe.com/xap/1.0/t/pg/";
const char *const XAP_NS_XAP_RIGHTS =
                    "http://ns.adobe.com/xap/1.0/rights/";
const char *const XAP_NS_XAP_MM     =
                    "http://ns.adobe.com/xap/1.0/mm/";
const char *const XAP_NS_XAP_S      =
                    "http://ns.adobe.com/xap/1.0/s/";
const char *const XAP_NS_XAP_BJ     =
                    "http://ns.adobe.com/xap/1.0/bj/";

const char *const XAP_NS_PDF        =
                    "http://ns.adobe.com/pdf/1.3/";
const char *const XAP_NS_PHOTOSHOP  =
                    "http://ns.adobe.com/photoshop/1.0/";
const char *const XAP_NS_EXIF       =
                    "http://ns.adobe.com/exif/1.0/";
const char *const XAP_NS_TIFF       =
                    "http://ns.adobe.com/tiff/1.0/";

const char *const XAP_NS_DC         =
                    "http://purl.org/dc/elements/1.1/";

const char *const XAP_NS_RDF        =
                    "http://www.w3.org/1999/02/22-rdf-syntax-ns#";

const char *const XAP_NS_META       =
                    "adobe:ns:meta/";

const char *const XAP_NS_ST_DIMENSIONS =
                    "http://ns.adobe.com/xap/1.0/sType/Dimensions#";
const char *const XAP_NS_ST_RESOLUTION =
                    "http://ns.adobe.com/xap/1.0/sType/Resolution#";
const char *const XAP_NS_ST_TRACK_DESC =
                    "http://ns.adobe.com/xap/1.0/sType/TrackDesc#";
const char *const XAP_NS_ST_FONT =
                    "http://ns.adobe.com/xap/1.0/sType/Font#";
const char *const XAP_NS_ST_RESOURCE_REF =
                    "http://ns.adobe.com/xap/1.0/sType/ResourceRef#";
const char *const XAP_NS_ST_VERSION =
                    "http://ns.adobe.com/xap/1.0/sType/Version#";
const char *const XAP_NS_ST_RESOURCE_EVENT =
                  "http://ns.adobe.com/xap/1.0/sType/ResourceEvent#";
const char *const XAP_NS_ST_FILE_DISPOSITION =
                "http://ns.adobe.com/xap/1.0/sType/FileDisposition#";
const char *const XAP_NS_ST_JOB =
                "http://ns.adobe.com/xap/1.0/sType/Job#";
const char *const XAP_NS_ST_RIGHT =
                "http://ns.adobe.com/xap/1.0/sType/Right#";

#ifdef __cplusplus
}
#endif

const char *const XAP_PREFIX_INFO_XAP = "iX";
const char *const XAP_PREFIX_RDF = "rdf";

/* +++++ Exception Messages +++++ */

const char *const XAP_INVALID_ARGUMENT = "XAP_INVALID_ARGUMENT";

const char *const XAP_BAD_ALLOC = "XAP_BAD_ALLOC";

const char *const XAP_FAULT_BAD_FEATURE = "XAP_FAULT_BAD_FEATURE";
const char *const XAP_FAULT_BAD_SCHEMA = "XAP_FAULT_BAD_SCHEMA";
const char *const XAP_FAULT_BAD_TYPE = "XAP_FAULT_BAD_TYPE";
const char *const XAP_FAULT_BAD_PATH = "XAP_FAULT_BAD_PATH";
const char *const XAP_FAULT_BAD_ACCESS = "XAP_FAULT_BAD_ACCESS";
const char *const XAP_FAULT_BAD_NUMBER = "XAP_FAULT_BAD_NUMBER";

const char *const XAP_ERR_BAD_XAP = "XAP_ERR_BAD_XAP";
const char *const XAP_ERR_BAD_XML = "XAP_ERR_BAD_XML";
const char *const XAP_ERR_NO_MATCH = "XAP_ERR_NO_MATCH";
const char *const XAP_ERR_ASSERT = "XAP_ERR_ASSERT";

/* +++++ Schema Alt Selectors +++++ */

const char *const XAP_ALT_SEL_DEFAULT = "@xml:lang";
const char *const XAP_ALT_SEL_FIRST = "1!";

/* +++++ Setup Strings +++++ */

const char *const XAP_SETUP_VENDOR = "XAP_SETUP_VENDOR";
const char *const XAP_SETUP_APP = "XAP_SETUP_APP";
const char *const XAP_SETUP_VERSION = "XAP_SETUP_VERSION";
const char *const XAP_SETUP_PLATFORM = "XAP_SETUP_PLATFORM";

/* +++++ Boolean String Constants +++++ */

const char *const XAP_BOOL_TRUE = "True";
const char *const XAP_BOOL_FALSE = "False";

/* +++++ XML literals for normalized trees +++++ */

const char *const XAPTK_NS_MXAP = "adobe:ns:x:xap/meta/";
const char *const XAPTK_NS_INFO_XAP = "http://ns.adobe.com/iX/1.0/";

const char *const XAPTK_TAG_META_ROOT = "mxap:mxap";

const char *const XAPTK_TAG_META_PROP = "mxap:prop";

const char *const XAPTK_TAG_META_FAKE_ALT = "mxap:alt";
const char *const XAPTK_TAG_META_FAKE_BAG = "mxap:bag";
const char *const XAPTK_TAG_META_FAKE_SEQ = "mxap:seq";

const char *const XAPTK_TAG_META_ALT = "rdf:Alt";
const char *const XAPTK_TAG_META_BAG = "rdf:Bag";
const char *const XAPTK_TAG_META_SEQ = "rdf:Seq";

const char *const XAPTK_TAG_RDF_VALUE = "rdf:value";
const char *const XAPTK_TAG_RDF_TYPE = "rdf:type";
const char *const XAPTK_TAG_RDF_ID = "rdf:ID";

const char *const XAPTK_ATTR_NS = "ns";
const char *const XAPTK_ATTR_XMLNS_MXAP = "xmlns:mxap";

const char *const XAPTK_ATTR_XML_LANG = "xml:lang";
const char *const XAPTK_ATTR_RDF_RESOURCE = "rdf:resource";

const char *const XAPTK_ATTR_META_FEATURES = "mxap:f";

const char *const XAPTK_FEATURE_XML = "X";
const char *const XAPTK_FEATURE_RDF_RESOURCE = "R";
const char *const XAPTK_FEATURE_RDF_VALUE = "V";

const char *const XAPTK_TAG_IX_IS = "iX:is"; // see XAPTK_QUAL_IS_XML

const char *const XAPTK_QUAL_IS_XML = "XML";

const char *const XAPTK_XML_LANG_NOTDEF = "notdef";

const char *const XAPTK_DEL_SCHEMATX = "\177\177\177";

/* +++++ Timestamp element names +++++ */

const char *const XAPTK_TAG_TS_CHANGES = "iX:changes";

#ifdef XXX
const char *const XAPTK_TAG_TS_TS = "iX:timestamps";
const char *const XAPTK_TAG_TS_PATH = "iX:p";
const char *const XAPTK_TAG_TS_LAST = "iX:l";
const char *const XAPTK_TAG_TS_SEQ = "iX:s";
const char *const XAPTK_TAG_TS_CODES = "iX:c";
#endif

/* +++++ Timestamp change codes  +++++ */

const char *const XAPTK_CCODE_CREATED = "c";
const char *const XAPTK_CCODE_SET = "s";
const char *const XAPTK_CCODE_REMOVED = "r";
const char *const XAPTK_CCODE_FORCED = "f";
const char *const XAPTK_CCODE_SUSPECT = "?";

/* +++++ AWE Support Constants +++++ */

const char *const XAPTK_TAG_AWE_PROPPARMS = "propParms";

const char *const XAPTK_ATTR_AWE_VERSION = "ver";
const char *const XAPTK_ATTR_AWE_NAME = "n";
const char *const XAPTK_ATTR_AWE_TYPE = "t";
const char *const XAPTK_ATTR_AWE_LANG = "l";
const char *const XAPTK_ATTR_AWE_REQ = "r";
const char *const XAPTK_ATTR_AWE_ADD = "add";

/* +++++ Other Strings +++++ */

const char *const XAPTK_XTAB = " ";
const char *const XAPTK_DEFAULT_LANG = "en-us";
const char *const XAPTK_METADATA_DATE = "MetadataDate";
const char *const XAPTK_TAG_META = "xmpmeta";
const char *const XAPTK_TAG_META_OLD = "xapmeta";
const char *const XAPTK_ALT_DEFAULT_LANG = "x-default";

/* ===== Support for ConvCodes.h ===== */

#ifdef __cplusplus
extern "C" {
#endif

/*
MAINTAIN INDEX EQUIVALENCE!!!

XAPTk_iso_639[N] must map to XAPTk_rfc_1766[N], and vice versa.

Alpha-order is not important.
*/

static const char* XAPTk_iso_639_2[] = {
    "chi",         //Chinese
    "chs",         //Chinese Simplified     /* NOT STANDARD ISO! */
    "cht",         //Chinese Traditional    /* NOT STANDARD ISO! */
    "dan",         //Danish
    "dut",         //Dutch
    "eng",         //English International
    "eng",         //English UK
    "enu",         //English USA
    "fin",         //Finnish
    "fra",         //French
    "deu",         //German
    "ita",         //Italian
    "jpn",         //Japanese
    "kor",         //Korean
    "nor",         //Norwegian
    "por",         //Portuguese
    "ptb",         //Portuguese Brazilian   /* NOT STANDARD ISO! */
    "spa",         //Spanish                /* NOT STANDARD ISO! */
    "esl/spa",     //Spanish
    "esm",         //Spanish Latin American /* NOT STANDARD ISO! */
    "sve",         //Swedish
    NULL
};

static const char* XAPTk_rfc_1766[] = {
    "zh",           //Chinese
    "x-chs",        //Chinese Simplified (no standard equivalent)
    "x-cht",        //Chinese Traditional (no standard equivalent)
    "da",           //Danish
    "nl",           //Dutch
    "en",           //English International
    "en-gb",        //English UK
    "en-us",        //English USA
    "fi",           //Finnish
    "fr",           //French
    "de",           //German
    "it",           //Italian
    "ja",           //Japanese
    "ko",           //Korean
    "no",           //Norwedigan
    "pt",           //Portuguese
    "pt-br",        //Portuguese Brazilian
    "x-spa",        //Spanish (no standard equivalent)
    "es",           //Spanish
    "x-esm",        //Spanish Latin American (no standard equivalent)
    "sv",           //Swedish
    NULL
};

    /** Language-Country codes: ISO 639-2 alpha-3 to RFC 1766 ll-cc.  */
const char*
To_RFC1766(const char* alpha3) {
    const char* iso;
    int i = 0;

    if (alpha3 == NULL)
        return(NULL);
    while ((iso = XAPTk_iso_639_2[i]) != NULL) {
        if (!strcmp(iso, alpha3)) {
            return(XAPTk_rfc_1766[i]);
        }
        ++i;
    }
    return(NULL);
}

    /** Language-Country codes: RFC 1766 ll-cc to ISO 639-2 alpha-3. */
const char*
To_ISO639_2(const char* ll_cc) {
    const char* rfc;
    int i = 0;

    if (ll_cc == NULL)
        return(NULL);
    while ((rfc = XAPTk_rfc_1766[i]) != NULL) {
        if (!strcmp(rfc, ll_cc)) {
            return(XAPTk_iso_639_2[i]);
        }
        ++i;
    }
    return(NULL);
}


#ifdef __cplusplus
}
#endif


/*
$Log$
*/
