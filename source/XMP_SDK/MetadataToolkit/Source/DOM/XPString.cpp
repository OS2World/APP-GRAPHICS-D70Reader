/* $Header: //xaptk/xmldom/XPString.cpp#3 $ */
/*
 *  XPString.cpp
 *
 *
 *	ADOBE SYSTEMS INCORPORATED
 *	Copyright 2001 Adobe Systems Incorporated
 *	All Rights Reserved
 *
 *	NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
 *	terms of the Adobe license agreement accompanying it.
 *
 *
 *  XPString.cpp
 *
 *      C++ XPString class
 */

#include "XMPAssert.h"
#include "XAP_XPDOMConf.h"
#include "XPString.h"
#include "XMPInitTerm.h"

const XPBaseString::size_type XPString::npos = (size_t)(-1);
const char XPString::nullchar = 0;
const char *XPString::whitespaceChars = " \r\n\t";
XPRefCountedString* XPString::m_XPRefCountedStringAtomArray;
unsigned long XPString::m_XPRefCountedStringAtomArraySize;
unsigned long XPString::m_XPRefCountedStringLastAtom;
XPHashtable* XPString::m_atom_lookup;

// Destructor
XPString::~XPString()
{
    if ( m_str ) m_str->Release();
}

// Construct an XPString which is a substring of a given XPString s
// of length len starting at position pos.
XPString::XPString(const XPString& s, size_type pos, size_type len)
{
    if (s.m_str == NULL)
    {
        m_str = NULL;
    }
    else
    {
        size_type s_len = s.length();
        if (pos == 0 && len >= s_len)
        {
            m_str = s.m_str;
            m_str->AddRef();
        }
        else if (pos >= s_len)
        {
            assert(0);
            m_str = NULL;
        }
        else
            m_str = GetXPRefCountedStringPtr(s.c_str() + pos, len);
    }
}

// len defaults to npos, however this will cause problems if s is not null terminated
XPString::XPString(const char* cp, size_type len) :
    m_str(GetXPRefCountedStringPtr(cp, len))
{
    assert(m_str);
}

// copy constructor
XPString::XPString(const XPString& s) : m_str(s.m_str)
{
    if (m_str)
        m_str->AddRef();
}

// CURRENTLY NEEDED ONLY FOR THE MAC I THINK
// SHOULD PROBABLY STOP USING string ON THE MAC IF POSSIBLE
// conversion from XPBaseString to XPString
XPString::XPString(const XPBaseString& s)
{
    m_str = GetXPRefCountedStringPtr(s.c_str());
}

// two conversion operators from XPString to XPBaseString
XPString::operator XPBaseString() const
{
    if (m_str)
        return m_str->getBaseString();
    else
        return "";
}


// operator[]
const char& XPString::operator[](size_type pos) const
{
    if (m_str && pos < length())
        return (*m_str)[pos];
    return nullchar;
}

// It greatly simplifies lift for users of c_str to know that
// it will never return NULL. It always points to a non-NULL,
// (though possibly empty) c string
//
// Is it maybe the case that STL already specifies this behavior so
// that we don't have to worry about it?
const char* XPString::c_str() const
{
    const char* s = NULL;
    if (m_str)
    {
        s = m_str->c_str();
        assert(s != NULL); // I'm curious to know if we ever get NULL back from the STL string -- bkaskel
    }

    assert(s == NULL || s[length()] == 0); // assert that we always get back a null terminated string
    return s ? s : &nullchar; // NEVER return NULL
}

// assignment operator
const XPString& XPString::operator=(const XPString& s)
{
    if (&s != this)
    {
        if (m_str)
            m_str->Release();
        m_str = s.m_str;
        if (m_str)
            m_str->AddRef();
    }

    return *this;
}

const XPString& XPString::operator+=(const XPString& s2)
{
    if (m_str == NULL)
    {
        m_str = s2.m_str;
        if (m_str)
            m_str->AddRef();
    }
    else if (s2.m_str != NULL)
    {
        // non-trivial concat
        XPBaseString tmp = m_str->getBaseString() + s2.m_str->getBaseString();
        m_str->Release();
        m_str = GetXPRefCountedStringPtr(tmp.c_str());
    }
    return *this;
}

XPString XPString::substr(size_type pos, size_type n) const
{
    if (m_str)
    {
        size_type len = length();
        if (pos >= len)
            return ""; // empty string

        if (n >= len - pos)
        {
            // substring includes end of this string
            if (pos)
                return XPString(c_str() + pos);
            // else falls down to 'return *this since pos == 0 && n >= length()
            // this takes advantage of reference counting
        }
        else
        {
            return XPString(c_str() + pos, n);
        }
    }

    return *this;
}

XPString::size_type XPString::copy(char *dest, size_type n, size_type pos) const
{
    assert(dest);
    size_type i = 0;
    if (m_str && dest)
    {
        size_type len = length();
        if (pos > len)
            pos = len;
        const char* src = c_str();
        src += pos;
        for (; i < n && *src; i++)
            *dest++ = *src++;
        // NOTE: copy() doesn't add a null terminator to the string
    }
    return i;
}

XPString& XPString::append(size_type n, char c)
{
    if (n > 0)
    {
        XPBaseString nc(n, c);
        if (m_str)
        {
            XPBaseString tmp = m_str->getBaseString() + nc;
            m_str->Release();
            m_str = GetXPRefCountedStringPtr(tmp.c_str());
        }
        else
        {
            m_str = GetXPRefCountedStringPtr(nc.c_str());
        }
    }
    return *this;
}

XPString& XPString::insert(size_type p0, const char *src)
{
    assert(src);
    if (src)
    {
        if (m_str)
        {
            XPBaseString tmp(m_str->getBaseString());
            tmp.insert(p0, src);
            m_str->Release();
            m_str = GetXPRefCountedStringPtr(tmp.c_str());
        }
        else
        {
            assert(p0 == 0);
            m_str = GetXPRefCountedStringPtr(src);
        }
    }
    return *this;
}

XPString::size_type XPString::find_first_not_of(const XPString& s, size_type pos) const
{
    if (empty() || pos >= length())
        return npos;
    if (s.empty())
        return pos; // any char is by default not in an empty string
    return m_str->getBaseString().find_first_not_of(s, pos); // conversion s -> base string happens automatically here
}

XPString::size_type XPString::find_first_of(const XPString& s, size_type pos) const
{
    if (empty() || pos >= length() || s.empty())
        return npos;
    return m_str->getBaseString().find_first_of(s, pos); // conversion s -> base string happens automatically here
}

XPString::size_type XPString::find_last_of(const XPString& s, size_type pos) const
{
    if (empty() || s.empty())
        return npos;
    return m_str->getBaseString().find_last_of(s, pos); // conversion s -> base string happens automatically here
}

XPString::size_type XPString::find_last_not_of(const XPString& s, size_type pos) const
{
    if (empty() || s.empty())
        return npos;
    return m_str->getBaseString().find_last_not_of(s, pos); // conversion s -> base string happens automatically here
}

int XPString::compare(size_type p0, size_type n0, const XPString& s,
                      size_type pos, size_type n) const
{
    if (p0 >= length() || pos >= s.length())
        return -1; // XXX STL string raises
    if (empty())
    {
        return (s.empty()) ? 0 : -1;
    }
    if (s.empty())
        return 1;

//#ifdef UNIX_ENV
#ifdef JON
    /*
    5 parameter compare not available in GCC basic_string, so do it
    the hard way.
    */
    const XPString tmp2 = s.substr(pos, n);
    return ( m_str->getBaseString().compare(tmp2, p0, n0) );
#else
    return m_str->getBaseString().compare(p0, n0, s, pos, n);
#endif

}

// typically, we expect to be comparing strings where at least one
// of them is an 'atom' with stringID >= 0, so the first 2 checks
// will usually suffice.
bool XPString::StrsEqual(const XPString& s1, const XPString& s2)
{
    if (s1.m_str == s2.m_str)
        return true;

    if (!s1.m_str)
        return(s2.m_str->length() == 0);

    if (!s2.m_str)
        return(s1.m_str->length() == 0);

    // Both strings have non-null, non-equal pointers.
    // Thus, if either is an 'atom', they must be different
    if (s2.IsAtom() || s1.IsAtom()) // IsAtom doesn't perform the costly DIV that getStringID does
        return false;

    // Neither string is an atom
    return (s1.m_str->getBaseString() == s2.m_str->getBaseString());
}

XPString XPString::Concat(const XPString& s1, const XPString& s2)
{
    XPString temp(s1);
    temp += s2;
    return temp;
}

// Strip off the whitespace (as defined in common) from <leading> and/or
// <trailing> the string
XPString &XPString::stripWhitespace(bool leading, bool trailing)
{
    if (! m_str)
        return(*this);

    // strip leading whitespace
    size_t          start = 0;
    XPBaseString    str = m_str->getBaseString();

    if (leading)
    {
        start = str.find_first_not_of(whitespaceChars);

        if (start == XPString::npos)
            start = 0;
    }

    // strip trailing whitepsace
    size_t  finish = str.size() - 1;

    if ((trailing) &&
        (str.size() > 0))
    {
        finish = str.find_last_not_of(whitespaceChars);

        if (finish == XPString::npos)
            finish = str.size() - 1;
    }

    XPString    newString = str.substr(start, (finish - start) + 1);

    m_str->Release();
    m_str = GetXPRefCountedStringPtr(newString.c_str());

    return(*this);
}

// Coalesce runs of whitespace (e.g., tab, space, return) into a single
// space character
XPString &XPString::coalesceWhitespace()
{
    if (! m_str)
        return(*this);

    XPBaseString    str = m_str->getBaseString();
    XPString        newString;
    size_t          pos = 0, last = str.size() - 1;
    size_t          runStart = 0, runEnd = 0;

    while (pos < last)
    {
        runStart = str.find_first_of(whitespaceChars, pos);

        // If there are no more space chars, copy what remains and return
        if (runStart == XPString::npos)
        {
            newString.append(str.substr(pos, (last - pos) + 1));
            break;
        }

        // Find where this ends
        runEnd = str.find_first_not_of(whitespaceChars, runStart + 1);

        // If there weren't any found, pretend that it was at the end
        if (runEnd == XPString::npos)
            runEnd = last;

        // Copy the chars from <pos> to <runStart>, add a space (if not at end)
        // and look for the next run
        newString.append(str.substr(pos, (runStart - pos)));
        newString.append(" ");
        pos = runEnd;
    }

    // Special case for one-char strings
    if ((pos == 0) &&
        (last == 0))
        newString.append(str);

    m_str->Release();
    m_str = GetXPRefCountedStringPtr(newString.c_str());

    return(*this);
}

XPString& XPString::toLower()
{
    if (m_str)
    {
        XPBaseString tmp = m_str->getBaseString();
        size_type len = tmp.length();
        for (size_type i = 0; i < len; i++)
        {
            if ( 'A' <= tmp[i] && tmp[i] <= 'Z' )
                tmp[i] += 0x20;
        }

        m_str->Release();
        m_str = GetXPRefCountedStringPtr(tmp.c_str());
    }
    return *this;
}

// ATOM related XPString methods
/////////////////////////////////////////////////////////////////

// special constructors: first initializes array in addition to the string into the array
//                       second initializes string into atom array
// NOTE: cp MUST point to a permanently maintainted (i.e., static) string in memory
XPString::XPString(unsigned long totalNumberOfAtoms, unsigned long index, const char* cp)
{
    //assert(m_XPRefCountedStringAtomArraySize == 0);    // was firing on the Mac side on reload.
    //assert(m_XPRefCountedStringLastAtom == 0);         // was firing on the Mac side on reload.
    //assert(m_XPRefCountedStringAtomArray == NULL);     // was firing on the Mac side on reload.
    initAtomArray(totalNumberOfAtoms);	// *** initAtomArray is a class static function, but this is a constructor! 
    assert(index < m_XPRefCountedStringAtomArraySize);
    m_str = setAtom(index, cp);
    assert(m_str);
    if (m_str)
        m_str->AddRef();
    assert(IsAtom());
}
XPString::XPString(unsigned long index, const char* cp)
{
    assert(m_XPRefCountedStringAtomArraySize != 0);
    assert(m_XPRefCountedStringLastAtom != 0);
    assert(index < m_XPRefCountedStringAtomArraySize);
    m_str = setAtom(index, cp);
    assert(m_str >= m_XPRefCountedStringAtomArray && m_str < m_XPRefCountedStringAtomArray + m_XPRefCountedStringAtomArraySize);
    assert(m_str);
    if (m_str)
        m_str->AddRef();
    assert(IsAtom());
}

// this routine takes care of the AddRef, so the caller should not AddRef
XPRefCountedString*
XPString::GetXPRefCountedStringPtr(const char* src, size_type len)
{
    assert(nullchar == 0);
    if (src == NULL)
    {
        src = &nullchar;
        len = npos;
    }

    XPRefCountedString* refCountedStringPtr;

    // check to see if src matches an atom
    assert(m_atom_lookup);
    void* p = m_atom_lookup->get(src, len);

    if (p != NULL)
    {
        refCountedStringPtr = reinterpret_cast<XPRefCountedString*>(p);
        refCountedStringPtr->AddRef();
    }
    else
    {
        refCountedStringPtr = new XMP_Debug_new XPRefCountedString(src, len);
    }

    return refCountedStringPtr;
}

void XPString::initAtomArray ( unsigned long arraySize )
{
    assert ( arraySize > 0 );

	// This gets called redundantly, at least from XAPTk_InitDOM.
	
    if ( m_atom_lookup == 0 ) {
	    m_atom_lookup = new XMP_Debug_new XPHashtable ( 1024, false );
	    assert ( m_atom_lookup != 0 );
    }

    if ( m_XPRefCountedStringAtomArray == 0 ) {
	    m_XPRefCountedStringAtomArray = new XMP_Debug_new XPRefCountedString [ arraySize ];
	    assert ( m_XPRefCountedStringAtomArray != 0 );
        m_XPRefCountedStringAtomArraySize = arraySize;
        m_XPRefCountedStringLastAtom = (arraySize - 1) * sizeof(XPRefCountedString);
    }
}

XPRefCountedString* XPString::setAtom ( unsigned long index, const char* cp )
{
    assert ( m_XPRefCountedStringAtomArray != NULL );
    assert ( m_XPRefCountedStringAtomArraySize > 0 );

    // We generally don't use XPBaseString except in XPRefCountedString.
    // This is an exception. The alternative was to define an assignment operator
    // to XPRefCountedString which would be dangerous if used elsewhere.
 
    XPBaseString baseString ( cp );
    m_XPRefCountedStringAtomArray[index].m_baseStr = baseString;
    assert ( m_XPRefCountedStringAtomArray[index].m_count == 1 );

    void* old;	// Split to avoid UNIX warning about unused variable.
    old = m_atom_lookup->put ( cp, (void*)&m_XPRefCountedStringAtomArray[index] );
    assert ( old == NULL );

    return &m_XPRefCountedStringAtomArray[index];
}

bool XPString::IsAtom() const
{
    unsigned long i =
        ((unsigned long)m_str) - ((unsigned long)m_XPRefCountedStringAtomArray);
    return (i <= m_XPRefCountedStringLastAtom);
}

long XPString::getStringID() const
{
    // should be ok even if m_str == NULL
    unsigned long i =
        ((unsigned long)m_str) - ((unsigned long)m_XPRefCountedStringAtomArray);

    if (i > m_XPRefCountedStringLastAtom)
        return -1;

    // on windows currently (3/99), sizeof(XPRefCountedString) == 20
    // since the STL string takes 16 bytes and the unsigned long takes another 4.
    // unfortunately, this DIV could be costly
    return long(i / sizeof(XPRefCountedString));
}


/*
$Log$
*/
