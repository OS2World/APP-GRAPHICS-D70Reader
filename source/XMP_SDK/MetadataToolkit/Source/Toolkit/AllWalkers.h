
/* $Header: //xaptk/xaptk/allwalkers.h#7 $ */
/* allwalkers.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.  If you have received this file from a 
source other than Adobe, then your use, modification, or distribution of it requires the prior 
written permission of Adobe.
*/

/* All general purpose DOMWalker class implementations go here. */


#ifndef ALLWALKERS_H
#define ALLWALKERS_H /* as nothing */


#include <string>

#include "XAPTkInternals.h"
#include "DOMGlue.h"


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


using namespace std;

#if defined(XAP_DEBUG_CERR) && 1

namespace XAPTk {

class DebugSerializeAll : public DOMWalker {
public:
    /* ===== Constants ===== */

    /* ===== Instance Variables ===== */
    string* m_str;
    bool m_charOK;
    bool m_needHeader;

    /* ===== Public Constructor ===== */
    DebugSerializeAll() : // Default
        m_str(NULL),
        m_charOK(true),
        m_needHeader(true),
        m_finished(false),
        m_level(0)
    {}

    explicit
    DebugSerializeAll(string* str) :
        m_str(str),
        m_charOK(true),
        m_needHeader(true),
        m_finished(false),
        m_level(0)
    {}

    /* ===== Public Destructor ===== */
    virtual ~DebugSerializeAll() {}

    /* ===== Public Member Functions ===== */

        /** Found an element node. */
    inline virtual bool
    enterElement(Element* e) {
        if (m_needHeader) {
            m_needHeader = false;
        }
        for (size_t i = 0; i < m_level; ++i)
            m_str->append("    ");
        string tag = e->getTagName();
        m_str->append("<");
        m_str->append(tag);
        NamedNodeMap* attrs = e->getAttributes();
        if (attrs != NULL) {
            const unsigned long n = attrs->getLength();
            for (unsigned long i = 0; i < n; ++i) {
                Attr* a = dynamic_cast<Attr*>(attrs->item(i));
                m_str->append(" ");
                m_str->append(a->getName());
                m_str->append("='");
                m_str->append(a->getValue());
                m_str->append("'");
            }
        }
        if (e->hasChildNodes())
            m_str->append(">");
        else
            m_str->append("/>");
        Element* check = dynamic_cast<Element*>(e->getFirstChild());
        if (check != NULL)
            m_str->append(S_STNL);
        ++m_level;
        return true;	// Continue processing this element and its children.
    }

        /** All children of element done. */
    inline virtual void
    exitElement(Element* e) {
       --m_level;
        size_t eCount = 0;
        size_t tCount = 0;
        DOMGlue_CountNodes(e, eCount, tCount);
        if (eCount > 0)
            for (size_t i = 0; i < m_level; ++i)
                m_str->append("    ");
        string tag = e->getTagName();
        if (e->hasChildNodes()) {
            m_str->append("</");
            m_str->append(tag);
            m_str->append(">");
        }
        m_str->append(S_STNL);
    }

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
    inline virtual void
    text(CharacterData* cd) {
        if (m_charOK)
            m_str->append(cd->getData());
    }

    void
    finishUp() {
        // NO-OP
    }

private:
    bool m_finished;
    size_t m_level;
};


inline void
DebugSerializer(Node* root, std::string& xml) {
    DebugSerializeAll elf(&xml);
    DOMGlue_WalkTree(&elf, root);
    elf.finishUp();
}

} // XAPTk

#endif /* XAP_DEBUG_CERR */


#if macintosh
	#pragma options align=reset
#endif


#endif // ALLWALKERS

/*
$Log$
*/

