/* $Header: //xaptk/xmldom/XPHashtable.cpp#2 $ */
/*
 *  Expat hash table - based on hashtable.c
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
 *  The Initial Developer of the Original Code is James Clark.
 *  Portions created by James Clark are Copyright (C) 1998
 *  James Clark. All Rights Reserved.
 *
 *  XPHashtable.cpp
 *
 *      Implementation of C++ wrapper to Expat hash table.
 */

#include "XAP_XPDOMConf.h"
#include "XPHashtable.h"
#include "XPString.h"
#include "XMPAssert.h"

const int INIT_SIZE = 64;

XPHashtable::XPHashtable(size_t initSize, bool copyStrings)
{
    #define IsAPowerOfTwo(x) ( !( (x) & ((x)-1) ) )
    assert(IsAPowerOfTwo(initSize));

    m_initSize = (initSize > 0) ? initSize : INIT_SIZE;
    m_size = 0;
    m_usedLim = 0;
    m_used = 0;
    m_v = 0;
    m_usesCopiedStrings = copyStrings;
}

// Strings were duplicated, values not, so don't delete everything.
XPHashtable::~XPHashtable()
{
    if (m_usesCopiedStrings)
    {
        for (size_t  i = 0; i < m_size; i++)
        {
            delete [] (char*)(m_v[2*i]);	// Cast to match allocation.
        }
    }
    delete [] m_v;
}

unsigned long XPHashtable::hash(const char *s, unsigned long len)
{
    assert(s);
    unsigned long h = 0;
    while (len-- && *s)
        h = (h << 5) + h + (unsigned long)*s++;
    return h;
}

// Returned index is either index of found key or where key should go.
bool XPHashtable::lookup(const char* key, size_t *index, unsigned long len) const
{
    assert(key);
    bool foundKey = false;
    unsigned long h = hash(key, len);
    if (m_size == 0)
    {
        *index = h & (m_initSize - 1);
        return false;
    }
    size_t i = h & (m_size - 1);
    while (m_v[2*i] != NULL)
    {
        // strcmp
        const char *c1 = (const char *) m_v[2*i];
        const char *c2 = key;
        unsigned long tmp_len = len;
        if (tmp_len != (unsigned long)(-1L))
            tmp_len++;
        for(; tmp_len; tmp_len--)
        {
            if (*c1 == 0)
            {
                tmp_len = 0;
                break;
            }
            if (*c1 != *c2)
            {
                if (*c1 == 0 && tmp_len == 1)
                    tmp_len = 0;
                break;
            }
            c1++;
            c2++;
        }

        if (tmp_len == 0) // string match?
        {
            foundKey = true;
            break;
        }
        if (i == 0)
            i = m_size - 1;
        else
            --i;
    }
    *index = i;
    return foundKey;
}

void *XPHashtable::get(const char* key, unsigned long len) const
{
    assert(key); // not sure if this can happen
    if (key == NULL)
        return NULL;

    size_t i;
    return (lookup(key, &i, len)) ? m_v[2*i + 1] : NULL;
}

void *XPHashtable::get(const XPString& key) const
{
    return get(key.c_str());
}

void *XPHashtable::put(const char* key, void *value)
{
    assert(key); // not sure if this can happen
    if (key == NULL)
        return NULL;

    bool found = false;
    size_t i;
    if (m_size == 0)
    {
        m_v = new XMP_Debug_new void *[2*m_initSize];
        if (!m_v)
            return NULL;
        memset(m_v, '\0', (2 * m_initSize) * sizeof(m_v[0]));
        for (i = 0; i < 2 * m_initSize; i++)
            m_v[i] = NULL;
        m_size = m_initSize;
        m_usedLim = m_initSize / 2;
        i = hash(key) & (m_size - 1);
    }
    else
    {
        found = lookup(key, &i);
        if (!found && m_used == m_usedLim)
        {
            /* check for overflow */
            size_t newSize = m_size * 2;
            void **newV = new XMP_Debug_new void *[2*newSize];
            if (!newV)
                return NULL;
            memset(newV, '\0', (2 * newSize) * sizeof(newV[0]));
            for (i = 0; i < m_size; i++)
            {
                if (m_v[2*i] != NULL)
                {
                    size_t j;
                    j = hash((const char *) m_v[2*i]) & (newSize - 1);
                    while(newV[2*j] != NULL)
                    {
                        if (j == 0)
                            j = newSize - 1;
                        else
                            --j;
                    }
                    newV[2*j] = m_v[2*i];
                    newV[2*j + 1] = m_v[2*i + 1];
                }
            }
            delete [] m_v;
            m_v = newV;
            m_size = newSize;
            m_usedLim = newSize / 2;
            lookup(key, &i);
        }
    }
    if (!found)
    {
        long keylengthplus1 = strlen(key) + 1;
        char *p;
        if (m_usesCopiedStrings == true)
        {
            p = new XMP_Debug_new char[keylengthplus1];
            memcpy(p, key, keylengthplus1);
        }
        else
        {
            // use the pointer to the key passed in
            // IT MUST BE PERMANENTLY MAINTAINED (at least as long as the hashtable is)
            p = const_cast<char *>(key);
        }
        m_v[2*i] = (void *) p;
        m_used++;
    }
    void *oldValue = m_v[2*i+1];
    m_v[2*i+1] = value;
    return oldValue;
}

void *XPHashtable::put(const XPString& key, void *value)  // return old value
{
    assert(m_usesCopiedStrings == true);
    return put(key.c_str(), value);
}


/*
$Log$
*/
