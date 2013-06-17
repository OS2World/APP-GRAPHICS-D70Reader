
/* $Header: //xaptk/expat/xmlparse/hashtable.h#3 $ */
/*
The contents of this file are subject to the Mozilla Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is expat.

The Initial Developer of the Original Code is James Clark.
Portions created by James Clark are Copyright (C) 1998
James Clark. All Rights Reserved.

Contributor(s):
*/


#include <stddef.h>


#if macintosh	/* Both MrC and Code Warrior define this automatically. */
	#pragma options align=native
#endif


#ifdef XAP_LIB
/*
XAP Library Modifications

When expat is built-in to the XAP Library, XAP_LIB is defined.
Since this copy of expat might conflict with other copies that
XAP is linked to, we munged the public symbols of expat to hide
them from the linker.
*/

#ifndef XML_NAME_PREFIX
#define XML_NAME_PREFIX XAP
#endif /* XML_NAME_PREFIX */

#ifndef SYMACRO
#define SYMACRO(x) x
#define SYMCAT(p,n) p ## n
#define SYMEVAL(p,n) SYMCAT(p,n)
#define SYMUNGE(n) SYMEVAL(XML_NAME_PREFIX,n)
#define SYMUSE(n) SYMEVAL(XML_NAME_PREFIX,n)
#endif /* SYMACRO */

#else /* not XAP_LIB */

#ifndef SYMACRO
#define SYMACRO(x) x
#define SYMUNGE(n) n
#define SYMUSE(n) n
#endif /* SYMACRO */

#endif /* XAP_LIB */

#ifdef XML_UNICODE
typedef const wchar_t *KEY;
#else
typedef const char *KEY;
#endif

typedef struct {
  KEY name;
} NAMED;

typedef struct {
  NAMED **v;
  size_t size;
  size_t used;
  size_t usedLim;
} HASH_TABLE;

NAMED * SYMUNGE(lookup)(HASH_TABLE *table, KEY name, size_t createSize);
void SYMUNGE(hashTableInit)(HASH_TABLE *);
void SYMUNGE(hashTableDestroy)(HASH_TABLE *);

typedef struct {
  NAMED **p;
  NAMED **end;
} HASH_TABLE_ITER;

void SYMUNGE(hashTableIterInit)(HASH_TABLE_ITER *, const HASH_TABLE *);
NAMED * SYMUNGE(hashTableIterNext)(HASH_TABLE_ITER *);


#if macintosh
	#pragma options align=reset
#endif


/*
$Log$
*/

