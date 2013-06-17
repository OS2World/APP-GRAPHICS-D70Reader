
/* $Header: //xaptk/xmldom/XPNode.h#1 $ */ 
/*******************************************************************************
 * XPNode.h
 *                       ADOBE CONFIDENTIAL
 *                     _ _ _ _ _ _ _ _ _ _ _ _
 *
 *
 *	ADOBE SYSTEMS INCORPORATED
 *	Copyright 2001 Adobe Systems Incorporated
 *	All Rights Reserved
 *
 *	NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
 *	terms of the Adobe license agreement accompanying it.  If you have received this file from a 
 *	source other than Adobe, then your use, modification, or distribution of it requires the prior 
 *	ritten permission of Adobe.
 *
 *
 *
 *	DESC:	Implementation of DOM Node class and subclasses.
 *
 *  $Header: //xaptk/xmldom/XPNode.h#1 $
 *******************************************************************************/


#ifndef _H_XPNode
#define _H_XPNode


#ifdef WIN_ENV
#pragma warning(disable:4250)   // disable C4250 warning about inheriting via dominance
#pragma warning(disable:4275)   // disable C4275 warning about non dll-interface base class
#endif

#include <stdarg.h>

#include "DOM.h"


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


class XPDocument;
class XPNamedNodeMap;
class XPAttr;

/************************************************************************************
 ********************************** XPNode ******************************************
 ************************************************************************************/
class XMLDOMAPI XPNode : public virtual Node
{
	friend class XPNamedNodeMap;
	friend class XPAttr;

	public:
		virtual XPString		getNodeName() const;
		virtual XPString		getNodeValue() const;
		virtual void			setNodeValue( const XPString& value ); 
		virtual unsigned short	getNodeType() const { return m_nodeType; }
		virtual Node*			getParentNode() const { return m_parent; }
		virtual NodeList*		getChildNodes() const;
		virtual Node*			getFirstChild() const { return m_firstChild; }
		virtual Node*           getLastChild() const { return m_lastChild; }
		virtual Node*			getPreviousSibling() const { return m_prev; }
		virtual Node*			getNextSibling() const { return m_next; }
		virtual NamedNodeMap*	getAttributes() const;
		virtual Document*       getOwnerDocument() const;
		virtual Node*			insertBefore( Node *newChild, Node *refChild );
		virtual Node*			replaceChild( Node *newChild, Node *oldChild );
		virtual Node*			removeChild( Node *oldChild );
		virtual Node*           appendChild( Node* newChild );
		virtual bool			hasChildNodes() const { return m_firstChild != NULL; }
		virtual Node*			cloneNode( bool deep ) const = 0;

		virtual void			removeAndDeleteAllChildren();
		
		virtual void			setFlagBits ( long bits ) { m_flagBits |= bits; }
		virtual void			clearFlagBits ( long bits ) { m_flagBits &= ~bits; }
		virtual long			getFlagBits() const { return m_flagBits; }
		virtual bool			checkFlagBits ( long bits ) const { return ((m_flagBits & bits) != 0); }

	protected:
		XPNode(XPDocument* ownerDocument, unsigned short nodeType, 
			   const XPString& name, const XPString& value = "", 
			   const NamedNodeMap* attributes = NULL );
		XPNode(const XPNode &other);
		virtual ~XPNode();

		// allow node to set state inherited from parent
		virtual void	setParentNode(XPNode *parent) { m_parent = parent; }
		virtual bool	isDescendedFrom( XPNode* node );
		virtual bool	hasChild( XPNode* node );

		unsigned short		m_nodeType;
		XPString			m_nodeName;
		XPString			m_nodeValue;
		XPNode *			m_parent;
		XPNode *			m_firstChild;
		XPNode *			m_lastChild;
		XPNode *			m_prev;
		XPNode *			m_next;
		XPDocument*			m_ownerDocument;
		XPNamedNodeMap*		m_attributes;
		mutable NodeList*	m_childNodes;   // mutable so that XPSibsList can set it, even though getChildNodes() is const.
		long                m_flagBits;

};

/************************************************************************************
 ********************************** XPNamedNodeMap **********************************
 ************************************************************************************/
class XMLDOMAPI XPNamedNodeMap : public NamedNodeMap
{
	public:
								XPNamedNodeMap(XPDocument* ownerDocument);
								XPNamedNodeMap(const XPNamedNodeMap& map);
								virtual		~XPNamedNodeMap();

		XPNamedNodeMap& operator=(const XPNamedNodeMap &map);

		virtual Node*			getNamedItem(const XPString& name) const;
		virtual Node*			setNamedItem( Node* arg );
		virtual Node*			removeNamedItem( const XPString& name );
		virtual Node*			item(unsigned long index) const;
		virtual unsigned long	getLength() const;

	protected:
		XPNode*		m_first;
		XPDocument* m_ownerDocument;
};

/************************************************************************************
 ********************************** XPDocumentType **********************************
 ************************************************************************************/
class XMLDOMAPI XPDocumentType : public XPNode, public virtual DocumentType
{
	public:
				XPDocumentType( XPDocument* ownerDocument, const XPString& name );
		virtual ~XPDocumentType();

		virtual XPString		getName() { return m_nodeName; }
		virtual NamedNodeMap*	getEntities() { return m_entities; }
		virtual NamedNodeMap*	getNotations() { return m_notations; }

	protected:
		XPNamedNodeMap* m_entities;
		XPNamedNodeMap* m_notations;
};

/************************************************************************************
 ********************************** XPAttr ******************************************
 ************************************************************************************/
class XMLDOMAPI XPAttr : public XPNode, public virtual Attr
{
	public:
					XPAttr(XPDocument* ownerDocument, const XPString& name, const XPString& value);
					XPAttr(const XPAttr &other);
		virtual		~XPAttr();

		XPString	getName() const { return m_nodeName; }
		XPString	getValue() const;
		XPString	getNodeValue() const;
		void		setValue(const XPString& value);
		bool		getSpecified() const { return m_specified; }

		Node*		cloneNode( bool deep ) const;

		virtual Node*	getParentNode() const;
		virtual Node*	getPreviousSibling() const;
		virtual Node*	getNextSibling() const;
	protected:
		bool		m_specified;
};

/************************************************************************************
 ********************************** XPElement ***************************************
 ************************************************************************************/
class XMLDOMAPI XPElement : public XPNode, public virtual Element
{
	public:
					XPElement(XPDocument* ownerDocument, const XPString& name, const NamedNodeMap* attributes);
					XPElement(const XPElement &other);
		virtual		~XPElement();

		XPString	getTagName() const { return m_nodeName; }
		XPString	getAttribute(const XPString& name) const { return getAttribute(name, ""); }
		void		setAttribute(const XPString& name, const XPString& value);
		void		removeAttribute(const XPString& name);
		Attr*		getAttributeNode(const XPString& name) const;
		Attr*		setAttributeNode(Attr *newAttr);
		Attr*		removeAttributeNode(Attr *oldAttr);
		NodeList*	getElementsByTagName(const XPString& name) const;
		void		normalize();
		Node*		cloneNode( bool deep ) const;

		// extra helper methods not in DOM
		XPString	getAttribute(const XPString& name, const XPString& defaultValue) const;
		XPString	getInheritedAttribute(const XPString &name, const XPString &defaultValue) const;
		bool		getBooleanAttribute(const XPString& name, bool defaultValue = false) const;
		int			getIntAttribute(const XPString& name, int defaultValue = 0) const;
		float		getNumberAttribute(const XPString& name, float defaultValue = 0.0f) const;
		
		void		setIsAnAlias();
		void		setHasAliases();
		bool		checkIsAnAlias();
		bool		checkHasAliases();

		static bool parseBoolean(const XPString& sValue, bool defaultValue = false);
		static int	parseInt(const XPString& sValue, int defaultValue = 0);
		static float parseNumber(const XPString& sValue, float defaultValue = 0.0f);
		// static int	parseEnum(const XPString &value, int defaultVal, const XPString item, 
		//				int itemVal, ...);		?? Unused dubious va_arg in body.
		
		enum {	// Node option bits that are valid for elements.
			kXPE_isAnAliasMask	= 0x01,
			kXPE_hasAliasesMask	= 0x02
		};

};

/************************************************************************************
 *************************** XPProcessingInstruction ********************************
 ************************************************************************************/
class XMLDOMAPI XPProcessingInstruction : public XPNode, public virtual ProcessingInstruction
{
	public:
					XPProcessingInstruction( XPDocument* ownerDocument, const XPString& name, const XPString& data );
					XPProcessingInstruction( const XPProcessingInstruction& other );
		virtual		~XPProcessingInstruction();
		XPString	getTarget() const { return m_nodeName; }
		XPString	getData() const { return m_nodeValue; }
		void		setData(const XPString& arg);
		Node*		cloneNode( bool deep ) const;

	protected:
		XPString	m_data;
};

/************************************************************************************
 ********************************** XPCharacterData *********************************
 ************************************************************************************/
class XMLDOMAPI XPCharacterData : public XPNode, public virtual CharacterData
{
	public:
		virtual XPString getData() const { return m_nodeValue; }
		virtual void	setData(const XPString& arg) { m_nodeValue = arg; }
		virtual unsigned long getLength() const { return m_nodeValue.length(); }
		virtual XPString	substringData(unsigned long offset, unsigned long count) const;
		virtual void	append(const char* arg, int len=-1);
		virtual void	appendData(const XPString& arg);
		virtual void	insertData(unsigned long offset, const DOMString& arg);
		virtual void	deleteData(unsigned long offset, unsigned long count);
		virtual void	replaceData(unsigned long offset, unsigned long count, const XPString& arg);

	protected:
					XPCharacterData(XPDocument* ownerDocument, unsigned short nodeType, const XPString& name, const XPString& data);
					XPCharacterData(const XPCharacterData& other);
		virtual		~XPCharacterData();
};

/************************************************************************************
 ********************************** XPComment ***************************************
 ************************************************************************************/
class XMLDOMAPI XPComment : public XPCharacterData, public virtual Comment
{
	public:
					XPComment(XPDocument* ownerDocument, const XPString& data);
					XPComment(const XPComment& other);
		virtual		~XPComment();
		
		virtual Node*	cloneNode( bool deep ) const;
};

/************************************************************************************
 ********************************** XPText ******************************************
 ************************************************************************************/
class XMLDOMAPI XPText : public XPCharacterData, public virtual Text
{
	public:
				XPText(XPDocument* ownerDocument, const char* arg, int len=-1);
				XPText(XPDocument* ownerDocument, const XPString& data, int pos=0, int len=-1); // something fishy here, do we need pos, len with an XPString?
				XPText(const XPText& other);
		virtual ~XPText();

		virtual Text*   splitText( unsigned long offset );
		virtual Node*	cloneNode( bool deep ) const;
		
		// In the case that there's <m_displayText>, we need to be sure
		// to update that text as well...
		virtual void	setData(const XPString &a);  
		
		void		updateDisplayText(bool preserveSpace = false);
		XPString	getDisplayText() const;
	
	protected:
		XPString	*m_displayText;
};

/************************************************************************************
 ********************************** XPDocument **************************************
 ************************************************************************************/
class XMLDOMAPI XPDocument : public XPNode, public virtual Document
{
	public:
							XPDocument(DOMImplementation* pDomImpl);
		virtual				~XPDocument();

		DocumentType*		getDoctype() const;
		void				setDoctype(DocumentType* arg);
		DOMImplementation*	getImplementation() const;
		Element*			getDocumentElement() const;

		Element*			createElement(const XPString& tagName) const;
		DocumentFragment*   createDocumentFragment() const;
		Text*				createTextNode(const XPString& data) const;
		Comment*			createComment(const XPString& data) const;
		CDATASection*		createCDATASection( const DOMString& data ) const;
		ProcessingInstruction* createProcessingInstruction(const XPString& name, const XPString& data) const;
		Attr*				createAttribute(const XPString& name) const;
		EntityReference*	createEntityReference(const DOMString& name ) const;
		NodeList*			getElementsByTagName(const DOMString& tagname) const;
		Node*				cloneNode( bool deep ) const;

	protected:
		XPDocumentType*		m_pDocType;
		DOMImplementation*	m_pDomImpl;
};


#if macintosh
	#pragma options align=reset
#endif


#endif /* not _H_XPNode */
 
/* 
$Log$ 
*/ 

