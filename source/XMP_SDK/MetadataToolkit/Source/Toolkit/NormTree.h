
///* $Header: //xaptk/xaptk/NormTree.h#14 $ */
/* NormTree.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.  If you have received this file from a 
source other than Adobe, then your use, modification, or distribution of it requires the prior 
written permission of Adobe.
*/


#ifndef XAPTK_NORMTREE
#define XAPTK_NORMTREE /* as nothing */


/*
The NormTree class represents the parsed XML DOM tree after it has been normalized for several characteristics:
* No text node has a text sibling.
* RDF syntax alternatives normalized to base syntax (many-to-one).
* Metadata organized into trees of properties.
* Schemas organized into trees of property definitions.
* GOTCHA: The only attribute supported is xml:lang.
*/

/* ===== Forwards ===== */

/* ===== Includes ===== */

#include <string>

#include "DOM.h"
#include "DOMWalker.h"
#include "XAPToolkit.h"
#include "XAPObjWrapper.h"
#include "XAPTkDefs.h"

#ifdef UNIX_ENV
#include <ostream.h>
#else
#include <ostream>
#endif


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


/* ===== Global Types ===== */

typedef class XAPObjWrapper<Element> WElement;

typedef class vector<WElement> VectOElem;

namespace XAPTk {

/* ===== Types ===== */

/* +++++ STL Data Structures +++++ */

class NormTree;

typedef std::map< std::string , NormTree* > NormTreeByStr;

/* ===== Class ===== */

class NormTree {
public:
    /* ===== Static Member (Class) Functions ===== */

        /** Generate a standardized XPath step for this element. */
    static void
    CanonicalStep(NormTree* norm, Element* e, std::string& step);
        /*^
        Only the step for this path, regardless of its ancestors.
        */

        /** Generate canonical path from path. */
    static void
    CanonicalPath(NormTree* norm, const std::string& orig, std::string& canon, Node* propOrAttr = NULL);

        /** Create a new, empty metadata tree. */
    static NormTree*
    NewMeta(const std::string& ns, XAPTk_StringByString* masterNSMap = NULL);
        /*^
        Create a new DOM Doc and create the metadata root element for the
        specified namespace.
        */

        /** Look up the NormTree by schema or ID. */
    static NormTree*
    GetNormTree(const NormTreeByStr& bySchema, const std::string& key);
        /*^
        N/A
        */

        /** Get container type. */
    static XAPStructContainerType
    WhatsMySCType(Element* me);
        /*^
        Only looks at children, not self or parent.
        */

        /** Get form of value. */
    static XAPValForm
    WhatsMyForm(Element* me);
        /*^
        Only looks at children, not self or parent.
        */

        /** Check if child of container, return ordinal. */
    static size_t
    WhatsMyOrd(Element* me);
        /*^
        Returns 0 if not child of container.
        */

    /* ===== Public Instance Variables ===== */

    Element* m_currentElem;
    XAPTk_StringByString m_byPath; // For schema's only
    XAPTk_StringByString m_nsMap; // [first: prefix, second: ns]
    XAPTk_StringByString* m_masterNSMap;

    /* ===== Public Constructor  ===== */

    NormTree(NormTree* orig, const std::string& ns); // clone

    /* ===== Public Destructor  ===== */

    ~NormTree();

    /* ===== Public Member Functions ===== */

        /** Make qualified name. */
    void
    contractName(const std::string& ns, const std::string& localPart, std::string& qName);
        /*^
        Look up prefix for ns, compose qualified name.
        */

        /** Count members in container by path. */
    bool
    countMembers(const std::string& xpath, size_t& ct);
        /*^
        Returns false if not a container or valid path.
        */

        /** Count members in container by element. */
    bool
    countMembers(Element*, size_t& ct);
        /*^
        Returns false if not a container or valid path.
        */

        /** Create an element by ordinal. */
    Element*
    createElement(Element* parent, const size_t ord);
        /*^
        Might need a "before" index.
        */

        /** Create an element with qualified name. */
    Element*
    createElement(const std::string& localPart);
        /*^
        Compose qualified name and create element.
        */

        /** List all paths to leaf nodes from Element. */
    XAPPaths*
    enumerate(Element* e = NULL, XAPPaths* context = NULL, const int steps = 0);
        /*^
        Supports XPathTree::enumerate implementations.  NULL means enumerate
        from root.
        */

        /** Evaluate the XPath to a Node (Element or Attr). */
    Node*
    evalXPath(const std::string& xpath, const bool isRequired = false);
        /*^
        If isRequired is false, returns NULL if any part of the path is not found.  If isRequired is true, creates nodes to ensure that the path is complete.
        */

        /** With throw mode, evaluate the XPath to a Node (Element or Attr). */
    Node*
    evalXPath(const XAPTk_ThrowMode mode, const std::string& xpath, const bool isRequired = false);
        /*^
        If isRequired is false, returns NULL if any part of the path is not found.  If isRequired is true, creates nodes to ensure that the path is complete.
        */

        /** Find element in NodeList that matches attribute. */
    Element*
    findByAttr(VectOElem& elems, const std::string& attr, const std::string* val = NULL);
        /*^
        Matching the value is optional.  Returns NULL if no match.
        */

        /** Find element in NodeList whose named child matches text. */
    Element*
    findByText(VectOElem& elems, const std::string& childName, const std::string& childText);
        /*^
        Returns NULL if no match.
        */

        /** Get the features settings for this element. */
    bool
    getFeatures(Element* e, XAPFeatures& f);
        /*^
        Returns false if no features defined.
        */

        /** Get farthest ancestor container. */
    Element*
    getOldestContainer(Element* here);
        /*^
        Returns NULL if not a member of a container.
        */

        /** Get DOM document. */
    Document*
    getDOMDoc();
        /*^
        Don't you dare delete this doc!
        */

        /** Get namespace. */
    void
    getNS(std::string& ns);

        /** Get the root element (child of doc) */
    Element*
    getRoot() const;
        /*^
        Only child of Document::getDocumentElement.
        */

        /** Get a text value from a leaf node. */
    bool
    getText(Element* leaf, std::string& val,
            XAPFeatures& f);
        /*^
        Supports XPathTree::get, etc.
        */

        /** Select child by name. */
    Element*
    selectChild(Element* e, const std::string& name);
        /*^
        Select child of <VAR>e</VAR> whose tag name matches <VAR>name</VAR>.
        */

        /** Select child by number. */
    Element*
    selectChild(Element* e, const size_t ord);
        /*^
        Select child of <VAR>e</VAR> whose ordinal matches <VAR>ord</VAR>. If
        <VAR>ord</VAR> is 0, match the last child.
        */

        /** Select child by attribute value. */
    Element*
    selectChild(Element* e, const std::string& attr, const std::string& val);
        /*^
        Select child of <VAR>e</VAR> which defines attribute <VAR>attr</VAR>,
        which also has the value <VAR>val</VAR>.
        */

        /** Serialize to a string. */
    void
    serialize(const XAPFormatType f, const int escnl, std::string& serialXML, const std::string* id = NULL, const std::string* about = NULL, const bool needDefaultNS = false); //needDefaultNS changed to false to fix V430433.
        /*^
        Write one rdf:Description.  If id is non-null, use the
        ID form, otherwise use the about form. If needDefaultNS is true, include a default xmlns declaration.
        */

        /** Set features for element. */
    void
    setFeatures(Element* e, const XAPFeatures& f);
        /*^
        If is XAP_FEATURE_NONE, remove features from element.
        Replaces existing features, if any.
        */

        /** Set first child to text */
    void
    setFirstText(Element* e, const std::string& text);
        /*^
        If e has no children, append a text node.
        If e's first child is text, replace it's text value.
        If e's first child is an element, insert a text
        child before it.
        */

        /** Define namespace and prefix for this tree. */
    void
    setNSDef(const std::string& prefix, const std::string& ns);
        /*^
        If prefix already defined, does nothing.
        */

        /** Set text for selected child. */
    Element*
    setSelectedChild(Element* parent, const std::string& tag, const std::string& val, const bool isRequired = false);
        /*^
        Create text node as needed.  Return child.
        If isRequired is false, return NULL if child not found.
        If isRequired is true, create child with tag.
        */

        /** Set a text value for an attribute. */
    void
    setText(Attr* a, const std::string& value,
        XAPFeatures f = XAP_FEATURE_DEFAULT);
        /*^
        Sets the value, creating if needed.
        */

        /** Set a text value for an element. */
    Element*
    setText(Element* e, const std::string& value,
        XAPFeatures f = XAP_FEATURE_DEFAULT);
        /*^
        Supports XPathTree::set, etc. If no text child is defined,
        creates one, otherwise replaces existing value.
        If e is rdf:value, e is removed from its parent
        and deleted, and the text is assigned to the parent.
        In all cases, the element actually assigned to
        is returned (for timestamping purposes).
        */

        /** Ensure that prefix is registered. */
    void
    validateNSPrefix(const std::string& qName);
        /*^
        Make sure prefix is registered.  If not,
        throw xap_bad_schema.
        */

protected:
    /* ===== Instance Variables ===== */
    Document* m_doc; // Holds normalized tree
    Element* m_rootElem;
    std::string m_ns;
    std::string m_rootTag;

    /* ===== Protected Constructors ===== */

    NormTree(); // Default

    NormTree(const NormTree& rhs); // Copy

    NormTree(const std::string& ns, const char *const rootTag, XAPTk_StringByString* masterNSMap = NULL);

}; // NormTree

} // XAPTk


#ifdef XAP_DEBUG_CERR
ostream& operator<<(ostream& os, XAPTk::NormTree* nt);
#endif


#if macintosh
	#pragma options align=reset
#endif


#endif // XAPTK_NORMTREE

/*
$Log$
*/

