/* $Header: //xaptk/xaptk/NormTree.cpp#20 $ */
/* NormTree.cpp */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/

#ifdef BIG_COMMENT

The in-memory structure for all metadata for a specific schema is a
normalized tree, or NormTree. This is distinguished by the name of the
root element (m_rootElem): <XAPTK_NS_MXAP,XAPTK_TAG_META_ROOT>.

For metadata, the following maps different types of RDF data to NormTree
structures. The structures are shown as their XML serialization, but they
are actually maintained as DOM documents.  Note that the Qualified Name
(original prefix and local part) is stored as the element name.  Prefixes
are mapped to namespaces with the m_masterNSMap when expanded names are
required for comparison.

* Simple property with literal text value

RDF
    <rdf:RDF ...>
      <rdf:Description ...>
        <xap:Prop>TEXT
        </xap:Prop>
      </rdf:Description>
    </rdf:RDF>

NormTree (for "xap:")
    <mxap:mxap>
      <xap:Prop>TEXT
      </xap:Prop>
    </mxap:mxap>

* Simple property with rdf:value literal text value with original qualifier(s)

The rdf:value element replaced by a direct text child. Qualifiers are made siblings
of the text.  The attribute mxap:f is set to "V" to indicate that this
is a not an internal usage of rdf:value.

RDF
    <rdf:RDF ...>
      <rdf:Description ...>
        <xap:Prop parseType='Resource'>
            <rdf:value>TEXT1
            </rdf:value>
            <xap:Qualifier>TEXT2</xap:Qualifier>
        </xap:Prop>
      </rdf:Description>
    </rdf:RDF>

NormTree (for "xap:")
    <mxap:mxap>
      <xap:Prop mxap:f="V">TEXT1
        <xap:Qualifier>TEXT2</xap:Qualifier>
      </xap:Prop>
    </mxap:mxap>

* Simple property with XML literal text value

The rdf:parseType='Literal' construct is not included in our subset (we
do not process it, and we skip all elements which set it).  Instead, we use
an rdf:value construct with a qualifier indicating that the rdf:value is to
be interpreted as XML.  We detect this case, remove the rdf:value and the
qualifier, and set mxap:f to "X".

XML
    <dad><!--comment-->
        <kid1 a="1"/>
        <kid2>foo</kid2>
    </dad>

RDF
    <rdf:RDF ...>
      <rdf:Description ...>
        <xap:Prop parseType="Resource">
          <rdf:value>
            &lt;dad&gt;&lt;!--comment--&gt;
                &lt;kid1 a="1"/&gt;
                &lt;kid2&gt;foo&lt;/kid2&gt;
            &lt;/dad&gt;
          <rdf:value>
          <iX:features>X</iX:features>
        </xap:Prop>
      </rdf:Description>
    </rdf:RDF>

NormTree (for "xap:")
    <mxap:mxap>
      <xap:Prop mxap:f="X">
            &lt;dad&gt;&lt;!--comment--&gt;
                &lt;kid1 a="1"/&gt;
                &lt;kid2&gt;foo&lt;/kid2&gt;
            &lt;/dad&gt;
      </xap:Prop>
    </mxap:mxap>

* Simple property with rdf:resource value

The NormTree indicates that the text node is to be treated as a resource by
using mxap:f="R".

RDF
    <rdf:RDF ...>
      <rdf:Description ...>
        <xap:Prop rdf:resource="URL"/>
      </rdf:Description>
    </rdf:RDF>

NormTree (for "xap:")
    <mxap:mxap>
      <xap:Prop mxap:f="R">URL</xap:Prop>
    </mxap:mxap>

* Simple property with rdf:resource value and qualifiers

The NormTree indicates that the text node is to be treated as a resource by
using mxap:f="R".  There is no indication of the qualifiers, other than they show up in an enumeration.

RDF
    <rdf:RDF ...>
      <rdf:Description ...>
        <xap:Prop rdf:resource="URL" xap:Qualifier="TEXT2"/>
      </rdf:Description>
    </rdf:RDF>

NormTree (for "xap:")
    <mxap:mxap>
      <xap:Prop mxap:f="R">URL<xap:Qualifier>TEXT2</xap:Qualifier></xap:Prop>
    </mxap:mxap>

* Simple property with rdf:value that is an rdf:resource

The NormTree indicates that the text node is to be treated as a resource, and that the value is qualifed, by setting both features with mxap:f="RV".

RDF [Basic Syntax, Implicit rdf:Description]
    <rdf:RDF ...>
      <rdf:Description ...>
        <xap:Prop parseType="Resource">
          <rdf:value rdf:resource="URL"/>
          <xap:Qualifier="TEXT2"/>
        </xap:Prop>
      </rdf:Description>
    </rdf:RDF>

RDF [Abbreviated Syntax]
    <rdf:RDF ...>
      <rdf:Description ...>
        <xap:Prop rdf:resource="URL" xap:Qualifier="TEXT2"/>
      </rdf:Description>
    </rdf:RDF>

NormTree (for "xap:")
    <mxap:mxap>
      <xap:Prop mxap:f="RV">URL<xap:Qualifier>TEXT2</xap:Qualifier></xap:Prop>
    </mxap:mxap>

* Simple property with xml:lang attribute and text literal value

The attribute is simply copied into the element for the property (or the
memeber item, in the case of a structured container, see below).

RDF
    <rdf:RDF ...>
      <rdf:Description ...>
        <xap:Prop xml:lang="en">TEXT
        </xap:Prop>
      </rdf:Description>
    </rdf:RDF>

NormTree (for "xap:")
    <mxap:mxap>
      <xap:Prop xml:lang="en">TEXT
      </xap:Prop>
    </mxap:mxap>

* Nested description

Sub-properties become direct children in the NormTree. This example uses
children that are all simple text literal values, but any child, including
another nested description or an structured container (next item), can be
used.  The three main equivalent variations of RDF sub-property syntax are shown.

RDF [Basic Syntax, Explicit rdf:Description]
    <rdf:RDF ...>
      <rdf:Description ...>
        <xap:Prop>
          <rdf:Description>
            <xap:Sub1>A</xap:Sub1>
            <xap:Sub2>B</xap:Sub2>
          </rdf:Description>
        </xap:Prop>
      </rdf:Description>
    </rdf:RDF>

RDF [Basic Syntax, Implicit rdf:Description]
    <rdf:RDF ...>
      <rdf:Description ...>
        <xap:Prop parseType="Resource">
          <xap:Sub1>A</xap:Sub1>
          <xap:Sub2>B</xap:Sub2>
        </xap:Prop>
      </rdf:Description>
    </rdf:RDF>

RDF [Abbreviated Syntax]
    <rdf:RDF ...>
      <rdf:Description ...>
        <xap:Prop xap:Sub1="A" xap:Sub2="B"/>
      </rdf:Description>
    </rdf:RDF>

NormTree (for "xap:")
    <mxap:mxap>
      <xap:Prop>
        <xap:Sub1>A</xap:Sub1>
        <xap:Sub2>B</xap:Sub2>
      </xap:Prop>
    </mxap:mxap>


* Structured constainer

Member items are special elements that identify the container type: mxap:Alt
for rdf:alt, mxap:bag for rdf:Bag, and mxap:seq for rdf:Seq.

RDF
    <rdf:RDF ...>
      <rdf:Description ...>
        <xap:Prop>
          <rdf:Alt>
            <rdf:li xml:lang="en">TEXT-EN</rdf:li>
            <rdf:li xml:lang="fr">TEXT-FR</rdf:li>
          </rdf:Alt>
        </xap:Prop>
      </rdf:Description>
    </rdf:RDF>

NormTree (for "xap:")
    <mxap:mxap>
      <xap:Prop>
        <mxap:alt xml:lang="en">TEXT-EN</mxap:alt>
        <mxap:alt xml:lang="fr">TEXT-FR</mxap:alt>
      </xap:Prop>
    </mxap:mxap>

**************************

The following BNF productions describe the relationship between simple properties (xap_simple), nested descriptions (xap_description), and structured containers (xap_container).

m_rootElem      ::= ( xap_simple | xap_description | xap_container )*
xap_simple      ::= [text, or rdf:value with qualifiers]
xap_description ::= ( xap_simple | xap_description | xap_container )*
xap_container   ::= ( alt )+ | ( bag )+ | ( seq )+
alt             ::= "mxap:alt" member
bag             ::= "mxap:bag" member
seq             ::= "mxap:seq" member
member          ::= ( xap_simple | xap_description | xap_container )*

Of the features, each is exclusive of the others, except that V may be combined with R (rdf:value may be an rdf:resource).

#endif // BIG_COMMENT

/* ===== Includes ===== */

#ifndef XAP_FORCE_NORMAL_ALLOC
	#include "XAPTkAlloc.h"
#endif

#include "XAPConfigure.h"

#include "DOM.h"
#include "DOMGlue.h"
#include "DOMDoc.h"
#include "XAPTkInternals.h"
#include "NormTree.h"
#include "Paths.h"
#include "XMPInitTerm.h"

#include <math.h>

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

/* ===== "Package" Module Functions ===== */

XAPTk_StringByString*
MetaXAP_GetMasterNSMap();

/* +++++ BEGIN Outside of namespace XAPTk +++++ */

/*
For some reason the compiler complains if any of the following declarations are inside of the XAPTk namespace.
*/

/* ===== Types ===== */

XAP_ENUM_TYPE ( XAPTk_StepStates ) {
    sInit,          // expr | leaf | childset
    sEmpty,         // leaf ::= '.'
    sAttr,          // leaf ::= attr | childset ::= '@' '*'
    sList,          // childset ::= childElems | nodep ::= childElems
    sName,          // expr | leaf ::= 'comment()' ...
    sPred,          // expr ::= nodep '[ pred ']'
    pAttr,          // lhs ::= attr
    pOrd,           // pred ::= ordinal
    pAName,         // attr ::= '@' name
    pName,          // pred ::= lhs '=' literal | pred :: = 'last()'
    pFunc,          // pred ::= 'last()'
    pMatch,         // pred ::= lhs '=' literal
    pVal            // literal
};

XAP_ENUM_TYPE ( XAPTk_TokenType ) {
    tDot,
    tAt,
    tStar,
    tLB,
    tRB,
    tParens,
    tEquals,
    tChars
};

class XAPTk_Token {
public:
    XAPTk_TokenType type;
    string::iterator begin;
    string::iterator end;
};

typedef class vector<class XAPTk_Token> VectOToken;

/* ===== Module Operators ===== */

inline ostream&
operator<<(ostream& os, DOMString& ds) {
    const string s = ds;
    return os << s;
}

inline bool
operator<(const class XAPTk_Token& lhs, const class XAPTk_Token& rhs) {
    return(&lhs < &rhs);
}

inline bool
operator==(const class XAPTk_Token& lhs, const class XAPTk_Token& rhs) {
    return(&lhs == &rhs);
}

/* ===== Constants ===== */

/* +++++ END Outside of namespace XAPTk +++++ */

namespace XAPTk {

/* ===== Forwards ===== */

static bool GetFeatures(Element* e, XAPFeatures& f);

/* ===== Support Classes ===== */

    /*C*
    Support for enumerate methods. Load property paths
    into a vector.
    */

class DW4_enumeratePropElem : public DOMWalker {
public:
    /* ===== Instance Variables ===== */
    NormTree* m_norm;
    std::string m_ns;
    Element* m_rootElem;
    Paths* m_enumPaths;
    const int m_steps;
    XAPTk_StackOfString m_strPaths;

    /* ===== Public Constructor ===== */
    DW4_enumeratePropElem() : // Default
        m_norm(NULL),
        m_rootElem(NULL),
        m_enumPaths(NULL),
        m_steps(0),
        m_finished(false)
    {}

    DW4_enumeratePropElem(NormTree* norm, const std::string& ns, Element* rootElem, Paths* enumPaths, const int steps) :
        m_norm(norm),
        m_ns(ns),
        m_rootElem(rootElem),
        m_enumPaths(enumPaths),
        m_steps(steps),
        m_finished(false)
    {}

    /* ===== Public Destructor ===== */
    virtual ~DW4_enumeratePropElem() {}

    /* ===== Public Member Functions ===== */

        /** Found an element node. */
    inline virtual bool
    enterElement ( Element* e ) {
        std::string step;
        std::string path;
        long int steps;

		if ( e == m_rootElem ) return true;	// Don't do anything at the root, just continue on to the children.
		
        // Get the canonical step for the current node.
        NormTree::CanonicalStep ( m_norm, e, step );
        if ( step.empty() ) {
            /*
            This happens if we skipped a helper node, inserted
            for the sake of managing the norm tree.
            */
            return true;	// Continue processing this element and its children.
        }
        if ( m_strPaths.empty() ) {
            path = step;							// Start a new stack with relative paths.
	   	} else {
            path = m_strPaths.top() + '/' + step;	// Add this step to the parent's path.
        }
        
        steps = (long int) m_strPaths.size() + 1;		// Where are we with this node included?
        if ( (m_steps == 0) || (steps < m_steps) ) {
	        m_strPaths.push ( path );					// More to go, push this path on the stack.
        } else {
            assert ( steps == m_steps );
            m_enumPaths->addPath ( m_ns, path );		// Add this node's path and stop going down this subtree.
            return false;
        }
        
        // Add attributes for this node before children.
        NamedNodeMap* attrs = e->getAttributes();
        if ( attrs != NULL ) {
            // Skip the case that CanonicalStep handled
            Element* parent = dynamic_cast<Element*>(e->getParentNode());
            XAPValForm form = NormTree::WhatsMyForm(parent);
            if ( ((form != xap_container) && (NormTree::WhatsMyForm(e) == xap_simple)) ||
				 ((form == xap_container) && (NormTree::WhatsMySCType(parent) != xap_alt)) ) {
                // Deal with attributes
                const unsigned long maxa = attrs->getLength();
                for ( unsigned long i = 0; i < maxa; ++i ) {
                    Attr* a = dynamic_cast<Attr*>(attrs->item(i));
                    const std::string aName = a->getName();
                    /*
                    GOTCHA: We only care about xml:lang.  All other attributes
                    are ignored for the purposes of enumeration.
                    */
                    if ( aName != XAPTK_ATTR_XML_LANG ) continue;
                    path = m_strPaths.top() + "/@" + aName;
                    m_enumPaths->addPath ( m_ns, path );
                    break;
                }
            }
        }

		// ------------------------------
		// Add this node if it is a leaf.
		
		bool isLeaf = false;
		size_t eCount;
		size_t tCount;
		
		isLeaf = !DOMGlue_CountNodes ( e, eCount, tCount );
		if ( (eCount == 0) && (tCount == 1) ) {
			isLeaf = true;
		} else if ( eCount > 0 ) {
			XAPFeatures f;
			m_norm->getFeatures ( e, f );
			isLeaf = ( (f & (XAP_FEATURE_XML|XAP_FEATURE_RDF_VALUE|XAP_FEATURE_RDF_RESOURCE)) != 0 );
		}
		
		if ( isLeaf ) {
			#ifdef XAP_DEBUG_VALIDATE
				long top = 0xADBEADBE;
				XAPTk::CheckStack(NULL, &top);
			#endif // XAP_DEBUG_VALIDATE
			m_enumPaths->addPath ( m_ns, m_strPaths.top() );
		}
        
        return true;	// Continue processing this element and its children.
        
    }

        /** All children of element done. */
    inline virtual void
    exitElement(Element* /* e */ ) {

        if ( ! m_strPaths.empty() ) m_strPaths.pop();

    }

    inline virtual void
    handleNode(Node* node) {
        std::string t(DOMGlue_NodeTypes[node->getNodeType()]);
        t = "Unknown node type: "+t;
        throw xap_bad_xap ( t.c_str() );
    }

        /** Abort the tree walk. */
    inline virtual bool
    finished() {
        return m_finished;
    }

        /** Found a char data node. */
    inline virtual void
    text(CharacterData* /* cd */ ) {
        // NO-OP
    }

private:
    bool m_finished;
};



class DW4_dupTree : public DOMWalker {
public:
    /* ===== Instance Variables ===== */
    Document* m_cloneDoc;

    /* ===== Public Constructor ===== */
    DW4_dupTree() : // Default
        m_cloneDoc(NULL),
        m_current(NULL),
        m_finished(false)
    {}

    DW4_dupTree(Document* m_cloneDoc) :
        m_cloneDoc(m_cloneDoc),
        m_current(NULL),
        m_finished(false)
    {}

    /* ===== Public Destructor ===== */
    virtual ~DW4_dupTree() {}

    /* ===== Public Member Functions ===== */

        /** Found an element node. */
    inline virtual bool
    enterElement(Element* e) {
        if (m_current == NULL) {
            /*
            This must be the doc element of the original, so we use
            our own existing doc element, which should already be identical.
            */
            m_current = m_cloneDoc->getDocumentElement();
            assert(m_current->getTagName() == e->getTagName());
            return true;	// Continue processing this element and its children.
        }
        // Clone the node in a new tree
        Element* dolly = m_cloneDoc->createElement(e->getTagName());
        // Attributes?
        NamedNodeMap* origAttrs = e->getAttributes();
        if (origAttrs != NULL) {
            const unsigned long n = origAttrs->getLength();
            for (unsigned long i = 0; i < n; ++i) {
                Node* an = origAttrs->item(i);
                Attr* a = dynamic_cast<Attr*>(an);
                assert(a != NULL);
                dolly->setAttribute(a->getName(), a->getValue());
            }
        }
        // Link up with ancestor
        m_current->appendChild(dolly);
        m_current = dolly;
        return true;	// Continue processing this element and its children.
    }

        /** All children of element done. */
    inline virtual void
    exitElement(Element* /* e */ ) {
        if (m_current == m_cloneDoc->getDocumentElement())
            return;
        Node* p = m_current->getParentNode();
        m_current = dynamic_cast<Element*>(p);
    }

    inline virtual void
    handleNode(Node* /* node */ ) {
        // NO-OP: skip other node types
    }

        /** Abort the tree walk. */
    inline virtual bool
    finished() {
        return m_finished;
    }

        /** Found a char data node. */
    inline virtual void
    text(CharacterData* cd) {
        Text* tn = m_cloneDoc->createTextNode(cd->getData());
        m_current->appendChild(tn);
#ifdef XAP_DEBUG_VALIDATE
            long top = 0xADBEADBE;
            XAPTk::CheckStack(NULL, &top);
#endif // XAP_DEBUG_VALIDATE
    }

private:
    Element* m_current;
    bool m_finished;
};

/* ===== Class NormTree ===== */

/* ===== Static (Class) Functions ===== */

    /** Generate a standardized XPath step for this element. */
DECL_STATIC void
NormTree::CanonicalStep ( NormTree* norm, Element* e, std::string& step ) {
    /*
    Canon:
    * If element is a member of a structured container, the general
      canonical pattern is "*[ord]", where ord is the ordinal of
      the member.
    * One exception is the case of a member of an Alt that is selected
      by xml:lang.  In this case, the pattern we use is
      "*[@xml:lang='xx']", where xx is the value of the xml:lang attribute.
    * All other elements are just their names. If a QName belongs to
      a registered namespace, we favor the registered prefix.
    */
    bool mightBeAltByLang = false;
    std::string localPart;
    std::string prefix;

    step = "";

    const std::string domTagName = e->getTagName();
    StripPrefix ( domTagName, &localPart, &prefix );
    if ( localPart == "mxap" ) return; // Skip helper nodes
    
    #if 0
    	cout << "In NormTree::CanonicalStep for " << prefix << ":" << localPart << endl;
    #endif

    size_t myOrd = NormTree::WhatsMyOrd ( e );

    if ( myOrd > 0 ) {

        Element* parent = dynamic_cast<Element*> ( e->getParentNode() );
        if ( (NormTree::WhatsMyForm ( parent ) == xap_container) &&
        	 (NormTree::WhatsMySCType ( parent ) == xap_alt) ) {
            mightBeAltByLang = true;
        }
        std::string ord;
        ToString(myOrd, ord);
        localPart = "*[" + ord + "]";
        step = localPart;

    } else if ( prefix.empty() ) {
    
    	step = localPart;
    
    } else {

        XAPTk_StringByString::const_iterator def = norm->m_nsMap.find ( prefix );
        XAPTk_StringByString::const_iterator gloPre = norm->m_masterNSMap->find ( prefix );
 
        if ( def != norm->m_nsMap.end() ) {

            // Check namespace prefix
            XAPTk_StringByString::const_iterator reg = norm->m_masterNSMap->begin();
            XAPTk_StringByString::const_iterator regDone = norm->m_masterNSMap->end();

            for ( ; reg != regDone; ++reg ) {
                if ( reg->second == def->second ) break;
            }

            if ( (reg == regDone) || (def->second == norm->m_ns) ) {
                /*
                There's no registered prefix, or the namespace matches
                the default, so omit the prefix.
                */
                step = localPart;
            } else if ( reg->first != def->first ) {
                // Same namespace, use registered prefix if different
                step = reg->first + ":" + localPart;
            } else {
                step = prefix + ":" + localPart;
            }

        } else if (gloPre != norm->m_masterNSMap->end()) {
        
        	#if 0
        	{
        		XAPTk_StringByString::const_iterator map;
        		cout << "Uh oh! Prefix in master map but not local map" << endl;
        		cout << "Local map:" << endl;
        		for ( map = norm->m_nsMap.begin(); map != norm->m_nsMap.end(); ++map ) {
        			cout << "   " << map->first << " = " << map->second << endl;
        		}
        		cout << "Master map:" << endl;
        		for ( map = norm->m_masterNSMap->begin(); map != norm->m_masterNSMap->end(); ++map ) {
        			cout << "   " << map->first << " = " << map->second << endl;
        		}
        	}
        	#endif

            // Only allow RDF as global!
            if ( gloPre->first != XAP_PREFIX_RDF ) {
                throw xap_bad_path ();
            }
            step = gloPre->first + ":" + localPart;

        } else {

            assert ( prefix == "Registered Prefix" ); //CantHappen!

        }

    }

    NamedNodeMap* attrs = e->getAttributes();
    if ( (! mightBeAltByLang) || (attrs == NULL) ) return;

    // Deal with attributes
    const unsigned long maxa = attrs->getLength();
    //GOTCHA: We're only looking for an xml:lang attribute.
    for ( unsigned long i = 0; i < maxa; ++i ) {
        Attr* a = dynamic_cast<Attr*> ( attrs->item ( i ) );
        const std::string aName ( a->getName() );
        if ( aName != XAPTK_ATTR_XML_LANG ) continue;
        if ( mightBeAltByLang ) {
            // It definitely is, so catch up
            const std::string domAVal ( a->getValue() );
            step = "*[@xml:lang='" + domAVal + "']";
        }
        break;
    }
}

    /** Convert path into canonical path. */
DECL_STATIC void
NormTree::CanonicalPath(NormTree* norm, const std::string& orig, std::string& canon, Node* propOrAttr /*= NULL*/) {
    /*
    Grab the element at the end of orig, and then working backwards,
    create the canonical path.
    */
    Element* e = 0;
    Node* n = 0;

    canon = "";

    if (propOrAttr == NULL) {
        n = norm->evalXPath(orig);
        if (n == NULL) {
            throw xap_no_match ();
        }
        e = dynamic_cast<Element*>(n);
    } else {
        e = dynamic_cast<Element*>(propOrAttr);
    }

    if (e == NULL) {
        assert(propOrAttr == NULL); //GOTCHA: NormTree only supports xml:lang attribute
        // It has to be an attribute (leaf node)
        Attr* a = dynamic_cast<Attr*>(n);
        assert(a != NULL);
        // Just chop off end of orig path and try again
        std::string::size_type slash = orig.rfind("/");
        std::string mini = orig.substr(0, slash);
        CanonicalPath(norm, mini, canon);
        // Now add it back
        canon += orig.substr(slash);
        return;
    }

    // Get canonical step, loop up ancestors
    const Element* first = e;
    std::string step;
    while (e != norm->getRoot()) {
        if (e != first) {
            canon.insert(0, "/");
        }
        CanonicalStep(norm, e, step);
        canon.insert(0, step);
        e = dynamic_cast<Element*>(e->getParentNode());
    }
}

    /** Create a new, empty metadata tree. */
DECL_STATIC NormTree*
NormTree::NewMeta(const string& ns, XAPTk_StringByString* masterNSMap /*=NULL*/) {
    NormTree* t = new XMP_Debug_new NormTree(ns, XAPTK_TAG_META_ROOT, masterNSMap);
    t->m_rootElem->setAttribute(XAPTK_ATTR_XMLNS_MXAP, XAPTK_NS_MXAP);
    if (masterNSMap != NULL) {
        // Register the prefix for this tree's namespace
        XAPTk_StringByString::const_iterator i;
        XAPTk_StringByString::const_iterator done = masterNSMap->end();
        /* [first: prefix, second: ns] */
        // ??? The loop better succeed?
        for (i = masterNSMap->begin(); i != done; ++i) {
            if (i->second == ns) {
                t->setNSDef(i->first, i->second);
                break;
            }
        }
    }
    return(t);
}

    /** Look up the NormTree by schema or ID. */
DECL_STATIC NormTree*
NormTree::GetNormTree(const NormTreeByStr& bySchema, const std::string& key) {
    NormTreeByStr::const_iterator n = bySchema.find(key);
    if (n == bySchema.end())
        return(NULL);
    else
        return(n->second);
}

    /** Get container type. */
DECL_STATIC XAPStructContainerType
NormTree::WhatsMySCType(Element* me) {
    assert(me != NULL);
    /* Just look at the first child. */
    Element* k1 = dynamic_cast<Element*>(me->getFirstChild());
    if (k1 == NULL) {
        throw xap_no_match ( "Should be caught by caller and filled in." );
    }
    const std::string n1 = k1->getTagName();
    if (n1 == XAPTK_TAG_META_ALT)
        return(xap_alt);
    if (n1 == XAPTK_TAG_META_BAG)
        return(xap_bag);
    if (n1 == XAPTK_TAG_META_SEQ)
        return(xap_seq);
    throw xap_no_match ( "Should be caught by caller and filled in." );
    return(xap_sct_unknown); // Quiet compiler
}


    /** Get form of value. */
DECL_STATIC XAPValForm
NormTree::WhatsMyForm(Element* me) {
    /*
    xap_container: if first child is Alt, Bag, or Seq.
    xap_description: if number of element children > 0, and not rdf:value
    xap_simple: has 0-1 text child, or rdf:value
    Otherwise, xap_unknown.
    */
    std::string n1;

    if (me == NULL)
        return(xap_unknown);
    NodeList* kids = me->getChildNodes();
    const unsigned long mxk = kids->getLength();
    if (mxk == 0)
        return(xap_simple); //Special case: will become simple
    Element* first = NULL;
    int eCount = 0;
    for (unsigned long i = 0; i < mxk; ++i) {
        Node* kid = kids->item(i);
        Element* e = dynamic_cast<Element*>(kid);
        if (e != NULL) {
            ++eCount;
            if (first == NULL)
                first = e;
        }
    }

    if (eCount > 0 && first != NULL) {
        // Check special cases
        XAPFeatures f;
        XAPTk::GetFeatures(me, f);
        if ((f & (XAP_FEATURE_XML|XAP_FEATURE_RDF_VALUE|XAP_FEATURE_RDF_RESOURCE)) != 0) {
            return(xap_simple);
        }
        n1 = first->getTagName();
        if (n1 == XAPTK_TAG_META_ALT || n1 == XAPTK_TAG_META_BAG || n1 == XAPTK_TAG_META_SEQ) {
            return(xap_container);
        } else {
            return(xap_description);
        }
    }
    Text* t1 = dynamic_cast<Text*>(me->getFirstChild());
    if (t1 != NULL)
        return(xap_simple);
    else
        return(xap_unknown);
}

DECL_STATIC size_t
NormTree::WhatsMyOrd(Element* me) {
    const string myName = me->getTagName();
    if (myName == XAPTK_TAG_META_ALT || myName == XAPTK_TAG_META_BAG || myName == XAPTK_TAG_META_SEQ) {
        Element* parent = dynamic_cast<Element*>(me->getParentNode());
        NodeList* kids = parent->getChildNodes();
        const unsigned long maxk = kids->getLength();
        for (unsigned long i = 0; i < maxk; ++i) {
            if (me == kids->item(i))
                return(i+1);
        }
    }
    return(0);
}

/* ===== Public Constructor (For Cloning)  ===== */

NormTree::NormTree(NormTree* orig, const std::string& ns) :
    m_currentElem(NULL),
    m_masterNSMap(orig->m_masterNSMap),
    m_doc(NULL),
    m_rootElem(NULL),
    m_ns(ns),
    m_rootTag(orig->m_rootTag)
{
    // Common initialization (gee I miss Java this(...)!)
    m_doc = DOMGlue_NewDoc();
    assert(m_doc->getDocumentElement() == NULL);
    m_rootElem = m_doc->createElement(m_rootTag);
    m_rootElem->setAttribute(XAPTK_ATTR_NS, m_ns);
    m_doc->appendChild(m_rootElem);

    // Copy maps
    m_byPath = orig->m_byPath;
    m_nsMap = orig->m_nsMap;

    // Duplicate the tree
    DW4_dupTree elf(m_doc);
#ifdef XAP_DEBUG_VALIDATE
        long base = 0xADBEADBE;
        XAPTk::CheckStack(&base, NULL);
#endif // XAP_DEBUG_VALIDATE
    DOMGlue_WalkTree(&elf, orig->m_doc->getDocumentElement());
#ifdef XAP_DEBUG_VALIDATE
        // Reset
        XAPTk::CheckStack(NULL, NULL);
#endif // XAP_DEBUG_VALIDATE
}

/* ===== Public Destructor  ===== */

NormTree::~NormTree() {
    try {
        delete m_doc;
    } catch (...) {}
}

/* ===== Public Member Functions ===== */

void
NormTree::contractName ( const string& ns, const string& localPart, string& qName )
{

    XAPTk_StringByString* masterNSMap = MetaXAP_GetMasterNSMap();
    XAPTk_StringByString::const_iterator i = masterNSMap->begin();
    XAPTk_StringByString::const_iterator done = masterNSMap->end();
    
    #if 0
    	cout << "In NormTree::contractName for " << localPart << " in " << ns << endl;
    	cout.flush();
    #endif

    for ( ; i != done; ++i ) {
    	#if 0
    		cout << "   Checking master " << i->first << " = " << i->second << endl;
    		cout.flush();
    	#endif
        if ( i->second == ns ) {
            qName = i->first + ":" + localPart;
            #if 0
            	cout << "   returning " << qName << endl;
    			cout.flush();
            #endif
            return;
        }
    }

    // Try local NSDefs
    // *** This is now useless? Everything should be in master map.
    done = m_nsMap.end();
    for ( i = m_nsMap.begin(); i != done; ++i ) {
    	#if 0
    		cout << "   Checking local " << i->first << " = " << i->second << endl;
    		cout.flush();
    	#endif
        if ( i->second == ns ) {
            qName = i->first + ":" + localPart;
            #if 0
            	cout << "   returning " << qName << endl;
  			  	cout.flush();
            #endif
            return;
        }
    }

    #if 0
    	cout << "   returning " << qName << endl;
  		cout.flush();
    #endif
    qName = localPart;

}


bool
NormTree::countMembers(const std::string& xpath, size_t& ct) {
    std::string::const_reverse_iterator r = xpath.rbegin();
    if (*r != '*') {
        return(false);
    }
    Node* node = this->evalXPath(xpath);
    Element* e = dynamic_cast<Element*>(node);
    return this->countMembers(e, ct);
}


bool
NormTree::countMembers(Element* e, size_t& ct) {
    if (e == NULL
    || XAPTk::NormTree::WhatsMyForm(e) != xap_container) {
        return(false);
    }
    /*
    GOTCHA: We need to count rdf:li child elements, but because
    RDF 1.0 only allows rdf:li as a child of a container, we just
    count the elements.  This is be a source of errors if RDF changes
    in the future to allow non rdf:li children.
    */
    ct = XMLDOM::CountElements(e);
    return(true);
}


Element*
NormTree::createElement(Element* parent, const size_t ord) {
    NodeList* seq = parent->getChildNodes();
    const unsigned long imax = seq->getLength();
    //GOTCHA: Ord must be at the end of the list
    if (ord != imax+1) {
        throw xap_bad_path ();
    }
    /*
    Get the tag name of the first child and create a new child to tack on
    the end.
    */
    Element* n1 = dynamic_cast<Element*>(seq->item(0));
    if (n1 == NULL) {
        throw xap_bad_path ();
    }
    const string n1Name = n1->getTagName();
    Element* e = this->createElement(n1Name);
    parent->appendChild(e);
    return(e);
}

    /** Create element with qualified name. */
Element*
NormTree::createElement(const string& localPart) {
    string qName;
    string::size_type i = localPart.find(":");
    if (i == string::npos) {
        this->contractName(m_ns, localPart, qName);
    } else {
        qName = localPart; // Already qualified
    }
    return m_doc->createElement(qName);
}


    /** List all paths to leaf nodes from Element. */
XAPPaths*
NormTree::enumerate(Element* e /*= NULL*/, XAPPaths* context /*= NULL*/, const int steps /*=0*/) {
    Paths* enumPaths =
        (context == NULL) ? Paths::New() : dynamic_cast<Paths*>(context);
    Element* root = (e == NULL) ? m_rootElem : e;
    DW4_enumeratePropElem elf ( this, m_ns, root, enumPaths, steps );
#ifdef XAP_DEBUG_VALIDATE
        long base = 0xADBEADBE;
        XAPTk::CheckStack ( &base, NULL );
#endif // XAP_DEBUG_VALIDATE
    DOMGlue_WalkTree ( &elf, root );
#ifdef XAP_DEBUG_VALIDATE
        //Reset
        XAPTk::CheckStack(NULL, NULL);
#endif // XAP_DEBUG_VALIDATE
    return(enumPaths);
}

    /** Evaluate the XPath to a Node (Attr or Element). */
Node*
NormTree::evalXPath(const string& xpath, const bool isRequired /*= false*/) {
    return this->evalXPath(xaptk_throw, xpath, isRequired);
}

    /** With throw mode, evaluate the XPath to a Node (Attr or Element). */
Node*
NormTree::evalXPath(const XAPTk_ThrowMode mode, const string& xpath, const bool isRequired /*= false*/) {
    /*
    This is the heart of the implementation.  All methods that take an xpath
    as a parameter end up calling this routine.

    Each slash in the path effectively means "evaluate this subexpression".

    NOTE: exceptions may still be thrown even if the mode is "xaptk_no_throw".
    Invalid paths still cause exceptions to be raised.  The case in question
    is what to do when a *valid* path evaluates to a non-existant node.
    If xaptk_throw is the mode, an exception is thrown, otherwise NULL
    is simply returned.
    */
    string nodePattern;
    string pred;
    Node* ret = NULL;
    Node* current = m_rootElem;
    Node* backup = NULL;
    XAPTk_VectorOfString expr;

    XAPTk::ExplodePath(xpath, expr);

    const size_t maxn = expr.size();
    VectOToken tokens;
    for (size_t iStep = 0; iStep < maxn; ++iStep) {
        tokens.clear();

        /* Lexical analysis */
        string& full = expr[iStep];
        string::iterator t = full.begin();
        const string::iterator tail = full.end();
        XAPTk_Token token;
        token.begin = tail;
        token.type = tChars;
        while (t != tail) {
            XAPTk_TokenType type;
            switch (*t) {
                case '*': type = tStar; break;
                case '@': type = tAt; break;
                case '[': type = tLB; break;
                case ']': type = tRB; break;
                case '(': type = tParens; ++t; continue;
                case ')': type = tParens; break;
                case '=': type = tEquals; break;
                default: {
                    type = tChars;
                    if (token.begin == tail) {
                        token.begin = t;
                    }
                    token.end = t;
                }
            }
            if (type == tChars) {
                token.type = type;
            } else {
                if (token.type == tChars && token.begin != tail) {
                    // We were collecting tChars
                    token.end = (type == tParens) ? t-1 : t;
                    tokens.push_back(token);
                    token.begin = tail; // reset
                }
                // Push the token
                token.type = type;
                tokens.push_back(token);
                token.type = tChars; // reset
            }
            ++t;
        }
        if (token.type == tChars && token.begin != tail) {
            // final tChars
            token.end = t;
            tokens.push_back(token);
        }

        /*
        We start out with a current node which is the root node.
        Each step should evaluate to a replacement for the current
        node which matches a child or an attribute.
        */

        /* Parse and evaluate step */

        XAPTk_StepStates state = sInit;
        const size_t maxt = tokens.size();
        // Node name
        long nodeName = -1;
        bool isAll = false;
        // Stuff for matching predicate
        long lhs = -1;
        long rhs = -1;
        bool isAttr = false;

        CERR_PR(full);
        CERR_PR(" => ");
        CERR_PATH("BEFORE: ", m_rootElem, current);

        for (size_t tix = 0; tix < maxt; ++tix) {
            VectOToken::const_reference token = tokens[tix];
            switch (state) {
                case sInit: {
                    if (token.type == tDot)
                        state = sEmpty;
                    else if (token.type == tAt)
                        state = sAttr;
                    else if (token.type == tStar)
                        state = sList;
                    else if (token.type == tChars) {
                        state = sName;
                        nodeName = tix;
                        if (tix+1 == maxt) {
                            /* We have no more tokens, so this is the final
                               name to match. */
                            const XAPTk_Token& nodeToken = tokens[nodeName];
                            string nName(nodeToken.begin, nodeToken.end);
                            backup = current;
                            Element* curElem = dynamic_cast<Element*>(current);
                            if (!curElem->hasChildNodes())
                                current = NULL;
                            else
                                current = this->selectChild(curElem, nName);
                            if (current == NULL) {
                                if (isRequired) {
                                    this->validateNSPrefix(nName);
                                    current = this->createElement(nName);
                                    backup->appendChild(current);
                                } else
                                    return(NULL);  // Not found!
                            }
                        }
                    } else { // Unrecognized token
                        throw xap_bad_path ();
                    }
                    break;
                }
                case sEmpty: {
                    /*
                    A '.' at the end of the path means this node
                    right cheer.  If valid, assume this is the last
                    step.
                    */
                    if (tix+1 < maxt) { // Must be last token in step!
                        throw xap_bad_path ();
                    }
                    // Setting "ret" to non-NULL breaks out of loop
                    ret = current; // We're done!
                    break;
                }
                case sAttr: {
                    if (token.type == tStar) {
                        /*
                        Paths that end with "@*" can only be passed to
                        count(). Therefore, return the element that contains
                        the attributes to be counted, and count() will do
                        the rest.
                        */
                        if (tix+1 < maxt) { // Must be last token in step!
                            throw xap_bad_path ();
                        }
                        ret = current; // We're done!
                    } else if (token.type == tChars) {
                        /* @name selects an attribute of the current node. */
                        if (tix+1 < maxt) { // Must be last token in step!
                            throw xap_bad_path ();
                        }
                        Element* e = dynamic_cast<Element*>(current);
                        // Is current element?
                        if (e == NULL && current != NULL) {
                            // Can only eval attributes on elements
                            throw xap_bad_path ();
                        }
                        if (current == NULL) {
                            // Prefix of path doesn't match anything
                            if (mode == xaptk_throw) {
                                throw xap_bad_path ();
                            } else {
                                return(NULL);
                            }
                        }
                        string aName(token.begin, token.end);
                        Attr* a = e->getAttributeNode(aName);
                        if (isRequired && a == NULL) {
                            e->setAttribute(aName, "");
                            /*
                            REVISIT: attribute needs a value, empty string seems the only choice.
                            */
                            a = e->getAttributeNode(aName);
                        }
                        /* We allow NULL attributes: wasn't defined, not found. */
                        current = a; // We're done!
                    } else { // Unrecognized token after '@'
                        throw xap_bad_path ();
                    }
                    break;
                }
                case sList: {
                    /*
                    Paths that end with "*" can only be passed to
                    count(). Therefore, return the parent element
                    and count will handle the rest.  If the '*' isn't
                    at the end of the path, we need to keep parsing.
                    */
                    if (token.type == tLB) {
                        state = sPred;
                        isAll = true;
                        nodeName = tix-1;
                        /*
                        We need to fix up the token as if it were tChars for the pVal state,
                        so we need a writeable reference to the token.
                        */
                        VectOToken::reference wToken = tokens[tix-1];
                        assert(wToken.type == tStar);
                        wToken.begin = full.begin();
                        wToken.end = wToken.begin+1;
                    } else if (tix+1 < maxt) { // Must be last token in step!
                        throw xap_bad_path ();
                    } else
                        ret = current; // We're done
                    break;
                }
                case sName: {
                    /*
                    nodeName is either the name of a child element, or it
                    is the name of a function.
                    */
                    if (token.type == tLB)
                        state = sPred;
                    else if (token.type == tParens) {
                        /*
                        We only support "comment()" and "processing-instruction()".
                        */
                        VectOToken::const_reference nToken = tokens[nodeName];
                        string funcName(nToken.begin, nToken.end);
                        //REVISIT: Need constants for strings?
                        if (funcName != "comment"
                        || funcName != "processing-instruction"
                        || tix+1 < maxt) {
                            // Unrecognized function
                            throw xap_bad_path ();
                        }
                        ret = NULL; //PENDING: See comment below.
                        /*
                        When underlying DOM supports processing instruction
                        and comment nodes, evaluate to those nodes.  For
                        now, act as if those nodes don't exist.
                        */
                        break;
                    } else {
                        // Unrecognized name token in path
                        throw xap_bad_path ();
                    }
                    break;
                }
                case sPred: {
                    /*
                    The predicate is either an ordinal, or a match
                    for a node or attribute.
                    */
                    if (token.type == tAt)
                        state = pAttr;
                    else if (token.type == tChars) {
                        // Could be an ordinal number
                        for (string::iterator o = token.begin; o < token.end; ++o) {
                            if (*o < '0' || *o > '9') {
                                state = pName;
                                break;
                            }
                        }
                        if (state != pName)
                            state = pOrd;
                        else if (*token.begin >= '0' && *token.begin <= '9') {
                            /* Bad ordinal or name, like "3foo". */
                            throw xap_bad_path ();
                        }
                        lhs = tix;
                    } else {
                        // Unrecognized token in path
                        throw xap_bad_path ();
                    }
                    break;
                }
                case pAttr: {
                    /*
                    This should be the left-hand side of a match,
                    and its an attribute name.
                    */
                    if (token.type == tChars) {
                        state = pAName;
                        lhs = tix; // Remember this token
                        isAttr = true;
                    } else {
                        // Unrecognized token after '[@'
                        throw xap_bad_path ();
                    }
                    break;
                }
                case pOrd: {
                    /*
                    Convert the string to an ordinal, and return
                    the child at that position.
                    */
                    if (token.type == tRB) {
                        assert(lhs > -1);
                        if (tix+1 < maxt) { // Must be last token in step!
                            throw xap_bad_path ();
                        }
                        const XAPTk_Token& ordToken = tokens[lhs];
                        string anOrd(ordToken.begin, ordToken.end);
                        long int ord = atol(anOrd.c_str());
                        backup = current;
                        current = this->selectChild(dynamic_cast<Element*>(current), ord);
                        if (current == NULL) {
                            if (isRequired) {
                                current = this->createElement(dynamic_cast<Element*>(backup), ord);
                            } else
                                return(NULL); // No match!
                        }
                        // No new state, we're done
                    } else {
                        // Unrecognized token after ordinal
                        throw xap_bad_path ();
                    }
                    break;
                }
                case pAName: {
                    /* We only support an equals sign. */
                    if (token.type == tEquals)
                        state = pMatch;
                    else {
                        // Unrecognized comparison token
                        throw xap_bad_path ();
                    }
                    break;
                }
                case pName: {
                    /* Either a function or a match pattern. */
                    if (token.type == tEquals)
                        state = pMatch;
                    else if (token.type == tParens) {
                        state = pFunc;
                    } else {
                        // Unrecognized token after '[name'
                        throw xap_bad_path ();
                    }
                    break;
                }
                case pFunc: {
                    if (token.type == tRB) {
                        /*
                        We only support "last()".  Get the children
                        of the current element, and return the last one.
                        */
                        const XAPTk_Token& funcToken = tokens[lhs];
                        string funcName(funcToken.begin, funcToken.end);
                        if (funcName != "last" // REVISIT: make constant?
                        || tix+1 < maxt) {
                            // Unrecognized function, or tokens
                            throw xap_bad_path ();
                        }
                        backup = current;
                        current = current->getLastChild();
                        if (current == NULL && isRequired) {
                            // Can't use last() during create!
                            throw xap_bad_path ();
                        }
                        // No new state, we're done.
                    } else {
                        // Unrecognized token after '[func()'
                        throw xap_bad_path ();
                    }
                    break;
                }
                case pMatch: {
                    /* Save the right-hand side of the match pattern. */
                    if (token.type == tChars) {
                        rhs = tix;
                        state = pVal;
                    } else {
                        // Unrecognized token after '[...='
                        throw xap_bad_path ();
                    }
                    break;
                }
                case pVal: {
                    if (token.type != tRB || tix+1 < maxt) {
                        // Unrecognized token after '[...='
                        throw xap_bad_path ();
                    }

                    /* Perform the match */

                    /*
                    Cases:

                    >>> node[@attr="val"]

                    Match child of current called node, which has
                    an attribute called attr whose value is "val".

                    >>> *[@attr="val"]

                    From all children of current, match child with
                    attribute called attr whose value is "val".

                    >>> node[kid="text"]

                    Match child of current called node, which has a child
                    (grandchild of current) called kid and whose text value is
                    "text".

                    >>> *[kid="text"]

                    From all children of current, match child (grandchild
                    of current) whose name is kid and whose text value is
                    "text".

                    */

                    VectOToken::const_reference nToken = tokens[nodeName];
                    string nameStr(nToken.begin, nToken.end);

                    VectOToken::const_reference lToken = tokens[lhs];
                    string lhsStr(lToken.begin, lToken.end);

                    // Strip quotes from literal
                    const XAPTk_Token& rToken = tokens[rhs];
                    string lit(rToken.begin, rToken.end);
                    const char& lq = lit.at(0);
                    const char& rq = lit.at(lit.length()-1);
                    if (lq != rq) {
                        // Mismatched quotes for value
                        throw xap_bad_path ();
                    }
                    lit.erase(lit.length()-1,1);
                    lit.erase(0,1);

                    // Create current node list
                    if (!current->hasChildNodes() && !isRequired)
                        return(NULL); // No match!
                    VectOElem nodeList;
                    NodeList* kids = current->getChildNodes();
                    const unsigned long tallest = (kids != NULL) ? kids->getLength() : 0;

                    for (unsigned long k = 0; k < tallest; ++k) {
                        Element* e = dynamic_cast<Element*>(kids->item(k));
                        if (e == NULL) continue; // Skip text, PI, etc.
                        // isAll means match all elements
                        if (isAll) {
                            WElement we(e);
                            nodeList.push_back(we);
                            continue;
                        }
                        // We have to find a matching name
                        const std::string eName = e->getTagName();
                        string localPart;
                        XAPTk::StripPrefix(eName, &localPart, NULL);
                        if (localPart == nameStr) {
                            WElement we(e);
                            nodeList.push_back(we);
                        }
                    }

                    // Now match predicate
                    backup = current;
                    if (isAttr)
                        current = this->findByAttr(nodeList, lhsStr, &lit);
                    else
                        current = this->findByText(nodeList, lhsStr, lit);

                    if (current == NULL) { // No match found
                        if (!isRequired)
                            return(NULL); // No match!
                        if (isAll) {
                            // Need name for new child
                            Node* n1 = backup->getFirstChild();
                            if (n1 == NULL) {
                                // Can't clone required sibling
                                throw xap_bad_path ();
                            }
                            nameStr = n1->getNodeName();
                            current = m_doc->createElement(nameStr);
                        } else {
                            current = this->createElement(nameStr);
                        }
                        // Create named child
                        backup->appendChild(current);
                        Element* e = dynamic_cast<Element*>(current);
                        if (isAttr) {
                            // Define matching attribute
                            e->setAttribute(lhsStr, lit);
                        } else {
                            // Not allowed
                            throw xap_bad_path ();
                            /*-
                            // Define matching named child with text
                            Element* gk = m_doc->createElement(lhsStr);
                            e->appendChild(gk);
                            Text* tx = m_doc->createTextNode(lit);
                            gk->appendChild(tx);
                            */
                        }
                    }
                    break;
                }
                default: assert(false); // CantHappen
            } // switch
            if (ret != NULL) {
                // Can't have a return value if not last token in step!
                if (tix+1 < maxt) {
                    throw xap_bad_path ();
                }
                break;
            }
        } // for

        if (ret != NULL) {
            // Can't have a return value if not last step!
            if (iStep+1 < maxn) {
                throw xap_bad_path ();
            }
            break;
        }
    } // for
    if (ret == NULL)
        ret = current;
    CERR_PATH("RETURN: ", m_rootElem, current);
    return(ret);
}

    /** Find element in NodeList that matches attribute. */
Element*
NormTree::findByAttr(VectOElem& elems, const string& attr, const string* val /*= NULL*/) {
    if (val == NULL)
        return(NULL); //REVISIT: Why is val a pointer?
    const size_t liMax = elems.size();
    bool foundIt = false;
    Element* ret = NULL;
    for (size_t i = 0; i < liMax; ++i) {
        /*
        Looking for child whose attribute name matches lhsStr, and whose value
        matches lit.
        */
        ret = elems[i].m_obj;
        NamedNodeMap* nMap = ret->getAttributes();
        if (nMap == NULL) {
            continue;
        }
        const unsigned long man = nMap->getLength();

        for (unsigned long m = 0; m < man; ++m) {
            Attr* a = dynamic_cast<Attr*>(nMap->item(m));
            assert(a != NULL);
            const std::string aName = a->getName();
            /*
            We base the comparison of attribute names on the literal QName,
            rather than the expanded name.  This is because the QNames in
            the NormTree have been normalized to standard prefixes, and the
            client is expected to use standard prefixes when specifying paths.
            */
            if (aName == attr) {
                // Value match?
                const std::string aVal = a->getValue();
                if (aVal == *val) {
                    foundIt = true;
                    break;
                }
                // Special case for xml:lang, val is case-insensitive
                if (aName == XAPTK_ATTR_XML_LANG) {
                    const string::size_type ls = val->size();
                    const string::size_type rs = aVal.size();
                    if (ls == rs) {
                        bool okSoFar = true;
                        for (string::size_type j = 0; j < ls; ++j) {
                            string::const_reference lc = val->at(j);
                            string::const_reference rc = aVal.at(j);
                            if (lc == rc) {
                                continue;
                            }
                            if (lc >= 'A' && lc <= 'Z'
                            && rc >= 'a' && rc <= 'z'
                            && (rc-lc) == 32) {
                                continue;
                            }
                            if (lc >= 'a' && lc <= 'z'
                            && rc >= 'A' && rc <= 'Z'
                            && (lc-rc) == 32) {
                                continue;
                            }
                            // If we get here, characters don't match
                            okSoFar = false;
                            break;
                        }
                        foundIt = okSoFar;
                        if (foundIt)
                            break;
                    }
                }
            }
        }
        if (foundIt)
            break;
    }
    if (!foundIt)
        ret = NULL;
    return(ret);
}


    /** Find element in NodeList whose named child matches text. */
Element*
NormTree::findByText(VectOElem& elems, const std::string& childName, const std::string&  childText) {
    const size_t liMax = elems.size();
    bool foundIt = false;
    Element* ret = NULL;
    Element* kid;
    for (size_t i = 0; i < liMax; ++i) {
        /*
        Looking for child whose name matches childName, and whose]
        text child matches childText.
        */
        ret = elems[i].m_obj;
        NodeList* kids = ret->getChildNodes();
        const unsigned long tallest = kids->getLength();

        for (unsigned long k = 0; k < tallest; ++k) {
            kid = dynamic_cast<Element*>(kids->item(k));
            if (kid == NULL)
                continue; // Skip text, PI, etc.
            const std::string kName = kid->getNodeName();
            string localPart;
            XAPTk::StripPrefix(kName, &localPart, NULL);
            // Child name match?
            if (localPart != childName)
                continue;
            // Text child match?
            NodeList* txts = kid->getChildNodes();
            const unsigned long biggest = txts->getLength();
            Text* vt = NULL;
            for (unsigned long tc = 0; tc < biggest; ++tc) {
                Node* tk = txts->item(tc);
                switch (tk->getNodeType()) {
                    case ELEMENT_NODE: {
                        continue;
#ifdef XXX
                        // Might be rdf:value node
                        Element* rv = dynamic_cast<Element*>(tk);
                        std::string rvTag = rv->getTagName();
                        if (rvTag != XAPTK_TAG_RDF_VALUE)
                            continue;
                        // First child is text
                        vt = dynamic_cast<Text*>(rv->getFirstChild());
                        break;
#endif
                    }
                    case TEXT_NODE: {
                        vt = dynamic_cast<Text*>(tk);
                        break;
                    }
                    default:;
                }
                if (vt != NULL)
                    break;
            }
            if (vt == NULL)
                continue;
            const std::string dt = vt->getData();
            if (dt == childText) {
                foundIt = true;
                break;
            }
        } // for: k
        if (foundIt)
            break;
    }
    if (!foundIt)
        ret = NULL;
    return(ret); // grandparent of matching text
}


    /** Get DOM document. */
Document*
NormTree::getDOMDoc() {
    return(m_doc);
}

    /* Get features for element. */
bool
NormTree::getFeatures(Element* e, XAPFeatures& f) {
    return XAPTk::GetFeatures(e, f);
}

    /** Get namespace. */
void
NormTree::getNS(std::string& ns) {
    ns = m_ns;
}

    /* Get oldest ancestor container. */
Element*
NormTree::getOldestContainer(Element* here) {
    if (here == NULL)
        return(NULL);
    Element* e = here;
    Element* ancestor = NULL;
    while (e != NULL && e != this->getRoot()) {
        if (NormTree::WhatsMyForm(e) == xap_container)
            ancestor = e;
        e = dynamic_cast<Element*>(e->getParentNode());
    }
    return(ancestor);
}

    /** Get the root element (child of doc) */
Element*
NormTree::getRoot() const {
    return(m_rootElem);
}

    /** Get a text value from a leaf node. */
bool
NormTree::getText(Element* leaf, string& val, XAPFeatures& f) {
    // First find an existing text node.
    f = XAP_FEATURE_NONE;
    NodeList* nl = leaf->getChildNodes();
    const unsigned long mxnl = nl->getLength();
    Text* tval;
    this->getFeatures(leaf, f);
    if (mxnl == 0) {
        // Interpret lack of children as empty text value
        val = *XAPTk::NullString;
        return(true);
    } else if (mxnl == 1) {
        tval = dynamic_cast<Text*>(nl->item(0));
        if (tval == NULL) {
            /* Fail-soft */
            return(false);
        }
    } else {
        // See if there is a text value
        tval = NULL;
        for (unsigned long i = 0; i < mxnl; ++i) {
            tval = dynamic_cast<Text*>(nl->item(i));
            if (tval != NULL)
                break;
        }
        if (tval == NULL) {
            /* Future RDF may allow rdf:value to be non-text value, so
               we fail-soft here. */
            return(false);
        }
    }
    val = tval->getData();
    return(true);
}

    /** Select child by name. */
Element*
NormTree::selectChild(Element* e, const string& name) {
    NodeList* kids = e->getChildNodes();
    if (kids == NULL)
        return(NULL);
    const unsigned long tallest = kids->getLength();
    string eName; //QName
    string ePrefix;
    string eLocalPart;
    string eNS;
    string prefix;
    string localPart;
    string ns;
    Element* kid;

    XAPTk::StripPrefix(name, &localPart, &prefix);

    /*
    Look up the namespace that matches the prefix, if any, on
    the name parameter.
    */
    if (!prefix.empty() && prefix != "*") {
        XAPTk_StringByString::const_iterator pTest = m_nsMap.find(prefix);
        /* [first: prefix, second: ns] */

        if (pTest == m_nsMap.end()) {
            // Maybe its defined in the master ns map
            // Can't use ExpandName here because of special handling
            pTest = m_masterNSMap->find(prefix);
            if (pTest == m_masterNSMap->end()) {
                DOMFREE(kids);
                throw xap_bad_path ();
            }
        }
        ns = pTest->second;
    } else {
        // No prefix means the default namespace for this tree
        ns = m_ns;
    }

    /*
    Search children for first match of expanded names.
    */
    for (unsigned long k = 0; k < tallest; ++k) {
        kid = dynamic_cast<Element*>(kids->item(k));
        if (kid == NULL) continue; // Skip text, PI, etc.
        eName = kid->getTagName();

        XAPTk::StripPrefix(eName, &eLocalPart, &ePrefix);

        if (!ePrefix.empty()) {
            XAPTk_StringByString::const_iterator pTest = m_nsMap.find(ePrefix);
            /* [first: prefix, second: ns] */

            /*
            We shouldn't have to test the master map here, because by
            definition all elements in a *complete* NormTree have
            prefixes that are registered in the tree's map.  Unfortunately,
            we may call this function *before* we have a complete tree, such
            as during loadFromTree.  Because some elements are in transition,
            we have to check the master map too.
            */
            if (pTest == m_nsMap.end()) {
                // Maybe its defined in the master ns map
                // Can't use ExpandName here because of special handling
                pTest = m_masterNSMap->find(ePrefix);
                if (pTest == m_masterNSMap->end()) {
                    DOMFREE(kids);
                    throw xap_bad_path ();
                }
            }
            eNS = pTest->second;
        } else {
            // No prefix means the default namespace for this tree
            eNS = m_ns;
        }

        // Compare expanded names
        if (ns == eNS && localPart == eLocalPart) {
            DOMFREE(kids);
            return(kid);
        }
    }
    DOMFREE(kids);
    return(NULL);
}

    /* Select child by number. */
Element*
NormTree::selectChild(Element* e, const size_t ord) {
    NodeList* kids = e->getChildNodes();
    const unsigned long maxk = kids->getLength();
    if (ord < 1 || ord > maxk) {
        throw xap_bad_number ();
    }
    Element* f = dynamic_cast<Element*>(kids->item(ord-1));
    return(f);
}

    /* Select child by attribute value. */
Element*
NormTree::selectChild(Element* e, const string& attr, const string& val) {
    NodeList* kids = e->getChildNodes();
    const unsigned long maxk = kids->getLength();
    for (unsigned long i = 0; i < maxk; ++i) {
        Element* f = dynamic_cast<Element*>(kids->item(i));
        if (f == NULL)
            continue;
        const string a = f->getAttribute(attr);
        if (a == val)
            return(f);
    }
    return(NULL);
}


    /* Set features for element. */
void
NormTree::setFeatures(Element* e, const XAPFeatures& f) {
    // Get existing features, if any
    XAPFeatures ff;
    if (!this->getFeatures(e, ff)) {
        ff = f;
    } else {
        ff = ff | f;
        if (f == XAP_FEATURE_NONE)
            ff = XAP_FEATURE_NONE;
    }
    // Convert feature bits to a string
    std::string val;
    if ((ff & XAP_FEATURE_XML) != 0)
        val.append(XAPTK_FEATURE_XML);
    if ((ff & XAP_FEATURE_RDF_RESOURCE) != 0)
        val.append(XAPTK_FEATURE_RDF_RESOURCE);
    if ((ff & XAP_FEATURE_RDF_VALUE) != 0)
        val.append(XAPTK_FEATURE_RDF_VALUE);

    // Set the attribute
    Attr* a = e->getAttributeNode(XAPTK_ATTR_META_FEATURES);
    if (a == NULL) {
        e->setAttribute(XAPTK_ATTR_META_FEATURES, val);
    } else {
        a->setValue(val);
    }
}

    /* Set first child to text. */
void
NormTree::setFirstText(Element* e, const string& text) {
    Node* node = e->getFirstChild();
    Text* tval;
    Element* first;
#ifdef XAP_DEBUG_VALIDATE
    long top = 0xADBEADBE;
    XAPTk::CheckStack(NULL, &top);
#endif // XAP_DEBUG_VALIDATE
    if (node == NULL) {
        // Go ahead and set one anyway
        tval = m_doc->createTextNode(text);
        e->appendChild(tval);
        return;
    }
    tval = dynamic_cast<Text*>(node);
    if (tval != NULL) {
        tval->setData(text);
        return;
    }
    first = dynamic_cast<Element*>(node);
    if (first != NULL) {
        // Insert text before
        tval = m_doc->createTextNode(text);
        e->insertBefore(tval, first);
        return;
    }
    // Unknown node type, fail-soft
}


    /* Define namespace and prefix for this tree. */
void
NormTree::setNSDef ( const string& prefix, const string& ns ) {
    assert( ! prefix.empty() );
    if ( ns.empty() ) throw xap_bad_xap ();
	
	string master;
	MetaXAP::GetNamespacePrefix ( ns, master );

	#if 0
		cout << "In NormTree::setNSDef for " << master << " (" << prefix << ") = " << ns << endl;
	#endif
	
    XAPTk_StringByString::const_iterator notDef = m_nsMap.end();
    XAPTk_StringByString::const_iterator i = m_nsMap.find ( master );

    if ( i == notDef ) {
        m_nsMap[master] = ns;
    }

}

Element*
NormTree::setSelectedChild(Element* parent, const string& tag, const string& val, const bool isRequired /*=true*/) {
    // Get the child
    Element* kid = this->selectChild(parent, tag);
    if (kid == NULL) {
        if (!isRequired)
            return(NULL);
        //else
        kid = m_doc->createElement(tag);
        parent->appendChild(kid);
    }
    // Set the text child
    Node* node = kid->getFirstChild();
    Text* tval;
    if (node == NULL) {
        tval = m_doc->createTextNode(val);
        kid->appendChild(tval);
    } else {
        tval = dynamic_cast<Text*>(node);
        if (tval == NULL) {
            if (!isRequired)
                return(NULL);
            //else
            tval = m_doc->createTextNode(val);
            kid->appendChild(tval);
            return(kid);
        }
        tval->setData(val);
    }
    return(kid);
}


    /** Set a text value for an attribute. */
void
NormTree::setText(Attr* a, const string& value, XAPFeatures /* f */ /*= XAP_FEATURE_DEFAULT*/) {
    // Features can be ignore, not relevant for attributes
    a->setValue(value);
}


    /** Set a text value for an element. */
Element*
NormTree::setText(Element* e, const string& value, XAPFeatures f /*= XAP_FEATURE_DEFAULT*/) {
    // Special case if tag name of e is rdf:value
    const string name = e->getTagName();
    Element* prop;
    if (name == XAPTK_TAG_RDF_VALUE) { // Ok to use QName, already normalized
        // Remove rdf:value, assign to parent instead
        prop = dynamic_cast<Element*>(e->getParentNode());
        if (prop == NULL) {
            // Can't Happen!
            throw xap_no_match ();
        }
        prop->removeChild(e);
        DOMGlue_DeleteNode(e);
    } else {
        prop = e;
    }

    // First find an existing text node.
    NodeList* nl = prop->getChildNodes();
    unsigned long mxnl = nl->getLength();
    Text* tval;
    XAPFeatures origF;
    Node* node;

    this->getFeatures(prop, origF);
    const bool isNone = (f == XAP_FEATURE_NONE);
    const bool isXML = ((f & XAP_FEATURE_XML) != 0);
    const bool wasNone = (origF == XAP_FEATURE_NONE);

    // Handle common case
    if (isNone && wasNone) {
        if (mxnl == 0) {
            this->setFirstText(prop, value);
        } else if (mxnl == 1) {
            tval = dynamic_cast<Text*>(nl->item(0));
            if (tval == NULL) {
                throw xap_bad_path ();
            }
            tval->setData(value);
        } else {
            /*
            This element has no feature bits set, and the incoming text
            has no feature bits. It looks like the client is trying to
            set a nested description or container as a simple value.
            So we raise an exception.
            */
            throw xap_bad_path ();
        }
        return(prop);
    }

    // Handle change in features
    if (f != origF) {
        if (mxnl > 0) {
            // Clean up old value
            for (unsigned long x = 0; x < mxnl; ++x) {
#ifdef XAP_1_0_RC1
				/* 
				CRASH!  After the removeChild, the list
				is shortened! Will always return NULL
				eventually!
				*/
                node = nl->item(x);
#else /* XAP_1_0_RC1 */
				/* This is the fix.  Just use the first child always. */
				node = prop->getFirstChild();
				if (node == NULL)
					continue; // Fail-soft
#endif /* XAP_1_0_RC1 */
                prop->removeChild(node);
                DOMGlue_DeleteNode(node);
            }
            mxnl = 0;
        }
        origF = f;
    }

    string val(value);

    // Handle special cases
    this->setFeatures(prop, f);

    // Set the first child to the text
    this->setFirstText(prop, val);

    // Ensure namespace for qualifiers
    if (isXML) {
        this->setNSDef(XAP_PREFIX_INFO_XAP, XAPTK_NS_INFO_XAP);
    }

    // Qualifiers

#ifdef XXX // All info is in attribute
    if (isXML) {
        Element* is = this->setSelectedChild(prop, XAPTK_TAG_IX_IS, XAPTK_QUAL_IS_XML, true);
        if (is == NULL)
            return(prop); // Fail-soft.
    }
#endif
    /*
    If also isResource, all that is needed is to set the feature,
    fall thru.
    */
    return(prop);
}


void
NormTree::validateNSPrefix(const std::string& qName) {
    if (qName.empty())
        return;
    string localPart;
    string prefix;
    XAPTk::StripPrefix(qName, &localPart, &prefix);
    if (!prefix.empty() && prefix != "*") {
        XAPTk_StringByString::const_iterator pTest = m_nsMap.find(prefix);
        /* [first: prefix, second: ns] */

        if (pTest == m_nsMap.end()) {
            // Maybe its defined in the master ns map
            // Can't use ExpandName here because of special handling
            pTest = m_masterNSMap->find(prefix);
            if (pTest == m_masterNSMap->end()) {
                throw xap_bad_schema ();
            } else {
                // Register in this NormTree
                assert(pTest->first == prefix);
                m_nsMap[pTest->first] = pTest->second;
            }
        }
    }
}


/* ===== Protected Constructors ===== */

NormTree::NormTree() :
    m_currentElem(NULL),
    m_masterNSMap(NULL),
    m_doc(NULL),
    m_rootElem(NULL)
{}

//NormTree::NormTree(const NormTree& rhs); // Copy

NormTree::NormTree(const string& ns, const char *const rootTag, XAPTk_StringByString* masterNSMap /*= NULL*/) :
    m_currentElem(NULL),
    m_masterNSMap(masterNSMap),
    m_doc(NULL),
    m_rootElem(NULL),
    m_ns(ns),
    m_rootTag(rootTag)
{
    m_doc = DOMGlue_NewDoc();
    assert(m_doc->getDocumentElement() == NULL);
    m_rootElem = m_doc->createElement(m_rootTag);
    m_rootElem->setAttribute(XAPTK_ATTR_NS, m_ns);
    m_doc->appendChild(m_rootElem);
}


/* ===== Module Functions ===== */

static bool
GetFeatures(Element* e, XAPFeatures& f) {
    f = XAP_FEATURE_NONE;
    Attr* a = e->getAttributeNode(XAPTK_ATTR_META_FEATURES);
    if (a == NULL) {
        return(false);
    }
    const std::string val(a->getValue());
    // Convert letter codes into feature bits
    if (val.empty())
        return(true);
    if (val.find(XAPTK_FEATURE_XML) != std::string::npos)
        f |= XAP_FEATURE_XML;
    if (val.find(XAPTK_FEATURE_RDF_RESOURCE) != std::string::npos)
        f |= XAP_FEATURE_RDF_RESOURCE;
    if (val.find(XAPTK_FEATURE_RDF_VALUE) != std::string::npos)
        f |= XAP_FEATURE_RDF_VALUE;
    return(true);
}


} // XAPTk

/* ======================= */

#if defined(XAP_DEBUG_CERR) && 0

namespace XAPTk {
extern void
DebugSerializer(Node* root, std::string& xml);
}

ostream& operator<<(ostream& os, XAPTk::NormTree* nt) {
    string tmp;
    XAPTk::DebugSerializer(nt->getRoot(), tmp);
    return os << tmp;
}

static void
dbg_path(const string& msg, Element* docElem, Node* n) {
    Element* e = dynamic_cast<Element*>(n);
    cerr << msg;
    if (e == NULL) {
        cerr << "Attribute: " << n->getNodeName() << endl;
        return;
    }
    string tmp;
    XAPTk::DOMGlue_ElementToPath(docElem, e, tmp);
    cerr << tmp << endl;
}

#endif


void
XAPTk_InitNormTree() {
    // No-op
}

void
XAPTk_KillNormTree() {
    // No-op
}


/*
$Log$
*/
