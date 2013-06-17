/* $Header: //xaptk/xaptk/DOMGlue.cpp#11 $ */
/* DOMGlue.cpp */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/

#ifndef XAP_FORCE_NORMAL_ALLOC
	#include "XAPTkAlloc.h"
#endif

/*
This code glues XAP to the SVG DOM implementation.
It also provides several DOM support functions.
*/

#include "XAPConfigure.h"
#include "DOMGlue.h"
#include "XAPTkInternals.h"
#include "DOMWalker.h"
#include "XMPInitTerm.h"
#include "DOMDoc.h"

#define DECL_STATIC /* as nothing */

/* ===== Constant Data ===== */

const char *const DOMGlue_NodeTypes[] = { //STATIC_INIT
    "NONE",
    "ELEMENT_NODE",
    "ATTRIBUTE_NODE",
    "TEXT_NODE",
    "CDATA_SECTION_NODE",
    "ENTITY_REFERENCE_NODE",
    "ENTITY_NODE",
    "PROCESSING_INSTRUCTION_NODE",
    "COMMENT_NODE",
    "DOCUMENT_NODE",
    "DOCUMENT_TYPE_NODE",
    "DOCUMENT_FRAGMENT_NODE",
    "NOTATION_NODE"
};

/* ===== Classes ===== */

    /*C*
    Predicate that finds the first matching element with
    a given tag name.
    */

class DW4_findElem : public XAPTk::DOMWalker {
public:
    /* ===== Instance Variables ===== */
    Element* m_elem;
    Node* m_root;

    /* ===== Public Constructor ===== */
    DW4_findElem() : // Default
        m_elem(NULL),
        m_root(NULL),
        m_finished(false),
        m_okToFind(true)
    {}

    DW4_findElem(const std::string ns, const std::string tag, Node* root) :
        m_elem(NULL),
        m_root(root),
        m_finished(false),
        m_ns(ns),
        m_tag(tag),
        m_okToFind(root == NULL)
    {
        XAPTk_StringByString floor;
        floor["xmlns"] = "XML";
        m_nsDefs.push(floor);
    }

    /* ===== Public Destructor ===== */
    virtual ~DW4_findElem() {}

    /* ===== Public Member Functions ===== */

        /** Found an element node. */
    inline virtual bool
    enterElement(Element* e) {
        // Process any namespace definitions
        XAPTk::DOMGlue_EnterNamespace(m_nsDefs, e->getAttributes());
        // We're only looking at m_root and below
        if (!m_okToFind && e != NULL) m_okToFind = (e == m_root);
        if (!m_okToFind) return true;	// Continue processing this element and its children.
        // Compare
        const DOMString tn = e->getTagName();
        std::string localPart;
        std::string prefix;
        std::string ns;
        XAPTk::StripPrefix(tn, &localPart, &prefix);
        if (prefix.empty())
            prefix = "xmlns"; // Default
        if (!m_nsDefs.empty()) {
            XAPTk_StringByString& nsDefs = m_nsDefs.top();
            ns = nsDefs[prefix];
            if (localPart == m_tag && ns == m_ns) {
                m_elem = e;
                m_finished = true;	// Stop processing this entire tree.
                return false;
            }
        }
        return true;	// Continue processing this element and its children.
    }

        /** All children of element done. */
    inline virtual void
    exitElement(Element* e) {
        // Pop namespace
        m_nsDefs.pop();
        // Are we finished?
        if (m_root == e && e != NULL) {
            m_okToFind = false;
            m_finished = true;
        }
    }

        /** Abort the tree walk. */
    inline virtual bool
    finished() {
        return m_finished;
    }

    inline virtual void
    handleNode(Node* /* node */) {
        // NO-OP
    }

        /** Found a char data node. */
    inline virtual void
    text(CharacterData* /* cd */) {
        // NO-OP
    }

private:
    bool m_finished;
    XAPTk::StackOfNSDefs m_nsDefs;
    std::string m_ns;
    std::string m_tag;
    bool m_okToFind;
};



/* ===== DOMGlue Module Functions ===== */

namespace XAPTk {

bool
DOMGlue_CountNodes(Node* me, size_t& nElem, size_t& nText, NodeList* kids /*=NULL*/) {
    bool meFree;
    if (kids == NULL) {
        kids = me->getChildNodes();
        meFree = true;
    } else
        meFree = false;
    nElem = 0;
    nText = 0;
    if (kids == NULL) {
        return(false);
    }
    unsigned long mxnl = kids->getLength();
    Node *k;
    for (unsigned long i = 0; i < mxnl; ++i) {
        k = kids->item(i);
        if (dynamic_cast<Element*>(k) != NULL) {
            ++nElem;
            continue;
        }
        if (dynamic_cast<Text*>(k) != NULL) {
            ++nText;
            continue;
        }
    }
    if (meFree) {
        DOMFREE(kids);
    }
    return(nElem > 0 || nText > 0);
}

void
DOMGlue_DeleteNode(Node* killMe) {
    delete killMe;
}

void
DOMGlue_ElementToPath(Element* docElem, Element* here, std::string& path) {
    //REVISIT: Do we need to expand these names?
    path = "";
    if (docElem == NULL || here == NULL) return;
    Element* e = here;
    while (e != docElem) {
        if (e != here) {
            path.insert(0, "/");
        }
        const DOMString name = e->getTagName();
        path.insert(0, name);
        e = dynamic_cast<Element*>(e->getParentNode());
    }
}


void
DOMGlue_EnterLang(XAPTk_StackOfString& langs, NamedNodeMap* attrs) {
    /*
    Always push a new lang definitions, even when the elemnt
    doesn't define a new one. This is for ease of popping.
    Just push the stack top.
    */
    assert(!langs.empty());
    const std::string& lastLang = langs.top();
    if (attrs == NULL) {
        langs.push(lastLang);
        return;
    }
    unsigned long mx = attrs->getLength();
    std::string aName;
    std::string aVal;
    for (unsigned long i = 0; i < mx ; ++i) {
        Attr* attr = dynamic_cast<Attr*>(attrs->item(i));
        if (attr == NULL)
            continue;
        aName = attr->getName();
        if (aName == XAPTK_ATTR_XML_LANG) {
            aVal = attr->getValue();
            break;
        }
    }
    if (aVal.empty())
        langs.push(lastLang);
    else
        langs.push(aVal);
}


void
DOMGlue_EnterNamespace ( StackOfNSDefs& s_nsDefs, NamedNodeMap* attrs )
{
    assert( ! s_nsDefs.empty() );
    XAPTk_StringByString nsDefs = s_nsDefs.top(); // this better do a copy!

    s_nsDefs.push ( nsDefs );		// Always push a new NSDef map level, even when the element doesn't
    if ( attrs == NULL ) return;	//	define any new namespaces.  This is for ease of popping.

	// ??? Wrong? Really stacking the actual prefixes because we're normalizing the input?
    // Process the xmlns attributes. First make sure that the namespace URI is in the global registry
    // with a unique prefix. This avoids lots of downstream problems with differing prefixes. Then add
    // the master prefix and URI to the current live map at the top of the stack. This handles scoping,
    // the live map has the innermost associations. If this is a default namespace declaration, also
    // add it to the stack using the prefix "xmlns".

    unsigned long mx = attrs->getLength();
    for ( unsigned long i = 0; i < mx ; ++i ) {

        Attr* attr = dynamic_cast < Attr* > (attrs->item(i));
        if ( attr == NULL )  continue;

        std::string b4colon;
        std::string nsPrefix;
        const DOMString domAttrName = attr->getName();

        XAPTk::StripPrefix ( domAttrName, &nsPrefix, &b4colon );
        
        // We might have an explicit namespace declaration of the form

        if ( (b4colon == "xmlns") || ((b4colon == "") && (nsPrefix == "xmlns")) ) {

	        assert ( nsPrefix != "" );
	        const std::string nsURL = attr->getValue();
	        (s_nsDefs.top())[nsPrefix] = nsURL;	// Stack default namespace with xmlns prefix, URL might be empty.
			
	        if ( nsPrefix == "xmlns" ) nsPrefix = "";
        	MetaXAP::RegisterNamespace ( nsURL, nsPrefix );	// Make sure the name is known globally.
	        
	        #if 0
	        	cout << "In DOMGlue_EnterNamespace, added " << nsPrefix << " = " << nsURL;
	        	string masterPrefix;
	        	MetaXAP::GetNamespacePrefix ( nsURL, masterPrefix );
	        	if ( masterPrefix != nsPrefix ) cout << " = " << masterPrefix;
	        	cout << endl;
	        #endif

        }

    }

}


    /** first = localPart, second = ns (or xmlns for attr = true) */
XAPTk_PairOfString
DOMGlue_ExpandName ( const XAPTk_StringByString& nsByPrefix, const std::string& name, const Element * attrParent /* = 0 */ )
{
	const bool	attr	= (attrParent != 0);
	
    std::string prefix;
    std::string localPart;
    XAPTk_PairOfString expandedName;	/* [first: localPart, second: ns] */

    XAPTk::StripPrefix ( name, &localPart, &prefix );
    assert ( !localPart.empty() );
    expandedName.first = localPart;
    expandedName.second = "";
    
    #if 0
    	cout << "In DOMGlue_ExpandName for " << name << " (" << prefix << ":" << localPart << "), nsDefs:" << endl;
    	for ( XAPTk_StringByString::const_iterator nsDef = nsByPrefix.begin(); nsDef != nsByPrefix.end(); ++nsDef ) {
			cout << "   " << nsDef->first << " = " << nsDef->second << endl;
    	}
    #endif

    if ( prefix.empty() ) {

        if ( ! attr ) {
            // non-prefixed element belongs to default namespace (xmlns)
            XAPTk_StringByString::const_iterator i = nsByPrefix.find ( "xmlns" );
            if ( i != nsByPrefix.end() ) expandedName.second = i->second;
        } else {
        	// non-prefixed attr belongs to element's namespace
        	XAPTk_PairOfString elemName ( DOMGlue_ExpandName ( nsByPrefix, attrParent->getTagName() ) );
        	expandedName.second = elemName.second;
        	#if 0
        		cout << "   non-prefixed attr of " << elemName.first << " in " << elemName.second << endl;
        	#endif
        }

    } else {

        if ( attr && (prefix == "xmlns") ) {
            expandedName.second = prefix;	// namespace declaration, just return prefix
        } else if ( attr && (prefix == "xml") ) {
            expandedName.second = prefix;	// Reserve xml namespace
        } else {
            // prefixed element or attribute name
            XAPTk_StringByString::const_iterator i = nsByPrefix.find ( prefix );
            if ( i != nsByPrefix.end() ) {
                expandedName.second = i->second;
            } else {
                expandedName.first = name;	// Not a Qualified name!
            }
        }

    }

	#if 0
		cout << "-- expanded to " << expandedName.first << " in " << expandedName.second << endl;
	#endif
	
    return expandedName;

}


bool
DOMGlue_GetAttribute(const XAPTk_StringByString& nsByPrefix, Element* e, const char* ns, const char* localPart, std::string& val) {
    NamedNodeMap* attrs = e->getAttributes();
    if (attrs == NULL)
        return(false);
    const unsigned long n = attrs->getLength();
    const std::string attrName = localPart;
    for (unsigned long i = 0; i < n; ++i) {
        Attr* a = dynamic_cast<Attr*>(attrs->item(i));
        if (a == NULL) continue;
        const DOMString origName = a->getName();

        XAPTk_PairOfString xName = DOMGlue_ExpandName(nsByPrefix, origName, e);
        if (attrName == xName.first
            && (ns == xName.second || xName.second == "")) {
            val = a->getValue();
            return(true);
        }
    }
    return(false);
}



Attr*
DOMGlue_GetAttribute(const XAPTk_StringByString& nsByPrefix, Element* e, const char* ns, const char* localPart) {
    NamedNodeMap* attrs = e->getAttributes();
    const unsigned long n = attrs->getLength();
    const std::string attrName(localPart);
    for (unsigned long i = 0; i < n; ++i) {
        Attr* a = dynamic_cast<Attr*>(attrs->item(i));
        if (a == NULL) continue;
        const DOMString origName = a->getName();

        XAPTk_PairOfString xName = DOMGlue_ExpandName(nsByPrefix, origName, e);
        if (attrName == xName.first
            && (ns == xName.second || xName.second.empty())) {
            return(a);
        }
    }
    return(NULL);
}



Document*
DOMGlue_NewDoc() {
    return XMLDOM::NewDoc();
}

void
DOMGlue_Parse(Document* d, const char* buf, const size_t n, const bool last /*=false*/) {
    if (!XMLDOM::Parse(d, buf, n, last)) {
        int ln;
        int cn;
        DOMString message = XMLDOM::GetError(d, &ln, &cn);
        throw xap_bad_xml ( message.c_str(), ln );
    }
}

    /** Generic tree walker, pre-order, depth first. */
void
DOMGlue_WalkTree ( DOMWalker* elf, Node* here ) {

    Node*	node;
    bool	doRest;

    assert  (elf != NULL );
    assert ( here != NULL);

    node = here;

    switch ( node->getNodeType() ) {
        case ELEMENT_NODE: {
            Element* e = dynamic_cast<Element*> (node);

            doRest = elf->enterElement ( e );
            if ( elf->finished() ) return;

			if ( doRest ) {
			
	            if  (node->hasChildNodes() ) {
	                NodeList* nl = node->getChildNodes();
	                int n = nl->getLength();
	                for ( int i = 0; i < n; i++ ) {
	                    DOMGlue_WalkTree ( elf, nl->item(i) );
	                    if ( elf->finished() ) return;
	                }
	            }

	            elf->exitElement(e);
	            if ( elf->finished() ) return;
			
			}
            break;
        }

        case TEXT_NODE: {
            Text* cd = dynamic_cast<Text*>(node);

            elf->text(cd);
            if ( elf->finished() ) return;

            break;
        }

        default: {
            elf->handleNode(node);
            if ( elf->finished() ) return;
        }

    }
}


} // XAPTk


/* ===== DOMDoc Module Functions (in xmldom) ===== */

namespace XMLDOM {

    /** Find an element by its tag name. */
Element*
FindFirstElement(Document* d, const DOMString& ns, const DOMString& tag, Element* root /*= NULL*/) {
    DW4_findElem elf(ns, tag, root);

	#ifdef XAP_DEBUG_VALIDATE
        long base = 0xADBEADBE;
        XAPTk::CheckStack(&base, NULL);
	#endif // XAP_DEBUG_VALIDATE
    XAPTk::DOMGlue_WalkTree(&elf, d->getDocumentElement());
	#ifdef XAP_DEBUG_VALIDATE
        // Reset
        XAPTk::CheckStack(NULL, NULL);
	#endif // XAP_DEBUG_VALIDATE
    return elf.m_elem;

}

} // XMLDOM


void
XAPTk_InitDOMGlue() {
    // No-op
}


void
XAPTk_KillDOMGlue() {
    // No-op
}


/*
$Log$
*/
