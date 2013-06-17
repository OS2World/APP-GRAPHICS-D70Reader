/*************************************************************************************************/

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/

/*************************************************************************************************/

#include "gen_convert_text.h"
#include "ConvertUTF.h"

#include <string>
#include <vector>

/*************************************************************************************************/

namespace adobe {
namespace gen {

/*************************************************************************************************/

std::pair < const char*, wchar_t* >
convert_utf ( const char* sourceBegin, const char* sourceEnd,
              wchar_t* targetBegin, wchar_t* targetEnd, bool lenient )
            throw ( illegal_utf_sequence )
{

    std::pair < const char*, wchar_t* > result ( sourceBegin, targetBegin );
    ConversionResult status;
    
    if ( sourceBegin == sourceEnd ) return result;  /* Nothing to convert. */
    if ( targetBegin == targetEnd ) throw illegal_utf_sequence ( "Empty target buffer." );

    if ( sizeof ( wchar_t ) == sizeof ( UTF16 ) ) {

        status = ConvertUTF8toUTF16 ( reinterpret_cast<const UTF8**> ( &result.first ),
                                      reinterpret_cast<const UTF8*> ( sourceEnd ),
                                      reinterpret_cast<UTF16**> ( &result.second ),
                                      reinterpret_cast<UTF16*> ( targetEnd ),
                                      (lenient ? lenientConversion : strictConversion) );

    } else {

        /* Gack!! The sizes of wchar_t and UTF16 don't match! */
        /* Convert into a local buffer and copy the result.   */

        const int targetLen = targetEnd - targetBegin;  /* Pointer subtraction, gives item count. */
        UTF16*    ubuf      = new UTF16 [targetLen];
        UTF16*    ubufPtr   = &ubuf[0];

        status = ConvertUTF8toUTF16 ( reinterpret_cast<const UTF8**> ( &result.first ),
                                      reinterpret_cast<const UTF8*> ( sourceEnd ),
                                      &ubufPtr, (ubufPtr + targetLen),
                                      (lenient ? lenientConversion : strictConversion) );

        const int convertLen = ubufPtr - &ubuf[0];
        for ( int i = 0; i < convertLen; ++i ) targetBegin[i] = ubuf[i];
        result.second = targetBegin + convertLen;
        delete[] ubuf;

    }

    if ( status == sourceIllegal ) throw illegal_utf_sequence ( "Bad UTF-8 sequence." );
    return result;

}
    
/*************************************************************************************************/

std::pair < const wchar_t*, char* >
convert_utf ( const wchar_t* sourceBegin, const wchar_t* sourceEnd,
              char* targetBegin, char* targetEnd, bool lenient )
            throw ( illegal_utf_sequence )
{
    std::pair<const wchar_t*, char*> result ( sourceBegin, targetBegin );
    ConversionResult status;
    
    if ( sourceBegin == sourceEnd ) return result;  /* Nothing to convert. */
    if ( targetBegin == targetEnd ) throw illegal_utf_sequence ( "Empty target buffer." );

    if ( sizeof ( wchar_t ) == sizeof ( UTF16 ) ) {

        status = ConvertUTF16toUTF8 ( reinterpret_cast<const UTF16**> ( &result.first ),
                                      reinterpret_cast<const UTF16*> ( sourceEnd ),
                                      reinterpret_cast<UTF8**> ( &result.second ),
                                      reinterpret_cast<UTF8*> ( targetEnd ),
                                      (lenient ? lenientConversion : strictConversion) );

    } else {

        /* Gack!! The sizes of wchar_t and UTF16 don't match!     */
        /* Repack into a local buffer and fix the source pointer. */

        const int    sourceLen = sourceEnd - sourceBegin;  /* Pointer subtraction, gives item count. */
        UTF16*       ubuf      = new UTF16 [sourceLen];
        const UTF16* ubufPtr   = &ubuf[0];

        for ( int i = 0; i < sourceLen; ++i ) ubuf[i] = sourceBegin[i];

        status = ConvertUTF16toUTF8 ( &ubufPtr, (ubufPtr + sourceLen),
                                      reinterpret_cast<UTF8**> ( &result.second ),
                                      reinterpret_cast<UTF8*> ( targetEnd ),
                                      (lenient ? lenientConversion : strictConversion) );

        const int convertLen = ubufPtr - &ubuf[0];
        result.first = sourceBegin + convertLen;
        delete[] ubuf;

    }

    if ( status == sourceIllegal ) throw illegal_utf_sequence ( "Bad UTF-16 sequence." );
    return result;

}
    
/*************************************************************************************************/

template<class Result, class Source>
Result convert_utf ( const Source& source, bool lenient )
{
    typedef    typename Result::value_type    result_char;
    typedef    typename Source::value_type    source_char;
    
    Result        result;
    result_char    buffer[256];
    
    const source_char* sourceBegin = source.c_str();
    const source_char* sourceEnd = sourceBegin + source.length();
    
    while ( sourceBegin != sourceEnd ) {
        std::pair<const source_char*, result_char*> intermediate ( convert_utf ( sourceBegin, sourceEnd,
                                                                                 &buffer[0], &buffer[256], lenient ) );
        result.append ( &buffer[0], intermediate.second );
        sourceBegin = intermediate.first;
    }
        
    return result;
}

/*************************************************************************************************/

// Note: Some GCC compilers fail to handle these instantiations.

#ifndef WSTRING_SUPPORTED
    #define WSTRING_SUPPORTED    1
#endif

#if WSTRING_SUPPORTED
    template std::string convert_utf <std::string, std::wstring> ( const std::wstring& source, bool lenient );
    template std::wstring convert_utf <std::wstring, std::string> ( const std::string& source, bool lenient );
#endif

/*************************************************************************************************/

} // namespace adobe
} // namespace gen

/*************************************************************************************************/
