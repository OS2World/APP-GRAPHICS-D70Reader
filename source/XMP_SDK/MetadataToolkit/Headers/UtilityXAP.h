
/* $Header: //xaptk/include/UtilityXAP.h#16 $ */
/* UtilityXAP.h */

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
Utility functions for the whole family!
See doc/UtilityXAP.html for details.
*/


#ifndef UTILITYXAP_H
#define UTILITYXAP_H /* as nothing */


#if defined ( WITHIN_PS ) && ( WITHIN_PS == 0 )
/*
This is unfortunate, but necessary in order to make both the Acrobat
Viewer and Distiller happy.
*/
#undef WITHIN_PS
#endif

#ifndef WITHIN_PS
#include "XAPDefs.h"
#include "XAPExcep.h"
#include "XAPPaths.h"
#include "XAPStart.h"
#endif /* WITHIN_PS */


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


class XAPPathTree;
class MetaXAP;

class XAP_API UtilityXAP {
public:


// =================================================================================================
// Property Access Utilities
// =========================

static bool
GetBoolean ( MetaXAP * meta, const std::string & ns, const std::string & path,  bool & val );
    /*^
    Get the boolean value specified by <VAR>ns</VAR> and <VAR>path</VAR>.
    Calls MetaXAP::get. If the property is not defined, returns
    <VAR>false</VAR>. Otherwise, the string value provided by MetaXAP::get is
    converted into a boolean and copied into <VAR>val</VAR> and
    <VAR>true</VAR> is returned.<p>

    Raises all the same exceptions as MetaXAP::get, plus xap_bad_xap if the
    property value cannot be converted to a boolean.
    */

static void
SetBoolean ( MetaXAP * meta, const std::string & ns, const std::string & path, const bool val );
    /*^
    Set the property specified by <VAR>ns</VAR> and <VAR>path</VAR> to
    the specified boolean value. Calls MetaXAP::set. Intermediate nodes
    on the path are created as needed.

    Raises all the same exceptions as MetaXAP::set.
    */

static bool
GetDateTime ( MetaXAP * meta, const std::string & ns, const std::string & path, XAPDateTime & dateTime );
    /*^
    Get property value as a date and time.<P>

    Get the Date value specified by <VAR>ns</VAR> and <VAR>path</VAR>. Calls
    MetaXAP::get. If the property is not defined, returns <VAR>false</VAR>.
    Otherwise, the string value provided by MetaXAP::get is converted into
    values of the XAPDateTime record as described below, and timezone offset
    from GMT, and <VAR>true</VAR> is returned. If <VAR>tzHour</VAR> and
    <VAR>tzMin</VAR> are both 0, the time returned is UTC (GMT). The
    <VAR>seq</VAR> field is always set to 0, and the <VAR>nano</VAR> field is
    set to the subsecond time defined in the value of the property, if
    any.<P>

<PRE>
        XAPDateTime field   Usage
        =================   =====
        sec                 seconds after the minute - [0,59]
        min                 minutes after the hour - [0,59]
        hour                hours since midnight - [0,23]
        mday                day of the month - [1,31]
        month               month of the year - [1,12]
        year                year A.D. (can be negative!)
        tzHour              hours +ahead/-behind UTC - [-12,12]
        tzMin               minutes offset of UTC - [0,59]
        nano                nanoseconds after second (if supported)
        seq                 sequence number (if nano not supported)</PRE>

    <H5>Examples (using HTML format for shorthand)</H5>

    1994-11-05T08:15:30-05:00 corresponds to November 5, 1994,
    8:15:30 am, US Eastern Standard Time.<P>

    1994-11-05T13:15:30Z corresponds to the same instant.<P>

    Raises all the same exceptions as MetaXAP::get, plus xap_bad_xap
    if the property value cannot be converted to a date and time.</P>
    */

static void
SetDateTime ( MetaXAP * meta, const std::string & ns, const std::string & path, const XAPDateTime & dateTime );
    /*^
    Set property value as a date and time.<P>

    Set the property specified by <VAR>ns</VAR> and <VAR>path</VAR>
    to the specified boolean value. Calls MetaXAP::set. Intermediate
    nodes on the path are created as needed.<P>

    See <B>GetDateTime</B> above for the details of usage for
    <VAR>dateTime</VAR>. The <VAR>seq</VAR> and <VAR>nano</VAR> fields
    are ignored.<P>

    Raises all the same exceptions as MetaXAP::set.
    */

static bool
GetInteger ( MetaXAP * meta, const std::string & ns, const std::string & path, long int & val );
    /*^
    Get the integer value specified by <VAR>ns</VAR> and <VAR>path</VAR>.
    Calls MetaXAP::get. If the property is not defined, returns
    <VAR>false</VAR>. Otherwise, the string value provided by MetaXAP::get is
    converted into an integer and copied into <VAR>val</VAR> and
    <VAR>true</VAR> is returned.<p>

    Raises all the same exceptions as MetaXAP::get, plus xap_bad_xap if the
    property value cannot be converted to an integer.
    */

static void
SetInteger ( MetaXAP * meta, const std::string & ns, const std::string & path, const long int val );
    /*^
    Set the property specified by <VAR>ns</VAR> and <VAR>path</VAR> to
    the specified integer value. Calls MetaXAP::set. Intermediate nodes
    on the path are created as needed.

    Raises all the same exceptions as MetaXAP::set.
    */

enum {
	xmpCLT_NoValues			= 0,
	xmpCLT_SpecificMatch	= 1,
	xmpCLT_GenericMatch		= 2,
	xmpCLT_SimilarMatch		= 3,
	xmpCLT_XDefault			= 4,
	xmpCLT_FirstItem		= 5
};

static int
ChooseLocalizedText ( const MetaXAP & meta, const std::string & ns, const std::string & container,
					  const std::string & genericLang, const std::string & specificLang,
					  std::string & actualLang, std::string & value, XAPFeatures & features );
	// Selects an appropriate item in a language alternative container, based on the preferred 
	// language and the rules given below.  The language of the selected item is returned in
	// actualLang.  The function result tells which of the rules was used.  The generic language
	// may be ignored by passing an empty string.
	//
	//	0. The container does not exist or is empty.
	//
	//	1. Look for an exact match with the specific language.
	//
	//	2. Look for an exact match with the generic language.
	//
	//	3. Look for a partial match with the generic language.  This looks through the container in
	//	positional order for a language of the form "<generic>-<suffix>".  For example, "en" would match
	//	"en-us", "en-ca", or "en-cockney", but not "enx-foo".
	//
	//	4. Look for an "x-default" item.
	//
	//	5. Select the first item in the container.
	//
	// NOTE: Most clients should only need GetLocalizedText and SetLocalizedText.  They use these
	// rules to decide what to do.  Use of ChooseLocalizedText is only necessary if you really care
	// exactly what item in the alternative is chosen.

static bool
GetLocalizedText ( const MetaXAP * meta, const std::string & ns, const std::string & container,
				   const std::string & genericLang, const std::string & specificLang,
				   std::string & value, XAPFeatures& features );
	// Get the value and features for an appropriate item in a language alternative container.  Uses
	// the rules defined for ChooseLocalizedText to select the item.  Returns false for rule #0, and
	// true for all other rules.

static void
SetLocalizedText ( MetaXAP * meta, const std::string & ns, const std::string & container,
				   const std::string & genericLang, const std::string & specificLang,
				   const std::string & value, const XAPFeatures features = XAP_FEATURE_DEFAULT );
	// Set the value and features for an appropriate item in a language alternative container.  Uses
	// the rules defined for ChooseLocalizedText to determine a "display" item.  The items that are
	// set depend on which ChooseLocalizedText rule applied.  The term "preferred language" refers to
	// the generic language if provided, otherwise to the specific language.
	//
	//	0. The preferred language item is created.  An "x-default" item is also created if that is
	//	not the preferred language.
	//
	//	1. The specific language item is updated.  An existing "x-default" item is updated if its
	//	value matches the old value of the specific language item.
	//
	//	2. If there are other items with the same generic language root, create a new item for the
	//	given specific language.  Otherwise (there was just the generic item) update the generic
	//	language item; also update an existing x-default item if the old values match.
	//
	//	3. The specific language item is created.
	//
	//	4. The preferred language item is created.  If the container only had the "x-default" item,
	//	that is also updated.
	//
	//	5. The preferred language item is created.
	//
	// In addition, if an "x-default" item exists after the update it will be forced to be the first
	// item in the container.  This improves RDF interoperability, RDF specifies that the first item
	// in an alternative should be the default.

static bool
GetReal ( MetaXAP * meta, const std::string & ns, const std::string & path, double & val );
    /*^
    Get the real (double) value specified by <VAR>ns</VAR> and
    <VAR>path</VAR>. Calls MetaXAP::get. If the property is not defined,
    returns <VAR>false</VAR>. Otherwise, the string value provided by
    MetaXAP::get is converted into a real and copied into <VAR>val</VAR> and
    <VAR>true</VAR> is returned.<p>

    Raises all the same exceptions as MetaXAP::get, plus xap_bad_xap if the
    property value cannot be converted to a real.
    */
	

static void
SetReal ( MetaXAP * meta, const std::string & ns, const std::string & path, const double val );
    /*^
    Set the property specified by <VAR>ns</VAR> and <VAR>path</VAR> to
    the specified real value. Calls MetaXAP::set. Intermediate nodes
    on the path are created as needed.

    Raises all the same exceptions as MetaXAP::set.
    */


// =================================================================================================
// XML Packet Utilities
// ====================

    /** Create an XML Packet for 8-bit encoded XML. */
static void
CreateXMLPacket( const std::string& encoding, const bool inPlaceEditOk,
                 signed long padBytes,
                 const std::string & nl, std::string & header,
                 std::string & trailer, std::string* xml = NULL );
    /*^
    Use this routine to compute the header and trailer string for a packet,
    which you use yourself to create an XML packet, or if you specify
    non-NULL XML data, it will also create the entire packet for you.<P>

    If <VAR>encoding</VAR> is empty (""), it defaults to UTF-8.  If
    <VAR>inPlaceEditOk</VAR> is true, mark the packet as okay to edit
    in-place, otherwise mark the packet as read-only. The <VAR>padBytes</VAR>
    parameter indicates either amount of whitespace padding to add or
    the total length for the packet.  If padBytes is positive, that
    many bytes of whitespace characters are included in the packet.
    The whitespace padding may contain spaces and the given nl string.
    If padBytes is negative, the total packet length will be abs(padBytes)
    with sufficient whitespace padding included.  A xap_bad_number
    exception is thrown if a negative padBytes is too small.
    The <VAR>nl</VAR> string is the character
    sequence to use as a newline between the <VAR>header</VAR> and the
    <VAR>xml</VAR> data if <VAR>xml</VAR> is non-NULL: it can be empty (""),
    or some combination of well-formed XML whitespace. The <VAR>header</VAR>
    is assigned to the string representing the computed header for the packet
    (which does <I>NOT</I> contain <VAR>nl</VAR>), and the <VAR>trailer</VAR>
    is assigned to the string representing the computed trailer of the
    packet. The characters in <VAR>xml</VAR> specify the XML data for the packet. The
    same non-NULL parameter <VAR>xml</VAR> is assigned the complete packet.
    The value of <VAR>encoding</VAR> must match the encoding of the XML data,
    but no checking is done to guarantee this.
    */

#if (WSTRING_SUPPORTED == 1)
    /** Create an XML Packet for 16-bit encoded XML. */
static void
CreateXMLPacket ( const std::wstring & encoding, const bool inPlaceEditOk,
                signed long padBytes,
                const std::wstring & nl, std::wstring & header,
                std::wstring & trailer, std::wstring* xml = NULL );
    /*^
    Same as <B>CreateXMLPacket</B> above, except that all of the string
    parameters are 16-bit character strings.<P>

    <B><I>NOTE:</I></B> This function assumes that the XML data is in
    the native byte order of this machine. It generates text in UCS-2
    encoding, with characters in the range U+0000 to U+007F, plus U+FEFF.
    This refers only to the additional material for the packet wrapper,
    NOT to the data contents, which are assumed to be XML compatible
    UCS-2 and are copied unchanged.
    */
#endif /* (WSTRING_SUPPORTED == 1) */

    /** Serialize to a string. */

static void
SerializeToStr(XAPPathTree* tree, std::string & xml,
               const XAPFormatType f = xap_format_pretty,
               const int escnl = XAP_ESCAPE_CR );
    /*^
    The previous contents of xml are replaced with the XML serialization of
    the specified XAPPathTree. Parameters are the same as for
    XAPPathTree::serialize. Raises all the same exceptions as serialize() for
    the derived class.
    */


// =================================================================================================
// XPath String Utilities
// ======================
	
    /** Remove last step from path, break it into pieces. */
static void
AnalyzeStep ( const std::string & fullPath, std::string & parentPath,
            std::string & lastStep, long int& ord, std::string & selectorName,
            std::string & selectorVal );
    /*^
    From <VAR>fullPath</VAR>, remove the last step and assign it to
    <VAR>lastStep</VAR>, and assign the front part of the path to
    <VAR>parentPath</VAR>. If the last step contains a predicate expression
    with an ordinal (which is always greater than 0), assign it to
    <VAR>ord</VAR>. If the ordinal predicate is the function
    <CODE>last()</CODE>, <VAR>ord</VAR> is set to 0. Otherwise,
    <VAR>ord</VAR> is set to -1. If the predicate is a selector, such as
    &quot;<CODE>*[@xml:lang='fr']</CODE>&quot;, <VAR>selectorName</VAR> would
    be assigned &quot;<CODE>@xml:lang&quot;</CODE> and <VAR>selectorVal</VAR>
    would be assigned &quot;<CODE>fr</CODE>&quot;. Otherwise,
    <VAR>selectorName</VAR> and <VAR>selectorVal</VAR> are assigned the empty
    string.
    */

    /** Filter UI text into valid XPath. */
static void
FilterPropPath ( const std::string & tx, std::string & propPath );
    /*^
    Convert UTF-8 string <VAR>tx</VAR> into a valid XPath, which is also a
    UTF-8 string <VAR>propPath</VAR>. For example, any disallowed characters,
    like spaces or slashes, or any Unicode characters greater than U+007A are
    converted into a series of hexidecimal digits, where every two digits
    represent a byte of UTF-8.  Such sequences are introduced by the
    character pattern "-_" and closed with "_". If the original text contains
    "-_", it is escaped with "-__". If the converted character is the
    initial character, the escape is modified to be "QQ-_".  If such a
    sequence exists in the original text, it is escaped as "QQ-__".<P>

    For example, if <VAR>tx</VAR> is the single Unicode character U+03A3
    GREEK CAPITAL LETTER SIGMA in UTF-8 encoding, it is filtered into
    "<CODE>QQ-_cea3_</CODE>", which represents the two bytes CE and A3 of
    UTF-8, in hex.
    */


// =================================================================================================
// FileInfo UI Utilities
// =====================

// These are utilities designed to help implement the Adobe standard "File Info" UI.

static void
CatenateContainerItems ( const MetaXAP & meta,
						 const std::string & ns, const std::string & container,
						 const std::string & separator, std::string & result );
	// Catenates all of the values from a bag or sequence container into one string using the given
	// separator between each.  The namespace and path must specify an existing bag or sequence
	// container, all of the items in the container must be simple.
	

static void
SeparateContainerItems ( MetaXAP & meta, const std::string & ns, const std::string & container,
						 const XAPStructContainerType cType,
						 const std::string & values, const bool preserveCommas );
	// Separates chunks of the values string into items in a bag or sequence container.  This is
	// more than just the inverse of CatenateContainerItems.  Separation is more general to allow
	// for input from other sources or ingrained typing habits.  The preserveCommas flag tells if
	// commas should be a separator or not.  If true, they are not a separator but are preserved
	// as part of the values.  Other separators are semicolon, tab, carriage return, linefeed, or
	// multiple spaces.  Any sequence of contiguous separators is one separator.  Whitespace at
	// either end of the separated values is removed.  Empty values are ignored.
	//
	// *** NOTE ***
	// This needs to be updated to recognize the broader set of documented separators.

static bool
RemoveProperties ( MetaXAP & meta,
				   const std::string * ns = 0, const std::string * path = 0,
				   const bool doAll = false );
	// Removes external properties from a MetaXAP object.  If the namespace is null, the path is
	// ignored and all external properties in all schema are removed.  If the namespace is not null
	// but the path is null, all external properties in the named schema are removed.  If the
	// namespace and path are both non-null, the named property is removed if it is external.  A
	// schema is removed if all of its properties are removed.
	//
	// The function returns true if all candidate properties are removed.  It returns false if any
	// properties are not removed because they are internal.  This holds even if the namespace or
	// path are null, in those cases the candidates are all properties in the schema.
	//
	// The optional "doAll" parameter causes all properties to be treated as external.

static void
AppendProperties ( const MetaXAP & source, MetaXAP & dest, 
				   const bool replaceOld, const bool doAll = false );
	// Appends external properties from one MetaXAP object to another.  A top level property is
	// copied from the source if it does not exist in the destination, or if replaceOld is true.
	// If the top level name exists and replaceOld is false, the processing depends on the forms
	// of the old and new properties.  If the forms do not match, the destination is left alone.
	// If the forms match and are simple, the destination is left alone.  If the forms match and
	// are a structure, each field is recursively processed like a top level property.
	//
	// Containers are a bit more complex.  If the container types (alt, bag, seq) differ, the
	// destination is left alone.  Otherwise the source is merged into the destination.  For
	// alt-by-lang containers the merge is based on the languages, a source item is copied if the
	// language does not yet exist in the destination.
	//
	// For other container types, the merge is based on the item values.  Each item in the source is
	// checked to see if it is in the destination already.  This compares the values for equality,
	// and the values of the xml:lang attribute if present.  If the source item is not already in the
	// destination, it is appened to the destination container.  If the source item is a structure,
	// the equality check recursively compares each field without regard to order.  The field names
	// and values must match, but they can be in different order.  The destination may contain extra
	// fields, it may be a superset of the source.	If the source item is a container, the equality
	// check recursively compares each item without regard for order.
	//
	// Aliases in the source are ignored, things will be caught with the base properties.
	//
	// The optional "doAll" parameter causes all properties to be treated as external.
	//
	// ! NOTE: There are special cases in the implementation for containers.  An item with an
	// ! xml:lang value of 'x-default' is inserted at the front of an alt container instead of being
	// ! appended.  This preserves RDF semantics of the first item in an alt being the default.
	// ! The existence checks do not care about duplicates.  For example if a source bag has three
	// ! copies "foo" and the destination has one, all three will match and no additional copies
	// ! will be added to the destination.  No attempt is made to be clever about the order of items
	// ! in a sequence.  For example if the first item in a source sequence is the only one missing
	// ! from the destination, it is appended to the end of the destination, not inserted in front.


// =================================================================================================
// Miscellaneous Utilities
// =======================

static void
SetTimeZone ( XAPDateTime & time );
	// Sets just the time zone part of the time to the local offset from UTC.  Useful for determining
	// the local time zone or for converting a "zone-less" time to a proper local time.

static void
MakeUTCTime ( const XAPDateTime & inTime, XAPDateTime & outTime );
	// Sets outTime to the value of inTime converted to UTC.  This depends only on the timezone of
	// inTime, it must be set correctly.  Does not assume inTime is a local time.

static void
MakeLocalTime ( const XAPDateTime & inTime, XAPDateTime & outTime );
	// Sets outTime to the value of inTime converted to a local time instead of UTC.  This depends
	// on the ANSI C functions gmtime, localtime, and mktime.  Which also depend on the host system
	// having a properly set time zone.

static XAPTimeRelOp
CompareTimestamps ( const XAPDateTime & a, const XAPDateTime & b );
	// Compares times a and b,  The timezones of a and b do not have to match.

static XAPTimeRelOp
CompareTimestamps ( MetaXAP * a, MetaXAP * b, const std::string & ns, const std::string & path );
    /*^
    Compare the property with the specified namespace <VAR>ns</VAR>
    and <VAR>path</VAR> in instance <VAR>a</VAR> with instance <VAR>b</VAR>,
    and return the relation as follows:<P>

<PRE>
    a < b   (a timestamp earlier than b)            xap_before
    a == b  (a timestamp same as b)                 xap_at
    a > b   (a timestamp later than b)              xap_after
    a ? b   (a or b does not have a timestamp)      xap_noTime
            (a or b not defined)                    xap_notDef</PRE><P>

    Raises all the same exceptions as MetaXAP::enumerate and
    MetaXAP::getTimestamp, except that xap_no_match is converted into the
    return value xap_notDef.
    */

static bool
IsAltByLang ( const XAPPathTree * tree, const std::string & ns, const std::string & path, std::string * langVal = NULL );
    /*^
    Return true if the specified <VAR>path</VAR> evaluates to a member of a
    structured container that is of type xap_alt, and which is selected by
    the attribute xml:lang. If a pointer to a string is passed in
    <VAR>langVal</VAR>, the string is assigned with the value of the xml:lang
    attribute.<p>

    This function is handy when you are doing an enumerate. If you are
    searching for a particular language alternative, pass the paths returned
    by <B>XAPPaths</B> to this function to test for the sought type, and then
    compare the <VAR>langVal</VAR> with the language you seek.<p>

    Raises all the same exceptions as XAPPathTree::get and MetaXAP::getForm.
    */


// =================================================================================================

};


#if macintosh
	#pragma options align=reset
#endif


#ifndef WITHIN_PS
#include "XAPStart.h"	// *** Why is this here?
#endif /* WITHIN_PS */


#endif /* UTILITYXAP_H */

/*
$Log$
*/

