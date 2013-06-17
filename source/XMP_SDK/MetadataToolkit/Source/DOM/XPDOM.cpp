/* $Header: //xaptk/xmldom/XPDOM.cpp#2 $ */ 
/*
 *	Expat XML parser
 *
 *
 *	ADOBE SYSTEMS INCORPORATED
 *	Copyright 2001 Adobe Systems Incorporated
 *	All Rights Reserved
 *
 *	NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
 *	terms of the Adobe license agreement accompanying it.
 * 
 *
 *	XPDOM.cpp
 *
 *		Implementation of C++ wrapper to Expat XML parser + DOM.
 */

#include "XAP_XPDOMConf.h"
#include "XPDOM.h"
#include "XPElementFactory.h"
#include "XPNode.h"


XPParser::XPParser(XPDocument* document, const XPString& encoding, DOMImplementation* pDomImpl) :
	m_document(document), m_pDomImpl(pDomImpl)
{ 
	assert(document);
	
	if (encoding.empty())
		m_encoding = NULL;
	else
	{
		XPString::size_type n = encoding.length();
		m_encoding = new XMP_Debug_new char[n+1];
		encoding.copy(m_encoding, XPString::npos);
		m_encoding[n]= '\0';
	}
	m_parser = new XMP_Debug_new XMLParser(m_encoding);
	m_parser->setDocumentHandler(this);
	m_factory = NULL;
	m_factories = new XMP_Debug_new XPHashtable();
	m_node = NULL;
	m_text = NULL;
}

XPParser::~XPParser()
{
	delete m_encoding;
	delete m_parser;
	// don't delete m_document!!!
	delete m_factory;
	delete m_factories;
}

/*
void XPParser::ensureDocument()
{
	if (m_document == NULL)
	{
		if (m_factory == NULL)
			m_factory = new XMP_Debug_new XPElementFactory();
		m_document = m_factory->createDocument( m_pDomImpl );
		m_node = m_document;
	}
}
*/

void XPParser::startElement(const char* name, const char** atts)
{
	ElementFactory* factory = reinterpret_cast<ElementFactory*>(m_factories->get(name));

	NamedNodeMap* attList = NULL;
	int n = 0;
	while (atts[n])
		n++;
	XPString* attStrings = new XMP_Debug_new XPString[n+1];
	for (int i = 0; i < n; i++)
		attStrings[i] = atts[i];
	attStrings[n] = "";

	attList = new XMP_Debug_new XPNamedNodeMap(m_document);

	// XXX: This entire method should be optimized to remove the below while loop! -jdg
	XPString* tempAttrStrings = attStrings;
	while ( tempAttrStrings->empty() == false )
	{
		XPAttr* att = new XMP_Debug_new XPAttr(m_document, *tempAttrStrings, *(tempAttrStrings+1));
		attList->setNamedItem( att );
		tempAttrStrings += 2;
	}

	Node* node = NULL;
	if (factory != NULL)
		node = factory->createElement(name, attList);
	if (node == NULL)
		node = m_factory->createElement(name, attList);
	delete attList;
	delete[] attStrings;

	m_node->insertBefore(node, NULL);
	m_node = node;
	m_text = NULL;
}

void XPParser::endElement(const char* name)
{
	ElementFactory* factory = reinterpret_cast<ElementFactory*>(m_factories->get(name));
	if (factory != NULL)
		factory->endElement(dynamic_cast<Element*>(m_node));
	else
		m_factory->endElement(dynamic_cast<Element*>(m_node));
	m_node = m_node->getParentNode();
	m_text = NULL;
}

void XPParser::characterData(const char* s, int len)
{
    if (m_text == NULL)
    {
        m_text = m_factory->createText(XPString(s, len));
        if (m_text != NULL)
            m_node->insertBefore(m_text, NULL);
    }
    else
	{
		XPText* xpText = dynamic_cast<XPText*>(m_text);
		assert(xpText);
        xpText->append(s, len);
	}
}

void XPParser::processingInstruction(const char* target, const char* data)
{
    ElementFactory* factory = reinterpret_cast<ElementFactory*>(m_factories->get(target));
    Node* node = NULL;
    if (factory != NULL)
        node = factory->createProcessingInstruction(target, data);
    if (node == NULL)
        node = m_factory->createProcessingInstruction(target, data);
    if (node == NULL)
        return;
    m_node->insertBefore(node, NULL);
    m_text = NULL;
}


bool XPDOMImplementation::hasFeature(const DOMString& feature, const DOMString& version)
{
	XPString lowerCaseFeature = feature;
	lowerCaseFeature.toLower();
	float v = XPElement::parseNumber(version, 1.0);

	if( (lowerCaseFeature == "xml") && v == 1.0 )
		return true;

	return false;
}

/* Call abortParse if you stop parsing and don't intend to
 * immediately destroy the parser. */
void XPParser::abortParse()
{
	while (m_node != NULL)
	{
		endElement(m_node->getNodeName().c_str());
	}
}

 
/* 
$Log$ 
*/ 
