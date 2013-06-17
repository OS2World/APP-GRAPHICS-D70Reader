/* $Header: //xaptk/xaptk/xaptkfuncs.cpp#12 $ */
/* xapfuncs.cpp */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/

/* 
   tidy.c - HTML parser and pretty printer Copyright (c) 1998-2000 World Wide 
   Web Consortium (Massachusetts Institute of Technology, Institut National 
   de Recherche en Informatique et en Automatique, Keio University). All Rights
   Reserved. Contributing Author(s): Dave Raggett The contributing author(s) 
   would like to thank all those who helped with testing, bug fixes, and patience.
   This wouldn't have been possible without all of you. 
   
   COPYRIGHT NOTICE: This software and documentation is provided "as is," and the 
   copyright holders and contributing author(s) make no representations or 
   warranties, express or implied, including but not limited to, warranties of 
   merchantability or fitness for any particular purpose or that the use of the 
   software or documentation will not infringe any third party patents, 
   copyrights, trademarks or other rights. The copyright holders and contributing
   author(s) will not be liable for any direct, indirect, special or consequential
   damages arising out of any use of the software or documentation, even if advised
   of the possibility of such damage. Permission is hereby granted to use, copy, 
   modify, and distribute this source code, or portions hereof, documentation and
   executables, for any purpose, without fee, subject to the following restrictions
   : 1. The origin of this source code must not be misrepresented. 2. Altered 
   versions must be plainly marked as such and must not be misrepresented as being 
   the original source. 3. This Copyright notice may not be removed or altered from
   any source or altered source distribution. The copyright holders and contributing 
   author(s) specifically permit, without fee, and encourage the use of this source
   code as a component for supporting the Hypertext Markup Language in commercial
   products. If you use this source code in a product, acknowledgment is not
   required but would be appreciated. 
*/

/*
General purpose functions.
*/

#ifndef XAP_FORCE_NORMAL_ALLOC
	#include "XAPTkAlloc.h"
#endif

#include "XAPTkInternals.h"
#include "XAPTkData.h"
#include "XMPInitTerm.h"
#include "ConvCodes.h"
#include "gen_convert_text.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>
//#include <strstream>
#include <sstream>
#include <iomanip>
#include <stdio.h>  // using: sprintf

#ifndef DBL_DIG
#define DBL_DIG 15
#endif

#ifndef SMALLSZ
#define SMALLSZ 200
#endif

#ifndef MINISZ
#define MINISZ 80
#endif

namespace XAPTk {

/* ===== Functions ===== */

bool
CDATAToRaw(const std::string& cdata, std::string& raw) {
    /*
    Return true if filtering was needed and copy filtered string to out.
    */
    std::string::size_type i = cdata.find_first_of("&");
    if (i == std::string::npos)
        return(false);
    const std::string::size_type done = cdata.size();
    raw = cdata.substr(0, i);
    while (i < done) {
        const char c = cdata.at(i);
        std::string::size_type j = std::string::npos;
        if (c == '&')
            j = cdata.find(";", i);
        if (j != std::string::npos) {
            const std::string entity = cdata.substr(i, j-i+1);
            if (entity == "&amp;") {
                raw.append("&");
            } else if (entity == "&lt;") {
                raw.append("<");
            } else if (entity == "&gt;") {
                raw.append(">");
            } else if (entity == "&apos;") {
                raw.append("'");
            } else if (entity == "&quot;") {
                raw.append("\"");
            } else if (entity == "&#xA;") {
                raw.append("\012"); // linefeed
            } else if (entity == "&#xD;") {
                raw.append("\015"); // carriage return
            } else if (entity.size() > 2
            && entity.at(0) == '&' && entity.at(1) == '#') {
                // Convert character entity
                int base = (entity.at(2) == 'x') ? 16 : 10;
                wchar_t wchar = 0;
                for (size_t k = 2; k < entity.size(); ++k) {
                    const char ch = entity.at(k);
                    if (ch == ';')
                        break;
                    else if (ch >= '0' && ch <= '9') {
                        wchar = (wchar * base) + (ch - '0');
                    } else if (ch >= 'a' && ch <= 'f') {
                        wchar = (wchar * base) + (ch - 'a' + 10);
                    } else if (ch >= 'A' && ch <= 'F') {
                        wchar = (wchar * base) + (ch - 'A' + 10);
                    }
                    /* else skip everything else, illegal??? */
                }
                char s[10];
                UCS2_To_UTF8(&wchar, 1, s, 9);
                raw.append(s);
            } else {
                raw.append(entity);
            }
            i = j+1;
        } else {
            raw += c;
            ++i;
        }
    }
    return(true);
}


#ifdef XAP_DEBUG_VALIDATE
size_t
CheckStack(const long* base, const long* high) {
    static const long* myBase = NULL;
    static size_t myPeak = 0;
    size_t lastSize;

    // If high is NULL, just remember base
    if (high == NULL) {
        myBase = base;
        return 0;
    }
    // Skip if base hasn't been set
    if (myBase == NULL)
        return 0;
    // Otherwise, compare and compute
    const size_t ubase = (size_t)myBase;
    const size_t uhigh = (size_t)high;
    lastSize = (ubase > uhigh) ? ubase - uhigh : uhigh - ubase;
    myPeak = (myPeak < lastSize) ? lastSize : myPeak;
#if defined(XAP_DEBUG_CERR) && 0
    std::string num;
    ToString(lastSize, num);
    const size_t jk = lastSize / 1024;
    std::string numK;
    ToString(jk, numK);
    std::cerr << "///////// STACK SIZE: " << numK << "k (" << num << ")" << std::endl;
#endif // XAP_DEBUG_CERR
    return lastSize;
}
#endif // XAP_DEBUG_VALIDATE


void
ContractName ( const std::string& ns, const std::string& localPart, std::string& qName )
{

    XAPTk_StringByString* masterNSMap = MetaXAP_GetMasterNSMap();
    XAPTk_StringByString::const_iterator i = masterNSMap->begin();
    XAPTk_StringByString::const_iterator done = masterNSMap->end();
    
    #if 0
    	cout << "In XAPTk::ContractName for " << localPart << " in " << ns << endl;
    	cout.flush();
    #endif

    for ( ; i != done; ++i ) {
    	#if 0
    		cout << "   Checking master " << i->first << " = " << i->second << endl;
    		cout.flush();
    	#endif
        if ( i->second == ns ) {
            qName = i->first + ":" + localPart;
            #if 0
            	cout << "   returning " << qName << endl;
    			cout.flush();
            #endif
            return;
        }
    }

    #if 0
    	cout << "   returning " << qName << endl;
  		cout.flush();
    #endif
    qName = localPart;

}


    /* Static non-member inline support function. */
static inline XAPTimeRelOp
CompareField ( register long a, register long b ) {
    if ( a < b ) return xap_before;
    if ( a > b ) return xap_after;
    return xap_at;
}

    /* Timestamp support function. */
XAPTimeRelOp
CompareDateTimes ( const XAPDateTime& adt, const XAPDateTime& bdt )
{
    
    if ( adt.year != bdt.year ) return CompareField ( adt.year, bdt.year );
    if ( adt.month != bdt.month ) return CompareField ( adt.month, bdt.month );
    if ( adt.mday != bdt.mday ) return CompareField ( adt.mday, bdt.mday );
    if ( adt.hour != bdt.hour ) return CompareField ( adt.hour, bdt.hour );
    if ( adt.min != bdt.min ) return CompareField ( adt.min, bdt.min );
    if ( adt.sec != bdt.sec ) return CompareField ( adt.sec, bdt.sec );

    // If one timestamp has a sequence number, and the other has
    // a nano, treat the nano as later in time always.
    if ((adt.seq != 0) && (bdt.nano != 0) ) return xap_before;
    if ((adt.nano != 0) && (bdt.seq != 0) ) return xap_after;

    if ( adt.seq != 0 ) {

        return CompareField ( adt.seq, bdt.seq );

    } else {

        // Need to deal with epsilon comparison.

        const long epsilon = 500;
        const long aNano = adt.nano;
        const long bNano = bdt.nano;

        if ( aNano < (bNano - epsilon) ) return xap_before;
        if ( aNano > (bNano + epsilon) ) return xap_after;
        return xap_at;

    }

    return xap_noTime;	// Keep the compiler happy.

}

void
ComposeAltLang(const std::string& contPath, const std::string& lang, std::string& path) {
    path = contPath;
    path.append("/*[@xml:lang='");
    path.append(lang);
    path.append("']");
}

void
ConformPath(const std::string& orig, std::string& fixed, std::string& var) {
    std::string::size_type i;
    std::string::size_type j;
    char c;

    var = "";
    fixed = "";
    if (orig.empty())
        return;

#ifdef A_MIGHTY_BIG_COMMENT_WITH_JUNK_IN_IT

//    There are several functions that require the "fixed" part of a path, which is a series of steps which contain no wildcards or selector predicates.  We assume that paths can only be in one of the following patterns:

  //                              orig                fixed       var
  //      top-level simple:       a                   a
  //      nested description:     a/b                 a/b
  //      container:              x                   x
  //      all members:            x/*                 x           *
  //      container member:       x/*[1]              x           *[1]
  //      container member:       x/*[last()]         x           *[last()]
  //      container member:       x/*[@xml:lang='en'] x           *[@xml:lang='en']
  //      nested container:       c/*[1]/d/*[2]       c           *[1]/d/[*2]
  //      attribute:              y/@foo              y           @foo
  //      selector:               z/@foo='bar'        z           @foo='bar'
  //      selector:               z/kid='text'        z           kid='text'

  // For these patterns, it is safe to assume that given a path that specifies
  //  a container member, the fixed part of the path will be the oldest container.

#endif /* A_MIGHTY_BIG_COMMENT_WITH_JUNK_IN_IT */

    // Top-level simple
    i = orig.find("/");
    if (i == std::string::npos) {
        fixed = orig;
        return;
    }

    // Loop over steps
    var = orig;
    while (true) {
        c = var[0];
        if (c == WILD || c == ATTR) {
            // We're done
            break;
        }
        j = var.find("=");
        if (j != std::string::npos) {
            if (i == std::string::npos || j < i) {
                // We're done
                break;
            }
        }
        // If we get here, the head step of var is a fixed step
        if (!fixed.empty())
            fixed += SLSH;
        if (i == std::string::npos)
            fixed.append(var);
        else
            fixed.append(var.substr(0, i)); // omit final slash
        // Remove head step from var
        if (i != std::string::npos) {
            var = var.substr(i+1, var.size() - i - 1);
            if (var.empty())
                break;
        } else {
            var = "";
            break;
        }
        i = var.find("/");
    }

}

void
DecomposeAltLang(const std::string& path, std::string& contPath, std::string& lang) {
    std::string::size_type at = path.find("/*[@xml:lang");
    if (at == std::string::npos) {
        contPath = "";
        lang = "";
        return;
    }
    contPath = path.substr(0, at);
    at = path.find("=", at);
    at += 2; // skip single or double quote
    std::string::size_type ex = path.find_first_of("'\"", at);
    lang = path.substr(at, ex-at);
}


void
ExplodePath(const std::string& xpath, XAPTk_VectorOfString& expr) {
    const char slash = '/';
    const std::string::size_type npos = std::string::npos;

    std::string::size_type first, last;

    if (xpath == "/") {
        //Special case
        expr.push_back(xpath);
    } else {
        first = last = 0;
        for (last = xpath.find(slash, last); last != npos; last = xpath.find(slash, first)) {
            expr.push_back(xpath.substr(first, last-first));
            first = last+1;
        }
        if (first == 0) {
            expr.push_back(xpath);
        } else {
            expr.push_back(xpath.substr(first));
        }
    }

#ifdef XAP_DEBUG_VALIDATE
    const size_t maxn = expr.size();
    for (size_t i = 0; i < maxn; ++i) {
        std::string s = expr[i];
        if (expr[i] == "") {
            throw xap_bad_path ();
        }
    }
#endif // XAP_DEBUG_VALIDATE
}

void
NotImplemented(const char* what) {
    throw not_implemented ( what );
}

void
RawToCDATA ( const std::string& raw, std::string& cdata, int escnl /* = XAP_ESCAPE_CR */ )
{
    std::string::size_type i = raw.find_first_of ( "&<>'\"\012\015" );

    if ( i == std::string::npos ) {
    	cdata = raw;
        return;
    }

    const std::string::size_type done = raw.size();
    cdata = raw.substr ( 0, i );

    while ( i < done ) {

        std::string::const_reference c = raw.at ( i );

        switch ( c ) {
            case '&':
                cdata.append ( "&amp;" );
                break;
            case '<':
                cdata.append ( "&lt;" );
                break;
            case '>':
                cdata.append ( "&gt;" );
                break;
            case '\'':
                cdata.append ( "&apos;" );
                break;
            case '"':
                cdata.append ( "&quot;" );
                break;
            case CHCR:
                if ( escnl & XAP_ESCAPE_CR ) {
                    cdata.append ( "&#xD;" );
                } else {
                    cdata += c;
                }
                break;
            case CHLF:
                if ( escnl & XAP_ESCAPE_LF ) {
                    cdata.append ( "&#xA;" );
                } else {
                    cdata += c;
                }
                break;
            default:
                cdata += c;
                break;
        }
 
        ++i;

    }

}

void
StripPrefix(const std::string& xmlName, std::string* localPart, std::string* prefix /*= NULL*/) {
    size_t i = xmlName.find(XAPTk::COLN);
    /*
    If colon not found, stick the entire name into localPart.
    */
    if (i == std::string::npos) {
        if (localPart != NULL)
            localPart->assign(xmlName);
        if (prefix != NULL)
            prefix->assign("");
        return;
    }
    if (prefix != NULL) {
        prefix->assign(xmlName.substr(0, i));
    }
    if (localPart != NULL) {
        localPart->assign(xmlName.substr(i+1));
    }
}

void
ToDateTime(const std::string& s, XAPDateTime& dateTime) {
    struct {
        int year;
        int month;
        int mday;
        int hour;
        int min;
        int sec;
        int tzHour;
        int tzMin;
        unsigned long nano;
        unsigned long seq;
    } dt;
    memset(&dt, 0, sizeof(dt));
    const char* buf = s.c_str();
    char tz[MINISZ];
    char pm;
    int tzh = 0;
    int tzm = 0;
    unsigned long nano = 0;
    if (s.find(".") != std::string::npos) {
        char secs_tz[MINISZ];
        double secs = 0.0;
        double frac = 0.0;
        sscanf(buf, "%4d-%02d-%02dT%02d:%02d:%s",
          &dt.year,
          &dt.month,
          &dt.mday,
          &dt.hour,
          &dt.min,
          &secs_tz[0]);
        tz[0] = 'Z'; // In case we fail
        tz[1] = '\0';
        for (char *p = secs_tz; *p != '\0'; ++p) {
            // Look for the beginning of the timezone
            if (*p == '-' || *p == '+' || *p == 'Z') {
                // Copy just the timezone part
                strncpy(tz, p, MINISZ-1);
                // Chop off the timezone part
                *p = '\0';
                // Do fraction first
                for (p = secs_tz; *p != '\0'; ++p)
                    if (*p == '.')
                        break;
                frac = atof(p);
                // Now do seconds
                if (*p != '\0') {
                    ++p; // one past decimal
                    *p = '\0';
                }
                secs = atof(secs_tz);
                break;
            }
        }
        dt.sec = (int) secs;
        nano = (unsigned long)(frac * 1000000000.0);
#ifdef XXX
        double whole;
        double frac = modf(secs, &whole);
        dt.sec = whole;
        // Test for loss of precision
        std::string oldSecs;
        std::string newSecs;
        ToString(secs, oldSecs);
        ToString(frac, newSecs);
        // Just compare fractions
        std::string::iterator sp = oldSecs.begin();
        while (sp != oldSecs.end() && *sp != '.')
            sp = oldSecs.erase(sp);
        std::string::iterator np = newSecs.begin();
        while (np != newSecs.end() && *np != '.')
            np = newSecs.erase(np);
        if (oldSecs != newSecs)
            frac = atof(oldSecs.c_str());
        nano = frac * 1000000000.0;
#endif
    } else {
        sscanf(buf, "%4d-%02d-%02dT%02d:%02d:%02d%s",
          &dt.year,
          &dt.month,
          &dt.mday,
          &dt.hour,
          &dt.min,
          &dt.sec,
          &tz[0]);
    }
    if (tz[0] != 'Z') {
        sscanf(tz, "%c%d:%d", &pm, &tzh, &tzm);
        tzh = (pm == '+') ? tzh : -tzh;
    }
    //GOTCHA: Need some error checking, bounds checking
    dt.tzHour = tzh;
    dt.tzMin = tzm;
    dt.seq = 0L;
    dt.nano = nano;
    dateTime.year = dt.year;
    dateTime.month = dt.month;
    dateTime.mday = dt.mday;
    dateTime.hour = dt.hour;
    dateTime.min = dt.min;
    dateTime.sec = dt.sec;
    dateTime.tzHour = dt.tzHour;
    dateTime.tzMin = dt.tzMin;
    dateTime.nano = dt.nano;
    dateTime.seq = dt.seq;
}

void
ToString(const size_t n, std::string& s) {
    char buf[SMALLSZ];
    buf[0] = '\0';
//    std::ostrstream ostm(buf, SMALLSZ-1);
    std::ostringstream ostm(buf, ostringstream::out);
    ostm << n << std::ends;
    s = buf;
}

void
ToString(const long int n, std::string& s) {
    char buf[SMALLSZ];
    buf[0] = '\0';
//    std::ostrstream ostm(buf, SMALLSZ-1);
    std::ostringstream ostm(buf, ostringstream::out);
    ostm << n << std::ends;
    s = buf;
}

void
ToString(const double n, std::string& s) {
    char buf[SMALLSZ];
    buf[0] = '\0';
//    std::ostrstream ostm(buf, SMALLSZ-1);
    std::ostringstream ostm(buf, ostringstream::out);
    ostm << std::setprecision(DBL_DIG) << n << std::ends;
    s = buf;
}


void
ToString(const XAPDateTime& dateTime, std::string& s) {
    /* Ignore seq */
    char buf[SMALLSZ];
    char secs[SMALLSZ];
    buf[0] = '\0';
    secs[0] = '\0';

    //GOTCHA: Need error checking, bounds checking

    if (dateTime.nano != 0) {
        double ss = dateTime.sec + (dateTime.nano / 1000000000.0);
        double origSS = ss;
        std::string prec;
        ToString(ss, prec);
        /* Don't allow exponential */
        while (prec.find_first_of("eE") != std::string::npos) {
            // Multiply fraction by 10 until we get a round number
            long big = (long int) ss;
            ss -= (long) big;
            ss *= 10.;
            ss += big;
            ToString(ss, prec);
            // Only do this a limited number of times, else give up
            if (ss == 0.0 || ss > 0.5) {
                long whole = (long) origSS; // trunc
                ss = whole + 1.; // round up to next second
                ToString(ss, prec);
                assert(prec.find_first_of("eE") == std::string::npos);
                break;
            }
        }
        if (ss < 10.0) {
            // Leading zero
            secs[0] = '0';
            secs[1] = '\0';
            strncpy(secs+1, prec.c_str(), SMALLSZ);
        } else {
            strncpy(secs, prec.c_str(), SMALLSZ);
        }
        secs[SMALLSZ-1] = '\0';
    } else
        sprintf(secs, "%02d", dateTime.sec);
    if (dateTime.tzHour == 0 && dateTime.tzMin == 0) {
        sprintf(buf, "%0d-%02d-%02dT%02d:%02d:%sZ",
          dateTime.year,
          dateTime.month,
          dateTime.mday,
          dateTime.hour,
          dateTime.min,
          secs);
    } else {
        sprintf(buf, "%0d-%02d-%02dT%02d:%02d:%s%s%02d:%02d",
          dateTime.year,
          dateTime.month,
          dateTime.mday,
          dateTime.hour,
          dateTime.min,
          secs,
          (dateTime.tzHour >= 0) ? "+" : "-",
          abs(dateTime.tzHour),
          dateTime.tzMin);
    }
    s = buf;
}


} // XAPTk


/* ===== Support functions for ConvCodes.h ===== */

typedef unsigned long UCS4;

const UCS4 SURROGATE_HIGH_START     = 0x0D800;
const UCS4 SURROGATE_HIGH_END       = 0x0DBFF;
const UCS4 SURROGATE_LOW_START      = 0x0DC00;
const UCS4 SURROGATE_LOW_END        = 0x0DFFF;

// ----------------------------------------------------------------------------
// Based on tidy GetUTF8.  Decode a single character from a UTF-8 encoded
// string and store it in wc.  Return the number of bytes that were
// read from the string.
//

inline static int
UTF8toUCS4(const char* str, UCS4* ucs4)
{
    unsigned char c;
    UCS4 n;
    int count;
    c = static_cast<unsigned char>(str[0]);
    if ((c & 0xE0) == 0xC0) {
        n = c & 31;
        count = 2;
    } else if ((c & 0xF0) == 0xE0) {
        n = c & 15;
        count = 3;
    } else if ((c & 0xF8) == 0xF0) {
        n = c & 7;
        count = 4;
    } else if ((c & 0xFC) == 0xF8) {
        n = c & 3;
        count = 5;
    } else if ((c & 0xFE) == 0xFC) {
        n = c & 1;
        count = 6;
    } else {
        n = c;
        count = 1;
    }
    for (int i = 1; i < count; i++) {
        c = static_cast<unsigned char>(str[i]);
        n = (n << 6) | (c & (unsigned char)0x3F);
    }
    *ucs4 = n;
    return count;
}

// ----------------------------------------------------------------------------
// Based on tidy AddCharToLexer.  Encode a single unicode character and
// store it in a UTF-8 encoded string.  Return the number of characters
// that were written to the string buffer.
//

#if 0	// unused
inline static int
UCS4toUTF8(const UCS4 ucs4, char* cstr)
{
    int n = 0;
    unsigned char* str = (unsigned char*)(cstr);

    if (ucs4 < (UCS4)128) {
        str[n++] = (0x07F & ucs4);
    } else if (ucs4 <= (UCS4)0x07FF) {
        str[n++] = (UCS4)0x0C0 | (ucs4 >> 6);
        str[n++] = (UCS4)0x080 | (ucs4 & (UCS4)0x03F);
    } else if (ucs4 <= (UCS4)0x0FFFF) {
        str[n++] = (UCS4)0x0E0 | (ucs4 >> 12);
        str[n++] = (UCS4)0x080 | ((ucs4 >> 6) & (UCS4)0x03F);
        str[n++] = (UCS4)0x080 | (ucs4 & (UCS4)0x03F);
    } else if (ucs4 <= (UCS4)0x01FFFFF) {
        str[n++] = (UCS4)0x0F0 | (ucs4 >> 18);
        str[n++] = (UCS4)0x080 | ((ucs4 >> 12) & (UCS4)0x03F);
        str[n++] = (UCS4)0x080 | ((ucs4 >> 6) & (UCS4)0x03F);
        str[n++] = (UCS4)0x080 | (ucs4 & (UCS4)0x03F);
    } else {
        str[n++] = (UCS4)0x0F8 | (ucs4 >> 24);
        str[n++] = (UCS4)0x080 | ((ucs4 >> 18) & (UCS4)0x03F);
        str[n++] = (UCS4)0x080 | ((ucs4 >> 12) & (UCS4)0x03F);
        str[n++] = (UCS4)0x080 | ((ucs4 >> 6) & (UCS4)0x03F);
        str[n++] = (UCS4)0x080 | (ucs4 & (UCS4)0x03F);
    }
    return n;
}
#endif

#ifdef XXX /* keep these for documentation */
// ----------------------------------------------------------------------------

static std::wstring
decode_UTF8(const std::string& s)
{
    std::wstring r;
    int slen = s.length();
    r.reserve(slen);    // First-order approximation to reduce alloc's
    int i = 0;
    while (i < slen) {
        wchar_t wc;
        int n = UTF8towc(&s[i], &wc);
        r += wc;
        i += n;
    }
    return r;
}

// ----------------------------------------------------------------------------

static std::string
encode_UTF8(const std::wstring& s)
{
    std::string r;
    int slen = s.length();
    r.reserve(slen + slen); // First-order approximation to reduce alloc's
    unsigned int i = 0;
    while (i < s.length()) {
        char str[6];
        int n = wctoUTF8(s[i], str);
        r.append(str, n);
        i++;
    }
    return r;
}

#endif


inline static int
isUCS4Boundary(register UCS4 ucs4) {
    register int isCombine;
    /* 7-bit ASCII always a boundary. */
    if (ucs4 < (UCS4)0x7F)
        return(1);
    /* Surrogates never a boundary. */
    if (ucs4 > (UCS4)0x0FFFF && ucs4 <= (UCS4)0x010FFFF)
        return(0);
    /*
    Break before non-spacing mark, hangul leading jamo, hangul vowel jamo,
    and hangul trailing jamo.
    */
    /* Combining Diacritics */
    isCombine = (ucs4 >= (UCS4)0x0300 && ucs4 <= (UCS4)0x036F);
    /* Zero Width No-Break Space */
    isCombine |= (ucs4 == (UCS4)0x0FEFF);
    /* Cyrillic */
    isCombine |= (ucs4 >= (UCS4)0x0483 && ucs4 <= (UCS4)0x0486);
    /* Hebrew Accent */
    isCombine |= (ucs4 >= (UCS4)0x0591 && ucs4 <= (UCS4)0x05BF);
    isCombine |= (ucs4 >= (UCS4)0x05C1 && ucs4 <= (UCS4)0x05C2);
    isCombine |= (ucs4 == (UCS4)0x05C4);
    /* Arabic  */
    isCombine |= (ucs4 >= (UCS4)0x064B && ucs4 <= (UCS4)0x0652);
    isCombine |= (ucs4 == (UCS4)0x0670);
    isCombine |= (ucs4 >= (UCS4)0x06D6 && ucs4 <= (UCS4)0x06E4);
    isCombine |= (ucs4 >= (UCS4)0x06E7 && ucs4 <= (UCS4)0x06E8);
    isCombine |= (ucs4 >= (UCS4)0x06EB && ucs4 <= (UCS4)0x06ED);
    /* Devanagari  */
    isCombine |= (ucs4 >= (UCS4)0x0901 && ucs4 <= (UCS4)0x0902);
    isCombine |= (ucs4 == (UCS4)0x093C);
    isCombine |= (ucs4 >= (UCS4)0x0941 && ucs4 <= (UCS4)0x0948);
    isCombine |= (ucs4 == (UCS4)0x094D);
    isCombine |= (ucs4 >= (UCS4)0x0951 && ucs4 <= (UCS4)0x0954);
    isCombine |= (ucs4 >= (UCS4)0x0962 && ucs4 <= (UCS4)0x0963);
    /* Bengali  */
    isCombine |= (ucs4 == (UCS4)0x0981);
    isCombine |= (ucs4 == (UCS4)0x09BC);
    isCombine |= (ucs4 >= (UCS4)0x09C1 && ucs4 <= (UCS4)0x09C4);
    isCombine |= (ucs4 == (UCS4)0x09CD);
    isCombine |= (ucs4 >= (UCS4)0x09E2 && ucs4 <= (UCS4)0x09E3);
    /* Gurmukhi  */
    isCombine |= (ucs4 == (UCS4)0x0A02);
    isCombine |= (ucs4 == (UCS4)0x0A3C);
    isCombine |= (ucs4 >= (UCS4)0x0A41 && ucs4 <= (UCS4)0x0A42);
    isCombine |= (ucs4 >= (UCS4)0x0A47 && ucs4 <= (UCS4)0x0A48);
    isCombine |= (ucs4 >= (UCS4)0x0A4B && ucs4 <= (UCS4)0x0A4D);
    isCombine |= (ucs4 >= (UCS4)0x0A70 && ucs4 <= (UCS4)0x0A71);
    /* Gujarati  */
    isCombine |= (ucs4 >= (UCS4)0x0A81 && ucs4 <= (UCS4)0x0A82);
    isCombine |= (ucs4 == (UCS4)0x0ABC);
    isCombine |= (ucs4 >= (UCS4)0x0AC1 && ucs4 <= (UCS4)0x0AC5);
    isCombine |= (ucs4 >= (UCS4)0x0AC7 && ucs4 <= (UCS4)0x0AC8);
    isCombine |= (ucs4 == (UCS4)0x0ACD);
    /* Oriya  */
    isCombine |= (ucs4 == (UCS4)0x0B01);
    isCombine |= (ucs4 == (UCS4)0x0B3C);
    isCombine |= (ucs4 == (UCS4)0x0B3F);
    isCombine |= (ucs4 >= (UCS4)0x0B41 && ucs4 <= (UCS4)0x0B43);
    isCombine |= (ucs4 == (UCS4)0x0B4D);
    isCombine |= (ucs4 == (UCS4)0x0B56);
    /* Tamil  */
    isCombine |= (ucs4 == (UCS4)0x0B82);
    isCombine |= (ucs4 == (UCS4)0x0BC0);
    isCombine |= (ucs4 == (UCS4)0x0BCD);
    /* Telugu  */
    isCombine |= (ucs4 >= (UCS4)0x0C3E && ucs4 <= (UCS4)0x0C40);
    isCombine |= (ucs4 >= (UCS4)0x0C46 && ucs4 <= (UCS4)0x0C48);
    isCombine |= (ucs4 >= (UCS4)0x0C4A && ucs4 <= (UCS4)0x0C4D);
    isCombine |= (ucs4 >= (UCS4)0x0C55 && ucs4 <= (UCS4)0x0C56);
    /* Kannada  */
    isCombine |= (ucs4 == (UCS4)0x0CBF);
    isCombine |= (ucs4 == (UCS4)0x0CC6);
    isCombine |= (ucs4 >= (UCS4)0x0CCC && ucs4 <= (UCS4)0x0CCD);
    /* Malayalam  */
    isCombine |= (ucs4 >= (UCS4)0x0D41 && ucs4 <= (UCS4)0x0D43);
    isCombine |= (ucs4 == (UCS4)0x0D4D);
    /* Thai  */
    isCombine |= (ucs4 == (UCS4)0x0E31);
    isCombine |= (ucs4 >= (UCS4)0x0E34 && ucs4 <= (UCS4)0x0E3A);
    isCombine |= (ucs4 >= (UCS4)0x0E47 && ucs4 <= (UCS4)0x0E4E);
    /* Lao  */
    isCombine |= (ucs4 == (UCS4)0x0EB1);
    isCombine |= (ucs4 >= (UCS4)0x0EB4 && ucs4 <= (UCS4)0x0EB9);
    isCombine |= (ucs4 >= (UCS4)0x0EBB && ucs4 <= (UCS4)0x0EBC);
    isCombine |= (ucs4 >= (UCS4)0x0EC8 && ucs4 <= (UCS4)0x0EC9);
    isCombine |= (ucs4 >= (UCS4)0x0ECA && ucs4 <= (UCS4)0x0ECD);
    /* Tibetan  */
    isCombine |= (ucs4 == (UCS4)0x0F37);
    isCombine |= (ucs4 >= (UCS4)0x0F71 && ucs4 <= (UCS4)0x0F7E);
    isCombine |= (ucs4 >= (UCS4)0x0F80 && ucs4 <= (UCS4)0x0F84);
    isCombine |= (ucs4 >= (UCS4)0x0F86 && ucs4 <= (UCS4)0x0F8B);
    isCombine |= (ucs4 >= (UCS4)0x0F90 && ucs4 <= (UCS4)0x0F95);
    isCombine |= (ucs4 == (UCS4)0x0F97);
    isCombine |= (ucs4 >= (UCS4)0x0F99 && ucs4 <= (UCS4)0x0FAD);
    isCombine |= (ucs4 >= (UCS4)0x0FB1 && ucs4 <= (UCS4)0x0FB9);
    /* Hangul Jamo */
    isCombine |= (ucs4 >= (UCS4)0x01100 && ucs4 <= (UCS4)0x011FF);
    /* Combining Diacritical Marks for Symbols */
    isCombine |= (ucs4 >= (UCS4)0x020D0 && ucs4 <= (UCS4)0x020FF);
    /* Ideographic/Hangul Tone Marks */
    isCombine |= (ucs4 >= (UCS4)0x0302A && ucs4 <= (UCS4)0x0302F);
    /* Katakana-Hiragana Sound Marks */
    isCombine |= (ucs4 >= (UCS4)0x03099 && ucs4 <= (UCS4)0x0309A);
    /* Hebrew Point Judeo-Spanish  */
    isCombine |= (ucs4 == (UCS4)0xFB1E);
    /* Combining Half Marks */
    isCombine |= (ucs4 >= (UCS4)0x0FE20 && ucs4 <= (UCS4)0x0FE2F);

    return(!isCombine);
}

#ifdef __cplusplus
extern "C" {
#endif


    /** Truncate UTF-8 string to Unicode char boundary. */
const char*
SafeTruncateUTF8(const char* in, const size_t maxBytes) {
    UCS4 ucs4;
    size_t len;
    size_t n;
    const char* last;
    const char* s;
    const char* fallback;

    if (in == NULL || maxBytes < 1)
        return(in);
    len = strlen(in);
    if (len <= maxBytes)
        return(in+len);
    /*
    We convert to UCS2 by pairs, and check for character boundaries.
    We inch along until we exceed maxBytes, and return the pointer
    to the last valid boundary.
    */
    last = in;
    s = in;
    fallback = in;
    while ((size_t)(s - in) <= maxBytes) {
        n = UTF8toUCS4(s, &ucs4);
        fallback = s;
        s += n;
        if (isUCS4Boundary(ucs4)) {
            last = s;
        }
    }
    /*
    It's better to have malformed text than no text at all.
    */
    if (last == in && fallback != in)
        last = fallback;
    return(last);
}


const wchar_t*
UCS2_To_UTF8 ( const wchar_t* ucs2, const int wlen, char* utf8, const int clen ) {

	// The conversion routines from the Unicode group and our C++ wrappers don't add a
	// trailing null, and don't have a scan-till-null-input mode.  So we need to add
	// that logic here.  Be careful that the output end pointer leaves room for the null!

	typedef std::pair < const wchar_t*, char* >		PtrPair;
	PtrPair	currPtrs ( ucs2, utf8 );
	PtrPair	endPtrs ( (ucs2 + wlen), (utf8 + clen - 1) );
	
	if ( (ucs2 == 0) || (wlen == 0) || (utf8 == 0) || (clen < 2) ) return 0;
	
	if ( wlen == -1 ) {	// Set the end pointer to the first null wchar.
		endPtrs.first = ucs2;
		while ( *endPtrs.first != 0 ) ++endPtrs.first;
	}
	
	currPtrs = adobe::gen::convert_utf ( currPtrs.first, endPtrs.first, currPtrs.second, endPtrs.second );
	
	// The actual conversion function always updates the current pointers to be just beyond
	// the processed range.  Because we limited the length, we can always write the trailing
	// null at the returned output buffer pointer.  This function returns the current input
	// buffer pointer if the input is not finished, NULL otherwise.
	
	*currPtrs.second = 0;	// Add the trailing null to the output buffer.
	if ( currPtrs.first == endPtrs.first ) currPtrs.first = 0;	// Did we process everything?
	
	return currPtrs.first;

}	// UCS2_To_UTF8


const char*
UTF8_To_UCS2 ( const char* utf8, const int clen, wchar_t* ucs2, const int wlen ) {

	// The conversion routines from the Unicode group and our C++ wrappers don't add a
	// trailing null, and don't have a scan-till-null-input mode.  So we need to add
	// that logic here.  Be careful that the output end pointer leaves room for the null!

	typedef std::pair < const char*, wchar_t* >		PtrPair;
	PtrPair	currPtrs ( utf8, ucs2 );
	PtrPair	endPtrs ( (utf8 + clen), (ucs2 + wlen - 1) );
	
	if ( (utf8 == 0) || (clen == 0) || (ucs2 == 0) || (wlen < 2) ) return 0;
	
	if ( clen == -1 ) {	// Set the end pointer to the first null char.
		endPtrs.first = utf8;
		while ( *endPtrs.first != 0 ) ++endPtrs.first;
	}
	
	currPtrs = adobe::gen::convert_utf ( currPtrs.first, endPtrs.first, currPtrs.second, endPtrs.second );
	
	// The actual conversion function always updates the current pointers to be just beyond
	// the processed range.  Because we limited the length, we can always write the trailing
	// null at the returned output buffer pointer.  This function returns the current input
	// buffer pointer if the input is not finished, NULL otherwise.
	
	*currPtrs.second = 0;	// Add the trailing null to the output buffer.
	if ( currPtrs.first == endPtrs.first ) currPtrs.first = 0;	// Did we process everything?
	
	return currPtrs.first;

}	// UTF8_To_UCS2


#ifdef __cplusplus
}
#endif


void
XAPTk_InitFuncs() {
    //No-op
}

void
XAPTk_KillFuncs() {
    //No-op
}

/*
$Log$
*/
