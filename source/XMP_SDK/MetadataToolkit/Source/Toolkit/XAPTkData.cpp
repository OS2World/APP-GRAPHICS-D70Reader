/* $Header: //xaptk/xaptk/XAPTk_Data.cpp#27 $ */
/* XAPTk_Data.cpp */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/

/*
Implementation of XAPTk_Data
*/

#ifndef XAP_FORCE_NORMAL_ALLOC
	#include "XAPTkAlloc.h"
#endif

#include <stdlib.h>

#include "XAPToolkit.h"
#include "UtilityXAP.h"

#include "XAPTkData.h"
#include "DOMGlue.h"
#include "NormTree.h"
#include "RDFToNormTrees.h"
#include "Paths.h"
#include "XMPInitTerm.h"
#include "DOMDoc.h"
#include "AllWalkers.h"

/* ===== Definitions ===== */

#define DECL_STATIC /* as nothing */

/* ===== Forwards ===== */

static std::string::size_type ChopToComma(const std::string& rec, std::string& field, const std::string::size_type wasAt);

/* ===== Private Module Stuff ===== */

//XXX static const char* noAtts[] = { NULL };

static const char* ADOBE_XAP_FILTERS = "adobe-xap-filters";

/* ===== Class Definitions ===== */

/* ===== Static Member (Class) Functions ===== */

DECL_STATIC XAPTk_Data*
XAPTk_Data::New(MetaXAP* owner, XAPTk_Data* orig /*= NULL*/) {
    if (orig == NULL)
        return new XMP_Debug_new XAPTk_Data(owner);
    else
        return orig->clone(owner);
}


/* ===== Static (Class) Variables ===== */

/* ===== Public Destructor ===== */

XAPTk_Data::~XAPTk_Data() {
    try {
        try {
            delete m_xmlStm;
        } catch (...) {}
        XAPTk::NormTreeByStr::iterator i;
        XAPTk::NormTreeByStr::iterator done  = m_bySchema.end();
        for (i = m_bySchema.begin(); i != done; ++i) try {
            delete i->second; // NormTree*
            i->second = NULL;
        } catch (...) {}
        done = m_byID.end();
        for (i = m_byID.begin(); i != done; ++i) try {
            delete i->second; // NormTree*
            i->second = NULL;
        } catch (...) {}
        delete m_domDoc;
    } catch (...) {}
}


/* ====== Protected Constructors ===== */

XAPTk_Data::XAPTk_Data() :
  m_owner(NULL),
  m_options(XAP_OPTION_DEFAULT),
  m_clock(NULL),
  m_domDoc(NULL),
  m_xmlStm(NULL),
  m_parsePass(false),
  m_parseFilter(0),
  m_preState(sInit),
  m_prePI(-1),
  m_preParseLen(0)
{
}

XAPTk_Data::XAPTk_Data(MetaXAP* owner) :
  m_owner(owner),
  m_options(XAP_OPTION_DEFAULT),
  m_clock(NULL),
  m_domDoc(NULL),
  m_xmlStm(NULL),
  m_parsePass(false),
  m_parseFilter(0),
  m_preState(sInit),
  m_prePI(-1),
  m_preParseLen(0)
{
    m_domDoc = XAPTk::DOMGlue_NewDoc();
    assert(m_domDoc != NULL); //REVISIT: Recover from bad alloc
}

/* ===== Operators ===== */

/* ===== Public Member Functions ===== */

void
XAPTk_Data::append(const std::string& ns, const std::string& path, const std::string& value, const bool inFront, const XAPFeatures f) {
    XAPTk::NormTree* norm = XAPTk::NormTree::GetNormTree(m_bySchema, ns);
    if  (norm == NULL ) throw xap_bad_schema();

    Node* node = norm->evalXPath(path);
    Element* e = dynamic_cast<Element*>(node); // Elder sibling
    if (( node == NULL) || (e == NULL) ) throw xap_bad_path();

    // Verify that this is the immediate child of a container
#ifdef XXX
    Element* oldest = norm->getOldestContainer(e);
#endif
    Element* youngest = NULL;
    Element* here = e;
    while (here != NULL && here != norm->getRoot()) {
        if (XAPTk::NormTree::WhatsMyForm(here) == xap_container) {
            youngest = here;
            break;
        }
        here = dynamic_cast<Element*>(here->getParentNode());
    }
    if ( youngest != dynamic_cast<Element*>(e->getParentNode()) ) throw xap_bad_path();

    // Climb the tree until we find a container parent.
    // Save the first non-zero ordinal while we're at it.
    // Spawn new kids as we climb.
    size_t ord = 0;
    Element* elderSib = NULL;
    Element* newKid = NULL;
    Element* parent;
    while (XAPTk::NormTree::WhatsMyForm(e) != xap_container) {
        parent = dynamic_cast<Element*>(e->getParentNode());
        if ( (parent == NULL) || (parent == norm->getRoot()) ) {
            if ( newKid != NULL ) XAPTk::DOMGlue_DeleteNode ( newKid );
            throw xap_bad_path();
        }
        if (newKid == NULL) {
            // Duplicate the member item name of sibling
            newKid = norm->getDOMDoc()->createElement(e->getTagName());
        } else {
            XAPTk::DOMGlue_DeleteNode(newKid);
            throw xap_bad_path();
        }
        if (ord == 0)
            ord = XAPTk::NormTree::WhatsMyOrd(e);
        if (elderSib == NULL && ord != 0)
            elderSib = e;
        e = parent;
    }

    if (newKid == NULL || elderSib == NULL) {
        throw xap_bad_path();
    }

    // The element e is now a container
    // Figure out where the new kid goes.
    if (inFront) {
        e->insertBefore(newKid, elderSib);
    } else {
        Node* youngerSib = elderSib->getNextSibling();
        if (youngerSib == NULL) {
            // Append to end
            e->appendChild(newKid);
        } else {
            e->insertBefore(newKid, youngerSib);
        }
    }
    // Set the value
    if (!value.empty()) // PAC:Beta2
        norm->setText(newKid, value, f);

    /*
    Rather than set a timestamp for individual members, set
    a stamp for the oldest ancestor container.  This keeps us from having
    to track changes as things move around inside a container.
    set() does a corresponding simplification.
    */
    this->punchClock(ns, norm, e, XAP_CHANGE_SET);
}

size_t
XAPTk_Data::count(const string& ns, const string& path) const {
    string::const_reverse_iterator r = path.rbegin();
    if (*r != '*') {
        throw xap_bad_path();
    }
    /*
    Cast Away Justification: In order to count the elements, we need
    to evaluate the path.  The internal evalXPath function is non-const
    because of the optional ability to force creation of a node for the
    path.  We don't actually need non-const access here (optional
    parameter is false), and we aren't actually changing "this" with
    this invocation of evalXPath.  However, the compiler requires
    casting away of const to use the function, so we do.  No instance
    data is actually harmed in this operation.

    It would probably be worthwhile creating a const version of evalXPath
    that does not have the default parameter.  This would eliminate the
    need for this const_cast.
    */
    Node* node = const_cast<XAPTk_Data*>(this)->evalXPath(ns, path); // Returns parent

    Element* e = dynamic_cast<Element*>(node);
    if (node == NULL
    || e == NULL
    || XAPTk::NormTree::WhatsMyForm(e) != xap_container) {
        throw xap_bad_path();
    }
    size_t nElem = 0;
    size_t nText;
    XAPTk::DOMGlue_CountNodes(e, nElem, nText);
    return(nElem);
}


void
XAPTk_Data::createFirstItem(const std::string& ns, const std::string& path, const std::string& value, const XAPStructContainerType type, const std::string* selectorName, const std::string* selectorVal, const bool isAttr, const XAPFeatures f) {
    if (selectorName != NULL && selectorVal != NULL) {
        assert(type == xap_alt);
    }

    // Must not exist already
    Node* test = NULL;
    try {
        test = this->evalXPath(xaptk_no_throw, ns, path);
    } catch (...) {
        ;
    }
    if (test != NULL) {
        throw xap_bad_path();
    }
    XAPTk::NormTree* norm = XAPTk::NormTree::GetNormTree(m_bySchema, ns);
    if (norm == NULL) {
        // Creating a new NormTree
        norm = XAPTk::NormTree::NewMeta(ns, MetaXAP_GetMasterNSMap());
        m_bySchema[ns] = norm;
    }
    Node* node = norm->evalXPath(path, true); // Force creation
    Element* container = dynamic_cast<Element*>(node);
    if (node == NULL || container == NULL) {
        throw xap_bad_path();
    }
    /*
    The type of the container is determined by the element name
    of its first child.
    */
    DOMString tagName;
    switch (type) {
        case xap_alt: tagName = XAPTK_TAG_META_ALT; break;
        case xap_bag: tagName = XAPTK_TAG_META_BAG; break;
        case xap_seq: tagName = XAPTK_TAG_META_SEQ; break;
        default: assert(false); //CantHappen
    }
    Element* item = norm->getDOMDoc()->createElement(tagName);
    container->appendChild(item);

    std::string newPath = path + "/*[1]";
    std::string newValue = value;
    /*
    Set the selector for xap_alt.
    */
    if (type == xap_alt && selectorName != NULL && selectorVal != NULL) {
        if (isAttr) {
            // First create the attribute on the Alt element
            newPath = newPath + "/@" + *selectorName;
            this->createProp(ns, newPath, *selectorVal, XAP_FEATURE_NONE);
            // Then set up to create the item and value
            newPath = path
              + "/*[@"
              + *selectorName
              + "='"
              + *selectorVal
              + "']";
        } else {
            newPath = newPath + "/" + *selectorName;
            newValue = *selectorVal;
        }
    }

    // If value is null, don't create a text child
    if (value.empty()) {
        this->punchClock(ns, norm, container, XAP_CHANGE_CREATED);
    } else {
        // Create the property
        this->createProp(ns, newPath, newValue, f);
    }
}

    /** Create a new property. */
void
XAPTk_Data::createProp(const std::string& ns, const std::string& path, const std::string& value, const XAPFeatures f) {
    XAPTk::NormTree* norm = XAPTk::NormTree::GetNormTree(m_bySchema, ns);
    if (norm == NULL) {
        // Creating a new NormTree
        norm = XAPTk::NormTree::NewMeta(ns, MetaXAP_GetMasterNSMap());
        m_bySchema[ns] = norm;
    }
    Node* node = norm->evalXPath(path, true); // Force creation
    Element* e = dynamic_cast<Element*>(node);
    if (e != NULL) {
        Element* punchMe = norm->setText(e, value, f);
        this->punchClock(ns, norm, punchMe, XAP_CHANGE_CREATED);
        return;
    }
    Attr* a = dynamic_cast<Attr*>(node);
    assert(a != NULL);
    norm->setText(a, value, f);
    this->punchClock(ns, norm, a, XAP_CHANGE_CREATED);
}

XAPPaths*
XAPTk_Data::enumerate ( const std::string* ns /*=NULL*/, const std::string* subPath /*=NULL*/, const int steps /*=0*/ )
{

    if ( ns == 0 ) {

        XAPPaths* context = 0;
        XAPTk::NormTreeByStr::const_iterator done = m_bySchema.end();
        XAPTk::NormTreeByStr::const_iterator in;

        for ( in = m_bySchema.begin(); in != done; ++in ) {
            /* first is namespace, second ins NormTree* */
            XAPTk::NormTree* norm = in->second;
            Element* root = 0;
            if ( subPath != 0 ) {
                root = dynamic_cast<Element*> ( norm->evalXPath ( *subPath ) );
                if ( root == 0 ) throw xap_bad_path();
            }
            context = norm->enumerate ( root, context, steps );
        }

        return context;

    } else {

        Element* root = 0;
        XAPTk::NormTree* norm = XAPTk::NormTree::GetNormTree ( m_bySchema, *ns );

        if ( norm == 0 ) throw xap_bad_schema();
        if ( subPath != 0 ) {
            root = dynamic_cast<Element*> ( norm->evalXPath ( *subPath ) );
            if ( (root == 0) && (*subPath != "/") ) throw xap_bad_path();
        }

        return norm->enumerate ( root, NULL, steps );

    }

}

XAPPaths*
XAPTk_Data::enumerate(const XAPTimeRelOp op, const XAPDateTime& dt, const XAPChangeBits how) {
    XAPTk::Paths* p = NULL;
    XAPTimeRelOp cmp;
    XAPPaths* all = 0;

    if (op == xap_noTime) try {
        //Enumerate all and match propertes with no timestamp
        all = this->m_owner->enumerate();
        if (all == NULL)
            return(NULL);
        std::string ns;
        std::string path;
        XAPDateTime ts;
        XAPChangeBits bits;

        while (all->hasMorePaths()) {
            all->nextPath(ns, path);
            if (!this->m_owner->getTimestamp(ns, path, ts, bits)) {
                continue;
            }
            if (bits != XAP_CHANGE_NONE)
                continue;

            //Found!
            if (p == NULL) {
                p = XAPTk::Paths::New();
            }

            p->addPath(ns, path);
        }
        delete all;
    } catch (...) {
        delete all;
        delete p;
        p = NULL;
    } else try {
        XAPTk_ChangeLog::const_iterator doneLog = m_changeLog.end();
        XAPTk_ChangeLog::const_iterator log;
        for (log = m_changeLog.begin(); log != doneLog; ++log) {
            // log: [first: ns, second: XAPTk_PunchCardByPath]
            //XXX const std::string& nsRack = log->first;
            const XAPTk_PunchCardByPath& rack = log->second;
            XAPTk_PunchCardByPath::const_iterator doneSlot = rack.end();
            XAPTk_PunchCardByPath::const_iterator slot = rack.begin();

            for (; slot != doneSlot; ++slot) {
                // slot: [first: path, second: PunchCard]
                const std::string& propPath = slot->first;
                const PunchCard& card = slot->second;
                assert(card.m_change != 0);

                cmp = XAPTk::CompareDateTimes(card.m_lastModified, dt);
                if (cmp != op)
                    continue;

                if ((card.m_change & how) == 0)
                    continue;

                //Found!
                if (p == NULL) {
                    p = XAPTk::Paths::New();
                }

                p->addPath(log->first, propPath);
            }
        }
    } catch (...) {
        delete p;
        p = NULL;
    }
    return(p);
}

Node*
XAPTk_Data::evalXPath(const std::string& ns, const std::string& xpath, const bool okNewSchema /*= false*/) {
    return this->evalXPath(xaptk_throw, ns, xpath, okNewSchema);
}

Node*
XAPTk_Data::evalXPath(const XAPTk_ThrowMode mode, const std::string& ns, const std::string& xpath, const bool okNewSchema /*= false*/) {
    XAPTk::NormTree* norm = XAPTk::NormTree::GetNormTree(m_bySchema, ns);
    if (norm == NULL) {
        if (!okNewSchema) {
            if (mode == xaptk_throw) {
                throw xap_bad_schema();
            } else {
                return(NULL);
            }
        } else {
            // Creating a new NormTree
            norm = XAPTk::NormTree::NewMeta(ns, MetaXAP_GetMasterNSMap());
            m_bySchema[ns] = norm;
            return(NULL);
        }
    }
    return norm->evalXPath(mode, xpath);
}

bool
XAPTk_Data::extractPropVal(const std::string& ns, Element* leafProp, std::string& val, XAPFeatures& f) const {
    XAPTk::NormTree* norm = XAPTk::NormTree::GetNormTree(m_bySchema, ns);
    if (norm == NULL) {
        throw xap_bad_schema();
    }
    return(norm->getText(leafProp, val, f));
}

bool
XAPTk_Data::fixup(XAPTk::NormTree* norm, const XAPFormatType f, const int escnl) {
    /*
    It's easiest to do transformation directly to the tree before
    serializing, so that's what we do.

    FUTURE: Allow clients to extend fixup.
    */
    string ns;
    norm->getNS(ns);

    /*
    The only fixups we have are for Dublin Core.  We look for
    container properties that only have a single member and convert
    them to singleton. If the container is alt and xml:lang is x-default,
    we remove the attribute.  This is all done in a clone, of course.
    */
    if (ns != XAP_NS_DC)
        return(false);
    XAPTk::NormTree dolly(norm, ns); // clone

    Element* root = dolly.getRoot();
    Element* prop = dynamic_cast<Element*>(root->getFirstChild());
    Node* next = prop;
    string aVal;
    while (next != NULL) {
        prop = dynamic_cast<Element*>(next);
        next = prop->getNextSibling();
#ifdef XAP_DEBUG_CERR
        const string ename(prop->getTagName());
#endif
        XAPValForm form = XAPTk::NormTree::WhatsMyForm(prop);
        if (form != xap_container)
            continue;
        XAPStructContainerType sct = XAPTk::NormTree::WhatsMySCType(prop);
        NodeList* kids = prop->getChildNodes();
        if (kids == NULL)
            continue;
        size_t n = kids->getLength();
        DOMFREE(kids);
        if (n > 1) {
            continue;
        }
        /*
        We copy child into an element that replaces prop.  We copy
        attributes as well, except for an xml:lang set to the default
        language.
        */
        Element* li = dynamic_cast<Element*>(prop->getFirstChild());
        Element* dup = dolly.getDOMDoc()->createElement(prop->getTagName());
        kids = li->getChildNodes();
        /* Copy kids */
        if (kids != NULL) {
            n = kids->getLength();
            DOMFREE(kids);
            for(size_t x = 0; x < n; ++x) {
                Node* gk = li->getFirstChild();
                li->removeChild(gk);
                dup->appendChild(gk);
            }
        }
        /* Copy attributes except ours */
        NamedNodeMap* attrs = li->getAttributes();
        if (attrs != NULL) {
            unsigned long sa = attrs->getLength();
            Attr* a = XAPTk::DOMGlue_GetAttribute(dolly.m_nsMap, li, "xml", "lang");
            if (a != NULL)
                aVal = a->getValue();
            else
                aVal = XAP_NO_VALUE;
            for (unsigned long z = 0; z < sa; ++z) {
                Attr* attr = dynamic_cast<Attr*>(attrs->item(z));
                if (attr == NULL)
                    continue;
                if (sct == xap_alt && attr->getValue() == XAPTK_ALT_DEFAULT_LANG)
                    continue;
                dup->setAttribute(attr->getName(), attr->getValue());
            }
            DOMFREE(attrs);
        }
        /* Insert duplicate before our node */
        root->insertBefore(dup, prop);
        /* Purge our node */
        root->removeChild(prop);
        delete prop;
    }

    if (m_about == "")
        dolly.serialize(f, escnl, m_serialXML);
    else
        dolly.serialize(f, escnl, m_serialXML, NULL, &m_about);
    return(true);
}

bool
XAPTk_Data::get(const string& ns, const string& path, string& val, XAPFeatures& f) const {
    /*
    NOTE: We don't filter the return value.  The API accepts and stores
    raw character data.  Any escapes (like &amp;) are converted into raw
    characters on entry into the API. When returned by get(), they are already
    in raw form.  This means get() will not return the four character
    string "&amp;" that was passed to set(): it will return the 1 character
    string "&".  This is the same as "\n" vs. a 1 char string containing
    LF in C code.
    */

    /*
    Cast Away Justification: In order to get the node, we need
    to evaluate the path.  The internal evalXPath function is non-const
    because of the optional ability to force creation of a node for the
    path.  We don't actually need non-const access here (optional
    parameter is false), and we aren't actually changing "this" with
    this invocation of evalXPath.  However, the compiler requires
    casting away of const to use the function, so we do.  No instance
    data is actually harmed in this operation.

    It would probably be worthwhile creating a const version of evalXPath
    that does not have the default parameter.  This would eliminate the
    need for this const_cast.
    */
    Node* node = const_cast<XAPTk_Data*>(this)->evalXPath(xaptk_no_throw, ns, path);
    if (node == NULL)
        return(false);
    // Check form, must be xap_simple
    if (this->m_owner->getForm(ns, path) != xap_simple) {
        throw xap_bad_path();
    }
    Element* prop = dynamic_cast<Element*>(node);
    if (prop == NULL) {
        /*
        Maybe it's an attribute.
        */
        Attr* attr = dynamic_cast<Attr*>(node);
        if (attr == NULL) {
            /* Maybe some future RDF allows PI or comment?  Fail-soft. */
            return(false);
        }
        val = attr->getValue();
        return(true);
    }
    // It's a property, so get the value
    bool ret = this->extractPropVal(ns, prop, val, f);
    return(ret);
}

XAPStructContainerType
XAPTk_Data::getContainerType ( const string& ns, const string& path ) const {
    /*
    Cast Away Justification: In order to get the container type, we need
    to evaluate the path.  The internal evalXPath function is non-const
    because of the optional ability to force creation of a node for the
    path.  We don't actually need non-const access here (optional
    parameter is false), and we aren't actually changing "this" with
    this invocation of evalXPath.  However, the compiler requires
    casting away of const to use the function, so we do.  No instance
    data is actually harmed in this operation.

    It would probably be worthwhile creating a const version of evalXPath
    that does not have the default parameter.  This would eliminate the
    need for this const_cast.
    */
    Node* node = const_cast<XAPTk_Data*>(this)->evalXPath ( ns, path );
    if ( node == NULL ) throw xap_no_match ( path.c_str() );

    Element* e = dynamic_cast<Element*>(node);
    if ( e == NULL ) throw xap_no_match ( path.c_str() );
    try {
        return(XAPTk::NormTree::WhatsMySCType(e));
    } catch ( xap_no_match& ) {
        throw xap_no_match (path.c_str() );
    }
    return xap_bag; // Just to shut up the compiler!
}

XAPValForm
XAPTk_Data::getForm(const string& ns, const string& path) const {
    /*
    Cast Away Justification: In order to get the value form, we need
    to evaluate the path.  The internal evalXPath function is non-const
    because of the optional ability to force creation of a node for the
    path.  We don't actually need non-const access here (optional
    parameter is false), and we aren't actually changing "this" with
    this invocation of evalXPath.  However, the compiler requires
    casting away of const to use the function, so we do.  No instance
    data is actually harmed in this operation.

    It would probably be worthwhile creating a const version of evalXPath
    that does not have the default parameter.  This would eliminate the
    need for this const_cast.
    */
    Node* node = const_cast<XAPTk_Data*>(this)->evalXPath(ns, path);
    if ( node == NULL ) throw xap_no_match ( path.c_str() );

    Element* e = dynamic_cast<Element*>(node);
    if ( e == NULL ) {
        // Maybe its an attribute?
        Attr* a = dynamic_cast<Attr*>(node);
        if ( a == NULL ) throw xap_no_match ( path.c_str() );
        return xap_simple;
    }
    try {
        return XAPTk::NormTree::WhatsMyForm ( e );
    } catch ( xap_no_match& ) {
        throw xap_no_match ( path.c_str() );
    }
    return xap_unknown;
}

bool
XAPTk_Data::getTimestamp(const std::string& ns, const std::string& path, XAPDateTime& dt, XAPChangeBits& how) const {
    // Canonicalize path
    std::string canon;
    if (!this->toTimestampPath(ns, path, canon)) {
        //It might have been removed, check later.
        canon = path;
    }

    // Look up log by schema
    const XAPTk_PunchCardByPath* log;
    if (!this->getLog(ns, &log))
        return(false);

    const PunchCard* card;

    // Look up change record (PunchCard) by canonical path and set
    if (!this->getPunchCard(*log, canon, &card))
        return(false);
    dt = card->m_lastModified;
    how = card->m_change;
    return(true);
}

void
XAPTk_Data::loadFromTree(Node* here /*= NULL*/) {
    /* BEGIN FIX(Acrobat/Brazil/######) */
    if (m_domDoc == NULL || m_domDoc->getDocumentElement() == NULL)
        throw xap_bad_xap ( "no XAP/RDF found" );
    /* END FIX */
    /* Normalize RDF tree into a XAP property tree */
    try {
        //REVISIT: do something about non-null here
        assert(here == NULL);
        XAPTk::RDFToNormTrees normalizer ( m_domDoc, &m_bySchema, &m_byID, m_owner->isEnabled ( XAP_OPTION_XAPMETA_ONLY ) );
#ifdef XAP_DEBUG_VALIDATE
            long base = 0xADBEADBE;
            XAPTk::CheckStack(&base, NULL);
#endif // XAP_DEBUG_VALIDATE
        XAPTk::DOMGlue_WalkTree(&normalizer, m_domDoc->getDocumentElement());
#ifdef XAP_DEBUG_VALIDATE
            // Reset
            XAPTk::CheckStack(NULL, NULL);
#endif // XAP_DEBUG_VALIDATE
        m_about = normalizer.m_about;
    } catch (...) {
        XAPTk::NormTreeByStr::iterator i;
        XAPTk::NormTreeByStr::iterator done = m_bySchema.end();
        for (i = m_bySchema.begin(); i != done; ++i)
            delete i->second; // NormTree*
        done = m_byID.end();
        for (i = m_byID.begin(); i != done; ++i)
            delete i->second; // NormTree*
        delete m_domDoc;
        m_domDoc = NULL;
        m_bySchema.erase(m_bySchema.begin(), m_bySchema.end());
        m_byID.erase(m_byID.begin(), m_byID.end());
#ifdef XAP_DEBUG_VALIDATE
        // Reset
        XAPTk::CheckStack(NULL, NULL);
#endif // XAP_DEBUG_VALIDATE
        throw;
    }

    /* We no longer need the original document. */
    delete m_domDoc;
    m_domDoc = NULL;


    //Loop invariants
    std::string ix;
    std::string ts;
    std::string prop;
    Element* tsProp;
    std::string val;
    std::string propPath;
    std::string dateTime;
    std::string seqNum;
    std::string changeCode;
    XAPFeatures f;
    std::string::size_type wasAt;

    /*
    Load timestamps, if any.  Enumerate schemas, using each one as
    a key into the m_byID dictionary. If there is an entry, load
    the timestamps into m_changeLog.
    */
    XAPTk::NormTreeByStr::const_iterator schem;
    XAPTk::NormTreeByStr::const_iterator doneSchem = m_bySchema.end();
    for (schem = m_bySchema.begin(); schem != doneSchem; ++schem) try {
        // [first: ns, second: NormTree*]
        const std::string& ns = schem->first;
        XAPTk::NormTree* normStamps = XAPTk::NormTree::GetNormTree(m_byID, ns);
        if (normStamps == NULL)
            continue;
        std::string path;
        XAPTk::StripPrefix(XAPTK_TAG_TS_CHANGES, &path); //PENDING: Use constant
        path += "/*";
        size_t num;
        if (!normStamps->countMembers(path, num))
            continue;

        // Get or create card rack
        XAPTk_PunchCardByPath& rack = m_changeLog[ns];

        // Step through entries, create cards and store
        for (size_t i = 1; i <= num; ++i) {
            // Compose indexed path
            ts = path + "[";
            XAPTk::ToString(i, ix);
            ts += ix;
            ts += "]";

            // Copy data to card
            PunchCard card;

            /*
            The entire timestamp record is encoded in the literal value
            of the bag member as a comma separate list, as follows:
                propPath,date-time,seq,code
            */

            // Get the literal value for the bag member indexed by ts
            tsProp = dynamic_cast<Element*>(normStamps->evalXPath(ts));
            if (tsProp == NULL) {
                //REVISIT: something is wrong, but we'll just ignore it
                continue;
            }
            if (!normStamps->getText(tsProp, val, f)) {
                //REVISIT: something is wrong, but we'll just ignore it
                continue;
            }

            // Parse each value out of the comma separated string
            wasAt = 0;
            wasAt = ChopToComma(val, propPath, wasAt);
            wasAt = ChopToComma(val, dateTime, wasAt);
            wasAt = ChopToComma(val, seqNum, wasAt);
            wasAt = ChopToComma(val, changeCode, wasAt);

            // Assign fields to card
            XAPTk::ToDateTime(dateTime, card.m_lastModified);
            card.m_lastModified.seq = atol(seqNum.c_str());
            if (changeCode.find(XAPTK_CCODE_CREATED) != std::string::npos)
                card.m_change |= XAP_CHANGE_CREATED;
            if (changeCode.find(XAPTK_CCODE_REMOVED) != std::string::npos)
                card.m_change |= XAP_CHANGE_REMOVED;
            if (changeCode.find(XAPTK_CCODE_SET) != std::string::npos)
                card.m_change |= XAP_CHANGE_SET;
            if (changeCode.find(XAPTK_CCODE_FORCED) != std::string::npos)
                card.m_change |= XAP_CHANGE_FORCED;
            if (changeCode.find(XAPTK_CCODE_SUSPECT) != std::string::npos)
                card.m_change |= XAP_CHANGE_SUSPECT;

            // Store the card, indexed by the property path
            rack[propPath] = card;
        }
    } catch (...) {
        continue; // We just skip change logs with errors
    }


#if defined(XAP_DEBUG_CERR) && 0
    XAPTk::NormTreeBySchema::const_iterator done = m_bySchema.end();
    XAPTk::NormTreeBySchema::iterator in;
    for (in = m_bySchema.begin(); in != done; ++in) {
        cerr << "========" << endl;
        cerr << in->second;
    }
    cerr << "========" << endl;
#endif

	#if 0
	{
		// Dump the list of namespaces
        XAPTk::NormTreeByStr::iterator			schema;
        XAPTk_StringByString *					nsMap;
	    XAPTk_StringByString::const_iterator	regNS;
        for ( schema = m_bySchema.begin(); schema != m_bySchema.end(); ++schema ) {
        	cout << endl << "Namespaces for schema " << schema->first << endl;
        	cout << "\tLocal namespaces" << endl;
        	nsMap = & schema->second->m_nsMap;
		    for ( regNS = nsMap->begin(); regNS != nsMap->end(); ++regNS ) {
		    	cout << "\t\t" << regNS->first << ":\t " << regNS->second << endl;
		    }
        	nsMap = schema->second->m_masterNSMap;
        	cout << "\tMaster namespaces @ 0x" << ios::hex << (int)nsMap << ios::dec << endl;
		    for ( regNS = nsMap->begin(); regNS != nsMap->end(); ++regNS ) {
		    	cout << "\t\t" << regNS->first << ":\t " << regNS->second << endl;
		    }
        }
		
	}
	#endif

}


void
XAPTk_Data::parse(const char* buf, const size_t n, const bool last /*=false*/) {
    /*
    Pre-parse to see if the optional adobe_xap_filter PI is present.
    */
    const char* bufBegin = buf;
    size_t bufNum = n;
    if (m_preState != sDone) {
        // We're looking for the PI
        for (size_t i = 0; i < n; ++i) {
            char c = buf[i];
            switch (m_preState) {
                case sInit: {
                    if ( ((unsigned char)c == 0xFFu) || ((unsigned char)c == 0xFEu) ) {
                        //It's UTF-16, give up
                        bufBegin = buf + i;
                        bufNum = n - i;
                        m_preState = sDone;
                    } else if (c == '<') {
                        m_preState = sStart;
                    }
                    break;
                }
                case sStart: {
                    if (c == '?') {
                        m_preState = sPIOrProlog;
                        m_prePI = 0;
                    } else {
                        // If neither PI nor Prolog, stop looking
                        bufBegin = buf + i;
                        bufNum = n - i;
                        m_preState = sDone;
                    }
                    break;
                }
                case sPIOrProlog: {
                    if (c == ADOBE_XAP_FILTERS[m_prePI]) {
                        ++m_prePI;
                        if (ADOBE_XAP_FILTERS[m_prePI] == '\0') {
                            // Found it!
                            m_preState = sFinishPI;
                        }
                    } else {
                        // It's some other PI or prolog, skip to end
                        m_prePI = -1;
                        m_preState = sFindEnd;
                    }
                    break;
                }
                case sFinishPI: {
                    if (c == '>') {
                        m_preState = sProcess;
                    } // else keep accumulating
                    break;
                }
                case sProcess: {
                    // We've found it, now process the instruction
                    std::string tmp(m_preParseBuf, m_preParseLen);
                    std::string::size_type escBegin = tmp.find("esc=");
                    if (escBegin != std::string::npos) {
                        // Skip "esc=" and quote
                        escBegin = tmp.find_first_of("\"'", escBegin+3);
                        ++escBegin;
                        std::string::size_type escEnd = tmp.find_first_of("\"'", escBegin);
                        std::string esc = tmp.substr(escBegin, escEnd - escBegin);
                        // Finally, set the filter
                        if (esc == "CR")
                            m_parseFilter = XAP_ESCAPE_CR;
                        else if (esc == "LF")
                            m_parseFilter = XAP_ESCAPE_LF;
                        else if (esc == "CRLF")
                            m_parseFilter = (XAP_ESCAPE_CR|XAP_ESCAPE_LF);
                        // else leave 0
                    }
                    bufBegin = buf + i;
                    bufNum = n - i;
                    m_preState = sDone;
                    break;
                }
                case sFindEnd: {
                    if (c == '>') {
                        m_preState = sInit;
                    } // else keep skipping
                    break;
                }
                case sBang:
                case sDone:
                default:
                    break; // Fall thru
            } //switch

            if (m_preState == sDone)
                break; // out of loop
            if (m_preParseLen >= XAPTk::CBUFSIZE) {
                if (m_preParseLen > XAPTk::CBUFSIZE) {
                    assert(false); // CantHappen
                    m_preParseLen = XAPTk::CBUFSIZE;
                }
                // We never found it, treat like no filtering
                bufBegin = buf + i;
                bufNum = n - i;
                m_preState = sDone;
                m_parseFilter = 0;
                break; // out of loop
            } else {
                m_preParseBuf[m_preParseLen++] = c;
            }
        } //for
    } //if

    if ( (m_preState == sDone) && (! m_parsePass) ) {
    	if ( m_preParseLen > 0 ) {
	        // We have a lingering buffer that needs to be parsed
	        try {
	            this->innerParse(m_preParseBuf, m_preParseLen, false);
	        } catch (...) {
	            m_parsePass = true;
	            throw;
	        }
        }
        m_parsePass = true;
    }

    if (m_parsePass && bufNum > 0) {
        try {
            this->innerParse(bufBegin, bufNum, last);
        } catch (...) {
            // Reset everything
            m_parsePass = false;
            m_parseFilter = 0;
            m_preState = sInit;
            m_prePI = -1;
            m_preParseLen = 0;

            // Rethrow
            throw;
        }
    }

    if (m_preState == sDone && last) {
        // Reset everything
        m_parsePass = false;
        m_parseFilter = 0;
        m_preState = sInit;
        m_prePI = -1;
        m_preParseLen = 0;
    }
}



    /** Update timestamp. */
void
XAPTk_Data::punchClock(const std::string& ns, XAPTk::NormTree* norm, Node* propOrAttr, const XAPChangeBits& change) {

    if ((m_options & XAP_OPTION_AUTO_TRACK) == 0)
        return;
    if (m_clock == NULL)
        return;

    Element* e;

    e = dynamic_cast<Element*>(propOrAttr);
    if (e == NULL) {
        return; //GOTCHA: NormTree only handles xml:lang attributes
    }

    /*
    Check to see if this is a member of a container.  Rather than
    stamp each individual member and then have to keep track of things
    as they move around, we just stamp the oldest ancestor container
    instead.
    */
    Element* ancestor = norm->getOldestContainer(e);
    if (ancestor != NULL)
        e = ancestor;

    std::string canon;
    XAPTk::NormTree::CanonicalPath(norm, "", canon, e);

    // Special case rejection
    if (canon == XAPTK_METADATA_DATE)
        return;

    XAPDateTime dt;
    m_clock->timestamp(dt);

    // Look up log by schema, create log by ns if needed
    XAPTk_PunchCardByPath& log = m_changeLog[ns];

    // Look up change record (PunchCard) by canonical path and set
    // Create card by path if needed
    PunchCard& card = log[canon];
    card.m_lastModified = dt;
    card.m_change = change;

    // Update metadata timestamp
    bool oldAutoTrack = m_owner->isEnabled(XAP_OPTION_AUTO_TRACK);
    m_owner->enable(XAP_OPTION_AUTO_TRACK, false);
    try {
        UtilityXAP::SetDateTime(m_owner, XAP_NS_XAP, XAPTK_METADATA_DATE, dt);
    } catch (...) {
    }
    m_owner->enable(XAP_OPTION_AUTO_TRACK, oldAutoTrack);
}



    /** Purge unwanted timestamps. */
void
XAPTk_Data::purgeTimestamps(const XAPChangeBits how, const XAPDateTime* dt) {
    XAPTk_ChangeLog::iterator log;
    XAPTk_ChangeLog::iterator doneLog = m_changeLog.end();
    for(log = m_changeLog.begin(); log != doneLog; ++log) {
        XAPTk_PunchCardByPath& rack = log->second;
        // Cycle through the cards, looking for matching change bits
        /*
        It says in the STL docs that iterators for std::map are not
        affected by insert or erase (except for the one being erased).
        Even so, I'm being careful to check end() on every loop, and to
        not do ++card in the erase case.
        */
        XAPTk_PunchCardByPath::iterator card;
        for(card = rack.begin(); card != rack.end();) {
            PunchCard& c = card->second;
            if ((c.m_change & how) != 0) {
                XAPTk_PunchCardByPath::iterator tmp = card;
                ++card;
                rack.erase(tmp);
                continue;
            } else if (dt != NULL) {
                c.m_lastModified = *dt;
                c.m_change = XAP_CHANGE_FORCED;
            }
            ++card;
        }
    }
}



void
XAPTk_Data::remove(const std::string& ns, const std::string& subPath) {
    XAPTk::NormTree* norm = XAPTk::NormTree::GetNormTree(m_bySchema, ns);
    if ( norm == NULL ) throw xap_bad_schema();

    Node* node = norm->evalXPath(subPath);
    if (node == NULL) return; // no harm done
    Element* child = dynamic_cast<Element*>(node);
    if (child == NULL) {
        // Attribute?
        //PENDING: Should remove(..."Foo/@xml:lang") be allowed?
        XAPTk::NotImplemented(XAP_SAY_WHAT("MetaXAP::remove (of attribute)"));
    }
    Element* parent = dynamic_cast<Element*>(child->getParentNode());
    if ( parent == NULL ) throw xap_bad_path();

    this->punchClock(ns, norm, child, XAP_CHANGE_REMOVED);

    parent->removeChild(child);
    XAPTk::DOMGlue_DeleteNode(child); // nuke them all!

#ifndef XAP_1_0_RC1
	/* If the parent has no children, nuke it too. */
	while (!parent->hasChildNodes()) {
		if (parent == norm->getRoot()) {
			/*
			We'll leave the norm tree lying around, but we won't 
			serialize it at serialization time, since the
			next thing the app might do is set a new property
			in this norm tree.
			*/
			break;
		}
		child = parent;
		parent = dynamic_cast<Element*>(child->getParentNode());
		assert(parent != NULL);
		if (parent == NULL) {
			break;
		}
		parent->removeChild(child);
		XAPTk::DOMGlue_DeleteNode(child);
	}
#endif /* XAP_1_0_RC1 */
}


    /** Replace property. */
void
XAPTk_Data::replaceProp(const std::string& ns, Node* prop, const std::string& value, const XAPFeatures f) {
    assert(prop != NULL);
    XAPTk::NormTree* norm = XAPTk::NormTree::GetNormTree(m_bySchema, ns);
    if ( norm == NULL ) throw xap_bad_schema();

    Element* e = dynamic_cast<Element*>(prop);
    if ( e != NULL ) {
        Element* punchMe = norm->setText(e, value, f);
        this->punchClock(ns, norm, punchMe, XAP_CHANGE_SET);
        return;
    }
    Attr* attr = dynamic_cast<Attr*>(prop);
    assert(attr != NULL);
    norm->setText(attr, value, f);
    this->punchClock(ns, norm, attr, XAP_CHANGE_SET);
}



    /** Select child by name. */
Element*
XAPTk_Data::selectChild(const std::string& ns, Element* e, const std::string& expr) {
    XAPTk::NormTree* norm = XAPTk::NormTree::GetNormTree(m_bySchema, ns);
    if ( norm == NULL ) throw xap_bad_schema();
    return norm->selectChild ( e, expr );
}


    /** Start a serialization. */
size_t
XAPTk_Data::serialize(XAPFormatType f, const int escnl) {
    // Clean up any previous mess
    delete m_xmlStm;
    m_xmlStm = NULL;

    // Start with the header
    std::string nl;
    if (ISCRLF(escnl)) {
        nl += XAPTk::CHCR;
        nl += XAPTk::CHLF;
    } else if (ISCR(escnl)) {
        nl += XAPTk::CHLF;
    } else if (ISLF(escnl)) {
        nl += XAPTk::CHCR;
    } else {
        nl = XAPTk::S_STNL;
    }

    m_serialXML = "";
#ifdef XXX
    /*XXX
    Might get embedded in existing XML, like WebDAV property, so
    we shouldn't generate a prolog.  Good thing it is optional anyway.
    */
    m_serialXML.assign("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    m_serialXML.append(nl);
#endif
    if (ISCR(escnl) || ISLF(escnl)) {
        m_serialXML.append("<?");
        m_serialXML.append(ADOBE_XAP_FILTERS);
        m_serialXML.append(" esc=");
	    if (ISCRLF(escnl)) {
	        m_serialXML.append("\"CRLF\"?>");
	    } else if (ISCR(escnl)) {
	        m_serialXML.append("\"CR\"?>");
	    } else if (ISLF(escnl)) {
	        m_serialXML.append("\"LF\"?>");
	    }
    	m_serialXML.append(nl);
    }

    // Need metadata element?
    if ( true /* m_owner->isEnabled(XAP_OPTION_XAPMETA_OUTPUT) */ ) {
        m_serialXML.append("<x:");
        m_serialXML.append(XAPTK_TAG_META);
        m_serialXML.append(" xmlns:x='");
        m_serialXML.append(XAP_NS_META);
        m_serialXML.append("' x:xmptk='");
        m_serialXML.append(XAP_LIB_VERSION_TEXT);
        m_serialXML.append("'>");
        m_serialXML.append(nl);
    }

    // Doc element
    m_serialXML.append("<rdf:RDF ");
    m_serialXML.append("xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'");
    // Namespaces used globally
    m_serialXML.append(" xmlns:iX='");
    m_serialXML.append(XAPTK_NS_INFO_XAP);
    m_serialXML.append("'>");
    m_serialXML.append(nl);

    // For each norm tree in the map, write an rdf:Description
    XAPTk::NormTreeByStr::const_iterator i = m_bySchema.begin();
    XAPTk::NormTreeByStr::const_iterator done = m_bySchema.end();
    while (i != done) {
        /* first is ns, second is NormTree* */
        XAPTk::NormTree* norm = i->second;
        if ( false /* this->fixup(norm, f, escnl) */ ) {	// Disable the special Dublin Core output tweaks.
            ++i;
            continue;
        }
        if (m_about == "")
            norm->serialize(f, escnl, m_serialXML);
        else
            norm->serialize(f, escnl, m_serialXML, NULL, &m_about);
        ++i;
    }

    // Add timestamps, if any
    std::string timestampData;
    this->serializeTimestamps(f, nl, timestampData);
    if (timestampData != "")
        m_serialXML.append(timestampData);

    // Finish with trailer
    m_serialXML.append("</rdf:RDF>");
    m_serialXML.append(nl);

    // Need metadata element?
    if ( true /* m_owner->isEnabled(XAP_OPTION_XAPMETA_OUTPUT) */ ) {
        m_serialXML.append("</x:");
        m_serialXML.append(XAPTK_TAG_META);
        m_serialXML.append(">");
    	m_serialXML.append(nl);
    }
    return m_serialXML.size();
}

    /** Get buffers of serialized XML. */
size_t
XAPTk_Data::extractSerialization(char* buf, const size_t nmax) {
    /*
    It's hard to know when to free the memory associated with
    m_serialXML. We make a couple of best guesses below, but
    it's possible that the memory could linger for the life
    of the MetaXAP object.
    */
    if (m_xmlStm == NULL) {
        m_xmlStm = new XMP_Debug_new std::istringstream(m_serialXML.c_str(), istringstream::in);
    }
    if (m_xmlStm->eof() || m_xmlStm->fail()) {
        m_serialXML = "";
        return(0);
    }
#ifndef READSOME_ALL_PLATFORMS
    /*
    Not all platforms (GCC on Linux) support readsome, so do this
    with normal i/o calls.
    */
    m_xmlStm->read(buf, nmax);
    size_t ret = m_xmlStm->gcount();
#else
    /*
    For historical and documentation reasons, keep this version:
    */
    size_t ret = m_xmlStm->readsome(buf, nmax);
#endif // READSOME_ALL_PLATFORMS
    if (ret == 0) {
        m_serialXML = "";
    }
    return(ret);
}

void
XAPTk_Data::set(const string& ns, const string& path, const string& value, const XAPStructContainerType sct, const XAPFeatures f /*= XAP_FEATURE_DEFAULT*/) {
    Node* node = this->evalXPath(ns, path, true); //Don't need isRequired here
    const string* val = &value;
    string filtered;
    if (XAPTk::CDATAToRaw(value, filtered))
        val = &filtered; //avoid copying the string
    if (node != NULL) {
        this->replaceProp(ns, node, *val, f);
    } else if (sct == xap_sct_unknown) {  // Could be Element or Attr
        this->createProp(ns, path, *val, f);
    } else { // new element for an actual path from an alias
        //Is it an attribute?
        string t0;
        string lastStep;
        long int t1;
        string t2;
        string t3;
        UtilityXAP::AnalyzeStep(path, t0, lastStep, t1, t2, t3);
        if (lastStep[0] == XAPTk::ATTR) {
            this->createProp(ns, path, *val, f);
            return;
        }

        //It's not an attribute, must be an element
        string fixed;
        string var;
        XAPTk::ConformPath(path, fixed, var);
        if (var.empty()) {
            this->createProp(ns, path, *val, f);
        } else {
            //Does container already exist?
            bool needFirst = true;
            std::string tmp = fixed;
            // Exists?
            try {
                if (this->evalXPath(xaptk_no_throw, ns, fixed) != NULL) {
                    needFirst = !(this->getForm(ns, fixed) == xap_container);
                }
            } catch (...) {
                ;
            }

            //Is it alt by lang?
            string langVal;
            if (UtilityXAP::IsAltByLang(this->m_owner, ns, path, &langVal)) {
                //Create alt by lang
                const std::string xmlLang = XAPTK_ATTR_XML_LANG;
                if (needFirst) {
                    this->createFirstItem(
                        ns,
                        fixed,
                        *val,
                        xap_alt,
                        &xmlLang,
                        &langVal,
                        true,
                        f);
                } else {
                    // Just append it
                    tmp.append("/*[last()]");
                    this->append(ns, tmp, *val, false, f);
                    // Set language attribute
                    tmp.append("/@xml:lang");
                    this->set(ns, tmp, langVal, sct);
                }
            } else {
                //Create member element
                if (needFirst) {
                    this->createFirstItem(
                        ns,
                        fixed,
                        *val,
                        sct,
                        NULL,
                        NULL,
                        false,
                        f);
                } else {
                     // Just append it
                    tmp.append("/*[last()]");
                    this->append(ns, tmp, *val, false, f);
               }
            }
        }
    }
}


    /** Set the timestamp. */
void
XAPTk_Data::setTimestamp(const std::string& ns, const std::string& path, const XAPDateTime& dt, const XAPChangeBits& change) {
    // Canonicalize path
    std::string canon;
    XAPTk_PunchCardByPath* log = NULL;
    PunchCard* card = NULL;
    bool checkRemoved = false;

    if (!this->toTimestampPath(ns, path, canon)) {
        //It might have been removed, check later.
        canon = path;
        checkRemoved = true;
    }

    // Special case rejection
    if (canon == XAPTK_METADATA_DATE)
        return;

    // Look up log by schema
    const XAPTk_PunchCardByPath* clog;
    if (!this->getLog(ns, &clog)) {
        if ( checkRemoved ) {
            throw xap_no_match();
        } else {
            log = &(m_changeLog[ns]); //Create entry, make pointer
        }
    } else {
        /*
        Cast Away Justification: the function getLog only has
        a const version, even though sometimes we want write access,
        and it is valid and permissible to do so.  Rather than have
        a non-const variant of the function, we just cast const
        away.

        It would be better to have a non-const variant, so add one
        when it is safe to do so.
        */
        log = const_cast<XAPTk_PunchCardByPath*>(clog);
    }

    const PunchCard* ccard = NULL;

    // Look up change record (PunchCard) by canonical path and set
    if (!this->getPunchCard(*clog, canon, &ccard)) {
        if ( checkRemoved ) {
            throw xap_no_match();
        } else {
            card = &((*log)[canon]); //Create entry, make pointer
        }
    } else {
        /*
        Cast Away Justification: the function getPunchCard only has
        a const version, even though sometimes we want write access,
        and it is valid and permissible to do so.  Rather than have
        a non-const variant of the function, we just cast const
        away.

        It would be better to have a non-const variant, so add one
        when it is safe to do so.
        */
        card = const_cast<PunchCard*>(ccard);
    }

    assert(card != NULL);

    card->m_lastModified = dt;
    card->m_change = change;
}

    /** Convert path into canonical timestamp path. */
bool
XAPTk_Data::toTimestampPath(const std::string& ns, const std::string& path, std::string& canon) const {
    XAPTk::NormTree* norm = XAPTk::NormTree::GetNormTree(m_bySchema, ns);
    if (norm == NULL)
        return(false);
    Element* e = dynamic_cast<Element*>(norm->evalXPath(path));
    if (e == NULL) {
        return(false);
    }

    /*
    Check to see if this is a member of a container.  Rather than
    stamp each individual member and then have to keep track of things
    as they move around, we just stamp the oldest ancestor container
    instead.
    */
    Element* ancestor = norm->getOldestContainer(e);
    if (ancestor != NULL)
        e = ancestor;

    // Canonicalize path
    XAPTk::NormTree::CanonicalPath(norm, "", canon, e);

#ifdef XAP_DEBUG_VALIDATE
    {   // Canonical path should always match the conformed path
        std::string fixed;
        std::string var;
        XAPTk::ConformPath(canon, fixed, var);
        assert(fixed == canon);
    }
#endif
    return(true);
}

/* ===== Protected Member Functions ===== */

XAPTk_Data*
XAPTk_Data::clone(MetaXAP* owner) const {
    XAPTk_Data* dolly = new XMP_Debug_new XAPTk_Data(owner);

    // Do simple assignments first
    dolly->m_options = this->m_options;
    dolly->m_clock = this->m_clock;
    dolly->m_serialXML = this->m_serialXML;
    dolly->m_about = this->m_about;

    // Hope this STL assignment works!
    dolly->m_changeLog = this->m_changeLog;

    // Duplicate NormTree by schema
    XAPTk::NormTreeByStr::const_iterator i = this->m_bySchema.begin();
    XAPTk::NormTreeByStr::const_iterator done = this->m_bySchema.end();
    while (i != done) {
        /* first is ns, second is NormTree* */
        XAPTk::NormTree* norm = new XMP_Debug_new
            XAPTk::NormTree(i->second, i->first);
        dolly->m_bySchema[i->first] = norm;
        ++i;
    }

    // Duplicate NormTree by id
    i = this->m_byID.begin();
    done = this->m_byID.end();
    while (i != done) {
        /* first is ns, second is NormTree* */
        XAPTk::NormTree* norm = new XMP_Debug_new
            XAPTk::NormTree(i->second, i->first);
        dolly->m_byID[i->first] = norm;
        ++i;
    }

    return(dolly);
}

void
XAPTk_Data::innerParse(const char* buf, const size_t n, const bool last) {
    // Do filtering if needed;
    if (m_parseFilter == 0) {
        XAPTk::DOMGlue_Parse(m_domDoc, buf, n, last);
    } else {
        std::string tmp;
        tmp.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            char c = buf[i];
            if (c == XAPTk::CHCR && ISCR(m_parseFilter)) {
                continue;
            } else if (c == XAPTk::CHLF && ISLF(m_parseFilter)) {
                continue;
            }
            tmp += c;
        }
        XAPTk::DOMGlue_Parse(m_domDoc, tmp.data(), tmp.size(), last);
    }
}


bool
XAPTk_Data::getLog(const std::string& ns, const XAPTk_PunchCardByPath** log) const {
    XAPTk_ChangeLog::const_iterator n = m_changeLog.find(ns);
    if (n != m_changeLog.end()) {
        *log = &n->second;
        return(true);
    }
    return(false);
}

bool
XAPTk_Data::getPunchCard(const XAPTk_PunchCardByPath& log, const std::string& propPath, const PunchCard** card) const {
    XAPTk_PunchCardByPath::const_iterator n = log.find(propPath);
    if (n != log.end()) {
        *card = &n->second;
        return(true);
    }
    return(false);
}


void
XAPTk_Data::serializeTimestamps(XAPFormatType f, const std::string& nl, std::string& tsd) {
    tsd = "";
    const bool isPretty = (f == xap_format_pretty);
    const std::string tab = XAPTK_XTAB;
    const std::string tab2 = tab + tab;
    const std::string tab3 = tab + tab + tab;
    const std::string tab4 = tab + tab + tab + tab;

    try {
        XAPTk_ChangeLog::const_iterator doneLog = m_changeLog.end();
        XAPTk_ChangeLog::const_iterator log;
        for (log = m_changeLog.begin(); log != doneLog; ++log) {
            // log: [first: ns, second: XAPTk_PunchCardByPath]
            const std::string& nsRack = log->first;
            const XAPTk_PunchCardByPath& rack = log->second;
            XAPTk_PunchCardByPath::const_iterator doneSlot = rack.end();
            XAPTk_PunchCardByPath::const_iterator slot = rack.begin();

            if (slot == doneSlot)
                continue;
            if (isPretty)
                tsd.append(nl);

            // Description header
            tsd.append("<rdf:Description ID='");
            tsd.append(nsRack);
#ifdef XXX
            tsd.append("'");
            if (isPretty)
                tsd.append(nl);
            tsd.append(" ");
            tsd.append("xmlns='");
            tsd.append(XAPTK_NS_INFO_XAP);
            tsd.append("'");
            if (isPretty)
                tsd.append(nl);
            tsd.append(" ");
            tsd.append("xmlns:iX='");
            tsd.append(XAPTK_NS_INFO_XAP);
#endif
            tsd.append("'>");
            if (isPretty) {
                tsd.append(nl);
                tsd.append(tab);
            }
            tsd.append("<");
            tsd.append(XAPTK_TAG_TS_CHANGES);
            tsd.append(">");
            if (isPretty) {
                tsd.append(nl);
                tsd.append(tab2);
            }
            tsd.append("<rdf:Bag>");

            std::string tmp;
            for (; slot != doneSlot; ++slot) {
                // slot: [first: path, second: PunchCard]
                const std::string& propPath = slot->first;
                const PunchCard& card = slot->second;
                assert(card.m_change != 0);

                if (isPretty) {
                    tsd.append(nl);
                    tsd.append(tab3);
                }
                tsd.append("<rdf:li>");
                assert(propPath.find(",") == std::string::npos);
                tsd.append(propPath);
                tsd.append(",");
                XAPTk::ToString(card.m_lastModified, tmp);
                tsd.append(tmp);
                tsd.append(",");
                XAPTk::ToString(card.m_lastModified.seq, tmp);
                tsd.append(tmp);
                tsd.append(",");
                tmp = "";
                if ((card.m_change & XAP_CHANGE_CREATED) != 0)
                    tmp += XAPTK_CCODE_CREATED;
                if ((card.m_change & XAP_CHANGE_SET) != 0)
                    tmp += XAPTK_CCODE_SET;
                if ((card.m_change & XAP_CHANGE_REMOVED) != 0)
                    tmp += XAPTK_CCODE_REMOVED;
                if ((card.m_change & XAP_CHANGE_FORCED) != 0)
                    tmp += XAPTK_CCODE_FORCED;
                if ((card.m_change & XAP_CHANGE_SUSPECT) != 0)
                    tmp += XAPTK_CCODE_SUSPECT;
                assert(!tmp.empty());
                tsd.append(tmp);
                tsd.append("</rdf:li>");
#ifdef XXX
                if (isPretty) {
                    tsd.append(nl);
                    tsd.append(tab4);
                }
                tsd.append("<rdf:Description");
                tsd.append(" ");
                XAPTk::ToString(card.m_lastModified.seq, tmp);
                tsd.append(XAPTK_TAG_TS_SEQ);
                tsd.append("='" + tmp + "'");
                tsd.append(" ");
                tmp = "";
                if ((card.m_change & XAP_CHANGE_CREATED) != 0)
                    tmp += XAPTK_CCODE_CREATED;
                if ((card.m_change & XAP_CHANGE_SET) != 0)
                    tmp += XAPTK_CCODE_SET;
                if ((card.m_change & XAP_CHANGE_REMOVED) != 0)
                    tmp += XAPTK_CCODE_REMOVED;
                if ((card.m_change & XAP_CHANGE_FORCED) != 0)
                    tmp += XAPTK_CCODE_FORCED;
                if ((card.m_change & XAP_CHANGE_SUSPECT) != 0)
                    tmp += XAPTK_CCODE_SUSPECT;
                tsd.append(XAPTK_TAG_TS_CODES);
                tsd.append("='" + tmp + "'");
                if (isPretty) {
                    tsd.append(nl);
                    tsd.append(tab4);
                }
                tsd.append(" ");
                tsd.append(XAPTK_TAG_TS_PATH);
                tsd.append("='" + propPath + "'");
                if (isPretty) {
                    tsd.append(nl);
                    tsd.append(tab4);
                }
                tsd.append(" ");
                XAPTk::ToString(card.m_lastModified, tmp);
                tsd.append(XAPTK_TAG_TS_LAST);
                tsd.append("='" + tmp + "'/>");
                if (isPretty) {
                    tsd.append(nl);
                    tsd.append(tab3);
                }
                tsd.append("</rdf:li>");
#endif
            }


            if (isPretty) {
                tsd.append(nl);
                tsd.append(tab2);
            }
            tsd.append("</rdf:Bag>");
            if (isPretty) {
                tsd.append(nl);
                tsd.append(tab);
            }
//XXX            tsd.append("</iX:timestamps>");
            tsd.append("</");
            tsd.append(XAPTK_TAG_TS_CHANGES);
            tsd.append(">");
            if (isPretty) {
                tsd.append(nl);
            }
            tsd.append("</rdf:Description>");
            if (isPretty) {
                tsd.append(nl);
            }
        }
        if (isPretty) {
            tsd.append(nl);
        }
    } catch (...) {
        tsd = "";
    }
}



static std::string::size_type
ChopToComma(const std::string& rec, std::string& field, const std::string::size_type wasAt) {
    std::string::size_type commaAt = rec.find(',', wasAt);
    if (commaAt == std::string::npos) {
        commaAt = rec.size();
    }
    field = rec.substr(wasAt, commaAt-wasAt);
    return(commaAt+1);
}




void
XAPTk_InitData() {
    //No-op
}

void
XAPTk_KillData() {
    //No-op
}


/*
$Log$
*/
