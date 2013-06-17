
/* $Header: //xaptk/xmldom/XPString.h#3 $ */
/*
 *  DOM String class (XPString)
 *
 *
 *	ADOBE SYSTEMS INCORPORATED
 *	Copyright 2001 Adobe Systems Incorporated
 *	All Rights Reserved
 *
 *	NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
 *	terms of the Adobe license agreement accompanying it.  If you have received this file from a 
 *	source other than Adobe, then your use, modification, or distribution of it requires the prior 
 *	written permission of Adobe.
 *
 *
 *  XPString.h
 *
 *      Interface for creating and manipulating string objects
 */


#ifndef _H_XPSTRING_
#define _H_XPSTRING_


#include "XPStringDefs.h"
#include "XPHashtable.h"


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif



typedef string XPBaseString;


// A ref counted string class which simply wraps an XPBaseString with a counter.
class XPRefCountedString
{
public:
    typedef XPBaseString::size_type size_type;

    // This class should only be used by the XPString class.
    // For this reason, it has been defined to essentially be mostly 'private' with XPString a friend.
    // Might want to try making it a nested class during a future cleanup?
    friend class XPString;

private:
    // Private data members
    XPBaseString m_baseStr;
    long m_count;

    // Private destructor
    ~XPRefCountedString() {}

    // Private constructors
    // default constructor only used when setting up atom array in XPString
    XPRefCountedString() : m_baseStr(), m_count(1)
    {
    }
    XPRefCountedString(const char* cp) : m_baseStr(cp), m_count(1)
    {
    }
    XPRefCountedString(const char* cp, size_type len) : m_count(1)
    {
        if (len == XPBaseString::npos)
            m_baseStr = XPBaseString(cp);
        else
            m_baseStr = XPBaseString(cp, len);
    }

    // Private declaration of copy constructor and assignment operator
    // with no associated definition. Insures they are never used.
    XPRefCountedString(const XPRefCountedString&);
    const XPRefCountedString& operator=(const XPRefCountedString& s);

    // other protected methods
    void AddRef() { m_count++; }
    void Release() { if (--m_count == 0) delete this; }
    const char* c_str() const { return m_baseStr.c_str(); }
    XPBaseString getBaseString() const { return m_baseStr; }
    size_type length() const { return m_baseStr.length(); }
	const char& operator[](size_type pos) const { const char& r = m_baseStr[pos]; return r; }
};





class XMLDOMAPI XPString {
public:
    typedef XPBaseString::size_type size_type;
    static const size_type npos;
    static const char nullchar;
    static const char *whitespaceChars;

    // XPString Constructors
    XPString() : m_str(NULL)
    {
    }
    XPString(const XPString& s, size_type pos, size_type len);
    XPString(const char* cp, size_type len = npos);
    XPString(const XPString& s);

    // special constructors used to initialize array of atoms
    XPString(unsigned long totalNumberOfAtoms, unsigned long index, const char* cp);
    XPString(unsigned long index, const char* cp);

    // conversion constructor XPBaseString -> XPString
    XPString(const XPBaseString& s);

    // conversion operators XPString -> XPBaseString
    operator XPBaseString() const;

    virtual ~XPString();

    const XPString& operator=(const XPString& s);
    const XPString& operator+=(const XPString& s);
    const char& operator[](size_type pos) const;
    const char* c_str() const;
    bool empty() const
    {
        return (m_str == NULL || c_str()[0] == 0);
    }

    size_type length() const { return m_str ? m_str->length() : 0; }
    size_type size() const { return m_str ? m_str->length() : 0; }

    XPString substr(size_type pos = 0, size_type n = npos) const;
    size_type copy(char *dest, size_type n, size_type pos = 0) const;
    XPString& append(size_type n, char c);
    XPString& append(const char* src)
    {
        if (src)
            insert(length(), src);
        return *this;
    }
    XPString& append(const XPString& s)
    {
        append(s.c_str());
        return *this;
    }
    XPString& insert(size_type p0, const char *src);
    XPString& insert(size_type p0, const XPString& s)
    {
        insert(p0, s.c_str());
        return *this;
    }

    size_type find_first_not_of(const XPString& s, size_type pos = 0) const;
    size_type find_first_of(const XPString& s, size_type pos = 0) const;
    size_type find_first_of(char c, size_type pos = 0) const
    {
        char s[2] = {c, 0};
        return find_first_of(s, pos);
    }
    size_type find_last_of(const XPString& s, size_type pos = npos) const;
    size_type find_last_not_of(const XPString& s, size_type pos = npos) const;

    int compare(const XPString& s) const
    {
        return compare(0, length(), s, 0, s.length());
    }
    int compare(size_type p0, size_type n0, const XPString& s) const
    {
        return compare(p0, n0, s, 0, npos);
    }

    int compare(size_type p0, size_type n0, const XPString& s,
                size_type pos, size_type n) const;

    static bool StrsEqual(const XPString& s1, const XPString& s2);
    static XPString Concat(const XPString& s1, const XPString& s2);

    XPString &stripWhitespace(bool leading = true, bool trailing = true);
    XPString &coalesceWhitespace();

    XPString& toLower();

    // ATOM related methods
    static XPRefCountedString* GetXPRefCountedStringPtr(const char* src, size_type len = npos);
    static void initAtomArray(unsigned long arraySize);
    static XPRefCountedString* setAtom(unsigned long index, const char* cp);
    bool IsAtom() const;
    long getStringID() const;

    static void KillXPString() {
        delete[] m_XPRefCountedStringAtomArray;
        m_XPRefCountedStringAtomArray = NULL;
        delete m_atom_lookup;
        m_atom_lookup = NULL;
    }
protected:
    XPRefCountedString* m_str;

    static XPRefCountedString* m_XPRefCountedStringAtomArray;
    static unsigned long m_XPRefCountedStringAtomArraySize;
    static unsigned long m_XPRefCountedStringLastAtom;
    // might want to replace this hash table with a finite state machine
    // at some point
    static XPHashtable* m_atom_lookup;
};


inline bool operator==(const XPString& s1, const XPString& s2)
{
    return XPString::StrsEqual(s1, s2);
}
inline bool operator!=(const XPString& s1, const XPString& s2)
{
    return !XPString::StrsEqual(s1, s2);
}
inline XPString operator+(const XPString&s1, const XPString& s2)
{
    return XPString::Concat(s1,s2);
}


#if macintosh
	#pragma options align=reset
#endif


#endif // _H_XPSTRING_


/*
$Log$
*/

