/* $Header: //xaptk/xaptk/RDFToNormTrees.cpp#17 $ */
/* RDFToNormTrees.cpp */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/

/* A DOMWalker to convert RDF to one NormTree per schema. */

/*
See the BIG_COMMENT in NormTree.cpp for an explanation of how RDF
XML is converted into a normalized DOM tree.
*/

#ifndef XAP_FORCE_NORMAL_ALLOC
	#include "XAPTkAlloc.h"
#endif

#include <stdio.h>  // using: sprintf

#include "RDFToNormTrees.h"
#include "DOMGlue.h"
#include "XMPInitTerm.h"

#define DECL_VIRTUAL /* as nothing */

/* ===== "Package" Module Functions ===== */

XAPTk_StringByString*
MetaXAP_GetMasterNSMap();

/* ===== Module Statics ===== */

static void RDFToNormTrees_InitNSReplacements();

static XAPTk::CTypeByPair* RDFToNormTrees_repeatCType = NULL; //STATIC_INIT

static XAPTk_StringByString* RDFToNormTrees_nsReplacements = NULL; //STATIC_INIT

namespace XAPTk {

/* ===== Class ===== */

/* ===== Public Constructors ===== */

RDFToNormTrees::RDFToNormTrees() :
    m_rdfDoc(NULL),
    m_normBySchema(NULL),
    m_normByID(NULL),
    m_keepText(false),
    m_needSchema(false),
    m_needTag(true),
    m_foundTag(false),
    m_state(state_init),
    m_saveState(state_init),
    m_beingIgnored(NULL),
    m_beingLiteral(NULL),
    m_beingRDFVal(NULL),
    m_selectBy(selectByNS),
    m_finished(false)
{}

RDFToNormTrees::RDFToNormTrees(Document* rdfDoc, NormTreeByStr* bySchema, NormTreeByStr* byID, const bool needTag) :
    m_rdfDoc(rdfDoc),
    m_normBySchema(bySchema),
    m_normByID(byID),
    m_keepText(false),
    m_needSchema(false),
    m_needTag(needTag),
    m_foundTag(false),
    m_state(state_init),
    m_saveState(state_init),
    m_beingIgnored(NULL),
    m_beingLiteral(NULL),
    m_beingRDFVal(NULL),
    m_selectBy(selectByNS),
    m_finished(false)
{
    XAPTk_StringByString floor;
    floor["xml"] = "xml";
    // Never want an empty stack
    m_nsDefs.push(floor);
    m_xmlLang.push(XAPTK_XML_LANG_NOTDEF);
    // If we don't need a specific tag, act like always found.
    if (!m_needTag)
        m_foundTag = true;
}

/* ===== Public Destructor ===== */
DECL_VIRTUAL
RDFToNormTrees::~RDFToNormTrees() {}

/* ===== Public Member Functions ===== */

    /** Found an element node. */
DECL_VIRTUAL bool
RDFToNormTrees::enterElement(Element* e) {
    std::string name;
    /*
    First we deal with any global (xml) attributes, such as xml:lang
    and xmlns.  We push the namespace state, and check for new xmlns
    defs.
    */
    this->grokGlobalAttrs(e->getAttributes());
    // Expand the element name [localPart, ns]
    const XAPTk_PairOfString exTag = this->expandName(e->getTagName());
    /* [first: localPart, second: ns] */

    /* NOTE: Always grok before you push on m_beingAssigned! */
    while (true) {

#if defined(XAP_DEBUG_CERR) && 0
    //if (m_state != state_prop)
        cerr << "  [" << exTag.first << ", " << exTag.second << "]" << endl;
#endif

    /*
    The state is determined by the previous enter to or exit from a tag,
    or from the previous loop.
    */

    switch (m_state) {
        case state_init: {
            /*
            We can either find all RDF elements, or only those that are
            children of a specific tag.
            */
            // Looking for RDF
            if (exTag == *XAPTk::TAG_RDF && m_foundTag) {
                m_state = state_rdf;
                this->grokRDF(e);
            } else if ( m_needTag ) {
                if ( (exTag == *XAPTk::TAG_META) || (exTag == *XAPTk::TAG_META_OLD) )
                    m_foundTag = true;
            } else {
                // Keep digging until we find RDF, stay in state_init
            }
            break;
        }
        case state_ignore: {
            // Just keep ignoring until the m_beingIgnore element is exited
            break;
        }
        case state_rdf: {
            // Looking for Description elements.
            bool ignore = false;
            bool typedNode = false;
            if (exTag == *XAPTk::TAG_DESC) {
                // We ignore aboutEach, aboutEachPrefix
                std::string ign;
                bool found
                    = DOMGlue_GetAttribute(m_nsDefs.top(), e, XAP_NS_RDF, "aboutEach", ign);
                if (found) {
                    ignore = true;
                } else {
                    found = DOMGlue_GetAttribute(m_nsDefs.top(), e, XAP_NS_RDF, "aboutEachPrefix",ign);
                    if (found)
                        ignore = true;
                }
            } else if (exTag == *XAPTk::TAG_ALT || exTag == *XAPTk::TAG_BAG || exTag == *XAPTk::TAG_SEQ) {
                ignore = true;
            } else {
                typedNode = true;
            }

            if (!ignore) {
                /*
                Each top-level property in each top-level Description
                defines the default namespace for all of its sub-properties.
                When m_needSchema is false, it indicates that we already
                know the schema for a sub-property.
                */
                m_needSchema = true;
                if (typedNode)
                    this->grokTypedNode(e);
                else
                    this->grokDescription(e);
                if (m_state != state_ignore)
                    m_state = state_desc;
            } else {
                /*
                Might be valid RDF, such as top-level Bag, but we ignore
                anything but literal Description elements and typedNodes
                for XAP 1.0.
                */
                m_beingIgnored = e;
                m_saveState = m_state;
                m_state = state_ignore;
            }
            break;
        }
        case state_desc: {
            if (m_beingLiteral != NULL) {
                this->enterLiteral(e);
            } else {
                size_t eCount = 0;
                size_t tCount = 0;
                // It's a property!  Are we initializing the namespace?
                bool isDiff = false;
                if (!m_needSchema && this->isTopLevel(m_currentSchema.second)) {
                    // If top-level differs, change current
                    isDiff = (exTag.second != m_currentSchema.second);
                }
                if (m_needSchema || isDiff) {
                    /*
                    Either we haven't defined a schema yet for this
                    top-level property, or this top-level property has
                    a different schema from the previous top-level property.
                    */
                    name = e->getTagName();
                    XAPTk::StripPrefix(name, NULL, &m_currentSchema.first);
                    m_currentSchema.second = exTag.second; // ns
                    m_needSchema = false;
                }
                //FUTURE this->grokProperty(e);
                /*
                If we were really supporting literals, grokProperty would go here.  But since
                we are ignoring them for now, we only do grokProperty in the non-literal case.
                */
                // Literal?
                std::string pt;
                bool found
                    = DOMGlue_GetAttribute(m_nsDefs.top(), e, XAP_NS_RDF, "parseType", pt);
                if (found && pt == "Literal") {
                    m_beingLiteral = e;
                    m_keepText = true;
                } else {
                    this->grokProperty(e);
                    // Peek to see if it's simple, which needs m_keepText=true
                    DOMGlue_CountNodes(e, eCount, tCount);
                    m_keepText = (eCount == 0 && tCount > 0);
                }

                // For RDF 1.0, eCount > 1 implies parseType="Resource"
                // Ignore otherwise.
                if (eCount > 1 && pt != "Resource") {
                    m_beingIgnored = e;
                    m_saveState = m_state;
                    m_state = state_ignore;
                } else {
                    /*
                    It's literal, so we stay literal, or
                    eCount == 0, which means this is a simple property
                    and the next node is a text node and m_keepText is true,
                    or this is an empty tag with attribute defs,
                    or parseType=Resource,
                    and the next nodes are nested properties, or eCount==1
                    and parseType!=Resource and the next node is
                    either an explicit nested Description, or a typedNode.
                    */
                    m_state = state_prop;
                    WElement w(e);
                    m_beingAssigned.push(w); // nesting context
                }
            }
            break;
        }
        case state_prop: {
            /*
            Deal with literal values.
            Deal with containers.
            Deal with explicit nested description.
            Otherwise, a single typedNode, or nested properites.
            */
            if (m_beingLiteral != NULL) {
                this->enterLiteral(e);
            } else if (exTag == *XAPTk::TAG_ALT) {
                m_state = state_container;
                this->grokStructContainer(e, xap_alt);
                WElement w(e);
                m_beingAssigned.push(w); // nesting context
            } else if (exTag == *XAPTk::TAG_BAG) {
                m_state = state_container;
                this->grokStructContainer(e, xap_bag);
                WElement w(e);
                m_beingAssigned.push(w); // nesting context
            } else if (exTag == *XAPTk::TAG_SEQ) {
                m_state = state_container;
                this->grokStructContainer(e, xap_seq);
                WElement w(e);
                m_beingAssigned.push(w); // nesting context
            } else if (exTag == *XAPTk::TAG_DESC) {
                // Explicitly nested Description (as opposed to
                // parseType="Resource")
                this->grokDescription(e);
                if (m_state == state_ignore)
                    break;
                WElement w(e);
                m_beingAssigned.push(w); // nesting context
                m_state = state_desc;
            } else {
                // Either typedNode, or parseType=Resource
                std::string pt;
                Element* parent = m_beingAssigned.top().m_obj;
                (void) DOMGlue_GetAttribute ( m_nsDefs.top(), parent, XAP_NS_RDF, "parseType", pt );	// Set pt.
                if ( pt != "Resource" ) {
                    // It's a nested typedNode
                    WElement w(e);
                    m_beingAssigned.push(w); // nesting context
                    this->grokTypedNode(e);
                    m_state = state_desc;
                } else {
                    // It's an implicit nested Description
                    // Notice that we don't grok here, let state_desc do it.
                    m_state = state_desc;
                    continue;
                    //this->enterElement(e);
                }
            }
            break;
        }
        case state_container: {
            // Looking for list item
            if (exTag == *XAPTk::TAG_LI) {
                size_t eCount = 0;
                size_t tCount = 0;
                //FUTURE this->grokItem(e);
                /*
                If we were really supporting literals, grokItem would go here.  But since
                we are ignoring them for now, we only do grokItem in the non-literal case.
                */
                // Literal?
                std::string pt;
                bool found = DOMGlue_GetAttribute(m_nsDefs.top(), e, XAP_NS_RDF, "parseType", pt);
                if (found && pt == "Literal") {
                    m_beingLiteral = e;
                    m_keepText = true;
                } else {
                    this->grokItem(e);
                    // Peek to see if it's simple, which needs m_keepText=true
                    DOMGlue_CountNodes(e, eCount, tCount);
                    m_keepText = (eCount == 0 && tCount > 0);
                }
                // For RDF 1.0, eCount > 1 implies parseType="Resource"
                // Ignore otherwise.
                if (eCount > 1 && pt != "Resource") {
                    m_beingIgnored = e;
                    m_saveState = m_state;
                    m_state = state_ignore;
                } else {
                    /*
                    It's literal, so we stay literal, or
                    eCount == 0, which means this is a simple member
                    and the next node is a text node and m_keepText is true,
                    or this is an empty tag with attribute defs,
                    or parseType=Resource,
                    and the next nodes are nested properties, or eCount==1
                    and parseType!=Resource and the next node is
                    either an explicit nested Description, or a typedNode.
                    */
                    m_state = state_prop;
                    WElement w(e);
                    m_beingAssigned.push(w); // nesting context
                }
            } else {
                // Not possible in RDF 1.0, but maybe a future extension?
                // Just ignore.
                m_beingIgnored = e;
                m_saveState = m_state;
                m_state = state_ignore;
            }
            break;
        }
        default:;
    }
    break;

    } // while (true)
    
    return true;	// Continue processing this element and its children.
    
}

    /** All children of element done. */
DECL_VIRTUAL void
RDFToNormTrees::exitElement(Element* e) {
    // Expand the element name [localPart, ns]
    const XAPTk_PairOfString exTag = this->expandName(e->getTagName());
 #if defined(XAP_DEBUG_CERR) && 0
    cerr << "EX[" << exTag.first << ", " << exTag.second << "]" << endl;
 #endif

    switch (m_state) {
        case state_init: {
            if (m_needTag) {
                if (exTag == *XAPTk::TAG_META)
                    m_foundTag = false;
            }
            // Just skip until RDF is found
            break;
        }
        case state_ignore: {
            if (e == m_beingIgnored) {
                m_beingIgnored = NULL;
                m_state = m_saveState;
            }
            break;
        }
        case state_rdf: {
            // Finished the RDF?
            if (exTag == *XAPTk::TAG_RDF) {
                m_state = state_init;
                // Check all top-level descriptions for repeats
                NormTreeByStr::iterator tops = m_normBySchema->begin();
                /* [first: ns, second: NormTree*] */
                for (; tops != m_normBySchema->end(); ++tops) {
                    NormTree* norm = tops->second;
                    (void)this->grokRepeats(norm, norm->getRoot());
                }
                // Do special case fix-ups
                tops = m_normBySchema->begin();
                for (; tops != m_normBySchema->end(); ++tops) {
                    //XXX NormTree* norm = tops->second;
                    this->postProcess(tops->first, tops->second);
                }
            }
            break;
        }
        case state_desc: {
            if (m_beingAssigned.empty()) {
                // Finished with this Description
                m_selectBy = selectByNS;
                m_state = state_rdf;
            } else {
                // Check for rdf:li
                const WElement& w = m_beingAssigned.top();
                const XAPTk_PairOfString parentTag = this->expandName(w.m_obj->getTagName());

                if (parentTag == *XAPTk::TAG_LI)
                    m_state = state_container;
                else
                    m_state = state_desc;

                /*
                Check for rdf:value.  Rearrange the normalized tree if
                found.
                */
                this->grokRDFValue(e);

                /*
                We're done assigning a property, so pop it, unless
                the "being assigned" context is a nested explicit Description,
                in which case we didn't push anything to begin with.
                */
                if (parentTag != *XAPTk::TAG_DESC)
                    this->popProp();

                m_beingAssigned.pop();
                // stay in state_desc

            }
            break;
        }
        case state_prop: {
            if (m_beingLiteral != NULL) {
                this->exitLiteral(e);
                if (m_beingLiteral == e) {
                    m_beingLiteral = NULL;
                    m_state = state_desc;
                    m_keepText = false;
                    this->popProp();
                    m_beingAssigned.pop();
                }
            } else {
                /*
                Check for empty tag.  There is some uncertainty about
                what an empty tag (prop with no value) means in RDF.
                The best assumption is that, in the case of a resource,
                it is an anonymous resource about which we know nothing.
                In the case of a literal (parseType='Literal'), it is
                a string of no characters.  We treat both cases the same
                here, and
                assign an empty text node to the current property.  Don't
                do this for the abbreviated sub-prop case, or for any
                element that has rdf:resource defined.
                */
                string tmp;
                if (!e->hasChildNodes()
                && !DOMGlue_GetAttribute(m_nsDefs.top(), e, XAP_NS_RDF, "resource", tmp)
                && this->grokPropAttrs(e, false) == 0) {
                    this->grokText(*XAPTk::NullString);
                }

                // Check for rdf:li
                const WElement& w = m_beingAssigned.top();
                const XAPTk_PairOfString parentTag = this->expandName(w.m_obj->getTagName());

                if (parentTag == *XAPTk::TAG_LI)
                    m_state = state_container;
                else
                    m_state = state_desc;


                /*
                Check for rdf:value.  Rearrange the normalized tree if
                found.
                */
                this->grokRDFValue(e);
                this->popProp();
                m_beingAssigned.pop();
                m_keepText = false;
            }
            break;
        }
        case state_container: {
            m_state = state_desc;
            this->popProp();
            m_beingAssigned.pop();
            break;
        }
        default:;
    }

    // Pop the xml:lang state
    m_xmlLang.pop();
    // Pop the namespace state
    m_nsDefs.pop();
}

    /** Abort the tree walk. */
DECL_VIRTUAL bool
RDFToNormTrees::finished() {
    return(m_finished);
}

    /** Found a char data node. */
DECL_VIRTUAL void
RDFToNormTrees::text(CharacterData* cd) {
    if (!m_keepText) return;
    switch (m_state) {
        case state_prop: {
            std::string txt = cd->getData();
            if (m_beingLiteral != NULL)
                this->literalText(txt);
            else {
                /*
                Assign to the top of the property assignment stack
                in the appropriate NormTree (by namespace or ID).
                */
                this->grokText(txt);
            }
            break;
        }
        case state_container:
        case state_desc:
        case state_rdf:
        case state_ignore:
        case state_init:
            // This should just be whitespace that is ignored
        default:;
    }
}

/* ===== Protected Member Functions ===== */

void
RDFToNormTrees::addAttribute(const std::string& attr, const std::string& val) {
    // Look up the normalized tree
    NormTree* norm = this->selectNorm();
    assert(norm != NULL);
    Element* prop = norm->m_currentElem;
    assert(prop != NULL);
    prop->setAttribute(attr, val);
}

void
RDFToNormTrees::assignProp(const std::string& val) {
    Element* prop;
    // Look up the normalized tree
    NormTree* norm = this->selectNorm();
    assert(norm != NULL);
    prop = norm->m_currentElem;
    assert(prop != NULL);

    if (prop == norm->getRoot())
        return;

    // Append value to text child of prop
    norm->setFirstText(prop, val);

#if defined(XAP_DEBUG_CERR) && 0
    std::string path;
    std::string normNS;
    XAPTk::DOMGlue_ElementToPath(norm->getRoot(), prop, path);
    norm->getNS(normNS);
    cerr << normNS << ":" << endl;
    cerr << path << "='" << val << "'" << endl;
#endif

    // Ensure xml:lang, if needed
    const std::string& lastLang = m_xmlLang.top();
    if (lastLang != XAPTK_XML_LANG_NOTDEF) {
        std::string langVal = prop->getAttribute(XAPTK_ATTR_XML_LANG);
        if (langVal.empty() || langVal != lastLang) {
            // Need an xml:lang definition here
            prop->setAttribute(XAPTK_ATTR_XML_LANG, lastLang);
        }
    }
}

void
RDFToNormTrees::enterLiteral(Element* /* e */ ) {
    //FUTURE
    /*
    For now, we gracefully ignore literal literals.  We're using
    XAP_FEATURE_XML as an alternative.
    */
}

void
RDFToNormTrees::exitLiteral(Element* /* e */ ) {
    //FUTURE
    /*
    For now, we gracefully ignore literal literals.  We're using
    XAP_FEATURE_XML as an alternative.
    */
}

/** first = localPart, second = ns (or xmlns for attr = true) */
const XAPTk_PairOfString
RDFToNormTrees::expandName ( const std::string& name, const Element * attrParent /* = 0 */ )
{
//	const bool	attr	= (attrParent != 0);

    assert ( ! m_nsDefs.empty() );
    const XAPTk_StringByString& nsByPrefix = m_nsDefs.top();

    XAPTk_PairOfString exName ( DOMGlue_ExpandName ( nsByPrefix, name, attrParent ) );
    string& nsURL = exName.second;
    
    #if 0
    	cout << "In RDFToNormTrees::expandName for " << name;
    	if ( attr ) cout << " (is attr)";
    	cout << " -> " << exName.first << " in " << nsURL << endl;
    #endif

	// ---------------------------------------------------------------------------------------------
	// Deal with replacement NS names. See RDFToNormTrees_InitNSReplacements. The strategy is to
	// replace the namespaces as the original RDF DOM tree is normalized into NormTrees. This means
	// that all name expansions MUST go through this member function, rather than going directly
	// through DOMGlue_ExpandName.

    XAPTk_StringByString::const_iterator rep = RDFToNormTrees_nsReplacements->find ( nsURL );
    if ( rep != RDFToNormTrees_nsReplacements->end() ) {
    	nsURL = rep->second;
    	#if 0
    		cout << "   new ns = " << rep->second << endl;
    	#endif
    }
    
    return exName;
}

void
RDFToNormTrees::grokDescription(Element* e) {

    // Get about
    std::string about;
    bool found	= DOMGlue_GetAttribute ( m_nsDefs.top(), e, XAP_NS_RDF, "about", about );

    if ( ! about.empty() ) {
    
    	if ( m_about.empty() ) {

            m_about = about;	// This is the first non-empty about value.

    	} else if ( m_about != about ) {

			#if XAP_DEBUG_CERR
            	cerr << "Was about = " << m_about << ", now = " << about << endl;
			#endif
            // GOTCHA: Description about a different resource, gracefully ignore
            m_beingIgnored = e;
            m_saveState = m_state;
            m_state = state_ignore;
            return;
            
        }

    }

    /*
    If this is top-level and ID is defined, we treat this description
    as a special case. Normally, properties are organized into schemas
    by the namespace of the top-level property. When the ID is defined,
    the properties are organized by the ID, regardless of the top-level
    property namespace.
    */
    if (m_state == state_rdf) {
        std::string rdfID;
        found = DOMGlue_GetAttribute(m_nsDefs.top(), e, XAP_NS_RDF, "ID", rdfID);
        if (found) {
            m_currentID = rdfID;
            m_selectBy = selectByID;
        } else
            m_selectBy = selectByNS;
    }

    // Grok abbreviated properties
    this->grokPropAttrs(e);

}

bool
RDFToNormTrees::getFeatures(XAPFeatures& f) {
    // Look up the normalized tree
    NormTree* norm = this->selectNorm();
    assert(norm != NULL);
    Element* prop = norm->m_currentElem;
    assert(prop != NULL);
    return norm->getFeatures(prop, f);
}

void
RDFToNormTrees::grokGlobalAttrs(NamedNodeMap* attrs) {
    /*
    We don't care about xml:space, because our default whitespace
    handling is to preserve all whitespace for literals (inter-element
    whitespace is ignored).
    */
    XAPTk::DOMGlue_EnterNamespace(m_nsDefs, attrs);
    XAPTk::DOMGlue_EnterLang(m_xmlLang, attrs);
}

void
RDFToNormTrees::grokItem(Element* e) {
    /*
    See grokStructContainer for trick.
    */
    // Look up the normalized tree
    NormTree* norm = this->selectNorm();
    assert(norm != NULL);
    // Examine fake node
    Element* fake = norm->m_currentElem;
    // Create a real node based on the container type
    std::string type = fake->getTagName();
    std::string good;
    if (type == XAPTK_TAG_META_FAKE_ALT) {
        good = XAPTK_TAG_META_ALT;
    } else if (type == XAPTK_TAG_META_FAKE_BAG) {
        good = XAPTK_TAG_META_BAG;
    } else if (type == XAPTK_TAG_META_FAKE_SEQ) {
        good = XAPTK_TAG_META_SEQ;
    } else {
        throw xap_bad_xml ();
    }
    this->pushProp(good);
    // Now process this rdf:li element
    // Might have an xml:lang attribute
    this->grokPropAttrs(e, false);
    if (!e->hasChildNodes()) {
        // Try to get [rdf:]resource attribute
        std::string resource;
        bool found = DOMGlue_GetAttribute(
          m_nsDefs.top(), e, XAP_NS_RDF, "resource", resource);
        if (found) {
            // Attribute value is value of member item
            this->assignProp(resource);
            this->setFeatures(XAP_FEATURE_RDF_RESOURCE);
        }
    }
}

size_t
RDFToNormTrees::grokPropAttrs(Element* e, const bool checkAbbrev /*=true*/) {
    size_t ret = 0;
    bool isResource = false;
    NamedNodeMap* attrs = e->getAttributes();
    if (attrs == NULL)
        return(0);
    const unsigned long n = attrs->getLength();

    static const std::string ignoreID("ID");
    static const std::string ignoreBagID("bagID");
    static const std::string ignoreAbout("about");
    static const std::string ignorePT("parseType");
    static const std::string ignoreRes("resource");
    static const std::string ignoreRDF(XAP_NS_RDF);
    static const std::string ignoreNS("xmlns");
    static const std::string ignoreSpace("space");
    static const std::string handleLang("lang");
    static const std::string handleXML("xml");

    for (unsigned long i = 0; i < n; ++i) {
        Attr* a = dynamic_cast<Attr*>(attrs->item(i));
        if (a == NULL) continue;
        std::string origName ( a->getName() );
        std::string propName;

        //XAPTk_PairOfString xName = DOMGlue_ExpandName(m_nsDefs.top(), origName, true);
        const XAPTk_PairOfString xName ( this->expandName ( origName, e ) );
    
	    #if 0
	    {
	    	cout << "In RDFToNormTrees::grokPropAttrs for " << origName << " in " << xName.second << ", checkAbbrev:" << checkAbbrev << endl;
	    	cout.flush();
	    }
	    #endif

        const std::string& localPart = xName.first;
        const std::string& ns = xName.second;
        if (ignoreID == localPart
            && (ignoreRDF == ns || ns.empty())) {
            continue;
        }
        if (ignoreBagID == localPart
            && (ignoreRDF == ns || ns.empty())) {
            continue;
        }
        if (ignoreAbout == localPart
            && (ignoreRDF == ns || ns.empty())) {
            continue;
        }
        if (ignorePT == localPart
            && (ignoreRDF == ns || ns.empty())) {
            continue;
        }
        if (ignoreRes == localPart
            && (ignoreRDF == ns || ns.empty())) {
            isResource = true;
            continue;
        }
        if (ignoreNS == ns || origName == ignoreNS) {
            continue;
        }
        if (ignoreSpace == localPart && handleXML == ns) {
            continue;
        }


        // Handle xml:lang as an attribute
        if (handleLang == localPart
            && (handleXML == ns || ns.empty())
            && !m_needSchema) {
            this->addAttribute(XAPTK_ATTR_XML_LANG, a->getValue());
            continue;
        }

        /*
        Track changes in schema, may be a global attribute.
        */
        bool isDiff = false;
        if (checkAbbrev) {
            if (!m_needSchema && this->isTopLevel(m_currentSchema.second)) {
                // If top-level differs, change current
                isDiff = (ns != m_currentSchema.second);
            }
            if (m_needSchema || (isDiff && !ns.empty())) {
                std::string name = e->getTagName();
                XAPTk::StripPrefix(name, NULL, &m_currentSchema.first);
                m_currentSchema.second = ns; // ns
                m_needSchema = false;
            }
        }

    	NormTree *	norm = this->selectNorm();
    	if ( norm != 0 ) {
	    	norm->contractName ( xName.second, xName.first, propName );
    	} else {
	    	XAPTk::ContractName ( xName.second, xName.first, propName );
    	}

        /* Found a valid property */
        if ( checkAbbrev ) {
            this->pushProp(propName);
            this->assignProp(a->getValue());
            this->popProp();
        }
        ++ret;

        /* Qualifiers of rdf:resource? */
        if (checkAbbrev && isResource) {
            // Must set rdf:value feature as well.
	        assert ( norm != NULL );
            Element* prop = norm->m_currentElem;
            assert(prop != NULL);
            if (prop != norm->getRoot()) {
                XAPFeatures f;
                norm->getFeatures(prop, f);
                f |= XAP_FEATURE_RDF_VALUE;
                norm->setFeatures(prop, f);
            }
            isResource = false; // So we only do this once
        }

        // Make sure prefix and namespace are defined for this tree
        if ( checkAbbrev ) this->setNSDef ( propName, ns );	// ??? Is this already done by now?
    }

    return ret;

}

void
RDFToNormTrees::grokProperty(Element* e) {
    std::string name = e->getTagName();
    const XAPTk_PairOfString exTag = this->expandName(name);
    /* [first: localPart, second: ns] */
    
    #if 0
    	cout << "In RDFToNormTrees::grokProperty for " << name << " in " << exTag.second << endl;
    #endif

    this->pushProp(name);
    this->setNSDef(name, exTag.second);
    if (!e->hasChildNodes()) {
        // Process property attributes
        this->grokPropAttrs(e);
        // Try to get [rdf:]resource attribute
        std::string resource;
        bool found = DOMGlue_GetAttribute(
          m_nsDefs.top(), e, XAP_NS_RDF, "resource", resource);
        if (found) {
            // Attribute value is value of property
            this->assignProp(resource);
            this->setFeatures(XAP_FEATURE_RDF_RESOURCE);
            //XXX Let exitElement do this// this->popProp();
        }
    } else {
        /*
        Has children, so all we do here is check for property
        attributes.
        */
        this->grokPropAttrs(e, false);
    }
}

void
RDFToNormTrees::grokRDF(Element* /* e */ ) {
    //GOTCHA: No-op
}


void
RDFToNormTrees::grokRDFValue(Element* /* e */ ) {
    /*
    Call at the exit of every prop and every rdf:Description.
    Check current normalized element.  If it does not have
    the features bits set, check for rdf:value as a child.
    Fix as needed.
    */
    XAPFeatures f;
    // Look up the normalized tree by the specified schema
    NormTree* norm = this->selectNorm();
    assert(norm != NULL);
    Element* parent = norm->m_currentElem;
    assert(parent != NULL);
    Text* txt;
    Element* q;
    Element* rdfv;
    bool isRDFVal = true;

    if (!parent->hasChildNodes())
        return;
    // Look for rdf:value
    rdfv = norm->selectChild(parent, XAPTK_TAG_RDF_VALUE);
    if (rdfv == NULL)
        return;
    else { // Do rdf:value
        /*
        Text child of current normalized element needs to be promoted to parent.
        Then remove current element from the normalized tree.
        */
        txt = dynamic_cast<Text*>(rdfv->getFirstChild());
        if (txt == NULL) {
            Document* doc = norm->getDOMDoc();
            txt = doc->createTextNode(*XAPTk::NullString);
        } else {
            rdfv->removeChild(txt);
        }
        // Need features, if any, from rdf:value
        XAPFeatures vf;
        norm->getFeatures(rdfv, vf);
        norm->getFeatures(parent, f);
        f |= vf;
        if (f != XAP_FEATURE_NONE)
            norm->setFeatures(parent, f);
        // Blow away rdf:value element
        parent->removeChild(rdfv);
        DOMGlue_DeleteNode(rdfv);
        Node* first = parent->getFirstChild();
        if (first == NULL)
            parent->appendChild(txt);
        else
            parent->insertBefore(txt, first);
    }

    string qVal;
    norm->getFeatures(parent, f);
    // XML?
    q = norm->selectChild(parent, XAPTK_TAG_IX_IS);
    if (q != NULL) {
        // Check the value, in case of future extensions
        txt = dynamic_cast<Text*>(q->getFirstChild());
        if (txt != NULL) {
            qVal = txt->getData();
            if (qVal == XAPTK_QUAL_IS_XML) {
                f |= XAP_FEATURE_XML;
                isRDFVal = false;
                // Delete node
                parent->removeChild(q);
                DOMGlue_DeleteNode(q);
            }
        }
    }

    if (isRDFVal)
        f |= XAP_FEATURE_RDF_VALUE;
    this->setFeatures(f);
}



Element*
RDFToNormTrees::grokRepeats(NormTree* norm, Element* p) {
    // p is the property element that's all done being assigned
    if (m_state == state_ignore)
        return(p);
    // We only care about xap_description nodes
    if (NormTree::WhatsMyForm(p) != xap_description)
        return(p);
    /*
    Search for repeated properties.  Load each property into a map.
    If same name found, check the map value, which is the count
    of props of this name found so far.  Increment on match.
    Make two passes through the node list (don't want to add nodes to parent
    on this pass, might change nodelist invariant).  On second pass,
    iterate through the map, looking for counts greater than 1.
    If found, transform children into container member items.
    */
    NodeList* kids = p->getChildNodes();
    if (kids == NULL)
        return(p);
    const unsigned long maxnl = kids->getLength();
    if (maxnl < 1) {	// ??? Useless?
        DOMFREE(kids);
        return(p);
    }
    SizeByPair roster;
    SizeByPair::iterator un;
    string qName;
    XAPTk_PairOfString exTag;
    Element* k;
    XAPValForm form;

    // First pass, just count repeats
    for (unsigned long i = 0; i < maxnl; ++i) {
        k = dynamic_cast<Element*>(kids->item(i));
        if (k == NULL)
            continue;
        /*
        It is an error for repeated simple props and containers to
        exist at the same time.
        */
        form = XAPTk::NormTree::WhatsMyForm(k);

        qName = k->getTagName();
        /*
        Okay to use DOMGlue_ExpandName and m_nsMap here, because
        grokRepeats is called only after we have finished groking
        a subtree of the original RDF.  m_nsMap has been properly
        set by now.
        */
        exTag = DOMGlue_ExpandName(norm->m_nsMap, qName);
        un = roster.find(exTag);
        if (un == roster.end()) {
            // No match found, register it (0 for container found)
            roster[exTag] = (form != xap_container) ? 1 : 0;
        } else {
            /* [first: pair<localPart,ns>, second: size_t] */
            if (un->second == 0 && form == xap_simple) {
                /* Container found previously, now found simple. */
                throw xap_bad_xap ();
            }
            if (form == xap_container) {
                if (un->second != 0) {
                    /* Simple found before, now found container. */
                    throw xap_bad_xap ();
                }
                continue; // leave 0
            }
            //  match found, increment
            un->second = un->second + 1;
        }
    }
    DOMFREE(kids);

    /*
    We want to check for other special cases, such as singleton Dublin Core,
    so we don't care if there are no actual repeats.  We continue
    processing anyway.
    */

    Document* doc = norm->getDOMDoc();
    Node* sib;
    Element* cont;
    Element* item;
    Element* match;
    string tmp;
    NamedNodeMap* attrs;
    Attr* attr;
    CTypeByPair::const_iterator whichCType;

    // Second pass, transform repeated kids and special cases
    for (un = roster.begin(); un != roster.end(); ++un) {
        /* [first: pair<localPart,ns>, second: size_t] */
        if (un->second == 0)
            continue; // Already a container
        if (un->second == 1) {
            /*
            Even singleton cases of some common properties should be treated as
            container types.

            whichCType is just a temporary variable here.  We're looking
            for a hit on un->first, which is the expanded property name.
            */
            whichCType = RDFToNormTrees_repeatCType->find(un->first);
            if (whichCType == RDFToNormTrees_repeatCType->end())
                continue;
        }
        /*
        Turn first occurrence of repeated property into
        the container for the others.
        */
        const string& ns = un->first.second;
        const string& localPart = un->first.first;
        norm->contractName(ns, localPart, qName);
        k = norm->selectChild(p, qName); // Uses expanded name for match
        if (k == NULL)
            continue;
        // Create a container with the same name, insert before original
        cont = doc->createElement(qName);
        p->insertBefore(cont, k);
        // Transfer all repeated properties to member items of new container
        while (k != NULL) {
            // Transfer children of this property k to container member
            kids = k->getChildNodes();
            if (kids != NULL) {
                // k is the former repeated property, to become a member item
                whichCType = RDFToNormTrees_repeatCType->find(un->first);
                /* [first: pair<localPart, ns>, second: XAPStructContainerType ] */
                 if (whichCType == RDFToNormTrees_repeatCType->end()) {
                    // Default cType
                    item = doc->createElement(XAPTK_TAG_META_SEQ);
                } else switch (whichCType->second) {
                    case xap_alt:
                        item = doc->createElement(XAPTK_TAG_META_ALT);
                        break;
                    case xap_bag:
                        item = doc->createElement(XAPTK_TAG_META_BAG);
                        break;
                    case xap_seq:
                        item = doc->createElement(XAPTK_TAG_META_SEQ);
                        break;
                    default:
                        item = doc->createElement(XAPTK_TAG_META_SEQ);
                        break;
                }
                unsigned long mn = kids->getLength();
                for (unsigned long j = 0; j < mn; ++j) {
                    //Node* node = kids->item(j);
                    Node* node = k->getFirstChild();
                    assert(node != NULL);
                    k->removeChild(node);
                    item->appendChild(node);
                }
                DOMFREE(kids);
                // Transfer attributes
                attrs = k->getAttributes();
                if (attrs != NULL) {
                    unsigned long sa = attrs->getLength();
                    for (unsigned long a = 0; a < sa; ++a) {
                        attr = dynamic_cast<Attr*>(attrs->item(a));
                        if (attr == NULL)
                            continue;
                        item->setAttribute(attr->getName(), attr->getValue());
                    }
                    DOMFREE(attrs);
                }
                // Add item to container
                cont->appendChild(item);
            }
            // Select next matching sibling
            sib = k;
            match = NULL;
            while (sib != NULL) {
                sib = sib->getNextSibling();
                if (sib == NULL)
                    continue;
                match = dynamic_cast<Element*>(sib);
                if (match == NULL)
                    continue;
                tmp = match->getTagName();
                // See DOMGlue_ExpandName usage comment above
                exTag = DOMGlue_ExpandName(norm->m_nsMap, tmp);
                if (exTag == un->first) {
                    break; // Found next match
                } else {
                    match = NULL;
                }
            }
            // Remove k from parent
            p->removeChild(k);
            delete k;
            // Next k is match
            k = match;
        }
    }
    return(p);
}

void
RDFToNormTrees::grokStructContainer(Element* e, XAPStructContainerType type) {
    /*
    This is tricky.  I need to remember what kind of container this is,
    but I can't define the property until grokItem.  So I push a fake
    property as a placeholder.  Real properties are then added as children,
    one for each grokItem.  The popProp must notice when the last real
    member is popped, and also pop the placeholder node.

    Example:
    <dc:title>
      <rdf:Seq>
        <rdf:li xml:lang="fr">French</rdf:li>
        <rdf:li xml:lang="en">English</rdf:li>
      </rdf:Seq>
    </dc:title>

    Normalized (before popping fake node):
    <dc:title>
      <mxap:seq>
        <rdf:Seq xml:lang="fr">French</rdf:Seq>
        <rdf:Seq xml:lang="en">English</rdf:Seq>

    */

    switch (type) {
        case xap_alt:
            this->pushProp(XAPTK_TAG_META_FAKE_ALT);
            break;
        case xap_bag:
            this->pushProp(XAPTK_TAG_META_FAKE_BAG);
            break;
        case xap_seq:
            this->pushProp(XAPTK_TAG_META_FAKE_SEQ);
            break;
        default: assert(false); // CantHappen
    }

    // Might have rdf:_n ordinals
    NamedNodeMap* attrs = e->getAttributes();
    bool isOrd = false;
    if (attrs == NULL)
        return;
    const unsigned long n = attrs->getLength();
    std::string origName;
    Attr* a;
    /*
    Use this loop to check for ordinals only.  Can't use the loop
    to assign members, since the ordinals may be out of order.
    */
    for (unsigned long i = 0; i < n; ++i) {
        a = dynamic_cast<Attr*>(attrs->item(i));
        if (a == NULL) continue;
        origName = a->getName();

        //xName = DOMGlue_ExpandName(m_nsDefs.top(), origName, true);
        const XAPTk_PairOfString xName(this->expandName(origName, e));
        /* xName.[first: localPart, second: ns] */
        const std::string& localPart = xName.first;
        const std::string& ns = xName.second;
        // Need to deal with rdf:_[ordinal]
        if ((ns.empty() || ns == XAP_NS_RDF)
        && localPart.size() > 1
        && localPart.at(0) == '_'
        && (localPart.at(1) >= '1' && localPart.at(1) <= '9')) {
            isOrd = true;
            break;
        }
    }
    DOMFREE(attrs);
    if (!isOrd)
        return;
    // Ask for ordinals in order, bail when no match.
    std::string li;
    switch (type) {
        case xap_alt:
            li = XAPTK_TAG_META_ALT;
            break;
        case xap_bag:
            li = XAPTK_TAG_META_BAG;
            break;
        case xap_seq:
            li = XAPTK_TAG_META_SEQ;
            break;
        default:;
    }

    size_t ord = 1;
    char buf[32];
    std::string val;
    while (ord > 0) {
        buf[0] = '\0';
        sprintf(buf, "rdf:_%u", ord);
        origName.assign(buf);
        val = e->getAttribute(origName);
        if (val.empty())
            break;
        this->pushProp(li);
        this->assignProp(val);
        this->popProp();
        ++ord;
    }
}

void
RDFToNormTrees::grokText(const std::string& txt) {
    this->assignProp(txt);
}

void
RDFToNormTrees::grokTypedNode(Element* e) {
    /*
    A typedNode is exactly the same as an explicit rdf:Description,
    except that the tag name of the typedNode becomes the value
    of an rdf:type property.
    */
    const string name = e->getTagName();
    const XAPTk_PairOfString typeName = this->expandName(name);
    /* [first: localPart, second: ns] */

    #if 0
    	cout << "In RDFToNormTrees::grokTypedNode for " << name << " in " << typeName.second << endl;
    #endif

    // Get about
    std::string about;
    bool found
        = DOMGlue_GetAttribute(m_nsDefs.top(), e, XAP_NS_RDF, "about", about);
    if (!about.empty()) {
        if (!m_about.empty() && m_about != about) {
#if defined(XAP_DEBUG_CERR) && 1
            cerr << "Was about=" << m_about << ", now=" << about << endl;
#endif
            // GOTCHA: Typed node about a different resource, gracefully ignore
            m_beingIgnored = e;
            m_saveState = m_state;
            m_state = state_ignore;
            return;
        } else if (m_about.empty())
            m_about = about;
#if defined(XAP_DEBUG_CERR) && 1
        cerr << "Found non-self about=" << about << endl;
#endif
    }
    /*
    If this is top-level and ID is defined, we treat this description
    as a special case. Normally, properties are organized into schemas
    by the namespace of the top-level property. When the ID is defined,
    the properties are organized by the ID, regardless of the top-level
    property namespace.
    */
    if (m_state == state_rdf) {
        std::string rdfID;
        found = DOMGlue_GetAttribute(m_nsDefs.top(), e, XAP_NS_RDF, "ID", rdfID);
        if (found) {
            m_currentID = rdfID;
            m_selectBy = selectByID;
        } else
            m_selectBy = selectByNS;
    }

    // Track change to schema. Type (tag name) defines default schema.
    bool isDiff = false;
    if (!m_needSchema && this->isTopLevel(m_currentSchema.second)) {
        // If top-level differs, change current
        isDiff = (typeName.second != m_currentSchema.second);
    }
    if (m_needSchema || (isDiff && !typeName.second.empty())) {
        XAPTk::StripPrefix(name, NULL, &m_currentSchema.first);
        m_currentSchema.second = typeName.second; // ns
        m_needSchema = false;
    }

    // The value of rdf:type wants to be the concatanated expanded name
    string tmp(typeName.second);
    tmp.append(typeName.first);
    this->pushProp(XAPTK_TAG_RDF_TYPE);
    this->assignProp(tmp);
    this->popProp();

    // Grok abbreviated properties
    this->grokPropAttrs(e);

}

bool
RDFToNormTrees::isTopLevel(const std::string& /* ns */ ) const {
    // Look up the normalized tree
    NormTree* norm = this->selectNorm();
    assert(norm != NULL);
    Element* top = norm->m_currentElem;
    if (top == NULL)
        return(true);

#if defined(XAP_DEBUG_CERR) && 0
    std::string path;
    XAPTk::DOMGlue_ElementToPath(norm->getDOMDoc()->getDocumentElement(), top, path);
    cerr << " top(" << top << "):" << path << endl;
    cerr << "root(" << norm->getRoot() << ")" << endl;
#endif
    return(top == norm->getRoot());
}

void
RDFToNormTrees::literalText(const std::string& /* txt */ ) {
    //FUTURE
    /*
    For now, we gracefully ignore literal literals.  We're using
    XAP_FEATURE_XML as an alternative.
    */
}

void
RDFToNormTrees::popProp() {
    // Look up the normalized tree by the specified schema
    NormTree* norm = this->selectNorm();
    assert(norm != NULL);
    Element* p = norm->m_currentElem;
    assert(p != NULL);
    bool wasTricked = false;

    // Check for trick, see grokStructContainer
    const std::string checkTrick = p->getTagName();
    if (XAPTK_TAG_META_FAKE_ALT == checkTrick
      ||XAPTK_TAG_META_FAKE_BAG == checkTrick
      ||XAPTK_TAG_META_FAKE_SEQ == checkTrick) {
        // Reparent all the children of p under p's parent
        Element* parent = dynamic_cast<Element*>(p->getParentNode());
        assert(parent != NULL);
#if defined (XAP_DEBUG_CERR) && 0
        std::string tp;
        DOMGlue_ElementToPath(norm->getRoot(), parent, tp);
        cerr << "Pop[" << tp << "] " << checkTrick << endl;
#endif
        while (p->hasChildNodes()) {
            Node* kid = p->getFirstChild();
            p->removeChild(kid);
            parent->appendChild(kid);
        }
        parent->removeChild(p);
        DOMGlue_DeleteNode(p);
        p = parent;
        wasTricked = true;
    }

    // Check for repeated properties in children
    if (p != NULL && !wasTricked)
        p = this->grokRepeats(norm, p);

    // Pop current context
    if (p != NULL && !wasTricked) {
        Element* pp = dynamic_cast<Element*>(p->getParentNode());
#if defined(XAP_DEBUG_CERR) && 0
        std::string tp;
        DOMGlue_ElementToPath(norm->getRoot(), pp, tp);
        std::string kn = p->getTagName();
        cerr << "Pop[" << tp << "] " << kn << endl;
#endif
        p = pp;
    }
    norm->m_currentElem = p;
}

void
RDFToNormTrees::postProcess(const std::string& ns, NormTree* norm) {
    /*
    By the time we get here, we've normalized a complete RDF element
    into a set of NormTrees.  This function is called for each NormTree
    in case there are any post-processing fixups that are needed.

    FUTURE: Maybe make this available for client plugin extension.
    */

    // We only care about Dublin Core for now
    if (ns != XAP_NS_DC)
        return;

    /*
    Make sure every Alt container with a single member has a default
    language.  If there are multiple members without language specifiers,
    do nothing.
    */
    Element* root = norm->getRoot();
    Element* prop = dynamic_cast<Element*>(root->getFirstChild());
    string ign;
    for (; prop != NULL; prop = dynamic_cast<Element*>(prop->getNextSibling())) {
#ifdef XAP_DEBUG_CERR
        const string ename(prop->getTagName());
#endif
        XAPValForm form = XAPTk::NormTree::WhatsMyForm(prop);
        if (form != xap_container)
            continue;
        XAPStructContainerType sct = XAPTk::NormTree::WhatsMySCType(prop);
        if (sct != xap_alt)
            continue;
        NodeList* kids = prop->getChildNodes();
        if (kids == NULL)
            continue;
        size_t n = kids->getLength();
        DOMFREE(kids);
        if (n > 1) {
            continue;
        }
        Element* altLi = dynamic_cast<Element*>(prop->getFirstChild());
        if (!DOMGlue_GetAttribute(norm->m_nsMap, altLi, "xml", "lang", ign)) {
            altLi->setAttribute(XAPTK_ATTR_XML_LANG, XAPTK_ALT_DEFAULT_LANG);
        }
    }
}

void
RDFToNormTrees::pushProp(const std::string& pName) {
    Element* prop;
    Element* parent;
    XAPTk_StringByString* masterNSMap = MetaXAP_GetMasterNSMap();
    std::string qName;
    // Look up the normalized tree
    NormTree* norm = this->selectNorm();
    if (norm == NULL) {
        // Need to create a new NormTree
        // ??? Where is m_currentSchema set? The prefix must be the master version.
        const std::string& ns = m_currentSchema.second;
        norm = NormTree::NewMeta(ns, masterNSMap);
        if (m_selectBy == selectByNS)
            (*m_normBySchema)[ns] = norm;
        else if (m_selectBy == selectByID)
            (*m_normByID)[m_currentID] = norm;
        else {
            assert(false); // CantHappen
        }
    }
    // Use the current context element as the parent
    if (norm->m_currentElem == NULL) {
        parent = norm->getRoot();
    } else
        parent = norm->m_currentElem;

    // Normalize the qualified name
    const XAPTk_PairOfString exTag = this->expandName(pName);
    /* [first: localPart, second: ns] */
    norm->contractName(exTag.second, exTag.first, qName);
    if (qName == exTag.first)
        qName = pName;

#if defined(XAP_DEBUG_CERR) && 0
    std::string tp;
    DOMGlue_ElementToPath(norm->getRoot(), parent, tp);
    cerr << "Push[" << tp << "] " << qName << endl;
#endif

    // Impregnate parent with child
    Document* doc = norm->getDOMDoc();
    prop = doc->createElement(qName);
    assert(prop != NULL);
    parent->appendChild(prop);
    // Update current element
    norm->m_currentElem = prop;
}

NormTree*
RDFToNormTrees::selectNorm() const {
    NormTree* norm = NULL;
    if (m_selectBy == selectByNS)
        norm = NormTree::GetNormTree(*m_normBySchema, m_currentSchema.second);
    else if (m_selectBy == selectByID) {
        norm = NormTree::GetNormTree(*m_normByID, m_currentID);
    } else {
        assert(false); // CantHappen
    }
    return(norm);
}

void
RDFToNormTrees::setFeatures(const XAPFeatures& f) {
    // Look up the normalized tree
    NormTree* norm = this->selectNorm();
    assert(norm != NULL);
    Element* prop = norm->m_currentElem;
    assert(prop != NULL);
    norm->setFeatures(prop, f);
#if defined(XAP_DEBUG_CERR) && 0
    if (f != XAP_FEATURE_NONE) {
        std::string tp;
        DOMGlue_ElementToPath(norm->getRoot(), prop, tp);
        cerr << "FE[" << tp << "] ";
        if ((f & XAP_FEATURE_XML) != 0)
            cerr << "X";
        if ((f & XAP_FEATURE_RDF_RESOURCE) != 0)
            cerr << "R";
        if ((f & XAP_FEATURE_RDF_VALUE) != 0)
            cerr << "V";
        cerr << endl;
    }
#endif
}

void
RDFToNormTrees::setNSDef ( const std::string& /* origName */, const std::string& ns )
{

	if ( ns == "" ) return;	// Ignore the empty namespace.
	
    std::string prefix;

	MetaXAP::GetNamespacePrefix ( ns, prefix );
	assert ( prefix != "" );
	
    NormTree* norm = this->selectNorm();
    norm->setNSDef ( prefix, ns );

}

} // XAPTk


static void
RDFToNormTrees_InitNSReplacements() {
    /*
    On parsing ONLY (not through API), handle cases of a single
    logical namespace having multiple URI's, either intentionally,
    as in the case of Dublin Core, or by accident due to some generator
    somewhere having a typo.

    FUTURE: This table should be exposed with a full set of Get, Set,
    Enumerate, Remove, functions, as well as an option to disable
    these replacements.
    */

    if ( RDFToNormTrees_nsReplacements == 0 ) {

	    RDFToNormTrees_nsReplacements = new XMP_Debug_new XAPTk_StringByString;
	    XAPTk_StringByString& replace = *RDFToNormTrees_nsReplacements;
	    /* [first: incoming ns, second: ns] */
	    replace["http://purl.org/dc/elements/1.0/"] = XAP_NS_DC;
    
    }

}


void
XAPTk_InitRDFToNormTrees() {
    /*
    When we convert repeated properties into containers, we need to pick a
    container type.  Use this table for commonly repeated properties.
    NOTE: This info is repeated in the schemas, but since we need to be
    able to run free of schema control, we duplicate this info here.
    Potential for getting out of sync.

    FUTURE: Expose this table through the API, and add an option to
    disable this normalization.
    */
    
    if ( RDFToNormTrees_repeatCType == 0 ) {
    
	    RDFToNormTrees_repeatCType = new XMP_Debug_new XAPTk::CTypeByPair;
	    XAPTk_PairOfString exTag;

	    exTag.second = XAP_NS_XAP;
	    exTag.first = "Authors";
	    (*RDFToNormTrees_repeatCType)[exTag] = xap_seq;
	    exTag.first = "Description";
	    (*RDFToNormTrees_repeatCType)[exTag] = xap_alt;
	    exTag.first = "Title";
	    (*RDFToNormTrees_repeatCType)[exTag] = xap_alt;

	    exTag.second = XAP_NS_DC;
	    exTag.first = "title";
	    (*RDFToNormTrees_repeatCType)[exTag] = xap_alt;
	    exTag.first = "creator";
	    (*RDFToNormTrees_repeatCType)[exTag] = xap_seq;
	    exTag.first = "subject";
	    (*RDFToNormTrees_repeatCType)[exTag] = xap_bag;
	    exTag.first = "description";
	    (*RDFToNormTrees_repeatCType)[exTag] = xap_alt;
	    exTag.first = "publisher";
	    (*RDFToNormTrees_repeatCType)[exTag] = xap_bag;
	    exTag.first = "contributor";
	    (*RDFToNormTrees_repeatCType)[exTag] = xap_bag;
	    exTag.first = "type";
	    (*RDFToNormTrees_repeatCType)[exTag] = xap_bag;
	    exTag.first = "language";
	    (*RDFToNormTrees_repeatCType)[exTag] = xap_bag;
	    exTag.first = "relation";
	    (*RDFToNormTrees_repeatCType)[exTag] = xap_bag;
	    exTag.first = "rights";
	    (*RDFToNormTrees_repeatCType)[exTag] = xap_alt;
	    exTag.first = "date";
	    (*RDFToNormTrees_repeatCType)[exTag] = xap_seq;
    
    }

    RDFToNormTrees_InitNSReplacements();

}

void
XAPTk_KillRDFToNormTrees() {
    delete RDFToNormTrees_nsReplacements;
    RDFToNormTrees_nsReplacements = NULL;
    delete RDFToNormTrees_repeatCType;
    RDFToNormTrees_repeatCType = NULL;
}


/*
$Log$
*/
