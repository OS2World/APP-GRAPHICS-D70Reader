/* $Header: //xaptk/xmldom/DOMDoc.cpp#5 $ */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/

#include "XMPAssert.h"

#include "DOMDoc.h"

#include "XPDOM.h"
#include "XPElementFactory.h"
#include "XPNode.h"
#include "XMPInitTerm.h"

using namespace std ;

#define DECL_STATIC /* as nothing */

#define NUMBER_OF_COMMON_STRING_IDS 7

#if 1
XMLDOMAPI XPString * COMMON_STRINGS = NULL;		// *** Avoid static objects.
#else
const XMLDOMAPI XPString COMMON_STRINGS[] = { //STATIC_INIT
    ////////////////////////////////////////////////////////////
    // special cases of strings we want permanent copies of
    XPString(NUMBER_OF_COMMON_STRING_IDS, 0, "")         // EMPTY_STRING
    // first in list uses special constructor to initialize internal array
    // in XPString class
    ,XPString( 1, " ")        // SPACE
    ,XPString( 2, " \r\n\t")  // WHITESPACE_CHARS
    ,XPString( 3, ",\r\n\t ") // PATHDATA SEPARATORS
    ,XPString( 4, "xml:space")
    ,XPString( 5, "xml:lang")
    ,XPString( 6, "xmlns")
    // NOTE: if new special cases are added, the indices must be kept 'in sync'
    ////////////////////////////////////////////////////////////
    // standard permanent common strings stored in external list
//  #define COMMON_STRING_LISTITEM(name)    ,XPString(COMMON_STRING_ID(name), #name)
//  #include "SVGCommonStringsList.h"
//  #undef COMMON_STRING_LISTITEM
    ////////////////////////////////////////////////////////////
};
#endif

namespace XMLDOM {

/* ===== Class Declaration ===== */

class DOMDoc : public XPDocument {
public:
    /* ===== Instance Variables ===== */
    XPDOMImplementation* m_xpimpl;
    XPElementFactory* m_eFactory;
    XPParser* m_parser;
    char* m_encoding;

    /* ===== Public Destructor ===== */
    virtual ~DOMDoc();

    /* ===== Public ===== */

    /* ===== Overrides Of XPDocument Functions ===== */
    Element*
    createElement(const DOMString& tagName) const;

    Text*
    createTextNode(const DOMString& data) const;

protected:
    friend Document* NewDoc();
    explicit DOMDoc(XPDOMImplementation* impl);
    DOMDoc(); // Default
    DOMDoc(DOMDoc& rhs); // Copy
    DOMDoc& operator=(DOMDoc& rhs);
}; // DOMDoc




/* ===== Module Functions ===== */

size_t
CountElements(Element* e) {
    NodeList* nl = e->getChildNodes();
    size_t eCount = 0;
    if (nl == NULL)
        return(eCount);
    unsigned long n = nl->getLength();
    for (unsigned long i = 0; i < n; ++i) {
        Node* node = nl->item(i);
        if (node->getNodeType() == ELEMENT_NODE)
            ++eCount;
    }
    return(eCount);
}

Document*
NewDoc() {
    //TBD: XAPAllocator
    XPDOMImplementation* impl = new XMP_Debug_new XPDOMImplementation();
    return new XMP_Debug_new DOMDoc(impl);
}

bool
Parse(Document* d, const char* buf, const size_t n, const bool last /*=false*/) {
    DOMDoc* g = dynamic_cast<DOMDoc*>(d);
    assert(g != NULL);
    return g->m_parser->parse(buf, n, last);
}


DOMString
GetError(Document* d, int* line, int* col) {
    DOMDoc* g = dynamic_cast<DOMDoc*>(d);
    assert(g != NULL);
    int code = g->m_parser->getErrorCode();
    const char* message = g->m_parser->errorString(code);
    int ln = g->m_parser->getErrorLineNumber();
    int cn = g->m_parser->getErrorColumnNumber();
    if (line != NULL)
        *line = ln;
    if (col != NULL)
        *col = cn;
    DOMString ret(message);
    return(ret);
}




/* ===== Explicit Constructor ===== */

DOMDoc::DOMDoc(XPDOMImplementation* impl) :
  XPDocument(impl),
  m_xpimpl(impl),
  m_eFactory(NULL),
  m_parser(NULL),
  m_encoding(NULL)
{
    m_eFactory = new XMP_Debug_new XPElementFactory(this);
    m_parser = new XMP_Debug_new XPParser(this, m_encoding, m_xpimpl);

    m_parser->setDefaultElementFactory(m_eFactory);
        // eFactory belongs to XParser now
}


/* ===== Destructor ===== */

DOMDoc::~DOMDoc() {
    delete m_parser; // deletes m_eFactory (I think ...)
    // superclass deletes m_xpimpl
}

/* ===== Overrides of XPDocument Functions ===== */

Element*
DOMDoc::createElement(const DOMString& tagName) const {
    // TBD: XAPAllocator
    XPDocument* doc = const_cast<DOMDoc*>(this);
    return new XMP_Debug_new XPElement(doc, tagName, NULL);
}

Text*
DOMDoc::createTextNode(const DOMString& data) const {
    // TBD: XAPAllocator
    XPDocument* doc = const_cast<DOMDoc*>(this);
    return new XMP_Debug_new XPText(doc, data, 0, data.size());
}


} // XMLDOM


void
XAPTk_InitDOM() {

	if ( COMMON_STRINGS == 0 ) {
	
		COMMON_STRINGS = new XPString [NUMBER_OF_COMMON_STRING_IDS];
		
		XPString::initAtomArray ( NUMBER_OF_COMMON_STRING_IDS );	// ! Was called by wacky array constructor.
		
		COMMON_STRINGS[0] = XPString ( "" );
		COMMON_STRINGS[1] = XPString ( " " );
		COMMON_STRINGS[2] = XPString ( " \r\n\t" );
		COMMON_STRINGS[3] = XPString ( ",\r\n\t " );
		COMMON_STRINGS[4] = XPString ( "xml:space" );
		COMMON_STRINGS[5] = XPString ( "xml:lang" );
		COMMON_STRINGS[6] = XPString ( "xmlns" );
	
	}

}

void
XAPTk_KillDOM() {
	delete [] COMMON_STRINGS;
	COMMON_STRINGS = 0;
    XPString::KillXPString();
}

/*
$Log$
*/
