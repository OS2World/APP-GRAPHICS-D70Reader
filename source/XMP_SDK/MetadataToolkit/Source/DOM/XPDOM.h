
/* $Header: //xaptk/xmldom/XPDOM.h#1 $ */ 
/*
 *	DOM class based on expat
 * 
 *
 *	ADOBE SYSTEMS INCORPORATED
 *	Copyright 2001 Adobe Systems Incorporated
 *	All Rights Reserved
 *
 *	NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
 *	terms of the Adobe license agreement accompanying it.  If you have received this file from a 
 *	source other than Adobe, then your use, modification, or distribution of it requires the prior 
 *	written permission of Adobe.
 *
 *
 *	XPDom.h
 *
 *		C++ wrapper to Expat XML parser + DOM.
 */


#ifndef _H_XPDOM
#define _H_XPDOM


#include "XMPAssert.h"
#include "DOM.h"
#include "XPXMLParser.h"
#include "ElementFactory.h"
#include "XPHashtable.h"
#include "XPNode.h"


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


#ifdef WIN_ENV
#pragma warning(disable:4275)   // disable C4275 warning about non dll-interface base class
#endif


/************************************************************************************
 ********************************** XPParser *************************************
 ************************************************************************************/
class XMLDOMAPI XPParser : public XMLDocumentHandler
{
public:
	XPParser(XPDocument* document, const XPString& encoding, DOMImplementation* pDomImpl);
	virtual ~XPParser();

	void setDefaultElementFactory(ElementFactory* factory)
	{
		assert(m_node == NULL);	// shouldn't be adding factories once we've started parsing
		m_factory = factory;
	}

	void setElementFactory(ElementFactory* factory, const XPString& elementName)
	{
		assert(m_node == NULL);	// shouldn't be adding factories once we've started parsing
		m_factories->put(elementName, factory);
	}

	/*
	Document* getDocument()
	{
		return m_document; 
	}
	*/

	/* Parses some input. Returns false if a fatal error is detected.
	 * The last call to Parse must have isFinal true;
	 * len may be zero for this call (or any other). */
	bool parse(const char* s, int len, bool isFinal)
	{
		assert(m_document != NULL);
		if (m_document == NULL)
			return false; // currently not supported by getError.... routines

		if (m_node == NULL)
		{
			assert(m_document->getDocumentElement() == NULL);
			m_node = m_document;
		}
		
		assert(m_node == NULL || m_node == m_document || m_node->getOwnerDocument() == m_document); // just a safety check

		return m_parser->parse(s, len, isFinal);
	}

	void* getBuffer(int len)
	{
		return m_parser->getBuffer(len);
	}

	bool parseBuffer(int len, bool isFinal)
	{
		if (m_node == NULL && m_document->getDocumentElement() == NULL)
			m_node = m_document;
		
		assert(m_node == m_document || m_node->getOwnerDocument() == m_document); // just a safety check
		
		return m_parser->parseBuffer(len, isFinal);
	}

	/* Call abortParse if you stop parsing and don't intend to
	 * immediately destroy the parser. */
	virtual void abortParse();

	/* If Parse or ParseBuffer have returned false, then getError*
	 * returns information about the error. */

	int getErrorCode() { return m_parser->getErrorCode(); }
	int getErrorLineNumber() { return m_parser->getErrorLineNumber(); }
	int getErrorColumnNumber() { return m_parser->getErrorColumnNumber(); }
	long getErrorByteIndex() { return m_parser->getErrorByteIndex(); }

	static const char* errorString(int code) { return XMLParser::errorString(code); }

private:
	char* m_encoding;
	XMLParser* m_parser;
	XPDocument* m_document;
	ElementFactory* m_factory;
	XPHashtable* m_factories;
	Node* m_node;
	Text* m_text;
	DOMImplementation* m_pDomImpl;

	/*
	void ensureDocument();
	*/

	// XMLDocumentHandler methods
	void startElement(const char* name, const char** atts);
	void endElement(const char* name);
	void characterData(const char* s, int len);
	void processingInstruction(const char* target, const char* data);
};

/************************************************************************************
 ***************************** XPDOMImplementation **********************************
 ************************************************************************************/
class XMLDOMAPI XPDOMImplementation : public DOMImplementation
{
public:
	static Document* createDocument ( const XPString& /* type */ )
	{
		throw DOMException( UNIMPLEMENTED_DOM_ERR );
		return NULL;
	}
	
	/************************************************************************
	 * XPDOMImplementation::hasFeature()		  overrides DOMImplementation
	 *
	 *	 Tests if the DOM implementation implements a specific feature.
	 *		feature - the package name of the feature to test.  In level 1,
	 *				  the legal values are "HTML" and "XML" (case-insensitive).
	 *		version - Version number of the package name to test.
	 *				  In Level 1, it is "1.0".  If the version is not supplied,
	 *				  supporting any version of the feature will cause the
	 *				  a return of true.
	 ************************************************************************/
	virtual bool hasFeature(const DOMString& feature, const DOMString& version);

	/*
	// Create a parser with the specified encoding. 
	static XPParser* createParser(const char* encoding, DOMImplementation* pDomImpl)
	{
		return new XMP_Debug_new XPParser(encoding, pDomImpl);
	}
	*/
};


#if macintosh
	#pragma options align=reset
#endif


#endif /* not _H_XPDOM */
 
/* 
$Log$ 
*/ 

