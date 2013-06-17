
/* $Header: //xaptk/xmldom/DOMDoc.h#5 $ */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.  If you have received this file from a 
source other than Adobe, then your use, modification, or distribution of it requires the prior 
written permission of Adobe.
*/


#ifndef DOMDOC_H
#define DOMDOC_H /* as nothing */


#include "XAP_XPDOMConf.h"


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


#include "DOM.h"

/* ===== Module Functions ===== */

namespace XMLDOM {

    /** Count element children. */
size_t XMLDOMAPI
CountElements(Element* e);
    /*^
    Returning zero doesn't mean no children, just no element
    children.
    */

    /** Find an element by its tag name. */
Element* XMLDOMAPI
FindFirstElement(Document* d, const DOMString& ns, const DOMString& tag,
                 Element* root = NULL);
    /*^
    Finds the first matching element only, starting from the specified root,
    returns null if no match found.  If the root is NULL, starts from the
    document element.
    */

    /** If Parse returned false, get the error. */
DOMString XMLDOMAPI
GetError(Document* d, int* line, int* col);
    /*^
    The returned string is a copy.
    */

    /** Create a new empty doc. */
Document* XMLDOMAPI
NewDoc();

    /** Parse a buffer. */
bool XMLDOMAPI
Parse(Document* d, const char* buf, const size_t n,
      const bool last = false);
    /*^
    If buf is null-terminated, do NOT include the null in size_t n.
    When buf is the last buffer, pass last as true.  This terminates
    parsing.  NOTE: The DOM tree should not be used until parsing
    has terminated.  Returns false if there is a parsing error,
    call GetError to inspect the problem.
    */

} // XMLDOM


#if macintosh
	#pragma options align=reset
#endif


#endif // DOMDOC_H

/*
$Log$
*/

