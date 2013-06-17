/* $Header:$ */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/

/*
Implementation of the NormTree::serialize member function.
*/

/* ===== Forwards ===== */

/* ===== Includes ===== */

#ifndef XAP_FORCE_NORMAL_ALLOC
	#include "XAPTkAlloc.h"
#endif

#include "XAPConfigure.h"
#include "DOM.h"
#include "DOMGlue.h"
#include "XAPTkInternals.h"
#include "NormTree.h"
#include "XPNode.h"		// For alias skipping check in enterElement.
#include "XMPInitTerm.h"

#define DECL_STATIC /* as nothing */
#define DECL_EXPLICIT /* as nothing */

#if defined(XAP_DEBUG_CERR) && 0
static void dbg_path(const string& msg, Element* docElem, Node* n);
#define CERR_PATH(msg, doc, here) dbg_path(msg, doc, here)
#define CERR_P(s) cerr << s << endl
#define CERR_PR(s) cerr << s
#else
#define CERR_PATH(doc, here, str) /* as nothing */
#define CERR_P(s) /* as nothing */
#define CERR_PR(s) /* as nothing */
#endif

using namespace std;


/* +++++ BEGIN Outside of namespace XAPTk +++++ */

/*
For some reason the compiler complains if any of the following declarations are inside of the XAPTk namespace.
*/

/* ===== Types ===== */

typedef XAPObjWrapper<Element> WElement;
typedef std::stack < WElement > StacOWElement;
typedef std::stack < XAPFeatures > StacOFeatures;

/* ===== Module Operators ===== */

inline ostream&
operator<<(ostream& os, const DOMString& ds) {
    string s = ds;
    return os << s;
}

/* ===== Constants ===== */

static const char *const XTAB = XAPTK_XTAB;

/* +++++ END Outside of namespace XAPTk +++++ */

namespace XAPTk {

/* ===== DOMWalker Classes ===== */

class SerializeCommon : public DOMWalker {
    /*
    All data and functions that are common between pretty and compact
    serializations.
    */
public:
    /* ===== Instance Variables ===== */
    string* m_str;
    XAPFeatures m_currFeats; // Cache of top of m_feats.
    Element* m_ignoreRoot;
    int m_escnl;
    NormTree* m_norm;
    size_t m_level;
    string m_nl;
    StacOWElement m_elems; // Modified stack of elements defining features
    StacOFeatures m_feats; // Modified stack of features

    /* ===== Public Constructor ===== */
    SerializeCommon() : // Default
        m_str(NULL),
        m_currFeats(XAP_FEATURE_NONE),
        m_ignoreRoot(NULL),
        m_escnl(XAP_ESCAPE_CR),
        m_norm(NULL),
        m_level(0),
        m_finished(false)
    {
        m_nl = S_STNL;
    }

    explicit
    SerializeCommon(string* str, Element* ignoreRoot, XAPFormatType /* f */, int escnl, NormTree* norm) :
        m_str(str),
        m_currFeats(XAP_FEATURE_NONE),
        m_ignoreRoot(ignoreRoot),
        m_escnl(escnl),
        m_norm(norm),
        m_level(0),
		m_finished(false)
    {
        if (ISCRLF(escnl)) {
            m_nl += CHCR;
            m_nl += CHLF;
        } else if (ISCR(escnl)) {
            m_nl += CHLF;
        } else if (ISLF(escnl)) {
            m_nl += CHCR;
        } else {
            m_nl = S_STNL;
        }
    }

    /* ===== Public Destructor ===== */
    virtual ~SerializeCommon() {}

    /* ===== Public Member Functions ===== */

        /** Found an element node. */
    virtual bool
    enterElement(Element* e) = 0;

        /** All children of element done. */
    virtual void
    exitElement(Element* e) = 0;

        /** Abort the tree walk. */
    inline virtual bool
    finished() {
        return m_finished;
    }

    inline virtual void
    handleNode(Node* node) {
        string t(DOMGlue_NodeTypes[node->getNodeType()]);
        t = "Unknown node type: "+t;
        throw xap_bad_xap ( t.c_str() );
    }

        /** Found a char data node. */
    virtual void
    text(CharacterData* cd) = 0;

private:
    bool m_finished;

protected:

    inline void
    add(const string& s) {
        m_str->append(s);
    }

    inline void
    add(const char c) {
        *m_str += c;
    }

    inline void
    addText ( const string& s ) {
        string filtered;
        RawToCDATA ( s, filtered, m_escnl );
        this->add ( filtered );
    }

    inline void
    breakLine() {
        this->add(m_nl);
    }

    void
    getProperName(Element* e, string& tag, const bool forcePrefix = false) {
        const XAPTk_StringByString* masterNSMap = m_norm->m_masterNSMap;
        std::string prefix;
        std::string localPart;

        if (e == NULL) {
            tag = *XAPTk::NullString;
            return;
        }
        tag = e->getTagName();

        if (masterNSMap == NULL)
            return;

        /*
        We want to use standardized prefixes if we can help it. We have to
        make sure the standards do not collide with the actuals.
        */
        // ??? Can be simplified? Everything is already unique?

        XAPTk::StripPrefix(tag, &localPart, &prefix);
        // Look up namespace by prefix
        std::string ns;
        XAPTk_StringByString::const_iterator i = m_norm->m_nsMap.find(prefix);
        XAPTk_StringByString::const_iterator done = m_norm->m_nsMap.end();
        if (i == done) { // no match found
            // If no prefix, use default for this NormTree
            // ??? Not found should only be for actual use of no namespace
            if (prefix.empty()) {
                m_norm->getNS(ns);
                if (forcePrefix)
                    m_norm->contractName(ns, localPart, tag);
                else
                    tag = localPart;
            } // Otherwise, just return tag
            return;
        }
        
        //Else
        return;

    }

    inline bool
    peekCurrent(Element** pe, XAPFeatures& f) {
        if (m_elems.empty())
            return(false);
        if (pe == NULL)
            return(false);
        *pe = m_elems.top().m_obj;
        if (m_feats.empty())
            f = XAP_FEATURE_NONE;
        else
            f = m_feats.top();
        assert(f == m_currFeats);
        return(true);
    }

    inline bool
    peekCurrent(Element** pe) {
        XAPFeatures tmp;
        return this->peekCurrent(pe, tmp);
    }

    inline void
    popCurrent() {
        if (!m_elems.empty())
            m_elems.pop();
        if (!m_feats.empty()) {
            m_feats.pop();
            m_currFeats = (m_feats.empty()) ? XAP_FEATURE_NONE : m_feats.top();
        }
    }

    inline XAPFeatures
    pushCurrent(Element* e) {
        XAPFeatures f;
        m_norm->getFeatures(e, f);
        WElement w(e);
        /*
        Special handling for features.  Features persist for node they
        are declared on, and for immediate children, unless child also
        has features.
        */
        if (m_currFeats == XAP_FEATURE_NONE || f != XAP_FEATURE_NONE) {
            m_elems.push(w);
            m_feats.push(f);
            m_currFeats = f;
        } else {
            assert(m_currFeats != XAP_FEATURE_NONE && f == XAP_FEATURE_NONE);
            assert(m_currFeats != f);
            /*
            Since the features for this element are NONE, we can repeat
            the last features if this element is the immediate child
            of the element that defined new features.
            */
            Element* qElem;
            XAPFeatures last;
            this->peekCurrent(&qElem, last);
            WElement old(qElem);
            if (m_feats.size() < 2) {
                m_elems.push(old);
                m_feats.push(m_currFeats);
            } else {
                last = m_feats.top();
                assert(last == m_currFeats);
                m_feats.pop();
                XAPFeatures anc = m_feats.top();
                m_feats.push(last); // Restore stack
                if (anc == last) {
                    // Lower descendents can be NONE
                    m_elems.push(w);
                    m_feats.push(f);
                    m_currFeats = f;
                } else {
                    // Repeat last
                    m_elems.push(old);
                    m_feats.push(last);
                    m_currFeats = last;
                }
            }
        }
        return(m_currFeats);
    }

    inline void
    tagAdd(const string& s) {
        m_str->append(s);
    }

    inline void
    tagAddAttr ( const string& a, const string& v )
    {
        m_str->append ( " " );
        m_str->append ( a );
        m_str->append ( "='" );
        string filtered;
        RawToCDATA ( v, filtered, m_escnl );
        m_str->append ( filtered );
        m_str->append ( "'" );
    }


    inline void
    tagClose(bool isEmpty) {
        *m_str += (isEmpty) ? "/>" : ">";
    }

    inline void
    tagStartOpen(const string& tag) {
        *m_str += "<" + tag;
    }

    inline void
    tagEndOpen(const string& tag) {
        *m_str += "</" + tag;
    }

};

/*==================================================================*/
/*==================================================================*/
/*==================================================================*/

class SerializePretty : public SerializeCommon {
public:
    /*
    This subclass is dedicated to pretty-printing the XML serialization.
    Basic syntax is used, though nested properties are represented
    with parseType='Resource', rather than with an explicit Description.
    Abbreviations are not used.
    */
    /* ===== Instance Variables ===== */
    size_t m_nResQuals; // Number of qualifers of an rdf:resource (non-rdf:value).

    /* ===== Public Constructor ===== */
    SerializePretty() : // Default
        SerializeCommon(),
        m_nResQuals(0)
    {}

    explicit
    SerializePretty(string* str, Element* ignoreRoot, XAPFormatType f, int escnl, NormTree* norm) :
        SerializeCommon(str, ignoreRoot, f, escnl, norm),
        m_nResQuals(0)
    {}

    /* ===== Public Destructor ===== */
    virtual ~SerializePretty() {}

    /* ===== Public Member Functions ===== */

        /** Found an element node. */
    inline virtual bool
    enterElement ( Element* e )
    {
        XAPFeatures features = this->pushCurrent(e);
        size_t eCount;
        size_t tCount;
        const bool isEmpty = !DOMGlue_CountNodes(e, eCount, tCount);
        string tag;
        this->getProperName(e, tag);
        bool isValueResource = false;
        bool isQValue = false;
        bool isResource = ((features & XAP_FEATURE_RDF_RESOURCE) != 0);
        bool isXML = ((features & XAP_FEATURE_XML) != 0);
        const XAPValForm form = NormTree::WhatsMyForm(e); //REVISIT: move closer?
        Text* uri = NULL;
        string tmp;
        Element* qElem;

		if ( true ) {	// *** Need a MetaXAP option to decide if all alias values should be output
			XPElement *	elem	= dynamic_cast<XPElement*> (e);
			if ( (elem != NULL) && (elem->checkIsAnAlias()) ) {
				this->indent();
				this->tagAdd ( "<!-- " );
				this->tagAdd ( tag );
				this->tagAdd ( " is aliased -->" );
	            this->breakLine();
				return false;	// Stop processing this element and its children.
			}
		}

        /*
        Handle containers, transform in place.  Parent needs
        to detect container children.  Member children need
        to transform to rdf:li elements.
        */
        if (tag == XAPTK_TAG_META_ALT
        || tag == XAPTK_TAG_META_BAG
        || tag == XAPTK_TAG_META_SEQ) {
            tag = "rdf:li"; //REVISIT: use constant?
        }

        // Handle special cases
        if (features == XAP_FEATURE_NONE)
            qElem = NULL;
        else
            this->peekCurrent(&qElem);
        if (isResource) {
            // If valid, issue entire element here.
            uri = dynamic_cast<Text*>(e->getFirstChild());
            if (uri != NULL) {
                if (e == qElem
                && uri->getNextSibling() == NULL
                && ((features & XAP_FEATURE_RDF_VALUE) == 0)) {
                    tmp = uri->getData();
                    this->indent();
                    this->tagStartOpen(tag);
                    this->tagAddAttr(XAPTK_ATTR_RDF_RESOURCE, tmp);
                    this->tagClose(true);
                    m_nResQuals = 0;
                    ++m_level;
                    return true;	// Continue processing this element and its children.
                } else if ((features & XAP_FEATURE_RDF_VALUE) != 0) {
                    // Handle as a description with rdf:value
                    isValueResource = true;
                } else if (e == qElem) {
                    // We need to abbreviate all qualifiers of rdf:resource
                    m_nResQuals = eCount + tCount;
                }
            }
        }
        if (!isValueResource && (features & XAP_FEATURE_RDF_VALUE) != 0) {
            isQValue = true;
        }

        /*
        If e == m_ignoreRoot, e is the placeholder DOM Node which
        represents a top-level Description element.
        */
        if (e == m_ignoreRoot) {
            if (isValueResource) {
                /*
                We didn't write any abbreviations, but we may still
                have an open Description tag waiting for an ending.
                */
                this->tagClose(false);
            }
            return true;	// Continue processing this element and its children.
        }

        /* Deal with rdf:resource special case. */
        if (isResource && !isValueResource && qElem != NULL && e != qElem) {
            // Generate an abbreviation, instead of a sub-tag
            assert(e->getAttributes() == NULL);
            assert(form == xap_simple);
            this->tagAdd(" ");
            this->tagAdd(tag);
            this->tagAdd("='");
            return true;	// Continue processing this element and its children.
        }

        /* Start tag */
        this->indent();
        this->tagStartOpen(tag);
        NamedNodeMap* attrs = e->getAttributes();
        if (attrs != NULL) {
            const unsigned long n = attrs->getLength();
            for (unsigned long i = 0; i < n; ++i) {
                //Only support xml:lang for now.
                Attr* a = dynamic_cast<Attr*>(attrs->item(i));
                tmp = a->getName();
                if (tmp != XAPTK_ATTR_XML_LANG)
                    continue;
                string tVal = a->getValue();
                this->tagAddAttr(tmp, tVal);
            }
        }
        /*
        If the element does not contain an rdf:resource (other than as the value
        of rdf:value), close the tag.
        If it does contain an rdf:resource, we must use the abbreviated
        form for all qualifiers, which will will write out in the course
        of walking the children of the m_qElem.
        */
        if (!isResource || isValueResource) {
            if (!isEmpty) {
                if ((e == qElem && features != XAP_FEATURE_NONE)
                || form == xap_description)
                    this->tagAddAttr("rdf:parseType", "Resource");
                this->tagClose(false);
            } else {
                this->tagClose(true);
            }
        }
        /*
        Check if first child is an element.  If so (not text),
        okay to break the line for pretty formatting.
        Also okay if this is a qualified resource of an rdf:value.
        Also okay if this is a qualified value.
        Also okay if this is an XML string.
        */
        Element* check = dynamic_cast<Element*>(e->getFirstChild());
        if (check != NULL
        || (e == qElem && (isQValue || isXML || isValueResource))) {
            this->breakLine();
        }
#ifdef XXX
        if (check != NULL
        || (e == qElem
         && ((eCount > 0 && (isValueResource || isQValue || isXML))
          ||(eCount == 0 && isXML)))) {
            this->breakLine();
        }
#endif
        ++m_level;

        // Handle container
        if (!isEmpty && form == xap_container) {
            XAPStructContainerType sct = NormTree::WhatsMySCType(e);
            switch (sct) {
                // REVISIT: Use const static strings instead of inline literals?
                case xap_alt: tag = "rdf:Alt"; break;
                case xap_bag: tag = "rdf:Bag"; break;
                case xap_seq: tag = "rdf:Seq"; break;
#ifdef XAP_1_0_RC1
                default: assert(false); // CantHappen
#else
				default:
					throw xap_bad_type();
#endif
            }
            this->indent();
            this->tagStartOpen(tag);
            this->tagClose(false);
            this->breakLine();
            ++m_level;
        }

		return true;	// Continue processing this element and its children.

    }

        /** All children of element done. */
    inline virtual void
    exitElement(Element* e) {
        Element* qElem;
        XAPFeatures features;
        this->peekCurrent(&qElem, features);
        if (features == XAP_FEATURE_NONE)
            qElem = NULL;
        this->popCurrent();

        bool isValueResource = false;
        bool isResource = ((features & XAP_FEATURE_RDF_RESOURCE) != 0);
        bool isXML = ((features & XAP_FEATURE_XML) != 0);
        bool isQValue = ((features & XAP_FEATURE_RDF_VALUE) != 0);

        if (m_level > 0)
            --m_level;

        // Handle early special cases.
        if (isResource) {
            if ((features & XAP_FEATURE_RDF_VALUE) != 0) {
                isValueResource = true;
            } else if (e == qElem) {
                // This tag is already done.
                if (m_nResQuals > 0)
                    this->tagClose(true);
                this->breakLine();
                m_nResQuals = 0;
                return;
            } else {
                // Already abbreviated during enterElement
                return;
            }
        }

        const bool isEmpty = !e->hasChildNodes();
        string tag;
        this->getProperName(e, tag);

        /* Okay to do this here (keep parallel to SerializeCompact). */
        if (qElem == e) {
            m_nResQuals = 0;
        }

        if (e == m_ignoreRoot) {
            this->add(XTAB);
            this->add("</rdf:Description>");
            this->breakLine();
            return;
        }

        /*
        Handle containers, transform in place.  Parent needs
        to detect container children.  Member children need
        to transform to rdf:li elements.
        */
        if (!isEmpty && NormTree::WhatsMyForm(e) == xap_container) {
            string cTag;
            XAPStructContainerType sct = NormTree::WhatsMySCType(e);
            switch (sct) {
                //REVISIT: Use static const strings instead of inline literals?
                case xap_alt: cTag = "rdf:Alt"; break;
                case xap_bag: cTag = "rdf:Bag"; break;
                case xap_seq: cTag = "rdf:Seq"; break;
#ifdef XAP_1_0_RC1
                default: assert(false); // CantHappen
#else
				default:
					throw xap_bad_type();
#endif
            }
            this->indent();
            this->tagEndOpen(cTag);
            this->tagClose(false);
            this->breakLine();
            --m_level;
        }

        // Handle container member
        if (tag == XAPTK_TAG_META_ALT
        || tag == XAPTK_TAG_META_BAG
        || tag == XAPTK_TAG_META_SEQ) {
            tag = "rdf:li"; //REVISIT: Use static const string instead?
        }

        // End tag
        size_t eCount;
        size_t tCount;
        DOMGlue_CountNodes(e, eCount, tCount);
        if (e == qElem && (isXML || isQValue))
            this->indent();
        else if (eCount > 0)
            this->indent();
        if (!isEmpty) {
            this->tagEndOpen(tag);
            this->tagClose(false);
        }
        this->breakLine();

    }

        /** Found a char data node. */
    inline virtual void
    text(CharacterData* cd) {
        XAPFeatures features;
        Element* qElem;
        this->peekCurrent(&qElem, features);
        if (features == XAP_FEATURE_NONE)
            qElem = NULL;

        string t;
        bool isValueResource = false;
        bool isXML = ((features & XAP_FEATURE_XML) != 0);
        bool isResource = ((features & XAP_FEATURE_RDF_RESOURCE) != 0);
        bool isQValue = false;

#ifdef XAP_DEBUG_VALIDATE
        long top = 0xADBEADBE;
        XAPTk::CheckStack(NULL, &top);
#endif // XAP_DEBUG_VALIDATE

        // Handle early special cases
        Element* parent = dynamic_cast<Element*>(cd->getParentNode());
        assert(parent != NULL);
        if (isResource) {
            if ((features & XAP_FEATURE_RDF_VALUE) != 0) {
                isValueResource = true;
            } else if (m_nResQuals == 0) {
                // Already handled.
                return;
            }
            /*
            Else fall thru.  isResource && !isValueResource is
            this case.
            */
        }
        if (!isValueResource && (features & XAP_FEATURE_RDF_VALUE) != 0) {
            isQValue = true;
        }

        t = cd->getData();

        if ((isValueResource || isQValue) && parent == qElem) {
            // Issue rdf:value
            this->indent();
            this->tagStartOpen(XAPTK_TAG_RDF_VALUE);
            if (isValueResource) { // with rdf:resource attribute
                this->tagAddAttr(XAPTK_ATTR_RDF_RESOURCE, t);
                this->tagClose(true);
            } else {
                this->tagClose(false);
                this->addText(t);
                this->tagEndOpen(XAPTK_TAG_RDF_VALUE);
                this->tagClose(false);
            }
            this->breakLine();
        } else if (isXML && parent == qElem) {
            // Issue rdf:value
            this->indent();
            this->tagStartOpen(XAPTK_TAG_RDF_VALUE);
            this->tagClose(false);
            this->addText(t);
            this->tagEndOpen(XAPTK_TAG_RDF_VALUE);
            this->tagClose(false);
            this->breakLine();
            // iX:is qualifier
            this->indent();
            this->tagStartOpen(XAPTK_TAG_IX_IS);
            this->tagClose(false);
            this->addText(XAPTK_QUAL_IS_XML);
            this->tagEndOpen(XAPTK_TAG_IX_IS);
            this->tagClose(false);
            this->breakLine();
        } else if (isResource && !isValueResource) {
            if (parent == qElem) {
                // Issue complete rdf:resource
                this->tagAddAttr(XAPTK_ATTR_RDF_RESOURCE, t);
            } else {
                // Issue value for qualifier attribute
                this->addText(t);
                this->tagAdd("'");
            }
        } else {
            this->addText(t);
        }
    }

private:
    inline void
    indent() {
        for (size_t i = 0; i < m_level+2; ++i)
            this->add(XTAB);
    }

};

/*==================================================================*/
/*==================================================================*/
/*==================================================================*/

class SerializeCompact : public SerializeCommon {
public:
    /*
    This subclass is dedicated to generating a compact XML serialization.
    Abbreviated syntax is used whenever possible.
    */

    /* ===== Types ===== */
    typedef stack < XAPTk_StringByString > StacOMaps;

    /* ===== Instance Variables ===== */
    StacOMaps m_abbrev;

    /* ===== Public Constructor ===== */
    SerializeCompact() : // Default
        SerializeCommon()
    {}

    explicit
    SerializeCompact(string* str, Element* ignoreRoot, XAPFormatType f, int escnl, NormTree* norm) :
        SerializeCommon(str, ignoreRoot, f, escnl, norm)
    {}

    /* ===== Public Destructor ===== */
    virtual ~SerializeCompact() {}

    /* ===== Public Member Functions ===== */

        /** Found an element node. */
    inline virtual bool
    enterElement(Element* e) {
        XAPFeatures features = this->pushCurrent(e);
        const bool isEmpty = !e->hasChildNodes();
        string tag;
        this->getProperName(e, tag);
        bool isAbbrev = false;
        bool isAll = false;
        bool isValueResource = false;
        bool isQValue = false;
        bool isResource = ((features & XAP_FEATURE_RDF_RESOURCE) != 0);
        //XXX bool isXML = ((features & XAP_FEATURE_XML) != 0);
        const XAPValForm form = NormTree::WhatsMyForm(e); //REVISIT: move closer?
        Text* uri = NULL;
        string tmp;
        Element* qElem;

		if ( true ) {	// *** Need a MetaXAP option to decide if all alias values should be output
			XPElement *	elem	= dynamic_cast<XPElement*> (e);
			if ( (elem != NULL) && (elem->checkIsAnAlias()) ) {
				return false;	// Stop processing this element and its children.
			}
		}

        // Handle special cases
        if (features == XAP_FEATURE_NONE)
            qElem = NULL;
        else
            this->peekCurrent(&qElem);
        if (isResource) {
            // If valid, issue entire element here.
            uri = dynamic_cast<Text*>(e->getFirstChild());
            if (uri != NULL) {
                if (e == qElem
                && uri->getNextSibling() == NULL
                && ((features & XAP_FEATURE_RDF_VALUE) == 0)) {
                    tmp = uri->getData();
                    /*
                    Handle containers, transform in place.  Parent needs
                    to detect container children.  Member children need
                    to transform to rdf:li elements.
                    */
                    if (tag == XAPTK_TAG_META_ALT
                    || tag == XAPTK_TAG_META_BAG
                    || tag == XAPTK_TAG_META_SEQ) {
                        tag = "rdf:li"; //REVISIT: static const string?
                    }
                    this->tagStartOpen(tag);
                    this->tagAddAttr(XAPTK_ATTR_RDF_RESOURCE, tmp);
                    this->tagClose(true);
                    ++m_level;
                    return true;	// Continue processing this element and its children.
                } else if ((features & XAP_FEATURE_RDF_VALUE) != 0) {
                    // Handle as a description with rdf:value
                    isValueResource = true;
                }
            }
        }
        if (!isValueResource && (features & XAP_FEATURE_RDF_VALUE) != 0) {
            isQValue = true;
        }

        /*
        Handle abbreviated syntax case. If we are in compact mode
        and the element has children, we test for abbreviations.
        */
        if (!isEmpty || isValueResource || isResource) {
            if (!m_abbrev.empty()) {
                const XAPTk_StringByString& ab = m_abbrev.top();
                XAPTk_StringByString::const_iterator notDef = ab.end();
                if (ab.find(tag) != notDef) {
                    // Skip this tag
                    return true;	// Continue processing this element and its children.
                }
            }
            if (this->abbrevOk(e, isAll) > 0) {
                /*
                In the rdf:value rdf:resource case, we can only abbreviate
                if all sub-props can be abbreviated.
                */
                isAbbrev = (isValueResource) ? isAll : true;
                if (!isValueResource && isResource && !isAll) {
                    throw xap_bad_xap (); // Invalid RDF!
                }
            }
        }

        /*
        If e == m_ignoreRoot, e is the placeholder DOM Node which
        represents a top-level Description element.
        */

        if (e == m_ignoreRoot) {
            /*
            The top-level Description might have been left open.
            We need to write out the abbreviations then close the tag.
            */
            if (isAbbrev) {
                isAll = this->abbrevStart(e);
                this->tagClose(isAll);
                if (isAll)
                    this->breakLine();
            } else {
                /*
                We didn't write any abbreviations, but we may still
                have an open Description tag waiting for an ending.
                */
                this->tagClose(false);
            }
            return true;	// Continue processing this element and its children.
        }

        /*
        We do this down here because we need the original tag
        above for looking up the abbreviation!  Not parallel to
        SerializePretty.

        Handle containers, transform in place.  Parent needs
        to detect container children.  Member children need
        to transform to rdf:li elements.
        */
        if (tag == XAPTK_TAG_META_ALT
        || tag == XAPTK_TAG_META_BAG
        || tag == XAPTK_TAG_META_SEQ) {
            tag = "rdf:li"; //REVISIT: static const string?
        }

        /* Start tag */
        this->tagStartOpen(tag);
        NamedNodeMap* attrs = e->getAttributes();
        if (attrs != NULL) {
            const unsigned long n = attrs->getLength();
            for (unsigned long i = 0; i < n; ++i) {
                //Only support xml:lang for now.
                Attr* a = dynamic_cast<Attr*>(attrs->item(i));
                tmp = a->getName();
                if (tmp != XAPTK_ATTR_XML_LANG)
                    continue;
                string tVal = a->getValue();
                this->tagAddAttr(tmp, tVal);
            }
        }
        if (!isAbbrev) {
            /*
            If the element does not contain an rdf:resource (other than as the value
            of rdf:value), close the tag.
            If it does contain an rdf:resource, we must use the abbreviated
            form for all qualifiers, which will will write out in the course
            of walking the children of the m_qElem.
            */
            if (!isResource || isValueResource) {
                if (!isEmpty) {
                    if ((e == qElem && features != XAP_FEATURE_NONE)
                    || form == xap_description)
                        this->tagAddAttr("rdf:parseType", "Resource"); //REVISIT: static const string?
                    this->tagClose(false);
                } else {
                    this->tagClose(true);
                }
            }
        }

        ++m_level;

        // Handle container
        if (!isEmpty && form == xap_container) {
            XAPStructContainerType sct = NormTree::WhatsMySCType(e);
            switch (sct) {
                //REVISIT: use static const string instead of inline literals?
                case xap_alt: tag = "rdf:Alt"; break;
                case xap_bag: tag = "rdf:Bag"; break;
                case xap_seq: tag = "rdf:Seq"; break;
#ifdef XAP_1_0_RC1
                default: assert(false); // CantHappen
#else
				default:
					throw xap_bad_type();
#endif
            }
            this->tagStartOpen(tag);
            this->tagClose(false);
            ++m_level;
        }
        if (isAbbrev) {
            isAll = this->abbrevStart(e);
            if (!isResource || isValueResource)
                this->tagClose(isAll);
        }

		return true;	// Continue processing this element and its children.

    }

        /** All children of element done. */
    inline virtual void
    exitElement(Element* e) {
        XAPFeatures features;
        Element* qElem;
        this->peekCurrent(&qElem, features);
        if (features == XAP_FEATURE_NONE)
            qElem = NULL;
        this->popCurrent();

        bool isValueResource = false;
        bool isResource = ((features & XAP_FEATURE_RDF_RESOURCE) != 0);

        if (m_level > 0)
            --m_level;

        // Handle early special cases.
        if (isResource) {
            if ((features & XAP_FEATURE_RDF_VALUE) != 0) {
                isValueResource = true;
            } else if (e == qElem) {
                size_t ne;
                size_t nt;
                DOMGlue_CountNodes(e, ne, nt);
                if (ne == 0 && nt == 1) {
                    // This tag is already done.
                    return;
                }
                // Else fall thru to close off abbreviated form
            }
        }

        const bool isEmpty = !e->hasChildNodes();
        string tag;
        this->getProperName(e, tag);

        // Handle abbreviations.
        if (!isEmpty || isValueResource || isResource) {
            if (!m_abbrev.empty()) {
                const XAPTk_StringByString& ab = m_abbrev.top();
                if (ab.find(tag) != ab.end()) {
                    // Skip this tag, we've handled it already
                    return;
                }
            }
            if (this->abbrevEnd(e, qElem, features)) {
                //Here if all sub-props handled as abbrevs
                return;
            }
        }

        if (e == m_ignoreRoot) {
            this->add("</rdf:Description>");
            this->breakLine();
            return;
        }

        /*
        Handle containers, transform in place.  Parent needs
        to detect container children.  Member children need
        to transform to rdf:li elements.
        */
        if (!isEmpty && NormTree::WhatsMyForm(e) == xap_container) {
            string cTag;
            XAPStructContainerType sct = NormTree::WhatsMySCType(e);
            switch (sct) {
                //REVISIT: Use static const strings instead of inline literals?
                case xap_alt: cTag = "rdf:Alt"; break;
                case xap_bag: cTag = "rdf:Bag"; break;
                case xap_seq: cTag = "rdf:Seq"; break;
#ifdef XAP_1_0_RC1
                default: assert(false); // CantHappen
#else
				default:
					throw xap_bad_type();
#endif
            }
            this->tagEndOpen(cTag);
            this->tagClose(false);
            --m_level;
        }

        // Handle container member
        if (tag == XAPTK_TAG_META_ALT
        || tag == XAPTK_TAG_META_BAG
        || tag == XAPTK_TAG_META_SEQ) {
            tag = "rdf:li"; //REVISIT: use static const string?
        }

        // End tag
        if (!isEmpty) {
            if (isResource && !isValueResource) {
                this->tagClose(true);
            } else {
                this->tagEndOpen(tag);
                this->tagClose(false);
            }
        }

    }

        /** Found a char data node. */
    inline virtual void
    text(CharacterData* cd) {
        XAPFeatures features;
        Element* qElem;
        this->peekCurrent(&qElem, features);
        if (features == XAP_FEATURE_NONE)
            qElem = NULL;

        string t;
        bool isValueResource = false;
        bool isQValue = false;
        bool isXML = ((features & XAP_FEATURE_XML) != 0);
        bool isResource = ((features & XAP_FEATURE_RDF_RESOURCE) != 0);
        bool isAll = false;

#ifdef XAP_DEBUG_VALIDATE
        long top = 0xADBEADBE;
        XAPTk::CheckStack(NULL, &top);
#endif // XAP_DEBUG_VALIDATE

        // Handle early special cases
        Element* parent = dynamic_cast<Element*>(cd->getParentNode());
        assert(parent != NULL);
        if (isResource) {
            if ((features & XAP_FEATURE_RDF_VALUE) != 0) {
                /*
                If all values have been handled as abbreviations,
                we don't need this text value (should be an rdf:resource
                already).  If we couldn't abbreviate something, we
                treat this text as an rdf:value with rdf:resource.
                */
                if (this->abbrevOk(parent, isAll) > 0) {
                    if (isAll)
                        return;
                }
                isValueResource = true;
            } else {
                size_t ne;
                size_t nt;
                DOMGlue_CountNodes(parent, ne, nt);
                if (ne == 0 && nt == 1) {
                    // Already handled
                    return;
                }
                assert(this->abbrevOk(parent, isAll) > 0);
                assert(isAll);
                // Fall thru to be handled below
            }
        }
        if (!isValueResource && (features & XAP_FEATURE_RDF_VALUE) != 0) {
            isQValue = true;
        }

        /*
        If we're compact and there are abbreviations, we may have
        already done this text.
        */
        if (!m_abbrev.empty()) {
            string tag;
            this->getProperName(parent, tag);
            const XAPTk_StringByString& ab = m_abbrev.top();
            XAPTk_StringByString::const_iterator notDef = ab.end();
            if (ab.find(tag) != notDef) {
                /* Already done, skip this text. */
                return;
            }
        }

        t = cd->getData();

        if ((isValueResource || isQValue) && parent == qElem) {
            // Issue rdf:value
            this->tagStartOpen(XAPTK_TAG_RDF_VALUE);
            if (isValueResource) { // with rdf:resource attribute
                this->tagAddAttr(XAPTK_ATTR_RDF_RESOURCE, t);
                this->tagClose(true);
            } else {
                this->tagClose(false);
                this->addText(t);
                this->tagEndOpen(XAPTK_TAG_RDF_VALUE);
                this->tagClose(false);
            }
            this->breakLine();
        } else if ((isXML) && parent == qElem) {
            // Issue rdf:value
            this->tagStartOpen(XAPTK_TAG_RDF_VALUE);
            this->tagClose(false);
            this->addText(t);
            this->tagEndOpen(XAPTK_TAG_RDF_VALUE);
            this->tagClose(false);
            if (isXML) {
                // iX:is qualifier
                this->tagStartOpen(XAPTK_TAG_IX_IS);
                this->tagClose(false);
                this->addText(XAPTK_QUAL_IS_XML);
                this->tagEndOpen(XAPTK_TAG_IX_IS);
                this->tagClose(false);
            }
        } else if (isResource && parent == qElem) {
            this->tagAddAttr(XAPTK_ATTR_RDF_RESOURCE, t);
        } else {
            this->addText(t);
        }
    }

private:
    bool
    abbrevEnd(Element* e, Element* qElem, XAPFeatures features) {
        /*
        Pop the dictionary of properties that were pre-defined with abbreviated syntax. Return true if all of the sub-props have been defined with abbreviated syntax.
        */
        if (features == XAP_FEATURE_NONE)
            qElem = NULL;
        bool isAll = true;

        if (m_abbrev.empty()) {
            return(false);
        }

        const XAPTk_StringByString& ab = m_abbrev.top();
        XAPTk_StringByString::const_iterator notDef = ab.end();

        if (ab.find("is all") == notDef)
            isAll = false;

        // If not simple, already done
        XAPValForm form = NormTree::WhatsMyForm(e);
        if (e != qElem) { // Skip this test for element with features
            // If simple, already done
            if (form == xap_simple)
                return(false);
        }

        // If parent, pop
        string tag;
        this->getProperName(e, tag);
        XAPTk_StringByString::const_iterator p = ab.find("this parent");
        assert(p != notDef);
        if (p->second == tag) {
            m_abbrev.pop();
        }

        return(isAll);
    }

    bool
    abbrevStart(Element* e) {
        /*
        Check each child sub-prop for qualification as an abbreviation.
        If okay, define as abbreviated and remember that it is already
        defined by storing the property name in a dictionary. Keep
        a dictionary for each node context (push on enter new tag).
        */
        XAPFeatures features;
        Element* qElem;
        this->peekCurrent(&qElem, features);
        if (features == XAP_FEATURE_NONE)
            qElem = NULL;

        bool isAll = true;
        XAPValForm form = NormTree::WhatsMyForm(e);

        if (e != qElem) { // Skip this test for qualified rdf:resource
            // If simple, already done
            if (form == xap_simple)
                return(false);
        }

        // Remember this parent (even containers)
        XAPTk_StringByString tmp;
        string d = "Description";
        //string tn =  e->getTagName();
        string tn;
        this->getProperName(e, tn);
        tmp["this parent"] = (m_level > 0) ? tn : d;
        m_abbrev.push(tmp);

        // Skip containers
        if (form == xap_container)
            return(false);

        // Scan for possible abbreviations.
        NodeList* kids = e->getChildNodes();
        unsigned long n = kids->getLength();
        XAPTk_StringByString& abbMap = m_abbrev.top();
        string val;
        string ktag;

        for (unsigned long i = 0; i < n; ++i) {
            Element* k = dynamic_cast<Element*>(kids->item(i));
            if (k == NULL && e == qElem) {
                if (((features & XAP_FEATURE_RDF_RESOURCE) != 0
                  && (features & XAP_FEATURE_RDF_VALUE) != 0)) {
                    // Is this the rdf:resource value?
                    Text* rt = dynamic_cast<Text*>(kids->item(i));
                    if (rt == NULL) {
                        isAll = false;
                        continue;
                    }
                    // Else, do a special assignment
                    val = rt->getData();
                    this->tagAddAttr(XAPTK_ATTR_RDF_RESOURCE, val);
                    // Already skipping text node, so no abbMap entry
                    continue;
                }
            }
            if (this->abbrevTest(k, val)) {
                this->getProperName(k, ktag, true); // force prefix
                this->tagAddAttr(ktag, val);
                // Skip this tag later
                this->getProperName(k, ktag); // don't force prefix
                abbMap[ktag] = val;
            } else {
                isAll = false;
            }
        }
        if (isAll) {
            abbMap["is all"] = "TRUE";
        }
        return(isAll);
    }

    int
    abbrevOk(Element* e, bool& isAll) {
        /*
        Pre-flight check of this element.  Count the number
        of potential abbreviations, and also indicate whether
        all the children can be abbreviated (changes end syntax
        for the tag.)
        */
        XAPFeatures features;
        Element* qElem;
        this->peekCurrent(&qElem, features);
        if (features == XAP_FEATURE_NONE)
            qElem = NULL;

        int numAbbrev = 0;
        isAll = true;
        string tag;
        this->getProperName(e, tag);
        XAPValForm form = NormTree::WhatsMyForm(e);

        // Skip containers
        if (form == xap_container) {
            isAll = false;
            return(0);
        }
        // Skip container members
        if (tag == XAPTK_TAG_META_ALT
        || tag == XAPTK_TAG_META_BAG
        || tag == XAPTK_TAG_META_SEQ) {
            isAll = false;
            return(0);
        }

        /*
        Scan for possible abbreviations.
        */
        NodeList* kids = e->getChildNodes();
        unsigned long n = kids->getLength();
        for (unsigned long i = 0; i < n; ++i) {
            Element* k = dynamic_cast<Element*>(kids->item(i));
            if (k == NULL && features != XAP_FEATURE_NONE) {
                if ((features & XAP_FEATURE_RDF_RESOURCE) != 0) {
                    /*
                    It's okay, this is just the text for the rdf:resource,
                    or the rdf:value.
                    */
                    continue;
                }
            }
            string val;
            if (this->abbrevTest(k, val)) {
                ++numAbbrev;
            } else {
                isAll = false;
            }
        }
        if (e != m_ignoreRoot && !isAll) {
            // It's all or nothing for property elements
            numAbbrev = 0;
        }
        // Special case
        if (features == XAP_FEATURE_RDF_RESOURCE && e == qElem
        && e != m_ignoreRoot) {
            // Abbreviated form the only one allowed!
            numAbbrev = n;
            isAll = true;
        }
        return(numAbbrev);
    }

    bool
    abbrevTest(Element* k, string& val) {
        /*
        Criteria for abbreviation:
        * not an alias, AND
        * simple, AND
        * no special features, AND
        * no attributes (xml:lang), AND
        * relatively short length, AND
        * doesn't contain a newline
        */
        if ( k == NULL ) return false;
        

		if ( true ) {	// *** Need a MetaXAP option to decide if all alias values should be output
			XPElement *	elem	= dynamic_cast<XPElement*> (k);
			if ( (elem != NULL) && (elem->checkIsAnAlias()) ) {
				return false;	// Stop processing this element and its children.
			}
		}
        
        if ( NormTree::WhatsMyForm(k) != xap_simple ) {

        	return false;

        } else {

            // Add an attribute?
            XAPFeatures f;
            m_norm->getText(k, val, f);
            string aName = k->getTagName();
            // Check attributes
            NamedNodeMap* attrs = k->getAttributes();
            bool hasAttrs = false;
            if (attrs != NULL) {
                const unsigned long mx = attrs->getLength();
                Attr* a;
                string attr;
                for (unsigned long i = 0; i < mx; ++i) {
                    a = dynamic_cast<Attr*>(attrs->item(i));
                    if (a == NULL)
                        continue;
                    attr = a->getName();
                    if (attr == XAPTK_ATTR_META_FEATURES)
                        continue;
                    hasAttrs = true;
                    break;
                }
                DOMFREE(attrs);
            }

            // Check criteria
            if ((f != XAP_FEATURE_NONE)
            || hasAttrs
            || (aName != XAPTK_ATTR_RDF_RESOURCE && val.size() > 48)
            || (ISCR(m_escnl) && val.find(S_STNL) != string::npos)
            || (ISLF(m_escnl) && val.find("\r") != string::npos)) {
                // Can't abbreviate
                return(false);
            } else {
                return(true);
            }

        }

    }

};


/* ===== Public Member Functions (class NormTree) ===== */

void
NormTree::serialize ( const XAPFormatType f, const int escnl, std::string& serialXML,
					  const std::string* id /*=NULL*/, const std::string* about /*=NULL*/,
					  const bool /* needDefaultNS */ /*=false*/ )
{
    const bool isPretty = (f == xap_format_pretty);
    const size_t maxl = 100;
    string line;
    string nl;
    string filtered;

#ifndef XAP_1_0_RC1
	/* Don't serialize this tree if it is empty! */
	if ( (m_rootElem == NULL) || (! m_rootElem->hasChildNodes()) ) return;
#endif /* XAP_1_0_RC1 */

    if ( ISCRLF ( escnl ) ) {
        nl += CHCR;
        nl += CHLF;
    } else if ( ISCR ( escnl ) ) {
        nl += CHLF;
    } else if ( ISLF ( escnl ) ) {
        nl += CHCR;
    } else {
        nl = S_STNL;
    }

    if ( isPretty ) {
        line.append ( nl );
        line.append ( XTAB );
    }

	// Write the start of the rdf:Description element.
	
    if ( id != NULL ) {
        line.append ( "<rdf:Description rdf:ID='" );
        line.append ( *id );
    } else {
        line.append ( "<rdf:Description rdf:about='" );
        if ( about != NULL ) {
            RawToCDATA ( *about, filtered, escnl );
            line.append ( filtered );
        }
    }
    line.append ( "'" );

	// Write the default namespace declaration for this schema if needed.
	// *** Never write the default namespace declaration, everything is qualified on output.
	
    if ( 0 /* needDefaultNS */ ) {
 
 	    if ( isPretty || (line.size() > maxl) ) {
		    line.append ( nl );
		    serialXML.append ( line );
		    line = *XAPTk::NullString;
	        if ( isPretty ) line.append ( XTAB );
	    }

        line.append ( " xmlns='" );
        RawToCDATA ( m_ns, filtered, escnl );
        line.append ( filtered );
        line.append ( "'" );

    }

    // Define the namespaces used in this NormTree.  Only do each once, use the
    // master map prefix if there is one.   (Which there should be.)

#if 0
{
    XAPTk_StringByString::const_iterator localNS;
    
    cout << "Local namespace map" << endl;
    for ( localNS = m_nsMap.begin(); localNS != m_nsMap.end(); ++localNS ) {
    	cout << '\t' << localNS->first << '\t' << localNS->second << endl;
    }

    XAPTk_StringByString::const_iterator masterNS;
    
    if ( m_masterNSMap != NULL ) {
	    cout << "Master namespace map" << endl;
	    for ( masterNS = m_masterNSMap->begin(); masterNS != m_masterNSMap->end(); ++masterNS ) {
	    	cout << '\t' << masterNS->first << '\t' << masterNS->second << endl;
	    }
    }
}
#endif

    XAPTk_StringByString::const_iterator	localNS;	// ->first is prefix, ->second is full name.
    std::string prefix;

    for ( localNS = m_nsMap.begin(); localNS != m_nsMap.end(); ++localNS ) {

		// Skip empty and "global" namespaces.
		
        if ( localNS->first.empty() || localNS->second.empty() ) continue;
        if ( (localNS->first == XAP_PREFIX_RDF) && (localNS->second == XAP_NS_RDF) ) continue;
        if ( (localNS->first == XAP_PREFIX_INFO_XAP) && (localNS->second == XAPTK_NS_INFO_XAP) ) continue;
        
        prefix = localNS->first;
 
 	    if ( isPretty || (line.size() > maxl) ) {
		    line.append ( nl );
		    serialXML.append ( line );
		    line = *XAPTk::NullString;
	        if ( isPretty ) line.append ( XTAB );
	    }

        line.append ( " xmlns:" );
        line.append ( prefix );
        line.append ( "='" );
        RawToCDATA ( localNS->second, filtered, escnl );
        line.append ( filtered );
        line.append("'");

    }
 
    if ( isPretty ) {	// Leave compact open for attribute properties.
    	line.append ( ">" );
    	line.append ( nl );
    }
    if ( line.size() > 0 ) {
	    serialXML.append ( line );
	    line = *XAPTk::NullString;
    }

#ifdef XAP_DEBUG_VALIDATE
        long base = 0xADBEADBE;
        XAPTk::CheckStack(&base, NULL);
#endif // XAP_DEBUG_VALIDATE

    // Serialize the normalized tree
    if ( isPretty ) {
        SerializePretty pretty ( &serialXML, m_rootElem, f, escnl, this );
        DOMGlue_WalkTree ( &pretty, m_rootElem );
    } else {
        SerializeCompact compact ( &serialXML, m_rootElem, f, escnl, this );
        DOMGlue_WalkTree ( &compact, m_rootElem );
    }

#ifdef XAP_DEBUG_VALIDATE
        //Reset
        XAPTk::CheckStack(NULL, NULL);
#endif // XAP_DEBUG_VALIDATE

}


} // XAPTk

/* ======================= */

void
XAPTk_InitNormTreeSerializer() {
    //No-op
}

void
XAPTk_KillNormTreeSerializer() {
    //No-op
}


/*
$Log$
*/
