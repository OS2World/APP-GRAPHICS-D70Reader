/* $Header: //xaptk/expat/xmltok/xmldef.h#2 $ */
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

/* This file can be used for any definitions needed in
particular environments. */

#ifdef MOZILLA

#include "nspr.h"
#define malloc(x) PR_Calloc(1,(x))
#define calloc(x, y) PR_Calloc((x),(y))
#define free(x) PR_Free(x)
#define int int32

#endif /* MOZILLA */

#ifdef XAP_LIB
#ifndef XAP_FORCE_NORMAL_ALLOC
#include "XAPTkAlloc.h"
#else
#ifndef CALLOC
#define CALLOC calloc
#define MALLOC malloc
#define REALLOC realloc
#define FREE free
#endif /* CALLOC */
#endif /* XAP_FORCE_NORMAL_ALLOC */
#endif /* XAP_LIB */

/*
$Log$
*/
