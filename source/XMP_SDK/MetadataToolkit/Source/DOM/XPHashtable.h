
/* $Header: //xaptk/xmldom/XPHashtable.h#1 $ */ 
/*
 *	Expat hash table
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
 *	XPHashtable.h
 *
 *		C++ generalization of Expat hash table.
 */


#ifndef _H_XPHashtable
#define _H_XPHashtable


#ifndef XMLDOMAPI
#define XMLDOMAPI /* as nothing */
#endif

#include "XPStringDefs.h"


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


class XMLDOMAPI XPHashtable
{
public:
	XPHashtable(size_t initSize = 64, bool copyStrings = true);
	~XPHashtable();
	void *get(const char* key, unsigned long len = (unsigned long)(-1L)) const;
	void *get(const XPString& key) const;
	void *put(const char* key, void *value); // return old value
	void *put(const XPString& key, void *value);  // return old value
	
private:
	void **m_v;				// array of keys and values
	size_t m_initSize;		// initial size for array
	size_t m_size;			// logical size of array (number of k-v pairs)
	size_t m_used;			// number of slots in use
	size_t m_usedLim;		// max number allowed to be used
	bool   m_usesCopiedStrings; 

	static unsigned long hash(const char *s, unsigned long len = (unsigned long)(-1L));
	bool lookup(const char* key, size_t *index, unsigned long len = (unsigned long)(-1L)) const;
};


#if macintosh
	#pragma options align=reset
#endif


#endif /* not _H_XPHashtable */
 
/* 
$Log$ 
*/ 

