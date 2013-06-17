
/* $Header:$ */
/* ConvCodes.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/


#ifndef CONVCODES_H
#define CONVCODES_H /* as nothing */


/*
Support functions for converting to and from various encodings.
*/

#if defined(WITHIN_PS) && (WITHIN_PS == 0)
/*
This is unfortunate, but necessary in order to make both the Acrobat
Viewer and Distiller happy.
*/
#undef WITHIN_PS
#endif

#ifndef WITHIN_PS
#include "XAPStart.h"
#endif /* WITHIN_PS */


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


#ifdef __cplusplus
extern "C" {
#endif


    /** Truncate UTF-8 string to Unicode char boundary. */
XAP_API const char*
SafeTruncateUTF8(const char* in, const size_t maxBytes);
    /*^
    Truncate the null-terminated UTF-8 input string to be less than or equal
    to the number of bytes specfied by maxBytes. Guarantees that the
    truncation happens at a "safe" Unicode character boundary, by accounting
    for combining characters, surrogates, etc. Returns a pointer to the byte
    in the input string which is ONE BYTE PAST the last valid byte: if "ret"
    is the returned pointer, (ret-in) = number of bytes in the truncated
    string. If ret == in, the string could not be truncated.
    */

    /** Language-Country codes: ISO 639-2 alpha-3 to RFC 1766 ll-cc.  */
XAP_API const char*
To_RFC1766(const char* alpha3);
    /*^
    Convert ISO 639-2 3-letter (alpha-3) language/country code to
    the RFC 1766 ll-cc code (-cc is optional and may not
    be returned by this function.) Returns NULL if not found,
    or if alpha3 is NULL.
    Returned string is const data and should not be altered!

    Example:
    const char* rfc = To_RFC1766("enu");
    // rfc = "en-us"
    rfc = To_RFC1766("eng");
    // rfc = "en"
    */

    /** Language-Country codes: RFC 1766 ll-cc to ISO 639-2 alpha-3. */
XAP_API const char*
To_ISO639_2(const char* ll_cc);
    /*^
    Convert RFC 1766 ll-cc code to ISO 639-2 alpha-3 code.
    The country subtag (-cc) is optional.
    Returns NULL if not found or if ll_cc is NULL.
    Returned string is const data and should not be altered!

    Example:
    const char* iso = To_ISO639_2("en-us");
    // iso = "enu"
    iso = To_ISO639_2("en");
    // iso = "eng"
    */


    /** Unicode: convert from 2-byte UCS-2 to multi-byte UTF-8. */
XAP_API const wchar_t*
UCS2_To_UTF8(const wchar_t* ucs2, const int wlen, char* utf8, const int
clen);
    /*^
    If utf8 or ucs2 is NULL, returns without doing anything.
    If wlen = -1, assumes ucs2 is a null-terminated string,
    otherwise, only processes wchar_t's from ucs2[0] to ucs2[wlen-1].
    If wlen = 0, returns without doing anything (returns NULL).
    If clen < 2, returns without doing anything (returns NULL).
    Converts wchar_t's in ucs2 into chars in utf8, up to clen char's.
    If the count of chars's copied into utf8 plus 1 for the null
    terminator are <= clen, NULL is returned, otherwise fills utf8
    to the limit and returns a
    pointer to the next wchar_t in ucs2 to be processed.  This function
    can be called repeatedly to complete the conversion, passing the
    returned pointer in every loop:

    char cbuf[8];
    wchar_t* more = L"This UCS-2 will convert to longer than 8 char's";

    while ((more = UCS2_To_UTF8(more, -1, cbuf, 8))) {
        // Copy cbuf to some other buffer, then ...
    }
    */

    /** Unicode: convert from multi-byte UTF-8 to 2-byte UCS-2. */
XAP_API const char*
UTF8_To_UCS2(const char* utf8, const int clen, wchar_t* ucs2, const int
wlen);
    /*^
    If utf8 or ucs2 is NULL, returns without doing anything (detect
    by noting that returned pointer is equal to utf8 parameter).
    If clen = -1, assumes utf8 is a null-terminated string,
    otherwise, only processes bytes from utf8[0] to utf8[clen-1].
    If clen = 0, returns without doing anything (returns utf8).
    If wlen < 2, returns without doing anything (returns utf8).
    The wlen must be 3 or greater in order to process surrogate character
    pairs. Converts chars in utf8 into wchar_t in ucs2, up to wlen wchar_t's.
    If the count of wchar_t's copied into ucs2 plus 1 for the null terminator
    are <= wlen, NULL is returned, otherwise fills ucs2 to the limit and
    returns a pointer to the next char in utf8 to be processed.  This
    function can be called repeatedly to complete the conversion, passing the
    returned pointer in every loop:

    wchar_t wbuf[8];
    char* more = "This UTF-8 will convert to longer than 8 wchar_t's";

    while ((more = UTF8_To_UCS2(more, -1, wbuf, 8))) {
        // Copy wbuf to some other buffer, then ...
    }
    */


#ifdef __cplusplus
}
#endif


#if macintosh
	#pragma options align=reset
#endif


#endif /* CONVCODES_H */

/*
$Log$
*/

