
/* $Header: //xaptk/xmldom/DOM.h#2 $ */
/*******************************************************************************
 * Dom.h
 *                       ADOBE CONFIDENTIAL
 *                     _ _ _ _ _ _ _ _ _ _ _ _
 *
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
 *
 *  DESC:   C++ language binding for DOM Level 1 Core.
 *          Here is the IDL -> C++ translation from the DOM specification.
 *          DOMStrings are passed by value and mapped to XPStrings
 *          all other objects are passed by pointer
 *          attributes are accessed via set and get methods
 *
 *  $Header: //xaptk/xmldom/DOM.h#2 $
 *******************************************************************************/


#ifndef _H_DOM
#define _H_DOM


#ifndef XMLDOMAPI
#define XMLDOMAPI /* as nothing */
#endif

#include "XPString.h"


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


typedef XPString DOMString;

class DOMException;
class DOMImplementation;
class DocumentFragment;
class Document;
class Node;
class NodeList;
class NamedNodeMap;
class CharacterData;
class Attr;
class Element;
class Text;
class Comment;
class CDATASection;
class DocumentType;
class Notation;
class Entity;
class EntityReference;
class ProcessingInstruction;


enum {
    INDEX_SIZE_ERR = 1,
    DOMSTRING_SIZE_ERR,
    HIERARCHY_REQUEST_ERR,
    WRONG_DOCUMENT_ERR,
    INVALID_CHARACTER_ERR,
    NO_DATA_ALLOWED_ERR,
    NO_MODIFICATION_ALLOWED_ERR,
    NOT_FOUND_ERR,
    NOT_SUPPORTED_ERR,
    INUSE_ATTRIBUTE_ERR,
    UNIMPLEMENTED_DOM_ERR   // our addition
};

enum {
    ELEMENT_NODE = 1,
    ATTRIBUTE_NODE,
    TEXT_NODE,
    CDATA_SECTION_NODE,
    ENTITY_REFERENCE_NODE,
    ENTITY_NODE,
    PROCESSING_INSTRUCTION_NODE,
    COMMENT_NODE,
    DOCUMENT_NODE,
    DOCUMENT_TYPE_NODE,
    DOCUMENT_FRAGMENT_NODE,
    NOTATION_NODE
};

class DOMException
{
    public:
        unsigned short code;
        explicit DOMException( unsigned short _code ) : code( _code ) {}
};


// ----------------------------------------------------------------------------

class DOMImplementation
{
    public:
        virtual bool  hasFeature( const DOMString& feature, const DOMString& version ) = 0;

        virtual ~DOMImplementation() {}
};

// ----------------------------------------------------------------------------

class Node
{
    public:
        virtual DOMString       getNodeName() const = 0;
        virtual DOMString       getNodeValue() const = 0;
        virtual void            setNodeValue( const DOMString& value ) = 0;
        virtual unsigned short  getNodeType() const = 0;
        virtual Node*           getParentNode() const = 0;
        virtual NodeList*       getChildNodes() const = 0;
        virtual Node*           getFirstChild() const = 0;
        virtual Node*           getLastChild() const = 0;
        virtual Node*           getPreviousSibling() const = 0;
        virtual Node*           getNextSibling() const = 0;
        virtual NamedNodeMap*   getAttributes() const = 0;
        virtual Document*       getOwnerDocument() const = 0;
        virtual Node*           insertBefore(Node *newChild, Node *refChild) = 0;
        virtual Node*           replaceChild(Node *newChild, Node *oldChild) = 0;
        virtual Node*           removeChild(Node *oldChild) = 0;
        virtual Node*           appendChild( Node* newChild ) = 0;
        virtual bool            hasChildNodes() const = 0;
        virtual Node*           cloneNode( bool deep ) const = 0;
		
		virtual void			setFlagBits ( long bits ) = 0;
		virtual void			clearFlagBits ( long bits ) = 0;
		virtual long			getFlagBits() const = 0;
		virtual bool			checkFlagBits ( long bits ) const = 0;

        virtual ~Node() {}
};

// ----------------------------------------------------------------------------

class DocumentFragment : public virtual Node
{
    protected:
        virtual ~DocumentFragment() {}
};

// ----------------------------------------------------------------------------

class Document : public virtual Node
{
    public:
        virtual ~Document() {}

        virtual DocumentType*       getDoctype() const = 0;
        virtual DOMImplementation*  getImplementation() const = 0;
        virtual Element*            getDocumentElement() const = 0;

        virtual Element*            createElement(const DOMString& tagName) const = 0;
/*U*/   virtual DocumentFragment*   createDocumentFragment() const = 0;
        virtual Text*               createTextNode(const DOMString& data) const = 0;
/*U*/   virtual Comment*            createComment(const DOMString& data) const = 0;
/*U*/   virtual CDATASection*       createCDATASection( const DOMString& data ) const = 0;
/*U*/   virtual ProcessingInstruction*   createProcessingInstruction(const DOMString& name, const DOMString& data) const = 0;
/*U*/   virtual Attr*               createAttribute(const DOMString& name) const = 0;
/*U*/   virtual EntityReference*    createEntityReference(const DOMString& name ) const = 0;
/*U*/   virtual NodeList*           getElementsByTagName(const DOMString& tagname) const = 0;
};

// ----------------------------------------------------------------------------

class NodeList
{
    public:
        virtual Node*           item( unsigned long index ) const = 0;
        virtual unsigned long   getLength() const = 0;

        virtual ~NodeList() {}
};

// ----------------------------------------------------------------------------

class NamedNodeMap
{
    public:
        virtual Node*           getNamedItem( const DOMString& name ) const = 0;
        virtual Node*           setNamedItem( Node* arg ) = 0;
/*U*/   virtual Node*           removeNamedItem( const DOMString& name ) = 0;
        virtual Node*           item( unsigned long index ) const = 0;
        virtual unsigned long   getLength() const = 0;

        virtual ~NamedNodeMap() {}
};

// ----------------------------------------------------------------------------

class Attr : public virtual Node
{
    public:
        virtual DOMString       getName() const = 0;
        virtual bool            getSpecified() const = 0;
        virtual DOMString       getValue() const = 0;
        virtual void            setValue( const DOMString& value ) = 0;

    protected:
        virtual ~Attr() {}
};

// ----------------------------------------------------------------------------

class Element : public virtual Node
{
    public:
        virtual ~Element() {}
        virtual DOMString           getTagName() const = 0;
        virtual DOMString           getAttribute(const DOMString& name) const = 0;
        virtual void                setAttribute(const DOMString& name, const DOMString& value) = 0;
/*U*/   virtual void                removeAttribute(const DOMString& name) = 0;
        virtual Attr*               getAttributeNode(const DOMString& name) const = 0;
        virtual Attr*               setAttributeNode(Attr* newAttr) = 0;
/*U*/   virtual Attr*               removeAttributeNode(Attr* oldAttr) = 0;
/*U*/   virtual NodeList *          getElementsByTagName(const DOMString& name) const = 0;
/*U*/   virtual void                normalize() = 0;
};

// ----------------------------------------------------------------------------

class CharacterData : public virtual Node
{
    public:
        virtual ~CharacterData() {}
        virtual DOMString   getData() const = 0;
        virtual void        setData(const DOMString& arg) = 0;
        virtual unsigned long getLength() const = 0;
        virtual DOMString   substringData(unsigned long offset, unsigned long count) const = 0;
        virtual void        appendData(const DOMString& arg) = 0;
        virtual void        insertData(unsigned long offset, const DOMString& arg) = 0;
        virtual void        deleteData(unsigned long offset, unsigned long count) = 0;
        virtual void        replaceData(unsigned long offset, unsigned long count, const DOMString& arg ) = 0;
};

// ----------------------------------------------------------------------------

class Text : public virtual CharacterData
{
    public:
/*U*/   virtual Text*      splitText( unsigned long offset ) = 0; // throw( DOMException )

    protected:
        virtual ~Text() {}
};


// ----------------------------------------------------------------------------

class Comment : public virtual CharacterData
{
    protected:
        virtual ~Comment() {}
};

// ----------------------------------------------------------------------------

class CDATASection : public Text
{
    protected:
        virtual ~CDATASection() {}
};

// ----------------------------------------------------------------------------

class DocumentType : public virtual Node
{
    public:
        virtual DOMString       getName() = 0;
        virtual NamedNodeMap*   getEntities() = 0;
        virtual NamedNodeMap*   getNotations() = 0;

    protected:
        virtual ~DocumentType() {}
};

// ----------------------------------------------------------------------------

class Notation : public virtual Node
{
    public:
        virtual DOMString   getPublicId() = 0;
        virtual DOMString   getSystemId() = 0;

    protected:
        virtual ~Notation() {}
};

// ----------------------------------------------------------------------------

class Entity : public virtual Node
{
    public:
        virtual DOMString   getPublicId() = 0;
        virtual DOMString   getSystemId() = 0;
        virtual DOMString   getNotationName() = 0;

    protected:
        virtual ~Entity() {}
};

// ----------------------------------------------------------------------------

class EntityReference : public virtual Node
{
    protected:
        virtual ~EntityReference() {}
};


// ----------------------------------------------------------------------------

class ProcessingInstruction : public virtual Node
{
     public:
        virtual ~ProcessingInstruction() {}

        virtual DOMString       getTarget() const = 0;
        virtual DOMString       getData() const = 0;
        virtual void            setData(const DOMString& arg) = 0;
};


#if macintosh
	#pragma options align=reset
#endif


#endif /* not _H_DOM */

/*
$Log$
*/

