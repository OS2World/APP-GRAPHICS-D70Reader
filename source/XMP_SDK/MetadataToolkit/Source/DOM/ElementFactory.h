
/* $Header: //xaptk/xmldom/ElementFactory.h#1 $ */
/*
 *  DOM ElementFactory interface
 *
 *	ADOBE SYSTEMS INCORPORATED
 * 	Copyright 2001 Adobe Systems Incorporated
 * 	All Rights Reserved
 *
 *	NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
 *	terms of the Adobe license agreement accompanying it.  If you have received this file from a 
 *	source other than Adobe, then your use, modification, or distribution of it requires the prior 
 *	written permission of Adobe.
 *
 *
 *  ElementFactory.h
 *
 *      Interface for creating DOM objects.
 *      Based on xml4j ElementFactory
 */


#ifndef _H_ElementFactory
#define _H_ElementFactory


#ifdef WIN_ENV
#pragma warning(disable:4275)   // disable warning about non dll-interface base class
#endif

#include "DOM.h"


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif

class ElementFactory
{
public:

    /**
     * Create <code>Element</code> instance.
     * This method is called after parsing start-tag before parsing its children.
     */
    virtual Element *createElement(const XPString& name, const NamedNodeMap* attributes) = 0;

    /**
     * Create <code>Text</code> instance.
     */
    virtual Text *createText(const XPString& s, int len=-1) = 0;

    /**
     * Create <code>PI</code> instance.
     */
    virtual ProcessingInstruction *createProcessingInstruction(const XPString& target, const XPString& data) = 0;

    /**
     * Create <code>Document</code> instance.
     */
    /*  TO BE REMOVED
    virtual Document *createDocument( DOMImplementation* pDomImpl ) = 0;
    */

    /**
     * Process the end of an element.
     */
    virtual void endElement(Element *element) = 0;
};

/* Default implementation of ElementFactory methods for subclassers.
 * Note that at least one implementation of ElementFactory should not
 * return NULL for the various create methods.
 */
class XMLDOMAPI BaseElementFactory : public ElementFactory
{
public:

    /**
     * Create <code>Element</code> instance.
     * This method is called after parsing start-tag before parsing its children.
     */
    virtual Element *createElement(const XPString& /* name */, const NamedNodeMap* /* attributes */)
    {
        return NULL;
    }

    /**
     * Create <code>Text</code> instance.
     */
    virtual Text *createText(const XPString& /* s */, int /* len */)
    {
        return NULL;
    }

    /**
     * Create <code>PI</code> instance.
     */
    virtual ProcessingInstruction *createProcessingInstruction(const XPString& /* target */, const XPString& /* data */)
    {
        return NULL;
    }

    /**
     * Create <code>Document</code> instance.
     */
    /*      TO BE REMOVED
    virtual Document *createDocument(DOMImplementation* pDomImpl)
    {
        return NULL;
    }
    */

    /**
     * Process the end of an element.
     */
    virtual void endElement(Element * /* element */ )
    {
    }
};


#if macintosh
	#pragma options align=reset
#endif


#endif /* not _H_ElementFactory */

/*
$Log$
*/

