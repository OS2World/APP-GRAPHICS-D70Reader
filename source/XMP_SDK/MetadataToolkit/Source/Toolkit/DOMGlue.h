
/* $Header: //xaptk/xaptk/DOMGlue.h#7 $ */
/* DOMGlue.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.  If you have received this file from a 
source other than Adobe, then your use, modification, or distribution of it requires the prior 
written permission of Adobe.
*/


#ifndef DOMGLUE_H
#define DOMGLUE_H /* as nothing */


/*
XAP implementation modules that use the DOM API should include
this header file.  It includes DOM.h and support headers.
*/

#include "XPString.h"
#include "DOM.h"
#include "DOMWalker.h"
#include "XAPObjWrapper.h"

#ifdef UNIX_ENV
#include <ostream.h>
#else
#include <ostream>
#endif

#include <iosfwd>


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


/* ===== Defines ===== */

/*
Depending on which DOM implementation is being used, DOM objects may or may
not need to be freed by the caller (I know, what a dumb API!)  If they need
to be freed, define this macro to do something, like "delete nn".
*/
#define DOMFREE(nn)

/* ===== Constant Data ===== */

extern const char *const DOMGlue_NodeTypes[];

/* ===== Operators ===== */

ostream& operator<<(ostream& os, DOMString& ds);

/* ===== DOMGlue Module Functions ===== */

namespace XAPTk {

bool
DOMGlue_CountNodes(Node* me, size_t& nElem, size_t& nText, NodeList* kids = NULL);

    /** Some DOM's only allow deletion of Nodes. */
void
DOMGlue_DeleteNode(Node* killMe);
    /*^
    Delete the node.
    */

    /** Construct a canonical path for the given element. */
void
DOMGlue_ElementToPath(Element* docElem, Element* here, std::string& path);
    /*^
    Assign to the path parameter.
    */

    /** Support for implementing xml:lang. */
void
DOMGlue_EnterLang(XAPTk_StackOfString& langs, NamedNodeMap* attrs);
    /*^
    Scan through the attribute list looking for xml:lang declarations.
    Remember an xml:lang declaration by pushing it on the langs
    stack.
    */

    /** Support for implementing namespaces. */
void
DOMGlue_EnterNamespace(StackOfNSDefs& s_nsDefs, NamedNodeMap* attrs);
    /*^
    Scan through the attribute list looking for xmlns declarations.
    Remember an xmlns namespace declaration by pushing it on the
    s_nsDefs stack.
    */

    /** Expand name. */
XAPTk_PairOfString
DOMGlue_ExpandName ( const XAPTk_StringByString & nsByPrefix, const std::string & name, const Element * attrParent = 0);
    /*^
    Convert a QName into an expanded name using the namespace context
    defined by nsByPrefix.  Return as a <localPart,ns> pair.  If attr is
    true, the QName is for an attribute, otherwise it is for an element.
    */

    /** Get attribute by name. */
bool
DOMGlue_GetAttribute(const XAPTk_StringByString& nsByPrefix, Element* e, const char* ns, const char* localPart, std::string& val);
    /*^
    Get the attribute with the specified expanded name.  Return false
    if no such attribute found on element e.
    */

    /** Get attribute by name. */
Attr*
DOMGlue_GetAttribute(const XAPTk_StringByString& nsByPrefix, Element* e, const char* ns, const char* localPart);
    /*^
    Return the Attr node with the specified expanded name. Return NULL
    if no such attribute found on element e.
    */

    /** Create a new DOM document. */
Document*
DOMGlue_NewDoc();
    /*^
    All allocations are associated with this node.  Deleting this node
    deletes everything allocated for this document.
    */

    /** Parser support. */
void
DOMGlue_Parse(Document* d, const char* buf, const size_t n, const bool last = false);
    /*^
    Accept buffers of XML and parse into the specified DOM document.
    The last buffer is indicated by setting last to true.
    */

    /** Generic tree walker, pre-order, depth first. */
void
DOMGlue_WalkTree(DOMWalker* elf, Node* here);
    /*^
    The elf object is not freed by this call.
    */

} // XAPTk


#if macintosh
	#pragma options align=reset
#endif


#endif // DOMGLUE_H

/*
$Log$
*/

