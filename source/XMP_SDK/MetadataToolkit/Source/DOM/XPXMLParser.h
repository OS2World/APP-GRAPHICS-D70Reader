
/* $Header: //xaptk/xmldom/XPXMLParser.h#2 $ */
/*
 *  Expat XML parser
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
 *  XMLParser.h
 *
 *      C++ wrapper to Expat XML parser.
 */

#ifndef _H_XMLParser
#define _H_XMLParser


#ifdef WIN_ENV
#pragma warning(disable:4786) // Win: truncated browse info symbol
#endif

#include "xmlparse.h"


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


#ifndef XMLDOMAPI
#define XMLDOMAPI /* as nothing */
#endif

/* from xmlparse.h ...
enum XML_Error {
  XML_ERROR_NONE,
  XML_ERROR_NO_MEMORY,
  XML_ERROR_SYNTAX,
  XML_ERROR_NO_ELEMENTS,
  XML_ERROR_INVALID_TOKEN,
  XML_ERROR_UNCLOSED_TOKEN,
  XML_ERROR_PARTIAL_CHAR,
  XML_ERROR_TAG_MISMATCH,
  XML_ERROR_DUPLICATE_ATTRIBUTE,
  XML_ERROR_JUNK_AFTER_DOC_ELEMENT,
  XML_ERROR_PARAM_ENTITY_REF,
  XML_ERROR_UNDEFINED_ENTITY,
  XML_ERROR_RECURSIVE_ENTITY_REF,
  XML_ERROR_ASYNC_ENTITY,
  XML_ERROR_BAD_CHAR_REF,
  XML_ERROR_BINARY_ENTITY_REF,
  XML_ERROR_ATTRIBUTE_EXTERNAL_ENTITY_REF,
  XML_ERROR_MISPLACED_XML_PI,
  XML_ERROR_UNKNOWN_ENCODING,
  XML_ERROR_INCORRECT_ENCODING
};
*/

/* Could add error code. message is for debugging (unless we internat. it) */
class XMLDOMAPI XMLParserException
{
public:
    XMLParserException() {}
    XMLParserException(const char *message) : m_message(message) {}

    const char *getMessage() { return m_message; }
private:
    const char *m_message;
};

class XMLDOMAPI XMLDocumentHandler
{
public:
    /* Information is UTF-8 encoded. */

    /* atts is array of name/value pairs, terminated by NULL;
       names and values are '\0' terminated. */

    virtual void startElement(const char *name, const char **atts) = 0;
    virtual void endElement(const char *name) = 0;
    virtual void characterData(const char *s, int len) = 0;
    virtual void processingInstruction(const char *target, const char *data) = 0;

    virtual ~XMLDocumentHandler() {}
};

class XMLDOMAPI XMLParser
{
public:
    XMLParser(const char *encoding)
    {
        m_parser = SYMUSE(XML_ParserCreate)(encoding);
        if (!m_parser)
            throw XMLParserException();
        SYMUSE(XML_SetUserData)(m_parser, this);
        SYMUSE(XML_SetElementHandler)(m_parser, startElement, endElement);
        SYMUSE(XML_SetCharacterDataHandler)(m_parser, characterData);
        SYMUSE(XML_SetProcessingInstructionHandler)(m_parser, processingInstruction);
    }

    ~XMLParser()
    {
        if (m_parser)
            SYMUSE(XML_ParserFree)(m_parser);
    }

    void setDocumentHandler(XMLDocumentHandler *handler)
    {
        m_handler = handler;
    }

    /* Parses some input. Returns false if a fatal error is detected.
    The last call to Parse must have isFinal true;
    len may be zero for this call (or any other). */
    bool parse(const char *s, int len, bool isFinal)
    {
        return SYMUSE(XML_Parse)(m_parser, s, len, isFinal) != 0;
    }

    void *getBuffer(int len)
    {
        return SYMUSE(XML_GetBuffer)(m_parser, len);
    }

    bool parseBuffer(int len, bool isFinal)
    {
        return SYMUSE(XML_ParseBuffer)(m_parser, len, isFinal)!= 0;
    }

    /* If Parse or ParseBuffer have returned false, then getError*
    returns information about the error. */

    int getErrorCode() { return SYMUSE(XML_GetErrorCode)(m_parser); }
    int getErrorLineNumber() { return SYMACRO(XML_GetErrorLineNumber)(m_parser); }
    int getErrorColumnNumber() { return SYMACRO(XML_GetErrorColumnNumber)(m_parser); }
    long getErrorByteIndex() { return SYMACRO(XML_GetErrorByteIndex)(m_parser); }

    static const char *errorString(int code) { return SYMUSE(XML_ErrorString)(code); }

private:
    XML_Parser m_parser;
    XMLDocumentHandler *m_handler;

    static void startElement(void *userData, const char *name, const char **atts)
    {
        XMLParser *thisParser = (XMLParser *) userData;
        XMLDocumentHandler *handler = thisParser->m_handler;
        if (handler)
            handler->startElement(name, atts);
    }

    static void endElement(void *userData, const char *name)
    {
        XMLParser *thisParser = (XMLParser *) userData;
        XMLDocumentHandler *handler = thisParser->m_handler;
        if (handler)
            handler->endElement(name);
    }

    static void characterData(void *userData, const char *s, int len)
    {
        XMLParser *thisParser = (XMLParser *) userData;
        XMLDocumentHandler *handler = thisParser->m_handler;
        if (handler)
            handler->characterData(s, len);
    }

    static void processingInstruction(void *userData, const char *target, const char *data)
    {
        XMLParser *thisParser = (XMLParser *) userData;
        XMLDocumentHandler *handler = thisParser->m_handler;
        if (handler)
            handler->processingInstruction(target, data);
    }

};


#if macintosh
	#pragma options align=reset
#endif


#endif /* not _H_XMLParser */

/*
$Log$
*/

