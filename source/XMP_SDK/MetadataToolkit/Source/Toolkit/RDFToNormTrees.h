
/* $Header: //xaptk/xaptk/RDFToNormTrees.h#12 $ */
/* RDFToNormTrees.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.  If you have received this file from a 
source other than Adobe, then your use, modification, or distribution of it requires the prior 
written permission of Adobe.
*/

/* A DOMWalker to convert RDF to one NormTree per schema. */


#ifndef RDFTONORMTREES_H
#define RDFTONORMTREES_H /* as nothing */


#include "XAPTkInternals.h"
#include "DOMWalker.h"
#include "NormTree.h"


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


/* ===== Types ===== */

// Global Namespace Types
typedef XAPObjWrapper<Element> WElement;

namespace XAPTk {

// XAPTk Namespace Types
typedef std::stack < WElement > StacOWElement;
typedef std::map < XAPTk_PairOfString , size_t > SizeByPair;
typedef std::map < XAPTk_PairOfString , XAPStructContainerType > CTypeByPair;

XAP_ENUM_TYPE ( RDFStates ) {
    state_init = 0,
    state_ignore,
    state_rdf,
    state_desc,
    state_prop,
    state_container
};

/* ===== Class ===== */

class RDFToNormTrees : public DOMWalker {
public:
    /* ===== Types ===== */
    XAP_ENUM_TYPE ( IDOrNS ) { selectByID, selectByNS };

    /* ===== Instance Variables ===== */
    Document* m_rdfDoc;
    NormTreeByStr* m_normBySchema;
    NormTreeByStr* m_normByID;
    bool m_keepText;
    bool m_needSchema;
    bool m_needTag;
    bool m_foundTag;
    RDFStates m_state;
    RDFStates m_saveState;
    Element* m_beingIgnored;
    Element* m_beingLiteral;
    Element* m_beingRDFVal;
    IDOrNS m_selectBy;
    std::string m_currentID;
    XAPTk_PairOfString m_currentSchema; /*[first: prefix, second: ns]*/
    StackOfNSDefs m_nsDefs;
    StacOWElement m_beingAssigned;
    XAPTk_StackOfString m_xmlLang;
    std::string m_about;

    /* ===== Public Constructor ===== */
    RDFToNormTrees();
    RDFToNormTrees(Document* rdfDoc, NormTreeByStr* bySchema, NormTreeByStr* byID, const bool needTag);

    /* ===== Public Destructor ===== */
    virtual ~RDFToNormTrees();

    /* ===== Public Member Functions ===== */

        /** Found an element node. */
    virtual bool
    enterElement(Element* e);

        /** All children of element done. */
    virtual void
    exitElement(Element* e);

        /** Abort the tree walk. */
    virtual bool
    finished();

        /** Handle non-element, non-text node. */
    inline virtual void
    handleNode ( Node * /* node */) { /* NO-OP */ }

        /** Found a char data node. */
    virtual void
    text(CharacterData* cd);

protected:
    /* ===== Instance Variables ===== */
    bool m_finished;

    /* ===== Protected Member Functions ===== */

    void
    addAttribute(const std::string& attr, const std::string& val);

    void
    assignProp(const std::string& val);

    void
    enterLiteral(Element* e);

    void
    exitLiteral(Element* e);

        /** first = localPart, second = ns */
    const XAPTk_PairOfString
    expandName ( const std::string& name, const Element * attrParent = 0 );

    bool
    getFeatures(XAPFeatures& f);

    void
    grokDescription(Element* e);

    void
    grokItem(Element* e);

    void
    grokGlobalAttrs(NamedNodeMap* e);

    size_t
    grokPropAttrs(Element* e, const bool checkAbbrev = true);

    void
    grokProperty(Element* e);

    void
    grokRDF(Element* e);

    void
    grokRDFValue(Element* e);

    Element*
    grokRepeats(NormTree* norm, Element* p);

    void
    grokStructContainer(Element* e, XAPStructContainerType type);

    void
    grokText(const std::string& txt);

    void
    grokTypedNode(Element* e);

        /** Are we defining a top-level property? */
    bool
    isTopLevel(const std::string& ns) const;

    void
    literalText(const std::string& txt);

    void
    popProp();

    void
    postProcess(const std::string& ns, NormTree* norm);

    void
    pushProp(const std::string& localPart);

    NormTree*
    selectNorm() const;

    void
    setFeatures(const XAPFeatures& f);

    void
    setNSDef(const std::string& origName, const std::string& ns);
};


} // XAPTk


#if macintosh
	#pragma options align=reset
#endif


#endif // RDFTONORMTREES_H

/*
$Log$
*/

