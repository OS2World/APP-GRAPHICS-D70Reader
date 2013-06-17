/* $Header: //xaptk/xaptk/UtilityXAP.cpp#21 $ */
/* UtilityXAP.cpp */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/

/*
Adobe patent application tracking #P435, entitled 'Unique markers to
simplify embedding data of one format in a file with a different format',
inventors: Sean Parent, Greg Gilley
*/

/*
Implementation of UtilityXAP.

IMPORTANT: This module should try to use public interfaces only.  Don't
include XAPTkData.h, NormTree.h, etc.  Instead, use XAPToolkit.h interfaces.
If you can't do this, something is wrong with the API.  These utilities should
be a pure layer on top of the public API's.  Any public client should be capable
of reimplementing these routines.
*/

#ifndef XAP_FORCE_NORMAL_ALLOC
	#include "XAPTkAlloc.h"
#endif

#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "XAPToolkit.h"
#include "XAPTkFuncs.h" //Only for NotImplemented
#include "UtilityXAP.h"
#include "XAPTkDefs.h" // Constants okay
#include "ConvCodes.h"
#include "XMPAssert.h"
#include "XMPInitTerm.h"

#ifdef XAP_DEBUG_CERR
#include <iostream>
#endif

using namespace std;


// =================================================================================================
// =================================================================================================
// Local static functions
// ======================


// =================================================================================================
// IsExistingProperty
// ==================
//
// Returns true if the given property exists.
// *** Should be a primitive in MetaXAP!

static bool
IsExistingProperty ( const MetaXAP & meta, const std::string & ns, const std::string & path )
{

	try {
		XAPValForm	form;	// Split to avoid UNIX unused variable warning.
		form = meta.getForm ( ns, path );	// Throws an exception if it doesn't exist.
		return true;
	} catch ( ... ) {
		return false;
	}

}	// IsExistingProperty ()


// =================================================================================================
// IsAliasProperty
// ===============
//
// Returns true if the given property is an alias of some other property.
// *** Should be a primitive in MetaXAP!

static bool
IsAliasProperty ( const std::string & ns, const std::string & path )
{
	string					baseNS, basePath;
	XAPStructContainerType	cType;
	
	try {
		return MetaXAP::GetAlias ( ns, path, baseNS, basePath, cType );
	} catch ( ... ) {
		return false;
	}

}	// IsAliasProperty ()


// =================================================================================================
// IsKnownSchema
// =============
//
// Returns true if the namespace is a known schema.  A known schema is not just a registered schema,
// it is one where we know about the names and types of the properties.
//
// ! Until we add dynamic schema support the known schema are hardwired.

#if 0	// unused
static bool
IsKnownSchema ( const std::string & ns )
{
	bool	isKnown	= false;
	
	if ( (ns == XAP_NS_DC)			||
		 (ns == XAP_NS_PDF)			||
		 (ns == XAP_NS_XAP)			||
		 (ns == XAP_NS_XAP_RIGHTS)	||
		 (ns == XAP_NS_XAP_MM)		||
		 (ns == XAP_NS_XAP_BJ)		||
		 (ns == XAP_NS_XAP_S) ) {
		 isKnown = true;
	}
	
	return isKnown;
	
}	// IsKnownSchema ()
#endif


// =================================================================================================
// IsInternalProperty
// ==================
//
// Returns true if the namespace is a known schema and the property is internal.  All properties in
// unknown schema and unknown properties in known schema are considered to be external.  The path
// can be any XPath, but only the top level name is used here.
//
// ! Until we add dynamic schema support the known schema and external properties are hardwired.

static bool
IsInternalProperty ( const std::string & ns, const std::string & path )
{
	bool	isInternal	= false;
	string	prop ( path );
	int		i, limit;
	
	// -----------------------------------------------------
	// First strip the path down to just the top level name.
	
	limit = path.size() ;
	for ( i = 0; i < limit; ++i ) {
		if ( prop[i] == '/' ) break;
	}
	if ( i < limit ) prop.erase ( i, limit-i );
	
	// -----------------------------------------------------
	// Now check the properties in each of the known schema.
	
	if ( ns == XAP_NS_DC ) {
	
		if ( (prop == "format")	||
			 (prop == "language") ) {
			 isInternal = true;
		}
	
	} else if ( ns == XAP_NS_PDF ) {
	
		if ( (prop == "BaseURL")		||
			 (prop == "CreationDate")	||
			 (prop == "Creator")		||
			 (prop == "ModDate")		||
			 (prop == "PDFVersion")		||
			 (prop == "Producer") ) {
			 isInternal = true;
		}
	
	} else if ( ns == XAP_NS_XAP ) {
	
		if ( (prop == "BaseURL")		||
			 (prop == "CreateDate")		||
			 (prop == "CreatorTool")	||
			 (prop == "Format")			||
			 (prop == "Locale")			||
			 (prop == "MetadataDate")	||
			 (prop == "ModifyDate") ) {
			 isInternal = true;
		}

	} else if ( ns == XAP_NS_XAP_MM ) {
	
		isInternal = true;	// ! The xmpMM schema has only internal properties.
	
	} else if ( ns == XAP_NS_XAP_S ) {
	
		isInternal = true;	// ! The xmpS schema has only internal properties.
	
	// } else if ( ns == XAP_NS_XAP_RIGHTS ) {	! The xmpRights schema has no internal properties.
	
	// } else if ( ns == XAP_NS_XAP_BJ ) {	! The xmpBJ schema has no internal properties.
	
	}
	
	return isInternal;
	
}	// IsInternalProperty ()


// =================================================================================================


/* ===== Definitions ===== */

/* ===== Operators ===== */

/* ===== Class Definitions ===== */

/* ===== Static Member (Class) Functions ===== */

DECL_EXPORT void
UtilityXAP::AnalyzeStep	( const std::string&	fullPath,
						  std::string&			parentPath,
						  std::string&			lastStep,
						  long int&				ord,
						  std::string&			selectorName,
						  std::string&			selectorVal )
{
	XMP_CheckToolkitInit();
	
    parentPath = "";
    lastStep = "";
    ord = -1L;
    selectorName = "";
    selectorVal = "";
    if (fullPath == "")
        return;

    string::size_type i = fullPath.rfind("/");
    if (i == string::npos) {
        lastStep = fullPath;
        return;
    }
    //Note that the last slash is eliminated
    if (i == fullPath.size()-1) {
        lastStep = "";
        parentPath = fullPath.substr(0, fullPath.size()-1);
    } else {
        lastStep = fullPath.substr(i+1, fullPath.size() - i - 1);
        parentPath = fullPath.substr(0, i);
    }
    if (lastStep == "")
        return;

    //Check for predicate
    i = lastStep.find("[");
    if (i == string::npos)
        return;

    //Get contents of predicate
    string::size_type j = lastStep.find("]");
    string pred(lastStep, i+1, j - i - 1);

    //Simple ordinal?
    if (pred[0] >= '1' && pred[0] <= '9') {
        ord = atol(pred.c_str());
        return;
    }

    //last()?
    if (pred == "last()") {
        ord = 0;
        return;
    }

    //Else its a selector
    i = pred.find("=");
    if (i == string::npos) {
        //Shouldn't happen, but we'll fail soft
        return;
    }
    selectorName = pred.substr(0, i);
    selectorVal = pred.substr(i+1, pred.size() - i - 1);
    // selectorVal might have quotes on it
    if (selectorVal[0] == '\'' || selectorVal[0] == '\"') {
        selectorVal = selectorVal.substr(1, selectorVal.size()-1);
        selectorVal = selectorVal.substr(0, selectorVal.size()-1);
    }
}


// =================================================================================================

DECL_EXPORT XAPTimeRelOp
UtilityXAP::CompareTimestamps ( const XAPDateTime & a, const XAPDateTime & b )
{
	XAPDateTime	aUTC, bUTC;
	
	UtilityXAP::MakeUTCTime ( a, aUTC );
	UtilityXAP::MakeUTCTime ( b, bUTC );
	
    return XAPTk::CompareDateTimes ( aUTC, bUTC );
	
}	// CompareTimestamps ()

// =================================================================================================

DECL_EXPORT XAPTimeRelOp
UtilityXAP::CompareTimestamps (  MetaXAP* a, MetaXAP* b, const string& ns, const string& path )
{
    XAPDateTime		adt, bdt;
    XAPChangeBits	aHow, bHow;
    bool ok;

	XMP_CheckToolkitInit();
	
    assert ((a != NULL) && (b != NULL));

    // First check that property is defined
    try {
        if ( a->getForm ( ns, path ) == xap_unknown )  return xap_notDef;
        if ( b->getForm ( ns, path ) == xap_unknown )  return xap_notDef;
    } catch ( ... ) {
        return xap_notDef;
    }

    // Special case treatment for MetadataDate
    if ( path == XAPTK_METADATA_DATE ) {
        if ( ! UtilityXAP::GetDateTime ( a, XAP_NS_XAP, XAPTK_METADATA_DATE, adt ) ) return xap_noTime;
        if ( ! UtilityXAP::GetDateTime ( b, XAP_NS_XAP, XAPTK_METADATA_DATE, bdt ) ) return xap_noTime;
        return XAPTk::CompareDateTimes ( adt, bdt );
    }

    //Get the timestamps
    ok = a->getTimestamp ( ns, path, adt, aHow );
    if ( !ok ) return xap_noTime;
    ok = b->getTimestamp ( ns, path, bdt, bHow );
    if ( !ok ) return xap_noTime;

    if ( (adt.tzHour != 0) || (adt.tzMin != 0) ||
		 (bdt.tzHour != 0) ||( bdt.tzMin != 0) ) return xap_noTime;

    return XAPTk::CompareDateTimes ( adt, bdt );

}


// =================================================================================================
// SetTimeZone ()
// ==============
//
// Sets just the time zone part of the time.  Useful for determining the local time zone or for
// converting a "zone-less" time to a proper local time.
//
// Figuring the offset and direction uses a simple trick of converting a local midnight to UTC.  If
// we are in San Jose (8 hours behind UTC) it will be 8AM of the same day in Greenwich.  If we are
// Perth (8 hours ahead of UTC) it will be 4PM of the previous day in Greenwich.

DECL_EXPORT void
UtilityXAP::SetTimeZone ( XAPDateTime & time )
{

	struct tm	someTime;
	time_t		localMidnight;

	(void) memset ( &someTime, 0, sizeof ( someTime ) );	// Create a local time of midnight, Jan 2, 2002.
	someTime.tm_mday = 2;	someTime.tm_year = 102;
	localMidnight = mktime ( &someTime );
	someTime = *(gmtime ( &localMidnight ));				// Get the corresponding UTC time.
	
	if ( someTime.tm_yday == 1 ) {
	
		// Days SINCE January 1, so "== 1" means today is already January 2 at Greenwich, so the
		// local time is behind (west of) GMT and we should have a negative tz offset.  E.g. if
		// the local time is midnight and it is 3AM GMT, we are 3 hours behind.

		time.tzHour = -someTime.tm_hour;
		time.tzMin = someTime.tm_min;

	} else {

		// If we are ahead (east) of GMT, when it is midnight locally it will be the previous night
		// in Greenwich.  E.g. if the GMT is 10PM (22:00) of January 1 then we are 2 hours ahead.
		
		assert ( someTime.tm_yday == 0 );
		time.tzHour = 24 - someTime.tm_hour;		// We are ahead of UTC.
		if ( someTime.tm_min != 0 ) {
			time.tzHour -= 1;
			time.tzMin = 60 - someTime.tm_min;
		}

	}

}	// SetTimeZone ()


// =================================================================================================

static bool
IsLeapYear ( const int	year )
{
	if ( (year % 4) != 0 ) return false;	// Not a multiple of 4.
	if ( (year % 100) != 0 ) return true;	// A multiple of 4 but not a multiple of 100. 
	if ( (year % 400) == 0 ) return true;	// A multiple of 400.
	return false;							// A multiple of 100 but not a multiple of 400. 
}


// =================================================================================================

static void
AdjustTimeOverflow ( XAPDateTime & time )
{

	static short	daysInMonth[13]	= { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
									   // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
	if ( IsLeapYear ( time.year ) )  daysInMonth[2] = 29;
	
	while ( time.min < 0 ) {
		time.hour -= 1;
		time.min += 60;
	}
	
	while ( time.min >= 60 ) {
		time.hour += 1;
		time.min -= 60;
	}
	
	while ( time.hour < 0 ) {
		time.mday -= 1;
		time.hour += 24;
	}
	
	while ( time.hour >= 24 ) {
		time.mday += 1;
		time.hour -= 24;
	}
	
	while ( time.mday < 1 ) {
		time.month -= 1;
		time.mday += daysInMonth[time.month];	// ! Decrement month before so index here is right!
	}
	
	while ( time.mday > daysInMonth[time.month] ) {
		time.mday -= daysInMonth[time.month];	// ! Increment month after so index here is right!
		time.month += 1;
	}
	
	while ( time.month < 1 ) {
		time.year -= 1;
		time.month += 12;
	}
	
	while ( time.month > 12 ) {
		time.year += 1;
		time.month -= 12;
	}

}


// =================================================================================================
// MakeUTCTime ()
// ==============
//
// Convert the time to UTC.  The timezone is already set, just add or subtract the offset and fix
// up any overflow or underflow.

DECL_EXPORT void
UtilityXAP::MakeUTCTime ( const XAPDateTime & inTime, XAPDateTime & outTime )
{
	
	outTime = inTime;	// Copy the time then adjust for the time zone.
	if ( (inTime.tzHour == 0) && (inTime.tzMin == 0) ) return;	// The time is already UTC.
	
	if ( outTime.tzHour >= 0 ) {
		// We are before (east of) GMT, subtract the offset from the time.
		outTime.hour -= outTime.tzHour;
		outTime.min -= outTime.tzMin;
	} else {
		// We are behind (west of) GMT, add the offset to the time.  Since the tzHour field
		// is negated to tell us we are behind, we subtract the hour but still add the minutes.
		outTime.hour -= outTime.tzHour;	// ! Subtract the negated outTime.tzHour!
		outTime.min += outTime.tzMin;
	}
	
	AdjustTimeOverflow ( outTime );
	outTime.tzHour = outTime.tzMin = 0;

}	// MakeUTCTime ()


// =================================================================================================
// MakeLocalTime ()
// ================
//
// Convert the time from UTC to local time.  A UTC time is defined as having a zero offset from UTC.
// The conversion is done by first using the UNIX time functions to determine the local offset from
// UTC, including whether we are behind or ahead of UTC.  Being ahead or behind has to do with where
// we are relative to the international date line.  The time offset is then subtracted or added to
// the UTC time and adjusted to keep everything in range.

DECL_EXPORT void
UtilityXAP::MakeLocalTime ( const XAPDateTime & inTime, XAPDateTime & outTime )
{
	
	outTime = inTime;	// Copy the time, set the offset and adjust later.
	if ( (inTime.tzHour != 0) || (inTime.tzMin != 0) ) return;	// The time is already local.
	
	SetTimeZone ( outTime );	// Get the local timezone offset, then adjust the time.
	
	if ( outTime.tzHour >= 0 ) {
		// We are before (east of) GMT, add the offset to the time.
		outTime.hour += outTime.tzHour;
		outTime.min += outTime.tzMin;
	} else {
		// We are behind (west of) GMT, subtract the offset from the time.  Since the tzHour field
		// is negated to tell us we are behind, we add the hour but still subtract the minutes.
		outTime.hour += outTime.tzHour;	// ! Add the negated outTime.tzHour!
		outTime.min -= outTime.tzMin;
	}
	
	AdjustTimeOverflow ( outTime );
	
}	// MakeLocalTime ()


// =================================================================================================

static const char *const	VER_10_ID_UTF8	=  "W5M0MpCehiHzreSzNTczkc9d";
static const wchar_t *const	VER_10_ID_UCS2	= L"W5M0MpCehiHzreSzNTczkc9d";

// The general packet layout is:
//	<header>nl<xmldata><padding><trailer>
// The returned header does not include the newline, but it is always in packets generated here
// for the sake of readbility.  I.e. if we compose a minimum length packet it will have a
// newline after the header.  The XML data is assumed to end with a newline.  The padding will
// end with a newline and is occasionally broken with a newline.

    /** Create an XML Packet for 8-bit encoded XML. */
DECL_EXPORT void
UtilityXAP::CreateXMLPacket	( const std::string&	encoding,
							  const bool			inPlaceEditOk,
							  signed long			padBytes,
							  const std::string&	nl,
							  std::string&			header,
							  std::string&			trailer,
							  std::string *			xml /* = NULL */ )
{
	const size_t	szNL	= nl.size();

	XMP_CheckToolkitInit();
	
    // Set up the header.
    header = "<?xpacket begin='\xEF\xBB\xBF' id='";
    header.append ( VER_10_ID_UTF8 );
    header.append ( "'" );
    if ( ! encoding.empty() ) {
        header.append ( " encoding='" );
        header.append ( encoding );
        header.append ( "'" );
    }
    header.append ( "?>" );

    // Set up the trailer.
    trailer = "<?xpacket end='";
    if ( inPlaceEditOk ) {
        trailer.append ( "w'?>" );
    } else {
        trailer.append ( "r'?>" );
    }

    // Compute and check the sizes, write the packet.
    if ( xml != NULL ) {

		const size_t	szHeader	= header.size();
		const size_t	szTrailer	= trailer.size();
		const size_t	szXML		= xml->size();

		size_t	szTotal	= szHeader + szNL + szXML + szTrailer;	// Minimum size, no padding yet.
    
	    if ( padBytes < 0 ) {
			
			padBytes = -padBytes;			// Still the desired total size, positive is less prone to mistakes.
			if ( padBytes < (signed long) szTotal ) throw xap_bad_number();	// The desired size is too small.
			padBytes = padBytes - szTotal;	// Now padBytes is the actual amount of padding.
			szTotal += padBytes;

	    }
	    
	    string packet;
	    packet.reserve ( szTotal );
	    
	    packet.append ( header );
   		packet.append ( nl );
	    packet.append ( *xml );
	    
	    bool nlAfterPad	= false;
    	if ( padBytes >= (signed long) szNL ) {
    		nlAfterPad = true;
    		padBytes -= szNL;
    	}
    	
	    while ( padBytes > 0 ) {
	    	int	count	= ( (padBytes > 100) ? 100 : padBytes );
	    	packet.append ( count, ' ' );
	    	padBytes -= count;
	    	if ( padBytes > (signed long) szNL ) {
	    		packet.append ( nl );
	    		padBytes -= szNL;
	    	}
	    }

		if ( nlAfterPad ) packet.append ( nl );
	    packet.append ( trailer );
	    
	    *xml = packet;
	    
	}

}

// =================================================================================================

#if (WSTRING_SUPPORTED == 1)

    /** Create an XML Packet for 16-bit encoded XML. */
DECL_EXPORT void
UtilityXAP::CreateXMLPacket	( const std::wstring&	encoding,
							  const bool			inPlaceEditOk,
							  signed long			padBytes,
							  const std::wstring&	nl,
							  std::wstring&			header,
							  std::wstring&			trailer,
							  std::wstring *		xml /* = NULL */ )
{
	const size_t	szNL	= nl.size() * sizeof ( wchar_t );
    const wchar_t	bom		= (wchar_t) 0x0FEFF;

	XMP_CheckToolkitInit();
	
    // Set up the header.
    header = L"<?xpacket begin='";
    header += bom;
    header.append ( L"' id='" );
    header.append ( VER_10_ID_UCS2 );
    header.append ( L"'" );
    if ( ! encoding.empty() ) {
        header.append ( L" encoding='" );
        header.append ( encoding );
        header.append ( L"'" );
    }
    header.append ( L"?>" );

    // Set up the trailer.
    trailer = L"<?xpacket end='";
    if ( inPlaceEditOk ) {
        trailer.append ( L"w'?>" );
    } else {
        trailer.append ( L"r'?>" );
    }

    // Compute and check the sizes, write the packet.
    if ( xml != NULL ) {

		const size_t	szHeader	= header.size() * sizeof ( wchar_t );
		const size_t	szTrailer	= trailer.size() * sizeof ( wchar_t );
		const size_t	szXML		= xml->size() * sizeof ( wchar_t );

		size_t	szTotal	= szHeader + szNL + szXML + szTrailer;	// Minimum size, no padding yet.
    
    	if ( padBytes & 1 ) throw xap_bad_number();	// The count must be even, whether it is positive or negative.
    	
	    if ( padBytes < 0 ) {
			
			padBytes = -padBytes;			// Still the desired total size, positive is less prone to mistakes.
			if ( padBytes < szTotal ) throw xap_bad_number();	// The desired size is too small.
			padBytes = padBytes - szTotal;	// Now padBytes is the actual amount of padding.
			szTotal += padBytes;

	    }
	    
	    wstring packet;
	    packet.reserve ( szTotal/2 + 1 );
	    
	    packet.append ( header );
   		packet.append ( nl );
	    packet.append ( *xml );
	    
	    bool nlAfterPad	= false;
    	if ( padBytes >= szNL ) {
    		nlAfterPad = true;
    		padBytes -= szNL;
    	}
	    
	    while ( padBytes > 0 ) {
	    	int	count	= ( (padBytes > 100) ? 100 : padBytes ) / sizeof ( wchar_t );
	    	packet.append ( count, L' ' );
	    	padBytes -= (count * sizeof ( wchar_t ));
	    	if ( padBytes > szNL ) {
	    		packet.append ( nl );
	    		padBytes -= szNL;
	    	}
	    }

		if ( nlAfterPad ) packet.append ( nl );
	    packet.append ( trailer );
	    
	    *xml = packet;
	    
	}

}

// =================================================================================================

DECL_EXPORT void
UtilityXAP::FilterPropPath(const std::string& tx, std::string& propPath) {

	XMP_CheckToolkitInit();
	
    if (tx == "")
        return;
    string orig = tx;
    wchar_t utf16[XAPTk::CBUFSIZE];
    char utf8[20];
    bool first = true;
    bool isShift = false;
    int state = 0;

    //Deal with initial pattern, if present
    if (tx.find("QQ-_") == 0) {
        propPath = "QQ-__";
        orig = tx.substr(4, tx.size()-4);
        first = false;
    } else
        propPath = "";

    const char* p = orig.c_str();
    register wchar_t wc;
    const wchar_t* wt;
    const wchar_t* wp;
    char* u;
    unsigned hex;
    char hexit;
    wstring ws;

    ws.reserve(tx.size() * 2);

    /* Convert the whole input string into UTF-16 */
    while ((p = UTF8_To_UCS2(p, -1, utf16, XAPTk::CBUFSIZE-1)) != NULL) {
        utf16[XAPTk::CBUFSIZE-1] = 0;
        ws.append(utf16);
    }
    utf16[XAPTk::CBUFSIZE-1] = 0;
    ws.append(utf16);

    /* Step through string */
    for (wp = ws.c_str(); *wp; ++wp) {
        // Get the next UTF-16 character
        wc = *wp;
        // Convert it back to UTF-8, so we have exactly one
        memset(utf8, 0, 20);
        wt = UCS2_To_UTF8(&wc, 1, utf8, 20);
        if (wt == &wc) {
            // Surrogate
            wt = UCS2_To_UTF8(wp, 2, utf8, 20);
            if (wt == wp) {
                // Skip this char
                first = false;
                continue;
            }
            wp = wt;
            /*
            wc is the first half of a surrogate pair, and is not an acceptable
            character.  We have loaded utf8 with the full encoding, so we
            just fall thru and let the unacceptable branch handle the whole
            thing.
            */
        }

        // Find acceptable characters
        if ((wc >= 0x0041 && wc <= 0x005A)
        || (wc >= 0x0061 && wc <= 0x007A)
        || (!first && wc >= L'0' && wc <= L'9')
        || (!first && wc == L'.')
        || (wc >= 0x00C0 && wc <= 0x00D6)
        || (wc >= 0x00D8 && wc <= 0x00F6)
        || (wc >= 0x00F8 && wc <= 0x00FF)
        || (wc >= 0x0100 && wc <= 0x0131)
        || (wc >= 0x0134 && wc <= 0x013E)
        || (wc >= 0x0141 && wc <= 0x0148)
        || (wc >= 0x014A && wc <= 0x017E)
        || (wc >= 0x0180 && wc <= 0x01C3)
        || (wc >= 0x01CD && wc <= 0x01F0)
        || (wc >= 0x01F4 && wc <= 0x01F5)
        || (wc >= 0x01FA && wc <= 0x0217)) {
            //Acceptable letter code
            if (isShift) {
                propPath.append("_");
                isShift = false;
            }
            propPath.append(utf8);
            state = 0;
        } else {
            // Needs escape
            // Deal with escaping escape
            if (state == 0 && utf8[0] == '-' && !first && !isShift) {
                propPath.append("-");
                state = 1;
                first = false;
                continue;
            }
            if (utf8[0] == '_' && !first && !isShift) {
                if (state == 1) {
                    propPath.append("__");
                } else {
                    propPath.append("_");
                }
                state = 0;
                continue;
            }
            state = 0;
            // Unacceptable character
            if (!isShift) {
                if (first)
                    propPath.append("QQ-_");
                else
                    propPath.append("-_");
                isShift = true;
            }
            u = utf8;
            while (*u) {
                hex = (((unsigned)*u) & 0x0F0) >> 4;
                if (hex > 9)
                    hexit = (hex - 10) + 'a';
                else
                    hexit = hex + '0';
                propPath += hexit;
                hex = (((unsigned)*u) & 0x0F);
                if (hex > 9)
                    hexit = (hex - 10) + 'a';
                else
                    hexit = hex + '0';
                propPath += hexit;
                ++u;
            }
        }
        first = false;
    }
    if (isShift)
        propPath.append("_");
}

#endif /* (WSTRING_SUPPORTED == 1) */

// =================================================================================================

DECL_EXPORT bool
UtilityXAP::GetBoolean(MetaXAP* meta, const string& ns, const string& path,
           bool &val) {
    string pval;
    XAPFeatures f = 0;
    bool ok = meta->get(ns, path, pval, f);
    if ( f & XAP_FEATURE_XML ) throw xap_bad_xap();
    if  (ok ) {
        if  (pval == XAP_BOOL_TRUE ) {
            val = true;
        } else if ( pval == XAP_BOOL_FALSE ) {
            val = false;
        } else {
            throw xap_bad_xap();
        }
    }
    return ok;
}

    /** Get property value as a date and time. */
DECL_EXPORT bool
UtilityXAP::GetDateTime(MetaXAP* meta, const string& ns, const string& path, XAPDateTime& dateTime) {
    bool ok = false;
    XAPFeatures f = XAP_FEATURE_NONE;
    string pval;
    ok = meta->get(ns, path, pval, f);
    if ( f & XAP_FEATURE_XML ) throw xap_bad_xap();
    if ( ok ) XAPTk::ToDateTime ( pval, dateTime );
    return(ok);
}

    /** Get property value as an integer. */
DECL_EXPORT bool
UtilityXAP::GetInteger(MetaXAP* meta, const string& ns, const string& path,
           long int &val) {
    string pval;
    XAPFeatures f = 0;
    bool ok = meta->get(ns, path, pval, f);
    if ( f & XAP_FEATURE_XML ) throw xap_bad_xap();
    if (ok) {
        val = atol(pval.c_str());
        if ( 0 /* *** val == 0L */ ) throw xap_bad_xap();	// GOTCHA: Need validation
    }
    return ok;
}


// =================================================================================================
// ChooseLocalizedText
// ===================

DECL_EXPORT int
UtilityXAP::ChooseLocalizedText ( const MetaXAP & meta, const std::string & ns, const std::string & container,
								  const std::string & genericLang, const std::string & specificLang,
								  std::string & actualLang, std::string & value, XAPFeatures & features )
{
	string		itemPath, itemLang, _ns;
	XAPPaths *	itemList	= 0;
	XAPValForm	form;
	XAPStructContainerType	cType;
	
	#if 0
	{
		cout << "In ChooseLocalizedText for " << container << " @ " << genericLang << '/' << specificLang << " from";
		try {
			itemList = (const_cast<MetaXAP &>(meta)).enumerate ( ns, container, 1 );	// ! Can't enumerate const objects!
		} catch ( ... ) {
			itemList = 0;
		}
		if ( itemList == 0 ) {
			cout << " <<none>>" << endl;
		} else {
			while ( itemList->hasMorePaths() ) {
				itemList->nextPath ( _ns, itemPath );
				meta.get ( ns, container+'/'+itemPath+"/@xml:lang", itemLang, features );
				cout << ' ' << itemLang;
			}
			cout << endl;
			delete itemList;
		}
	}
	#endif
	
	// *** The languages should be forced to lower case.  RFC 1766 says comparisons are case insensitive.
	
	// ---------------------------------------------
	// See if the container exists and is non-empty.
	
	try {
		form = meta.getForm ( ns, container );	// Throws an exception if it doesn't exist.
	} catch ( ... ) {
		return xmpCLT_NoValues;	// The container does not exist.
	}
	if ( form != xap_container ) throw xap_bad_type();
	cType = meta.getContainerType ( ns, container );
	if ( cType != xap_alt ) throw xap_bad_type();
	if ( meta.count ( ns, container+"/*" ) == 0 ) return xmpCLT_NoValues;	// The container is empty.
	
	// --------------------------------------------------
	// Look for an exact match for the specific language.
	
	try {
		itemPath = container + "/*[@xml:lang='" + specificLang + "']";
		if ( meta.get ( ns, itemPath, value, features ) ) {
			actualLang = specificLang;
			return xmpCLT_SpecificMatch;
		}
	} catch ( ... ) {}
	
	// ----------------------------------------------------------
	// If there is a generic language, look for matches using it.
	
	if ( genericLang != "" ) {
		
		// -------------------------------------------------
		// Look for an exact match for the generic language.
	
		try {
			itemPath = container + "/*[@xml:lang='" + genericLang + "']";
			if ( meta.get ( ns, itemPath, value, features ) ) {
				actualLang = genericLang;
				return xmpCLT_GenericMatch;
			}
		} catch ( ... ) {}

		
		// ---------------------------------------------------
		// Look for a partial match with the generic language.
		
		const int	genericLen	= genericLang.size();
		itemList = (const_cast<MetaXAP &>(meta)).enumerate ( ns, container, 1 );	// ! Can't enumerate const objects!
		
		while ( itemList->hasMorePaths() ) {
		
			itemList->nextPath ( _ns, itemPath );
			itemPath =  container + '/' + itemPath;
			
			if ( ! meta.get ( ns, itemPath+"/@xml:lang", itemLang, features ) ) {
				delete itemList;
				throw xap_bad_path();	// The item does not have an xml:lang attribute.
			}
			if ( (int)itemLang.size() <= genericLen ) continue;	// The item's language is too short.
			
			if ( (itemLang[genericLen] == '-') & (genericLang == itemLang.substr ( 0, genericLen )) ) {
				(void) meta.get ( ns, itemPath, value, features );
				actualLang = itemLang;
				delete itemList;
				return xmpCLT_SimilarMatch;
			}
		
		}
		
		delete itemList;

	}
	
	// -----------------------------
	// Look for an 'x-default' item.
	
	try {
		itemPath = container + "/*[@xml:lang='x-default']";
		if ( meta.get ( ns, itemPath, value, features ) ) {
			actualLang = "x-default";
			return xmpCLT_XDefault;
		}
	} catch ( ... ) {}
	
	// -----------------------------------------
	// Everything failed, choose the first item.
	
	itemPath = container + "/*[1]";
	if ( ! meta.get ( ns, itemPath+"/@xml:lang", actualLang, features ) ) {
		throw xap_bad_path();	// The first item does not have an xml:lang attribute.
	}
	(void) meta.get ( ns, itemPath, value, features );
	return xmpCLT_FirstItem;
	
}	// ChooseLocalizedText ()


// =================================================================================================
// GetLocalizedText
// ================

DECL_EXPORT bool
UtilityXAP::GetLocalizedText ( const MetaXAP* meta, const std::string& ns, const std::string& container,
							   const std::string & genericLang, const std::string & specificLang,
							   std::string& value, XAPFeatures& features )
{
	string	actualLang;
	
	try {
		int	match = ChooseLocalizedText ( *meta, ns, container, genericLang, specificLang,
										  actualLang, value, features );
		if ( match == xmpCLT_NoValues ) return false;
	} catch ( ... ) {
		return false;
	}
	
	return true;
	
}	// GetLocalizedText ()


// =================================================================================================

DECL_EXPORT bool
UtilityXAP::GetReal(MetaXAP* meta, const string& ns, const string& path,
        double &val) {
    string pval;
    XAPFeatures f = 0;
    bool ok = meta->get(ns, path, pval, f);
    if ( f & XAP_FEATURE_XML ) throw xap_bad_xap();
    if ( ok ) {
        val = atof ( pval.c_str() );
        if ( 0 /* *** val == 0.0 */ ) throw xap_bad_xap();	// GOTCHA: Need validation, isnan?
    }
    return ok;
}

// =================================================================================================

DECL_EXPORT bool
UtilityXAP::IsAltByLang ( const XAPPathTree* tree, const string& /* ns */, const string& path, string* langVal /* = NULL*/ )
{
    /* Validate parameters */
    if (tree == NULL) {
        XAPTk::NotImplemented(XAP_SAY_WHAT("UtilityXAP::IsAltByLang(NULL tree)"));
    }
    string rem;
    string parentPath;
    string lastStep;
    long int ord;
    string selectorName;
    string selectorVal;

    rem = path;
    while (!rem.empty()) {
        UtilityXAP::AnalyzeStep(rem, parentPath, lastStep, ord, selectorName, selectorVal);
        if (selectorName == XAP_ALT_SEL_DEFAULT) {
            if (langVal != NULL) {
                *langVal = selectorVal;
            }
            return(true);
        }
        rem = parentPath;
    }
    return(false);
}

// =================================================================================================

DECL_EXPORT void
UtilityXAP::SerializeToStr	( XAPPathTree *	tree,
							  string&		xml,
							  XAPFormatType	f /* = xap_format_pretty */,
							  const int		escnl /* = XAP_ESCAPE_CR */ )
{
    assert(tree != NULL);

    unsigned long iSize = tree->serialize(f, escnl);

    MetaXAP* meta = dynamic_cast<MetaXAP*>(tree);
    /* If it is a MetaXAP, we can use a short-cut. */
    if (meta != NULL) {
        meta->shortCutSerialize(xml);
#ifdef XXX
        xml = meta->iWantPackageAccess()->m_serialXML;
        meta->iWantPackageAccess()->m_serialXML = "";
#endif
        return;
    }
    /* Otherwise, do it the hard way. */

    char buffer[XAPTk::CSTRSIZE];
    xml = "";

    while (iSize > 0) {
        iSize = tree->extractSerialization(buffer, XAPTk::CSTRSIZE);
        if (iSize > 0)
            xml.append(buffer, iSize);
    }
}

// =================================================================================================

DECL_EXPORT void
UtilityXAP::SetBoolean ( MetaXAP* meta, const string& ns, const string& path, const bool val )
{
    assert(meta != NULL);
    string pval;
    if (val)
        pval.assign(XAP_BOOL_TRUE);
    else
        pval.assign(XAP_BOOL_FALSE);
    meta->set(ns, path, pval);
}


// =================================================================================================

DECL_EXPORT void
UtilityXAP::SetDateTime ( MetaXAP* meta, const string& ns, const string& path, const XAPDateTime& dateTime )
{
    string pval;
    XAPTk::ToString(dateTime, pval);
    meta->set(ns, path, pval);
#ifdef XXX
    /* Ignore seq and nano. */
    char buf[200];
    if (dateTime.tzHour == 0 && dateTime.tzMin == 0) {
        sprintf(buf, "%0d-%02d-%02dT%02d:%02d:%02dZ",
          dateTime.year,
          dateTime.month,
          dateTime.mday,
          dateTime.hour,
          dateTime.min,
          dateTime.sec);
    } else {
        sprintf(buf, "%0d-%02d-%02dT%02d:%02d:%02d%s%02d:%02d",
          dateTime.year,
          dateTime.month,
          dateTime.mday,
          dateTime.hour,
          dateTime.min,
          dateTime.sec,
          (dateTime.tzHour >= 0) ? "+" : "-",
          abs(dateTime.tzHour),
          dateTime.tzMin);
    }
    string pval(buf);
    meta->set(ns, path, pval);
#endif
}

// =================================================================================================

DECL_EXPORT void
UtilityXAP::SetInteger(MetaXAP* meta, const string& ns, const string& path,
           const long int val) {
    assert(meta != NULL);
    string pval;
    XAPTk::ToString(val, pval);
    meta->set(ns, path, pval);
}


// =================================================================================================
// SetLocalizedText
// ================

DECL_EXPORT void
UtilityXAP::SetLocalizedText ( MetaXAP* meta, const std::string& ns, const std::string& container,
							   const std::string & genericLang, const std::string & specificLang,
							   const std::string& value, const XAPFeatures features /* = XAP_FEATURE_DEFAULT */ )
{
	int			match;
	string		itemLang, oldValue, itemPath, defaultValue;
	XAPFeatures	oldFeatures;
	
	const string	defaultPath		= container + "/*[@xml:lang='x-default']";
	const bool		haveDefault		= meta->get ( ns, defaultPath, defaultValue, oldFeatures );
	
	match = ChooseLocalizedText ( *meta, ns, container, genericLang, specificLang,
								  itemLang, oldValue, oldFeatures );
	
	if ( genericLang != "" ) {		// Ignore the atcual language from ChooseLocalizedText.
		itemLang = genericLang;		//	Several of the rules here return the generic language if
	} else {						//	it is given, otherwise the specific language.
		itemLang = specificLang;
	}

	switch ( match ) {

		case xmpCLT_NoValues :

			// -----------------------------------------------------------------
			// Create the container with the appropriate language and x-default.
			
			meta->createFirstItem ( ns, container, value, "xml:lang", itemLang, true, features );
			if ( itemLang != "x-default" ) meta->set ( ns, defaultPath, value, features );

			break;
			
		case xmpCLT_SpecificMatch :
		
			// -----------------------------------------------
			// Update the specific language, update x-default.
			
			itemPath = container + "/*[@xml:lang='" + specificLang + "']";
			meta->set ( ns, itemPath, value, features );

			if ( haveDefault && (oldValue == defaultValue) ) {
				meta->set ( ns, defaultPath, value, features );
			}
			
			break;

		case xmpCLT_GenericMatch :
			{
				bool		haveOthers	= false;
				const int	genericLen	= genericLang.size();
				XAPPaths *	itemList	= meta->enumerate ( ns, container, 1 );	// ! Can't enumerate const objects!
				string		_ns;
				
				// -----------------------------------------------------------------------------------
				// First see if there are any items that are partial matches for the generic language.
				
				while ( itemList->hasMorePaths() ) {
				
					itemList->nextPath ( _ns, itemPath );
					itemPath =  container + '/' + itemPath;
					
					if ( ! meta->get ( ns, itemPath+"/@xml:lang", itemLang, oldFeatures ) ) {
						delete itemList;
						throw xap_bad_path();	// The item does not have an xml:lang attribute.
					}
					if ( (int)itemLang.size() <= genericLen ) continue;	// The item's language is too short.
					
					if ( (itemLang[genericLen] == '-') & (genericLang == itemLang.substr ( 0, genericLen )) ) {
						haveOthers = true;
						break;	// Exit the hasMorePaths loop.
					}
				
				}
				
				delete itemList;
				
				if ( haveOthers ) {
				
					// --------------------------------------------------------------
					// Write the new item to the specific language, ignore x-default.
			
					itemPath = container + "/*[@xml:lang='" + specificLang + "']";
					meta->set ( ns, itemPath, value, features );
				
				} else {
				
					// ---------------------------------------------------------------
					// No partial matches, set the generic language, update x-default.
			
					itemPath = container + "/*[@xml:lang='" + genericLang + "']";
					meta->set ( ns, itemPath, value, features );

					if ( haveDefault && (oldValue == defaultValue) ) {
						meta->set ( ns, defaultPath, value, features );
					}
				
				}
				
			}

			break;
			
		case xmpCLT_SimilarMatch :
		
			// -----------------------------------------------
			// Create the specific language, ignore x-default.
			
			itemPath = container + "/*[@xml:lang='" + specificLang + "']";
			meta->set ( ns, itemPath, value, features );
			
			break;
			
		case xmpCLT_XDefault :

			// -----------------------------------------------------------------------
			// Add the appropriate language, update x-default if it was the only item.
			
			itemPath = container + "/*[@xml:lang='" + itemLang + "']";
			meta->set ( ns, itemPath, value, features );

			if ( meta->count ( ns, container+"/*" ) == 2 ) {
				meta->set ( ns, defaultPath, value, features );
			}

			break;

		case xmpCLT_FirstItem	:

			// -----------------------------
			// Add the appropriate language.
			
			itemPath = container + "/*[@xml:lang='" + itemLang + "']";
			meta->set ( ns, itemPath, value, features );

			break;
			
		default :
			throw xap_assert_failure();

	}

	if ( meta->count ( ns, container+"/*" ) == 1 ) {
	
		// -----------------------------------------------------------------------------------------
		// There is only one item in the container, add an x-default at the front.  Check that there
		// isn't an x-default item.  We might have created a new container with just x-default.
		
		if ( ! meta->get ( ns, defaultPath, defaultValue, oldFeatures ) ) {
			itemPath = container + "/*[1]";	// Create a new x-default item at the front.
			meta->append ( ns, itemPath, value, true, features );
			meta->set ( ns, itemPath+"/@xml:lang", "x-default" );
		}
	
	} else {
	
		// ----------------------------------------------------
		// Make sure the x-default item is first, if it exists.
		
		(void) meta->get ( ns, container+"/*[1]/@xml:lang", itemLang, oldFeatures );	// Get the first item's language.

		if ( (itemLang != "x-default") &&
			 meta->get ( ns, defaultPath, defaultValue, oldFeatures ) ) {

			meta->set ( ns, defaultPath+"/@xml:lang", "-x-void-" );	// Tweak the old x-default item's language.
			
			itemPath = container + "/*[1]";	// Create a new x-default item at the front.
			meta->append ( ns, itemPath, defaultValue, true, oldFeatures );
			meta->set ( ns, itemPath+"/@xml:lang", "x-default" );
			
			meta->remove ( ns, container+"/*[@xml:lang='-x-void-']" );	// Remove the former x-default item.

		}
	
	}
	
	
}	// SetLocalizedText ()


// =================================================================================================


DECL_EXPORT void
UtilityXAP::SetReal(MetaXAP* meta, const string& ns, const string& path,
        const double val) {
    assert(meta != NULL);
    string pval;
    XAPTk::ToString(val, pval);
    meta->set(ns, path, pval);
}


// =================================================================================================
// CatenateContainerItems
// ======================

DECL_EXPORT void
UtilityXAP::CatenateContainerItems ( const MetaXAP & meta,
									 const std::string & ns, const std::string & container,
									 const std::string & separator, std::string & result )
{
	string	_ns, item, value;

	// -------------------------------------------------------------------------------------------
	// Return an empty result if the container does not exist, hurl if it isn't a bag or sequence.

	result.erase ( result.begin(), result.end() );
	if( ! IsExistingProperty ( meta, ns, container ) ) return;
	
	const XAPStructContainerType	cType	= meta.getContainerType ( ns, container );
	if ( (cType != xap_bag) && (cType != xap_seq) ) throw xap_bad_type();
	
	// ----------------------------------------------------------------------------------------------
	// Start the results string with the first item in the container.  Hurl if the item isn't simple.
	
	XAPPaths *	itemList	= (const_cast<MetaXAP &>(meta)).enumerate ( ns, container, 1 );	// ! Can't enumerate const objects!

	if ( ! itemList->hasMorePaths() ) return;	// Empty container.

	itemList->nextPath ( _ns, item );
	assert ( ns == _ns );
	item = container + '/' + item;
	
	XAPValForm	iForm	= meta.getForm ( ns, item );
	if ( iForm != xap_simple ) throw xap_bad_type();
	
	XAPFeatures	iFeat;
	(void) meta.get ( ns, item, result, iFeat );	// Start with the first item.
	
	// ---------------------------------------------------------------------------------
	// Add the remaining items, including the separator.  Hurl if the item isn't simple.
	
	while ( itemList->hasMorePaths() ) {

		itemList->nextPath ( _ns, item );
		assert ( ns == _ns );
		item = container + '/' + item;
		
		iForm = meta.getForm ( ns, item );
		if ( iForm != xap_simple ) throw xap_bad_type();
		
		(void) meta.get ( ns, item, value, iFeat );
		result += separator;	// Hopefully more efficient than "result += separator + value".
		result += value;

	}
	
}	// CatenateContainerItems ()


// =================================================================================================
// ClassifyCharacter
// =================

enum UniCharKind {
	UCK_normal,
	UCK_space,
	UCK_comma,
	UCK_semicolon,
	UCK_control
};
typedef enum UniCharKind	UniCharKind;

// const char * UCK_names[5] = { "normal", "space", "comma", "semicolon", "control" };

static void
ClassifyCharacter ( const string & fullString, int offset, UniCharKind & charKind, int & charSize )
{
	charKind = UCK_normal;	// Assume typical case.
	
	unsigned char	currByte = fullString[offset];
	
	if ( currByte < 0x80 ) {
	
		// ----------------------------------------
		// We've got a single byte ASCII character.

		charSize = 1;

		if ( currByte == 0x3B ) {
			charKind = UCK_semicolon;
		} else if ( currByte == 0x2C ) {
			charKind = UCK_comma;
		} else if ( currByte == 0x20 ) {
			charKind = UCK_space;
		} else if ( currByte < 0x20 ) {
			charKind = UCK_control;
		}
		
		#if 0
			if ( charKind != UCK_normal ) {
				cout << "Found ASCII " << UCK_names[charKind];
				cout.setf ( ios_base::hex, ios_base::basefield );
				cout << (int)currByte;
				cout << " @ " << offset << endl;
				cout.setf ( ios_base::dec, ios_base::basefield );
			}
		#endif

	} else {
	
		// ---------------------------------------------------------------------------------------
		// We've got a multibyte Unicode character. The first byte has the number of bytes and the
		// highest order bits. The other bytes each add 6 more bits. Compose the UTF-32 form so we
		// can classify directly with the Unicode code points. Order the upperBits tests to be
		// fastest for Japan, probably the most common non-ASCII usage.
		
		long	uniChar;
		
		charSize = 0;
		uniChar = currByte;
		while ( (uniChar & 0x80) != 0 ) {	// Count the leading 1 bits in the byte.
			++charSize;
			uniChar = uniChar << 1;
		}
		assert ( (offset + charSize) <= (int)fullString.size() );
		
		uniChar = uniChar & 0x7F;			// Put the character bits in the bottom of uniChar.
		uniChar = uniChar >> charSize;
		
		for ( int i = (offset + 1); i < (offset + charSize); ++i ) {
			uniChar = (uniChar << 6) | (fullString[i] & 0x3F);
		}
		
		long upperBits	= uniChar >> 8;	// First filter on just the high order 24 bits.

		if ( upperBits == 0xFF ) {			// U+FFxx

			if ( uniChar == 0xFF1B ) {			// U+FF1B, full width semicolon.
				charKind = UCK_semicolon;
			} else if ( uniChar == 0xFF0C ) {	// U+FF0C, full width comma.
				charKind = UCK_comma;
			} else if ( uniChar == 0xFF64 ) {	// U+FF64, half width ideographic comma.
				charKind = UCK_comma;
			}

		} else if ( upperBits == 0xFE ) {	// U+FE--

			if ( uniChar == 0xFE54 ) {			// U+FE54, small semicolon.
				charKind = UCK_semicolon;
			} else if ( uniChar == 0xFE50 ) {	// U+FE50, small comma.
				charKind = UCK_comma;
			} else if ( uniChar == 0xFE51 ) {	// U+FE51, small ideographic comma.
				charKind = UCK_comma;
			}

		} else if ( upperBits == 0x30 ) {	// U+30--

			if ( uniChar == 0x3001 ) {			// U+3001, ideographic comma.
				charKind = UCK_comma;
			} else if ( uniChar == 0x3000 ) {	// U+3000, ideographic space.
				charKind = UCK_space;
			} else if ( uniChar == 0x303F ) {	// U+303F, ideographic half fill space.
				charKind = UCK_space;
			}

		} else if ( upperBits == 0x20 ) {	// U+20--

			if ( uniChar == 0x2028 ) {			// U+2028, line separator.
				charKind = UCK_control;
			} else if ( uniChar == 0x2029 ) {	// U+2029, paragraph separator.
				charKind = UCK_control;
			} else if ( (0x2000 <= uniChar) && (uniChar <= 0x200B) ) {
				charKind = UCK_space;			// U+2000..U+200B, en quad through zero width space.
			}

		} else if ( upperBits == 0x06 ) {	// U+06--

			if ( uniChar == 0x060C ) {			// U+060C, Arabic comma.
				charKind = UCK_comma;
			} else if ( uniChar == 0x061B ) {	// U+061B, Arabic semicolon.
				charKind = UCK_semicolon;
			}

		} else if ( upperBits == 0x05 ) {	// U+05--

			if ( uniChar == 0x055D ) {
				charKind = UCK_comma;			// U+055D, Armenian comma.
			}

		} else if ( upperBits == 0x03 ) {	// U+03--

			if ( uniChar == 0x037E ) {
				charKind = UCK_semicolon;		// U+037E, Greek "semicolon" (really a question mark).
			}

		}
		
		#if 0
			if ( charKind != UCK_normal ) {
				cout << "Found Unicode " << UCK_names[charKind] << " = ";
				cout.setf ( ios_base::hex, ios_base::basefield );
				cout << uniChar;
				cout.setf ( ios_base::dec, ios_base::basefield );
				cout << " @ " << offset << " : " << charSize << endl;
			}
		#endif
				
	}

}	// ClassifyCharacter ()


// =================================================================================================
// SeparateContainerItems
// ======================

DECL_EXPORT void
UtilityXAP::SeparateContainerItems ( MetaXAP & meta,
									 const std::string & ns, const std::string & container,
						 			 const XAPStructContainerType cType,
									 const std::string & values, const bool preserveCommas )
{
	int			itemStart, itemEnd, charSize, nextSize;
	UniCharKind	charKind, nextKind;
	string		itemValue;

	const int		endPos		= values.size();
	const string	itemPath	= container + "/*[last()]";
	
	// --------------------------------------------------------------------------------------------
	// Delete any existing items, create a dummy first item which is deleted before returning. Then
	// extract the item values one at a time, until the whole input string is done. Be very careful
	// in the extraction about the string positions. They are essentially byte pointers, while the
	// contents are UTF-8. Adding or subtracting 1 does not necessarily move 1 Unicode character!

	if( IsExistingProperty ( meta, ns, container ) ) meta.remove ( ns, container );
	meta.createFirstItem ( ns, container, "", cType );
	
	itemStart = 0;
	while ( itemStart < endPos ) {
		
		// --------------------------------------------------
		// Skip any leading spaces and separation characters.
		
		while ( itemStart < endPos ) {
			ClassifyCharacter ( values, itemStart, charKind, charSize );
			if ( charKind == UCK_normal ) break;
			itemStart += charSize;
		}
		
		// ---------------------------------
		// Find the end of the current item.
		
		for ( itemEnd = itemStart; itemEnd < endPos; itemEnd += charSize ) {

			ClassifyCharacter ( values, itemEnd, charKind, charSize );
			if ( charKind == UCK_normal ) continue;

			if ( (charKind == UCK_semicolon) || (charKind == UCK_control) ) break;
			if ( (charKind == UCK_comma) && (! preserveCommas) ) break;

			if ( charKind == UCK_space ) {
				if ( (itemEnd + charSize) >= endPos ) break;	// Anything left?
				ClassifyCharacter ( values, (itemEnd+charSize), nextKind, nextSize );
				if ( nextKind != UCK_normal ) break;			// Keep single space.
			}

		}
		
		// ----------------------------
		// Save the value if not empty.
		
		if ( itemEnd != itemStart ) {
			itemValue = values.substr ( itemStart, (itemEnd - itemStart) );
			meta.append ( ns, itemPath, itemValue );
			itemStart = itemEnd + charSize;
		}
		
	}
	
	// ----------------------------
	// Delete the dummy first item.
	
	meta.remove ( ns, container+"/*[1]" );

}	// SeparateContainerItems ()


// =================================================================================================
// RemoveOneProperty
// =================
//
// Removes the given property if it is external.  Passing true for doAll will remove any property.
// The function result is true if the property was successfully removed.

static bool
RemoveOneProperty ( MetaXAP & meta,
					const std::string & ns,
					const std::string & path,
					const bool doAll /* = false */ )
{

	if ( (! doAll) && IsInternalProperty ( ns, path ) ) return false;
	
	try {
		// cout << "# Removing " << path << " from " << ns << endl;
		meta.remove ( ns, path );
		return true;
	} catch ( ... ) {
		return false;
	}

}	// RemoveOneProperty ()


// =================================================================================================
// RemoveProperties
// ================
//
// This is a public function.  The description is in UtilityXAP.h.

DECL_EXPORT bool
UtilityXAP::RemoveProperties ( MetaXAP & meta,
							   const std::string * ns /* = 0 */,
							   const std::string * path /* = 0 */,
							   const bool doAll /* = false */ )
{
	bool		allCandidatesRemoved	= true;	// Set to false if internal properties are skipped.
	XAPPaths *	candidatePaths			= 0;
	string		candidateSchema;
	string		candidateProp;
	
	if ( ns != 0 ) {
	
		if ( path != 0 ) {
		
			// -------------------------------------------------------------------------------------
			// The namespace and path are both non-null, remove this one property if it is external.
			
			allCandidatesRemoved = RemoveOneProperty ( meta, *ns, *path, doAll );
		
		} else {
		
			// --------------------------------------------------------------------------------------------
			// The namespace is non-null and the path is null, remove external properties from this schema.
			
			// ! This uses an ugly hack that depends on implementation details of XAPTkData::enumerate to
			// ! enumerate the top level properties of a specific schema.
			
			try {
				candidatePaths = meta.enumerate ( *ns, *((string *)0), 1 );
				while ( candidatePaths->hasMorePaths() ) {
					candidatePaths->nextPath ( candidateSchema, candidateProp );
					allCandidatesRemoved &= RemoveOneProperty ( meta, candidateSchema, candidateProp, doAll );
				}
			} catch ( ... ) {}
		
		}
	
	} else {
	
		// ------------------------------------------------------------------
		// The namespace is null, remove external properties from all schema.
		
		try {
			candidatePaths = meta.enumerate ( 1 );
			while ( candidatePaths->hasMorePaths() ) {
				candidatePaths->nextPath ( candidateSchema, candidateProp );
				allCandidatesRemoved &= RemoveOneProperty ( meta, candidateSchema, candidateProp, doAll );
			}
		} catch ( ... ) {}
	
	}
	
	delete candidatePaths;
	return allCandidatesRemoved;
	
}	// RemoveProperties ()


// =================================================================================================
// Forward declarations
// ====================

static bool
IsItemInDest ( const MetaXAP & source, MetaXAP & dest, const std::string & ns,
			   const std::string & sourceItemPath, const std::string & destContainerPath );

static void
DuplicateProperty ( const MetaXAP & source, MetaXAP & dest, const std::string & ns,
					const std::string & sourcePath, const std::string & destPath );

static void
DuplicateStructFields ( const MetaXAP & source, MetaXAP & dest, const std::string & ns,
						const std::string & sourcePath, const std::string & destPath );

static void
DuplicateContainerItems ( const MetaXAP & source, MetaXAP & dest, const std::string & ns,
						  const std::string & sourcePath, const std::string & destPath );


// =================================================================================================
// IsMatchingProperty
// ==================
//
// Returns true if the given properties have matching values.
// *** Should be a primitive in MetaXAP!

static bool
IsMatchingProperty ( const MetaXAP & source, MetaXAP & dest, const std::string & ns,
					 const std::string & sourcePath, const std::string & destPath )
{
	const XAPValForm	sourceForm	= source.getForm ( ns, sourcePath );
	const  XAPValForm	destForm	= dest.getForm ( ns, destPath );
	assert ( (sourceForm != xap_unknown) && (destForm != xap_unknown) );
	
	if ( sourceForm != destForm ) return false;

	XAPPaths *	childList	= 0;
	string		_ns, sourceValue, destValue;
	string		childName, sourceChildPath, destChildPath;
	
	switch ( sourceForm ) {
	
		case xap_simple:
			
			// ---------------------------------------------------------------
			// For simple properties compare the value and xml:lang attribute.

			{
				bool		sourceExists, destExists;
				XAPFeatures	sourceFeatures, destFeatures;

				sourceExists = source.get ( ns, sourcePath, sourceValue, sourceFeatures );
				destExists = dest.get ( ns, destPath, destValue, destFeatures );
				assert ( sourceExists & destExists );
				
				if ( (sourceValue != destValue) || (sourceFeatures != destFeatures) ) return false;

				sourceExists = source.get ( ns, sourcePath+"/@xml:lang", sourceValue, sourceFeatures );
				destExists = dest.get ( ns, destPath+"/@xml:lang", destValue, destFeatures );
				
				if ( sourceExists != destExists ) return false;
				if ( (sourceExists) && (sourceValue != destValue) ) return false;
			}

			break;
	
		case xap_description:
		
			// ------------------------------------------------------------------------------------
			// Structures are treated like local schema.  Iterate over the fields in the source and 
			// compare each to the corresponding field in the destination.  Don't compare the number
			// of fields, allow the destination to contain a superset of the source.
			
			{
				childList = (const_cast<MetaXAP &>(source)).enumerate ( ns, sourcePath, 1 );	// ! Can't enumerate const objects!

				while ( childList->hasMorePaths() ) {
					try {

						childList->nextPath ( _ns, childName );
						assert (ns == _ns);
						sourceChildPath = sourcePath + '/' + childName;
						destChildPath = destPath + '/' + childName;

						if ( ! IsExistingProperty ( dest, ns, destChildPath ) ) return false;
						if ( ! IsMatchingProperty ( source, dest, ns, sourceChildPath, destChildPath ) ) return false;

					} catch ( ... ) {}	// Don't let an exception kill the loop.
				}
				
				delete childList;
			}

			break;
	
		case xap_container:
		
			// -------------------------------------------------------------------------------------
			// All three containers (alt, bag, seq) are treated the same.  Each item in the source
			// is examined to see if it is in the destination already, without regard to order.
			// order.  The number of items in each container is also compared.
			
			{
				if ( source.count ( ns, sourcePath+"/*" ) != dest.count ( ns, destPath+"/*" ) ) return false;
				
				childList = (const_cast<MetaXAP &>(source)).enumerate ( ns, sourcePath, 1 );	// ! Can't enumerate const objects!

				while ( childList->hasMorePaths() ) {
					try {

						childList->nextPath ( _ns, childName );
						sourceChildPath = sourcePath + '/' + childName;

						if ( ! IsItemInDest ( source, dest, ns, sourceChildPath, destPath ) ) return false;

					} catch ( ... ) {}	// Don't let an exception kill the loop.
				}
				
				delete childList;
			}

			break;
			
		default:
			assert ( sourceForm == xap_unknown );	// ! Should fail, there are only 3 legit values.
	
	}
	
	return true;
	
}	// IsMatchingProperty ()


// =================================================================================================
// IsItemInDest
// ============
//
// Checks if the given item in a source container already exists in the destination container.  This
// does a value comparison against all items in the destination, without regard to order.

static bool
IsItemInDest ( const MetaXAP & source, MetaXAP & dest, const std::string & ns,
			   const std::string & sourceItemPath, const std::string & destContainerPath )
{
	XAPPaths *	itemList	= (const_cast<MetaXAP &>(dest)).enumerate ( ns, destContainerPath, 1 );	// ! Can't enumerate const objects!
	string		_ns, destItemName, destItemPath;

	while ( itemList->hasMorePaths() ) {
		try {
			itemList->nextPath ( _ns, destItemName );
			destItemPath = destContainerPath + '/' + destItemName;
			if ( IsMatchingProperty ( source, dest, ns, sourceItemPath, destItemPath ) ) return true;
		} catch ( ... ) {}	// Don't let an exception kill the loop.
	}
	
	delete itemList;
	return false;

}	// IsItemInDest ()


// =================================================================================================
// AppendItemToDest
// ================
//
// Appends the given item from a source container to the destination container.

static void
AppendItemToDest ( const MetaXAP & source, MetaXAP & dest, const std::string & ns,
				   const std::string & sourceItemPath, const std::string & destContainerPath,
				   bool hasFakeFirst )
{
	string		destItemPath	= destContainerPath + "/*[last()]";
	XAPValForm	itemForm		= source.getForm ( ns, sourceItemPath );
	assert ( itemForm != xap_unknown );

	string	_ns, value, lang;

	switch ( itemForm ) {
	
		case xap_simple:

			// -----------------------------------------------------------------------------------
			// For a simple item just append the value, features, and any xml:lang attribute.
			// If the xml:lang attribute is 'x-default' insert it at the front of the destination.

			{
				bool		exists, hasLang;
				XAPFeatures	features, _f;

				exists = source.get ( ns, sourceItemPath, value, features );
				assert ( exists );
				
				hasLang = source.get ( ns, sourceItemPath+"/@xml:lang", lang, _f );	// Have xml:lang?

				if ( (! hasLang) || (lang != "x-default") ) {
					dest.append ( ns, destItemPath, value, false, features );	// Not x-default, just append.
				} else {
					// Is x-default, put this item at the front.
					destItemPath = destContainerPath + "/*[1]";
					dest.append ( ns, destItemPath, value, !hasFakeFirst, features );
					if ( hasFakeFirst ) destItemPath = destContainerPath + "/*[2]";	// To set the language.
				}
				
				if ( hasLang ) dest.set ( ns, destItemPath+"/@xml:lang", lang );
			}

			break;
	
		case xap_description:

			// ------------------------------------------------------------------------------------
			// For a structure first create a simple item with no value, then add a bogus field
			// to magically convert the property to a structure, and remove the bogus field.
			// Then recursively call DuplicateProperty for the fields in the source.
			
			// *** A bug in the toolkit causes the structure to revert to a simple property when it
			// *** has no fields.

			{
				value = destItemPath + "/bogus";
				dest.append ( ns, destItemPath, XAP_NO_VALUE );
				dest.set ( ns, value, XAP_NO_VALUE );
				dest.remove ( ns, value );
				
				DuplicateStructFields ( source, dest, ns, sourceItemPath, destItemPath );
			}

			break;
	
		case xap_container:

			// ---------------------------------------------------------------------------------
			// Containers are pretty ugly, the toolkit API is not convenient.  You don't create
			// a container, you create it and add the first item at the same time.  You can't
			// use MetaXAP::set to add an item, you have to use MetaXAP::append.  Since both
			// MetaXAP::createFirstItem and MetaXAP::append take values and create simple items
			// having a container as an item of a container is tricky.
			//
			// The approach taken is to first create the container with a bogus first item, copy
			// all of the actual items, then remove the bogus first item.  Luckily RDF allows
			// heterogeneous containers, items can vary in form.
			//
			// ! Don't get too confused: We are adding a container as an item in a container!
				
			dest.createFirstItem ( ns, destItemPath, XAP_NO_VALUE, source.getContainerType ( ns, sourceItemPath ) );
			DuplicateContainerItems ( source, dest, ns, sourceItemPath, destItemPath );
			dest.remove ( ns, destItemPath+"/*[1]" );

			break;
			
		default:
			assert ( itemForm == xap_unknown );	// ! Should fail, there are only 3 legit values.
	
	}
	
}	// AppendItemToDest ()


// =================================================================================================
// DuplicateContainerItems
// =======================
//
// Duplicate the items in a container.  Each item in the source is appended to the destination.  The
// source and destination paths can be different when duplicating things that are inside containers.

static void
DuplicateContainerItems ( const MetaXAP & source, MetaXAP & dest, const std::string & ns,
						  const std::string & sourcePath, const std::string & destPath )
{
	XAPPaths *	itemList	= (const_cast<MetaXAP &>(source)).enumerate ( ns, sourcePath, 1 );	// ! Can't enumerate const objects!
	string		_ns, itemName, itemPath;

	while ( itemList->hasMorePaths() ) {
		try {
			itemList->nextPath ( _ns, itemName );
			itemPath = sourcePath + '/' + itemName;
			AppendItemToDest ( source, dest, ns, itemPath, destPath, true );
		} catch ( ... ) {}	// Don't let an exception kill the loop.
	}
	
	delete itemList;

}	// DuplicateContainerItems ()


// =================================================================================================
// DuplicateStructFields
// =====================
//
// Duplicate the fields in a structure.  The fields are known to not exist in the destination.  The
// source and destination paths can be different when duplicating things that are inside containers.

static void
DuplicateStructFields ( const MetaXAP & source, MetaXAP & dest, const std::string & ns,
						const std::string & sourcePath, const std::string & destPath )
{
	XAPPaths *	fieldList	= (const_cast<MetaXAP &>(source)).enumerate ( ns, sourcePath, 1 );	// ! Can't enumerate const objects!
	string		_ns, fieldName;

	while ( fieldList->hasMorePaths() ) {
		try {
			fieldList->nextPath ( _ns, fieldName );
			DuplicateProperty ( source, dest, ns, sourcePath+'/'+fieldName, destPath+'/'+fieldName );
		} catch ( ... ) {}	// Don't let an exception kill the loop.
	}
	
	delete fieldList;

}	// DuplicateStructFields ()


// =================================================================================================
// DuplicateProperty
// =================
//
// Duplicate a property from the source to the destination.  The property must not exist in the
// destination.  This does not handle adding one (isolated) item to a container.  The source and
// destination paths can be different when duplicating things that are inside containers.

static void
DuplicateProperty ( const MetaXAP & source, MetaXAP & dest, const std::string & ns,
					const std::string & sourcePath, const std::string & destPath )
{
	const XAPValForm	propForm	= source.getForm ( ns, sourcePath );
	assert ( propForm != xap_unknown );

	string	value, tempPath;

	// cout << "# Duplicating " << sourcePath << " to " << destPath << " in " << ns << endl;
	assert ( ! IsExistingProperty ( dest, ns, destPath ) );
	
	switch ( propForm ) {
	
		case xap_simple:

			// --------------------------------------------------------------------------------
			// For a simple property just copy the value, features, and any xml:lang attribute.

			{
				bool		exists;
				XAPFeatures	features;

				exists = source.get ( ns, sourcePath, value, features );
				assert ( exists );
				dest.set ( ns, destPath, value, features );
				
				if ( source.get ( ns, sourcePath+"/@xml:lang", value, features ) ) {
					dest.set ( ns, destPath+"/@xml:lang", value );
				}
			}

			break;
	
		case xap_description:

			// ------------------------------------------------------------------------------------
			// For a structure first create a simple property with no value, then add a bogus field
			// to magically convert the property to a structure, and remove the bogus field.  Then
			// recursively call DuplicateProperty for the fields in the source.
			
			// *** A bug in the toolkit causes the structure to revert to a simple property when it
			// *** has no fields.

			{
				tempPath = destPath + "/bogus";
				
				dest.set ( ns, destPath, XAP_NO_VALUE );
				dest.set ( ns, tempPath, XAP_NO_VALUE );
				dest.remove ( ns, tempPath );
				
				DuplicateStructFields ( source, dest, ns, sourcePath, destPath );
			}

			break;
	
		case xap_container:

			// ---------------------------------------------------------------------------------
			// Containers are pretty ugly, the toolkit API is not convenient.  You don't create
			// a container, you create it and add the first item at the same time.  You can't
			// use MetaXAP::set to add an item, you have to use MetaXAP::append.  Since both
			// MetaXAP::createFirstItem and MetaXAP::append take values and create simple items
			// having a container as an item of a container is tricky.
			//
			// The approach taken is to first create the container with a bogus first item, copy
			// all of the actual items, then remove the bogus first item.  Luckily RDF allows
			// heterogeneous containers, items can vary in form.
				
			dest.createFirstItem ( ns, destPath, XAP_NO_VALUE, source.getContainerType ( ns, sourcePath ) );
			DuplicateContainerItems ( source, dest, ns, sourcePath, destPath );
			dest.remove ( ns, destPath+"/*[1]" );

			break;
			
		default:
			assert ( propForm == xap_unknown );	// ! Should fail, there are only 3 legit values.
	
	}
	
}	// DuplicateProperty ()


// =================================================================================================
// MergeProperty
// ==============
//
// Merge structures and containers.  Structures are handled like local schema, new fields are added
// to the destination and existing structures and containers are merged.

static void
MergeProperty ( const MetaXAP & source, MetaXAP & dest,
				 const std::string & ns, const std::string & rootPath );

// -------------------------------------------------------------------------------------------------

static void
MergeStructure ( const MetaXAP & source, MetaXAP & dest,
				 const std::string & ns, const std::string & rootPath )
{
	XAPPaths *	childList	= 0;
	string		_ns, childName, childPath;

	// ---------------------------------------------------------------------------------------------
	// Structures are treated like local schema.  Iterate over the fields in the source.  Add the
	// field to the destination if it isn't there already.  If it is already in the destination call
	// MergeProperty recursively.
	
	childList = (const_cast<MetaXAP &>(source)).enumerate ( ns, rootPath, 1 );	// ! Can't enumerate const objects!
	assert ( childList != 0 );

	while ( childList->hasMorePaths() ) {
		try {

			childList->nextPath ( _ns, childName );
			assert ( ns == _ns );
			childPath = rootPath + '/' + childName;

			if ( IsExistingProperty ( dest, ns, childPath ) ) {
				MergeProperty ( source, dest, ns, childPath );
			} else {
				DuplicateProperty ( source, dest, ns, childPath, childPath );
			}

		} catch ( ... ) {}	// Don't let an exception kill the loop.
	}
	
	delete childList;

}	// MergeStructure ()

// -------------------------------------------------------------------------------------------------

static void
MergeContainer ( const MetaXAP & source, MetaXAP & dest,
				 const std::string & ns, const std::string & rootPath )
{
	XAPPaths *	childList	= 0;
	string		_ns, childName, childPath;
	
	// ---------------------------------------------------------------------------------------------
	// Regular (non AltText) containers are merged by value.  Add an item from the source if an item
	// with the same value is not already in the destination.

	childList = (const_cast<MetaXAP &>(source)).enumerate ( ns, rootPath, 1 );	// ! Can't enumerate const objects!
	assert ( childList != 0 );

	while ( childList->hasMorePaths() ) {
		try {

			childList->nextPath ( _ns, childName );
			assert ( ns == _ns );
			childPath = rootPath + '/' + childName;

			if ( ! IsItemInDest ( source, dest, ns, childPath, rootPath ) ) {
				AppendItemToDest ( source, dest, ns, childPath, rootPath, false );
			}

		} catch ( ... ) {}	// Don't let an exception kill the loop.
	}
	
	delete childList;

}	// MergeContainer ()

// -------------------------------------------------------------------------------------------------

static void
MergeAltText ( const MetaXAP & source, MetaXAP & dest,
				 const std::string & ns, const std::string & rootPath )
{
	XAPPaths *	childList	= 0;
	string		_ns, childName, childPath, _v;
	XAPFeatures	_f;
	
	// ---------------------------------------------------------------------------------------------
	// AltText containers are merged by language attributes.  Add an item from the source if an item
	// with the same languge is not already in the destination.

	childList = (const_cast<MetaXAP &>(source)).enumerate ( ns, rootPath, 1 );	// ! Can't enumerate const objects!
	assert ( childList != 0 );

	while ( childList->hasMorePaths() ) {
		try {

			childList->nextPath ( _ns, childName );
			assert ( ns == _ns );
			assert ( childName.substr ( 0, 12 ) == "*[@xml:lang=" );
			childPath = rootPath + '/' + childName;

			if ( ! dest.get ( ns, childPath, _v, _f ) ) {
				AppendItemToDest ( source, dest, ns, childPath, rootPath, false );
			}

		} catch ( ... ) {}	// Don't let an exception kill the loop.
	}
	
	delete childList;

}	// MergeAltText ()

// -------------------------------------------------------------------------------------------------

static bool
IsAltTextContainer ( const MetaXAP & meta, const std::string & ns, const std::string & rootPath )
{
	bool		isAltText	= true;
	XAPPaths *	childList	= 0;
	string		_ns, childName, childPath;
	
	// ----------------------------------------------
	// Ucko-Yucko!  Need a better way to decide this!

	try {
	
		childList = (const_cast<MetaXAP &>(meta)).enumerate ( ns, rootPath, 1 );	// ! Can't enumerate const objects!
		assert ( childList != 0 );

		while ( childList->hasMorePaths() ) {
			childList->nextPath ( _ns, childName );
			assert ( ns == _ns );
			if ( childName.substr ( 0, 12 ) != "*[@xml:lang=" ) {
				isAltText = false;
				break;
			}
		}
	
	} catch ( ... ) {
		isAltText = false;
	}
	
	delete childList;
	return isAltText;
	
}	// IsAltTextContainer ()

// -------------------------------------------------------------------------------------------------

static void
MergeProperty ( const MetaXAP & source, MetaXAP & dest,
				 const std::string & ns, const std::string & rootPath )
{
	const XAPValForm	sourceForm	= source.getForm ( ns, rootPath );
	const  XAPValForm	destForm	= dest.getForm ( ns, rootPath );
	assert ( (sourceForm != xap_unknown) && (destForm != xap_unknown) );
	
	XAPStructContainerType	sourceSCT, destSCT;
	bool	sourceAT, destAT;
	
	if ( sourceForm != destForm ) return;	// Bail if the forms don't match.
	// cout << "# Updating " << rootPath << " in " << ns << endl;

	switch ( sourceForm ) {
	
		case xap_simple:

			// Nothing to do.
			break;
	
		case xap_description:
		
			MergeStructure ( source, dest, ns, rootPath );
			break;
	
		case xap_container:

			sourceSCT = source.getContainerType ( ns, rootPath );
			destSCT = dest.getContainerType ( ns, rootPath );
			
			if ( sourceSCT != destSCT ) break;	// Bail if the container types don't match.
			
			if ( sourceSCT == xap_alt ) {
			
				// ---------------------------------------------------------------------
				// Make sure that both of the containers are AltText, or neither one is.
				// If both are, then do the language based merging.
				
				sourceAT = IsAltTextContainer ( source, ns, rootPath );
				destAT = IsAltTextContainer ( dest, ns, rootPath );
				if ( sourceAT != destAT ) break;	// Bail if one is AltText and the other isn't.

				if ( sourceAT ) {
					MergeAltText ( source, dest, ns, rootPath );
					break;	// This was a pair of AltText containers, we're done.
				}

			}
			
			// ------------------------------------------------------------------------------
			// We'll fall through to here for bags, sequences, and non-language alternatives.

			MergeContainer ( source, dest, ns, rootPath );
			break;
			
		default:
			assert ( sourceForm == xap_unknown );	// ! Should fail, there are only 3 legit values.
			break;
	
	}
	
}	// MergeProperty ()


// =================================================================================================
// AppendProperties
// ================
//
// This is a public function.  The description is in UtilityXAP.h.

DECL_EXPORT void
UtilityXAP::AppendProperties ( const MetaXAP & source, MetaXAP & dest,
							   const bool replaceOld, const bool doAll /* = false */ )
{
	XAPPaths *	topProps	= 0;
	string		ns, prop;
	
	// ---------------------------------------------------------------------------------------------
	// Iterate over all of the top level properties in the source, updating each appropriately.
	// Check for aliases and internal properties here, not in MergeProperty.  This only needs to be
	// done at the top level, not recursively for fields of structures or items in containers.
	
	assert ( source.isEnabled ( XAP_OPTION_ALIASING_ON ) );	// Aliasing must be on for both.
	assert ( dest.isEnabled ( XAP_OPTION_ALIASING_ON ) );
	
	topProps = (const_cast<MetaXAP &>(source)).enumerate ( 1 );	// ! Can't enumerate const objects!
	
	while ( topProps->hasMorePaths() ) {
		try {

			topProps->nextPath ( ns, prop );
			if ( IsAliasProperty ( ns, prop ) ) continue;	// Skip aliases, catch the base properties.
			if ( (! doAll) && IsInternalProperty ( ns, prop ) ) continue;	// Skip internal properties.

			if ( ! IsExistingProperty ( dest, ns, prop ) ) {
				DuplicateProperty ( source, dest, ns, prop, prop );
			} else if ( replaceOld ) {
				RemoveOneProperty ( dest, ns, prop, true );
				DuplicateProperty ( source, dest, ns, prop, prop );
			} else {
				MergeProperty ( source, dest, ns, prop );
			}

		} catch ( ... ) {}	// Don't let an exception kill the loop.
	}
	
	if ( topProps != 0 ) delete topProps;
	
}	// AppendProperties ()


/* ===== Static (Class) Variables ===== */

/* ===== Public Destructor ===== */

/* ====== Protected Constructors ===== */

/* ===== Operators ===== */

/* ===== Non-Member Static Functions ===== */





void
XAPTk_InitUtilityXAP() {
    //No-op
}

void
XAPTk_KillUtilityXAP() {
    //No-op
}


/*
$Log$
*/
