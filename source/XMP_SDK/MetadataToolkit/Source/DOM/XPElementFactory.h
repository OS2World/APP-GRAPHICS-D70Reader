
/* $Header: //xaptk/xmldom/XPElementFactory.h#2 $ */ 
/*
 *	XP DOM ElementFactory default implementation
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
 *	XPElementFactory.cpp
 *
 *		Factory for creating DOM objects.
 *		Based on xml4j ElementFactory
 */


#ifndef _H_XPElementFactory
#define _H_XPElementFactory


#ifdef WIN_ENV
#pragma warning(disable:4275)   // disable C4275 warning about non dll-interface base class
#endif

#include "XAP_XPDOMConf.h"
#include "ElementFactory.h"
#include "XPNode.h"


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


class XMLDOMAPI XPElementFactory : public ElementFactory
{
public:
	XPElementFactory(XPDocument* document) : m_document(document) 
	{
	}

    /**
     * Create <code>Element</code> instance.
     * This method is called after parsing start-tag before parsing its children.
     */
    Element *createElement(const XPString& name, const NamedNodeMap* attributes)
	{
		return new XMP_Debug_new XPElement(m_document, name, attributes);
	}

    /**
     * Create <code>Text</code> instance.
     */
    Text *createText(const XPString& s, int len)
	{
		return new XMP_Debug_new XPText(m_document, s, 0, len);
	}

    /**
     * Create <code>PI</code> instance.
     */
    ProcessingInstruction *createProcessingInstruction(const XPString& target, const XPString& data)
	{
		return new XMP_Debug_new XPProcessingInstruction(m_document, target, data);
	}

    /**
     * Create <code>Document</code> instance.
     */
    Document *createDocument( DOMImplementation* pDomImpl )
	{
		return new XMP_Debug_new XPDocument(pDomImpl);
	}

	/**
	 * Process the end of an element.
	 */
	void endElement ( Element * /* element */ )
	{
	}
protected:
	XPDocument* m_document;
};


#if macintosh
	#pragma options align=reset
#endif


#endif // _H_XPElementFactory
 
/* 
$Log$ 
*/ 

