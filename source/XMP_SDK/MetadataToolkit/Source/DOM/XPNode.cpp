/* $Header: //xaptk/xmldom/XPNode.cpp#2 $ */
/*******************************************************************************
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
 *******************************************************************************/


#include <cstdlib>

#include "XAP_XPDOMConf.h"
#include "XPDOM.h"
#include "XPNode.h"
#include "XMPAssert.h"

/************************************************************************************
 ********************************** XPSibsList **************************************
 ************************************************************************************/

class XPSibsList : public NodeList
{
    friend XPNode::~XPNode();

    public:
        /************************************************************************
         * XPSibsList::XPSibsList()
         *
         *   Constructor.
         ************************************************************************/
        XPSibsList(const Node* const parentOfSibs)
            : m_parentOfSibs(parentOfSibs)
        {
        }

        /************************************************************************
         * XPSibsList::getLength()                             overrides NodeList
         *
         *   Returns the number of nodes in the list. The range of valid child
         *   node indices is 0 to length-1 inclusive.
         ************************************************************************/
        unsigned long getLength() const
        {
            int n = 0;
            Node* node = m_parentOfSibs->getFirstChild();
            while (node != NULL)
            {
                n++;
                node = node->getNextSibling();
            }
            return n;
        }

        /************************************************************************
         * XPSibsList::item()                                  overrides NodeList
         *
         *   Returns the indexth item in the collection. If index is greater to
         *   or equal to the number of nodes in the list, this returns null.
         ************************************************************************/
        Node* item( unsigned long index ) const
        {
            Node* node = m_parentOfSibs->getFirstChild();
            while (node != NULL && index > 0)
            {
                node = node->getNextSibling();
                index--;
            }
            return (node != NULL) ? node : NULL;
        }

    private:
        const Node* const m_parentOfSibs;
        ~XPSibsList() { };
};



/************************************************************************************
 ********************************** XPNode ******************************************
 ************************************************************************************/

/************************************************************************
 * XPNode::XPNode()
 *
 *   Constructor
 ************************************************************************/
XPNode::XPNode(XPDocument* ownerDocument, unsigned short nodeType,
               const XPString& nodeName, const XPString& nodeValue, const NamedNodeMap* attributes) :
    m_nodeType(nodeType), m_nodeName(nodeName), m_nodeValue(nodeValue),
    m_parent(NULL), m_firstChild(NULL), m_lastChild(NULL),
    m_prev(NULL), m_next(NULL), m_ownerDocument(ownerDocument),
    m_childNodes(NULL), m_flagBits(0)
{
    if( attributes != NULL )
    {
        // invoke XPNamedNodeMap's copy-constructor
        m_attributes = new XMP_Debug_new XPNamedNodeMap( *(dynamic_cast<const XPNamedNodeMap*>(attributes)));
        assert(m_attributes);
    }
    else
        m_attributes = NULL;
}

/************************************************************************
 * XPNode::XPNode()
 *
 *   Copy-constructor
 ************************************************************************/
XPNode::XPNode(const XPNode &other) :
    m_nodeType(other.m_nodeType), m_nodeName(other.m_nodeName), m_nodeValue(other.m_nodeValue),
    m_parent(NULL), m_firstChild(NULL), m_lastChild(NULL),
    m_prev(NULL), m_next(NULL), m_ownerDocument(other.m_ownerDocument),
    m_childNodes(NULL), m_flagBits(other.m_flagBits)
{
    if( m_nodeType == ELEMENT_NODE && other.m_attributes != NULL )
    {
        // see description of cloneNode() in DOM 1.0 spec.
        m_attributes = new XMP_Debug_new XPNamedNodeMap( *(other.m_attributes) );
        assert(m_attributes);
    }
    else
        m_attributes = NULL;
}

/************************************************************************
 * XPNode::~XPNode()
 *
 *   Destructor
 ************************************************************************/
XPNode::~XPNode()
{
    removeAndDeleteAllChildren();

    if( m_attributes != NULL )
        delete m_attributes;

    if( m_childNodes != NULL )
        delete m_childNodes;
}

/************************************************************************
 * XPNode::getNodeName()                                  overrides Node
 *
 *   Node type:         Returns:
 *      Element             tagName
 *      Attr                name of attribute
 *      Text                #text
 *      CDATASection        #cdata-section
 *      EntityReference     name of entity referenced
 *      Entity              entity name
 *      ProcessingInstr     target
 *      Comment             #comment
 *      Document            #document
 *      DocumentType        document type name
 *      DocumentFragment    #document-fragment
 *      Notation            notation name
 ************************************************************************/
XPString XPNode::getNodeName() const
{
    return m_nodeName;
}

/************************************************************************
 * XPNode::getNodeValue()                                 overrides Node
 *
 *   Node type:         Returns:
 *      Element             null
 *      Attr                value of attribute
 *      Text                content of the text node
 *      CDATASection        content of the CDATASection
 *      EntityReference     null
 *      Entity              null
 *      ProcessingInstr     entire content excluding target
 *      Comment             content of comment
 *      Document            null
 *      DocumentType        null
 *      DocumentFragment    null
 *      Notation            null
 ************************************************************************/
XPString XPNode::getNodeValue() const
{
    return m_nodeValue;
}

/************************************************************************
 * XPNode::setNodeValue()                                 overrides Node
 *
 *   Throws an exception.  Overridden in certain types of nodes.
 ************************************************************************/
void XPNode::setNodeValue( const XPString& /* value */ )
{
    throw DOMException( NO_MODIFICATION_ALLOWED_ERR );
}


/************************************************************************
 * XPNode::getChildNodes()                                overrides Node
 *
 *   Returns list of all children of this node.  If no children, returns
 *   a NodeList containing no nodes.  The caller is not permitted to
 *   destroy the returned NodeList.  It will be destroyed when the Node
 *   is destroyed.
 ************************************************************************/
NodeList* XPNode::getChildNodes() const
{
    if( m_childNodes == NULL )
    {
        m_childNodes = new XMP_Debug_new XPSibsList(this);
        assert(m_childNodes);
    }
    return m_childNodes;
}

/************************************************************************
 * XPNode::getOwnerDocument()                             overrides Node
 *
 *   Returns the document associated with (and that created) this node.
 ************************************************************************/
Document* XPNode::getOwnerDocument() const
{
    return m_ownerDocument;
}

/************************************************************************
 * XPNode::getAttributes()                                overrides Node
 *
 *   Returns null, unless the node is an Element.  The returned
 *   NamedNodeMap must not be deleted!  Doing so will corrupt the tree!
 ************************************************************************/
NamedNodeMap* XPNode::getAttributes() const
{
    return m_attributes;
}

/************************************************************************
 * XPNode::insertBefore()                                 overrides Node
 *
 *   Inserts the node newChild before the existing child node refChild.
 *   If refChild is null, inserts newChild at end of the list of children.
 *
 *   If newChild is a DocumentFragment, all of its children are inserted,
 *   in the same order, before refChild.
 *
 *   If newChild is already in the tree, it is first removed.
 ************************************************************************/
Node *XPNode::insertBefore(Node *newChild, Node *refChild)
{
    XPNode *newXPNode = dynamic_cast<XPNode *>(newChild);
    XPNode *refXPNode = dynamic_cast<XPNode *>(refChild);

    if(( newXPNode == NULL && newChild != NULL ) || ( refXPNode == NULL && refChild != NULL ))
    {
        throw DOMException( WRONG_DOCUMENT_ERR );
    }
    if( newChild == NULL )
    {
        return NULL;
    }
    if( newChild->getOwnerDocument() != getOwnerDocument()
        && static_cast<Node*>(newChild->getOwnerDocument()) != this) // for when 'this' XPNode is the XPDocument
    {
        throw DOMException( WRONG_DOCUMENT_ERR );
    }
    if( newChild == refChild )
    {
        throw DOMException( HIERARCHY_REQUEST_ERR );
    }
    if( newChild->getNodeType() == ATTRIBUTE_NODE )
    {
        throw DOMException( HIERARCHY_REQUEST_ERR );
    }
    if( isDescendedFrom( newXPNode )) // also returns true if this == newXPNode
    {
        throw DOMException( HIERARCHY_REQUEST_ERR );
    }
    if( refChild != NULL && !hasChild( refXPNode ))
    {
        throw DOMException( NOT_FOUND_ERR );
    }

    if( newXPNode->getNodeType() == DOCUMENT_FRAGMENT_NODE )
    {
        XPNode* firstInserted = dynamic_cast<XPNode *>( newXPNode->getFirstChild() );
        NodeList* childNodes = newXPNode->getChildNodes();
        unsigned long length = childNodes->getLength();
        for( unsigned long i = 0; i < length; i++ )
        {
            // children are removed from the document fragment as they are inserted
            insertBefore( newXPNode->getFirstChild(), refXPNode );
        }
        newXPNode = firstInserted;
    }
    else
    {
        Node* parent = newXPNode->getParentNode();
        if( parent != NULL )
            parent->removeChild( newChild );

        if (refXPNode != NULL)
        {
            newXPNode->setParentNode(this);
            newXPNode->m_prev = refXPNode->m_prev;
            newXPNode->m_next = refXPNode;
            if( newXPNode->m_prev != NULL )
                newXPNode->m_prev->m_next = newXPNode;
            else
                m_firstChild = newXPNode;
            refXPNode->m_prev = newXPNode;
        }
        else
        {
            newXPNode->setParentNode(this);
            newXPNode->m_prev = m_lastChild;
            newXPNode->m_next = NULL;
            if( m_lastChild != NULL )
                m_lastChild->m_next = newXPNode;
            else
                m_firstChild = newXPNode;
            m_lastChild = newXPNode;
        }
    }
    return newChild;
}

/************************************************************************
 * XPNode::replaceChild()                                 overrides Node
 *
 *   Replaces the child node oldChild with newChild in the list of children,
 *   and returns the oldChild node.  If newChild is already in the tree,
 *   it's removed.
 ************************************************************************/
Node *XPNode::replaceChild(Node *newChild, Node *oldChild)
{
    if( newChild != oldChild )
    {
        insertBefore( newChild, oldChild );
        removeChild( oldChild );
    }
    return newChild;
}

/************************************************************************
 * XPNode::removeChild()                                  overrides Node
 *
 *   Removes the child node oldChild from the list of children,
 *   and returns it.
 ************************************************************************/
Node *XPNode::removeChild(Node *oldChild)
{
    XPNode* delXPNode = dynamic_cast<XPNode *>(oldChild);
    XPNode* parentXPNode = dynamic_cast<XPNode *>( delXPNode->getParentNode() );

    if( delXPNode == NULL && oldChild != NULL )
    {
        throw DOMException( NOT_FOUND_ERR );
    }
    if( oldChild == NULL || !hasChild( delXPNode ))
    {
        throw DOMException( NOT_FOUND_ERR );
    }

    // since the child is mine, detach it from its siblings and me
    if( delXPNode->m_prev != NULL )
        delXPNode->m_prev->m_next = delXPNode->m_next;

    if( delXPNode->m_next != NULL )
        delXPNode->m_next->m_prev = delXPNode->m_prev;

    if( parentXPNode != NULL && parentXPNode->m_firstChild == delXPNode )
        parentXPNode->m_firstChild = delXPNode->m_next;

    if( parentXPNode != NULL && parentXPNode->m_lastChild == delXPNode )
        parentXPNode->m_lastChild = delXPNode->m_prev;

    delXPNode->setParentNode(NULL);
    delXPNode->m_prev = NULL;
    delXPNode->m_next = NULL;

    return oldChild;
}

/************************************************************************
 * XPNode::appendChild()                                  overrides Node
 *
 *   Adds the node newChild to the end of the list of children of this node.
 *   If the newChild is already in the tree, it is first removed.
 ************************************************************************/
Node* XPNode::appendChild( Node *newChild )
{
    insertBefore( newChild, NULL );
    return newChild;
}


/************************************************************************
 * XPNode::removeAndDeleteAllChildren()
 *
 *   Removes and deletes all children of this node.  Used by ~XPNode().
 ************************************************************************/
void XPNode::removeAndDeleteAllChildren()
{
    XPNode *node = m_firstChild;
    while (node != NULL)
    {
        XPNode *delNode = node;
        node = node->m_next;
        delete delNode;
    }
    m_firstChild = m_lastChild = NULL;
}

/************************************************************************
 * XPNode::isDescendedFrom()
 *
 *   Returns true if we're decended from the specified node, or if we
 *   *ARE* the specified node.  Returns false, otherwise.
 ************************************************************************/
bool XPNode::isDescendedFrom( XPNode* node )
{
    XPNode* current = this;
    Node* target = node;
    while( current != NULL )
    {
        if( current == target )
            return true;
        current = current->m_parent;
    }
    return false;
}

/************************************************************************
 * XPNode::hasChild()
 *
 *   Returns true if we have the specified node as an immediate child.
 ************************************************************************/
bool XPNode::hasChild( XPNode* node )
{
    XPNode* current = this->m_firstChild;
    XPNode* target = node;
    while( current != NULL )
    {
        if( current == target )
            return true;
        current = current->m_next;
    }
    return false;
}


/************************************************************************************
 ********************************** XPNamedNodeMap **********************************
 ************************************************************************************/

/************************************************************************
 * XPNamedNodeMap::XPNamedNodeMap()
 *
 *   Constructor.
 ************************************************************************/
XPNamedNodeMap::XPNamedNodeMap( XPDocument* ownerDocument ) :
    m_first(NULL), m_ownerDocument( ownerDocument )
{

}

/************************************************************************
 * XPNamedNodeMap::XPNamedNodeMap()
 *
 *   Copy-constructor.
 ************************************************************************/
XPNamedNodeMap::XPNamedNodeMap(const XPNamedNodeMap& other) :
    m_ownerDocument( other.m_ownerDocument )
{
    m_first = NULL;
    XPNode* last = NULL;
    XPNode* old = other.m_first;

    while (old != NULL)
    {
        XPNode* node = dynamic_cast<XPNode*>(old->cloneNode(false));
        assert(node != NULL);

        if (m_first == NULL)
            m_first = node;
        if (last != NULL)
        {
            last->m_next = node;
            node->m_prev = last;
        }
        last = node;
        old = dynamic_cast<XPNode*>(old->m_next);
    }
}

/************************************************************************
 * XPNamedNodeMap::~XPNamedNodeMap()
 *
 *   Destructor.  Walks the list of nodes and deletes them.
 ************************************************************************/
XPNamedNodeMap::~XPNamedNodeMap()
{
    XPNode* xpNode = dynamic_cast<XPNode*>(m_first);
    while (xpNode != NULL)
    {
        XPNode* delNode = xpNode;
        xpNode = xpNode->m_next;
        delete delNode;
    }
}

/************************************************************************
 * XPNamedNodeMap::operator=()
 *
 *   Overloads assignment operator
 ************************************************************************/
XPNamedNodeMap& XPNamedNodeMap::operator=(const XPNamedNodeMap &map)
{
    if (this == &map)
        return *this;

    m_first = NULL;
    XPNode* last = NULL;
    XPNode* old = map.m_first;
    while (old != NULL)
    {
        XPNode* node = dynamic_cast<XPNode*>(old->cloneNode(false));
        if (m_first == NULL)
            m_first = node;
        if (last != NULL)
        {
            last->m_next = node;
            node->m_prev = last;
        }
        last = node;
        old = dynamic_cast<XPNode*>(old->m_next);
    }
    return *this;
}

/************************************************************************
 * XPNamedNodeMap::getNamedItem()                  overrides NamedNodeMap
 *
 *   Retrieves a node specified by name, if one can be found, else NULL.
 ************************************************************************/
Node* XPNamedNodeMap::getNamedItem(const XPString& name) const
{
    XPNode* xpNode = dynamic_cast<XPNode*>(m_first);
    while (xpNode != NULL)
    {
        XPString nodeName = xpNode->getNodeName();
        if (nodeName == name)
            return xpNode;
        xpNode = xpNode->m_next;
    }
    return NULL;
}

/************************************************************************
 * XPNamedNodeMap::setNamedItem()                  overrides NamedNodeMap
 *
 *   Adds a node using its nodeName attribute.  Note that multiple nodes
 *   of certain types (Text, CDATASection, Comment, Document, or
 *   DocumentFragment) can't be stored as the names would clash.
 *     (See comment for XPNode::getNodeName() ).
 *
 *   If a node of the same name already exists in the map, it is replaced
 *   by the new node and the old node is returned, otherwise null is
 *   returned.
 ************************************************************************/
Node* XPNamedNodeMap::setNamedItem( Node* arg )
{
    XPNode* xpNode = dynamic_cast<XPNode*>(arg);
    XPString argName = arg->getNodeName();
    assert( !argName.empty() );

    // Note: we must do the following static_cast due to a Mac bug.
    // (this was before the second check below was added)
    // XXX Let's find out what the story is here.
    if( xpNode->getOwnerDocument() != static_cast<Document*>(m_ownerDocument)
        && xpNode != m_ownerDocument)  // do I need to cast here for the mac? This in case someone tries to add the document element to a named node map
    {
        throw DOMException( WRONG_DOCUMENT_ERR );
    }

    // We're supposed to check whether the node is an attribute that is already assigned
    // to another element's NamedNodeMap, and then throw an INUSE_ATTRIBUTE_ERR, but figuring
    // this out is way too costly or complex... so we're not going to bother.  The user will
    // just have to make sure not to reuse the same attribute object. -jdg

    Node* foundNode = getNamedItem( argName );
    XPNode* xpFoundNode = dynamic_cast<XPNode*>( foundNode );

    // Since the specified node is not actually in the document tree, we reuse the Node's
    // m_next and m_prev fields to achieve the double-linked list capability for storage
    // of our quasi-"map".
    if (xpFoundNode != NULL)
    {
        xpNode->m_next = xpFoundNode->m_next;
        xpNode->m_prev = xpFoundNode->m_prev;
        XPNode* xpNext = xpNode->m_next;
        if (xpNext != NULL)
            xpNext->m_prev = xpNode;
        XPNode* xpPrev = xpNode->m_prev;
        if (xpPrev != NULL)
            xpPrev->m_next = xpNode;

        return xpFoundNode;
    }
    else
    {
        xpNode->m_next = NULL;
        if (m_first == NULL)
            m_first = xpNode;
        else
        {
#ifdef XXX
            // Original code reversed order of attributes.
            xpNode->m_next = m_first;
            m_first->m_prev = xpNode;
            m_first = xpNode;
#endif
            /*
            Replacement code preserves order of attributes. This would
            be better if we had an m_last variable, but I don't want
            to change the class definition at this point. --pac
            */
            for (XPNode* xn = m_first; xn != NULL; xn = xn->m_next) {
                if (xn->m_next == NULL) {
                    xpNode->m_prev = xn;
                    xn->m_next = xpNode;
                    break;
                }
            }
        }
    }
    return NULL;

}

/************************************************************************
 * XPNamedNodeMap::removeNamedItem()               overrides NamedNodeMap               INCOMPLETE
 *
 *   Removes a node specified by name.  If the removed node is an Attr
 *   with a default value then it is immediately replaced.
 ************************************************************************/
Node* XPNamedNodeMap::removeNamedItem( const XPString& name )
{
    Node* foundNode = getNamedItem( name );
    XPNode* xpFoundNode = dynamic_cast<XPNode*>( foundNode );

    if( xpFoundNode == NULL )
        throw DOMException( NOT_FOUND_ERR );

    throw DOMException( UNIMPLEMENTED_DOM_ERR );
    return NULL;
}

/************************************************************************
 * XPNamedNodeMap::item()                          overrides NamedNodeMap
 *
 *   Returns the indexth item in the map.  If index is greater than or
 *   equal to the number of nodes in the map, this returns NULL.
 ************************************************************************/
Node* XPNamedNodeMap::item(unsigned long index) const
{
    if( index >= getLength() )
        return NULL;

    XPNode* xpNode = dynamic_cast<XPNode*>(m_first);
    while (xpNode != NULL && index > 0)
    {
        index--;
        xpNode = xpNode->m_next;
    }
    return xpNode;
}

/************************************************************************
 * XPNamedNodeMap::getLength()                     overrides NamedNodeMap
 *
 *   Returns the number of nodes in the map.  The range of valid child
 *   node indices is 0 to length-1 inclusive.
 ************************************************************************/
unsigned long XPNamedNodeMap::getLength() const
{
    unsigned long n = 0;
    XPNode* xpNode = dynamic_cast<XPNode*>(m_first);
    while (xpNode != NULL)
    {
        n++;
        xpNode = xpNode->m_next;
    }
    return n;
}


/************************************************************************************
 ********************************** XPElement ***************************************
 ************************************************************************************/

/************************************************************************
 * XPElement::XPElement()
 *
 *   Constructor.
 ************************************************************************/
XPElement::XPElement(XPDocument* ownerDocument, const XPString& name, const NamedNodeMap* attributes) :
    XPNode(ownerDocument, ELEMENT_NODE, name, "", attributes)
{
}

/************************************************************************
 * XPElement::XPElement()
 *
 *   Copy-constructor.
 ************************************************************************/
XPElement::XPElement(const XPElement &other) :
    XPNode(other)
{
}

/************************************************************************
 * XPElement::XPElement()
 *
 *   Destructor.
 ************************************************************************/
XPElement::~XPElement()
{
}

/************************************************************************
 * XPElement::getAttribute()                            overrides Element
 *
 *   Retrieves an attribute value by name.
 ************************************************************************/
XPString XPElement::getAttribute(const XPString& name, const XPString& defaultValue) const
{
    if (m_attributes == NULL)
        return defaultValue;
    Node* node = m_attributes->getNamedItem(name);
    XPAttr* att = dynamic_cast<XPAttr*>(node);
    assert( att != NULL || node == NULL );
    return (att != NULL) ? att->getNodeValue() : defaultValue;
}


/************************************************************************
 * XPElement::getInheritedAttribute()
 *
 *  Retrieves an attribute value by name which may be inherited from
 *  a parent node.  Two examples are xml:space and xml:language.
 ************************************************************************/
XPString XPElement::getInheritedAttribute(const XPString &name, const XPString &defaultValue) const
{
    Node    *parent = getParentNode();

    while (parent)
    {
        XPElement   *parentEl = dynamic_cast<XPElement*>(parent);

        if (parentEl && parentEl->m_attributes)
        {
            Node    *node = parentEl->m_attributes->getNamedItem(name);

            if (node)
            {
                XPAttr  *a = dynamic_cast<XPAttr*>(node);

                if (a)
                    return(a->getNodeValue());
            }
        }

        parent = parent->getParentNode();
    }

    return(defaultValue);
}

/************************************************************************
 * XPElement::setAttribute()                            overrides Element
 *
 *   Adds a new attribute.  If an attribute with that name is already
 *   present in the element, its value is changed to be that of the value
 *   parameter.  This value is a simple string, it is not parsed as it is
 *   being set.  So any markup (such as syntax to be recognized as an
 *   entity reference) is treated as literal text, and needs to be
 *   appropriately escaped by the implementation when it is written out.
 *   In order to assign an attribute value that contains entity references,
 *   the user must create an Attr node pus any Text and EntityReference
 *   nodes, build the appropriate subtree, and use setAttributeNode() to
 *   assign it as the value of an attribute.
 ************************************************************************/
void XPElement::setAttribute(const XPString& name, const XPString& value)
{
    //XXX: we're supposed to throw an INVALID_CHARACTER_ERR if the specified name contains an invalid char... so what's invalid??? -jdg

    if (m_attributes == NULL)
        m_attributes = new XMP_Debug_new XPNamedNodeMap(dynamic_cast<XPDocument*>(getOwnerDocument()));

    Node* node = m_attributes->getNamedItem(name);
    XPAttr* att = dynamic_cast<XPAttr*>(node);
    assert( att != NULL || node == NULL );
    if (att != NULL)
        att->setValue(value);
    else
    {
        XPAttr* newAttr = new XMP_Debug_new XPAttr(dynamic_cast<XPDocument*>(getOwnerDocument()), name, value);
        assert(newAttr);
        m_attributes->setNamedItem(newAttr);
    }
}

/************************************************************************
 * XPElement::removeAttribute()                         overrides Element       //          INCOMPLETE
 *
 *   Removes an attribute by name.  If the removed attribute is known to
 *   have a default value, an attribute is immediately added containing
 *   the default value.
 ************************************************************************/
void XPElement::removeAttribute(const XPString& /* name */ )
{
    throw DOMException( UNIMPLEMENTED_DOM_ERR );
}

/************************************************************************
 * XPElement::getAttributeNode()                        overrides Element
 *
 *   Gets an Attr node by name, else NULL if there is no attribute of that
 *   name.
 ************************************************************************/
Attr* XPElement::getAttributeNode(const XPString& name) const
{
    if (m_attributes == NULL)
        return NULL;
    Node* node = m_attributes->getNamedItem(name);
    XPAttr* att = dynamic_cast<XPAttr*>(node);
    assert( att != NULL || node == NULL );
    return att;
}

/************************************************************************
 * XPElement::setAttributeNode()                        overrides Element
 *
 *   Adds a new attribute.  If an attribute with that name is already
 *   present in the element, it is replaced by the new one and the
 *   replaced attribute is returned, else NULL is returned.
 ************************************************************************/
Attr* XPElement::setAttributeNode(Attr* newAttr)
{
    Attr* attrOfSameName = getAttributeNode( newAttr->getName() );
    if( attrOfSameName )
        removeAttributeNode( attrOfSameName );

    if (m_attributes == NULL)
        m_attributes = new XMP_Debug_new XPNamedNodeMap(dynamic_cast<XPDocument*>(getOwnerDocument()));

    m_attributes->setNamedItem(newAttr);
    return attrOfSameName;
}

/************************************************************************
 * XPElement::removeAttributeNode()                     overrides Element           //          INCOMPLETE
 *
 *   Removes the specified attribute from the attribute list.  If the
 *   removed Attr has a defualt value, it is immediately replaced.
 ************************************************************************/
Attr* XPElement::removeAttributeNode(Attr* /* oldAttr */ )
{
    throw DOMException( UNIMPLEMENTED_DOM_ERR );
    return NULL;
}

/************************************************************************
 * XPElement::getElementsByTagName()                    overrides Element           //          INCOMPLETE
 *
 *   Returns a NodeList of all descendent elements with a given tag name,
 *   in the order in which they would be encountered ina preorder
 *   traversal of the Element tree.
 *
 *      name - the name of the tag to match on.  The special value "*"
 *             matches all tags.
 ************************************************************************/
NodeList *XPElement::getElementsByTagName(const XPString& /* name */ ) const
{
    throw DOMException( UNIMPLEMENTED_DOM_ERR );
    return NULL;
}

/************************************************************************
 * XPElement::normalize()                               overrides Element           //          INCOMPLETE
 *
 *   Puts all Text nodes in the full depth of the sub-tree underneath this
 *   this Element into a "normal" form where only markup (e.g. tags,
 *   comments, processing instructions, CDATA sections, and entity
 *   references) separates the Text nodes, i.e., there are no adjacent
 *   Text nodes.  This can be used to ensure that the DOM view of a
 *   document is the same as if it were saved and re-loaded, and is useful
 *   when operations (such as XPointer lookups) that depend on a
 *   particular document tree structure are to be used.
 ************************************************************************/
void XPElement::normalize()
{
    throw DOMException( UNIMPLEMENTED_DOM_ERR );
}


/************************************************************************
 * XPElement::parseBoolean()
 *
 *   Utility to convert the string sValue to a boolean value.
 *   If sValue is "1", returns true, else returns defaultValue.
 ************************************************************************/
bool XPElement::parseBoolean(const XPString& sValue, bool defaultValue)
{
    if (sValue.empty() == false)
        return sValue == "1"; // XXX Should we be checking != "0" instead of == "1"?   This method will die once we've fully switched to CSS, anyways.
    else
        return defaultValue;
}

/************************************************************************
 * XPElement::parseInt()
 *
 *   Utility to convert the string sValue to an int value.
 *   If the string can be converted, the resultant integer is returned,
 *   else the specified default value is returned.
 ************************************************************************/
int XPElement::parseInt(const XPString& sValue, int defaultValue)
{
    if (sValue.empty() == false)
        return (int) atoi(sValue.c_str());
    else
        return defaultValue;
}

/************************************************************************
 * XPElement::parseNumber()
 *
 *   Utility to convert the string sValue to a float value.
 *   If the string can be converted, the resultant float is returned,
 *   else the specified default value is returned.
 ************************************************************************/
float XPElement::parseNumber(const XPString& sValue, float defaultValue)
{
    if (sValue.empty() == false)
        return (float) atof(sValue.c_str());
    else
        return defaultValue;
}

#if 0	// Unused, dubious use of va_arg with XPString object.
/************************************************************************
 * XPElement::parseEnum()
 *
 *  Try to lookup <value> in the list of <item>/<itemVal> pairs.  If not
 *  found, return <defaultVal>.  Otherwise, return the found value.
 *  Note:  The varglist must be terminated with an empty ("") string and 0
 *         item value.
 ************************************************************************/
int XPElement::parseEnum(const XPString &value, int defaultVal,
                        const XPString item, int itemVal, ...)
{
    va_list     args;
    XPString    currentItem = item;
    int         currentVal = itemVal;
    bool        found = false;

    va_start(args, itemVal);

    while ((currentItem != "") || (currentVal != 0))
    {
        if (currentItem == value)
        {
            found = true;
            break;
        }

        currentItem = va_arg(args, XPString);
        currentVal = va_arg(args,  int);
    }

    if (! found)
        currentVal = defaultVal;

    return(currentVal);
}
#endif


/************************************************************************
 * XPElement::getBooleanAttribute()
 *
 *   Gets the specified attribute by name, and parses it to become
 *   a boolean (returned).
 ************************************************************************/
bool XPElement::getBooleanAttribute(const XPString& name, bool defaultValue) const
{
    return parseBoolean(getAttribute(name), defaultValue);
}

/************************************************************************
 * XPElement::getIntAttribute()
 *
 *   Gets the specified attribute by name, and parses it to become
 *   an integer (returned).  If conversion fails, returns defaultValue.
 ************************************************************************/
int XPElement::getIntAttribute(const XPString& name, int defaultValue) const
{
    return parseInt(getAttribute(name), defaultValue);
}

/************************************************************************
 * XPElement::getIntAttribute()
 *
 *   Gets the specified attribute by name, and parses it to become
 *   a float (returned). If conversion fails, returns defaultValue.
 ************************************************************************/
float XPElement::getNumberAttribute(const XPString& name, float defaultValue) const
{
    return parseNumber(getAttribute(name), defaultValue);
}

/************************************************************************
 * XPElement::setIsAnAlias()
 *
 *   Sets the isAnAlias option bit.
 ************************************************************************/
void XPElement::setIsAnAlias()
{
    setFlagBits ( kXPE_isAnAliasMask );
}

/************************************************************************
 * XPElement::setHasAliases()
 *
 *   Sets the hasAliases option bit.
 ************************************************************************/
void XPElement::setHasAliases()
{
    setFlagBits ( kXPE_hasAliasesMask );
}

/************************************************************************
 * XPElement::checkIsAnAlias()
 *
 *   Returns true if the isAnAlias option bit is set.
 ************************************************************************/
bool XPElement::checkIsAnAlias()
{
    return checkFlagBits ( kXPE_isAnAliasMask );
}

/************************************************************************
 * XPElement::checkHasAliases()
 *
 *   Returns true if the hasAliases option bit is set.
 ************************************************************************/
bool XPElement::checkHasAliases()
{
    return checkFlagBits ( kXPE_hasAliasesMask );
}


/************************************************************************
 * XPElement::cloneNode()                                overrides XPNode
 *
 *   Returns a duplicate of this node, i.e., serves as a generic copy
 *   constructor for nodes.  The duplicate node has no parent (parentNode
 *   returns NULL).
 *
 *   Cloning an Element copies all attributes and their values, including
 *   those generated by the XML processor to represent defaulted attributes,
 *   but this method does not copy any text it contains unless it is a deep
 *   clone, since the text is contained in a child Text node.  Cloning
 *   any other type of node simply returns a copy of this node.
 *
 *     deep  - If true, recursively clone the subtree under the specified
 *              node; if false, clone only the node itself (and its
 *              attributes, if the node is an Element).
 ************************************************************************/
Node* XPElement::cloneNode( bool deep ) const
{
    // XPNode's copy-constructor will take care of cloning the attributes.
    XPElement* element = new XMP_Debug_new XPElement(*this);
    assert(element);

    if( deep )
    {
        NodeList* list = getChildNodes();
        unsigned long len = list->getLength();
        for( unsigned long at=0; at < len; at++ )
        {
            Node* child = list->item(at);
            if( child == NULL )
                break;
            Node* cloned = child->cloneNode(true);
            if (cloned != NULL)
                element->insertBefore(cloned, NULL);
        }
    }

    return element;
}

/************************************************************************************
 ********************************** XPAttr ******************************************
 ************************************************************************************/

/************************************************************************
 * XPAttr::XPAttr()
 *
 *   Constructor.
 ************************************************************************/
XPAttr::XPAttr(XPDocument* ownerDocument, const XPString& name, const XPString& value) :
    XPNode(ownerDocument, ATTRIBUTE_NODE, name), m_specified(false)
{
    setValue(value);
}

/************************************************************************
 * XPAttr::XPAttr()                                                             //          INCOMPLETE
 *
 *   Copy-constructor.  Currently assumes only one child is permitted
 *   for an Attr, since we don't support EntityReferences yet.
 ************************************************************************/
XPAttr::XPAttr(const XPAttr &other) :
    XPNode(other), m_specified(other.m_specified)
{
    XPNode* old = other.m_firstChild;
    if( old != NULL )
    {
        XPNode* node = dynamic_cast<XPNode*>(old->cloneNode(false));
        m_firstChild = node;
    }
}

/************************************************************************
 * XPAttr::XPAttr()
 *
 *   Destructor.
 ************************************************************************/
XPAttr::~XPAttr()
{
    // the Node destructor will delete all of our children for us.
}

/************************************************************************
 * XPAttr::getValue()                                      overrides Attr       //          INCOMPLETE
 *
 *   Gets the value of the attribute.  For now, this assumes there is
 *   only one Text child node of the attribute, so it simply returns
 *   that Text node's value.  In the future, we'll need to handle
 *   EntityReference nodes as well.
 ************************************************************************/
XPString XPAttr::getValue() const
{
    if( m_firstChild == NULL )
        return "";

    return m_firstChild->getNodeValue();
}

/************************************************************************
 * XPAttr::getNodeValue()                                  overrides Node
 *
 *   Calls getValue() to get the value of the node.
 ************************************************************************/
XPString XPAttr::getNodeValue() const
{
    return getValue();
}

/************************************************************************
 * XPAttr::setValue()                                      overrides Attr       //          INCOMPLETE
 *
 *   Sets the value of the attribute.  This deletes all children of the
 *   attribute, then creates a Text node containing the value, and sets
 *   it as the attribute's child.
 ************************************************************************/
void XPAttr::setValue(const XPString& value)
{
    XPNode* xpNode = dynamic_cast<XPNode*>(m_firstChild);
    while (xpNode != NULL)
    {
        XPNode* delNode = xpNode;
        xpNode = dynamic_cast<XPNode*>(xpNode->getNextSibling());
        delete delNode;
    }

    m_firstChild = new XMP_Debug_new XPText(dynamic_cast<XPDocument*>(getOwnerDocument()), value );
    assert(m_firstChild);
}

/************************************************************************
 * XPAttr::getParentNode()                                overrides XPNode
 *
 *   An Attr node has no parent and should therefore return NULL.
 ************************************************************************/
Node* XPAttr::getParentNode() const
{
    return NULL;
}

/************************************************************************
 * XPAttr::getNextSibling()                               overrides XPNode
 *
 *   An Attr node has no siblings and should therefore return NULL.
 *  (When an XPAttr is placed within an XPNamedNodeMap, the latter reuses
 *   the m_next field to point to the owning Element of an XPAttr).
 ************************************************************************/
Node* XPAttr::getNextSibling() const
{
    return NULL;
}

/************************************************************************
 * XPAttr::getPreviousSibling()                          overrides XPNode
 *
 *   An Attr node has no siblings and should therefore return NULL.
 *  (When an XPAttr is placed within an XPNamedNodeMap, the latter reuses
 *   the m_prev field to point to the owning Element of an XPAttr).
 ************************************************************************/
Node* XPAttr::getPreviousSibling() const
{
    return NULL;
}


/************************************************************************
 * XPAttr::cloneNode()                                   overrides XPNode
 *
 *   Returns a duplicate of this node, i.e., serves as a generic copy
 *   constructor for nodes.  The duplicate node has no parent (parentNode
 *   returns NULL).
 *
 *     deep  - If true, recursively clone the subtree under the specified
 *              node; if false, clone only the node itself.
 ************************************************************************/
Node* XPAttr::cloneNode( bool /* deep */ ) const
{
    // XPNode's copy-constructor will take care of cloning the attributes.
    XPAttr* attr = new XMP_Debug_new XPAttr(*this);
    assert(attr);
    return attr;
}

/************************************************************************************
 *************************** XPProcessingInstruction ********************************
 ************************************************************************************/

/************************************************************************
 * XPProcessingInstruction::XPProcessingInstruction()
 *
 *   Constructor
 ************************************************************************/
XPProcessingInstruction::XPProcessingInstruction(XPDocument* ownerDocument, const XPString& name, const XPString& data) :
    XPNode(ownerDocument, PROCESSING_INSTRUCTION_NODE, name, data)
{
}

/************************************************************************
 * XPProcessingInstruction::XPProcessingInstruction()
 *
 *   Copy-constructor
 ************************************************************************/
XPProcessingInstruction::XPProcessingInstruction(const XPProcessingInstruction& other) :
    XPNode(other)
{
}

/************************************************************************
 * XPProcessingInstruction::~XPProcessingInstruction()
 *
 *   Destructor
 ************************************************************************/
XPProcessingInstruction::~XPProcessingInstruction()
{
}

/************************************************************************
 * XPProcessingInstruction::setData()
 *
 *   Sets the content of this processing instruction.  This is from the
 *   first non white space character after the target, to the character
 *   immediately preceding the ?>.
 ************************************************************************/
void XPProcessingInstruction::setData(const XPString& arg)
{
    m_nodeValue = arg;
}

/************************************************************************
 * XPProcessingInstruction::cloneNode()                  overrides XPNode
 *
 *   Returns a duplicate of this node, i.e., serves as a generic copy
 *   constructor for nodes.  The duplicate node has no parent (parentNode
 *   returns NULL).
 *
 *     deep  - If true, recursively clone the subtree under the specified
 *              node; if false, clone only the node itself.
 ************************************************************************/
Node* XPProcessingInstruction::cloneNode( bool /* deep */ ) const
{
    // XPNode's copy-constructor will take care of cloning the attributes.
    XPProcessingInstruction* pi = new XMP_Debug_new XPProcessingInstruction(*this);
    assert(pi);
    return pi;
}


/************************************************************************************
 ********************************** XPCharacterData *********************************
 ************************************************************************************/

/************************************************************************
 * XPCharacterData::XPCharacterData()
 *
 *   Constructor.
 ************************************************************************/
XPCharacterData::XPCharacterData(XPDocument* ownerDocument, unsigned short nodeType, const XPString& name, const XPString& arg) :
    XPNode(ownerDocument, nodeType, name, arg)
{
}

/************************************************************************
 * XPCharacterData::XPCharacterData()
 *
 *   Copy-constructor.
 ************************************************************************/
XPCharacterData::XPCharacterData(const XPCharacterData &other) :
    XPNode(other)
{
}

/************************************************************************
 * XPCharacterData::~XPCharacterData()
 *
 *   Destructor.
 ************************************************************************/
XPCharacterData::~XPCharacterData()
{
}

/************************************************************************
 * XPCharacterData::substringData()               overrides CharacterData
 *
 *   Extracts a range of data from the node.  If the sum of offset and
 *   count exceed the length, then all characters to the end of the data
 *   are returned.
 *
 *      offset - Start offset of substring to extract.
 *      count  - the number of characters to extract.
 ************************************************************************/
XPString XPCharacterData::substringData(unsigned long offset, unsigned long count) const
{
    unsigned long len = m_nodeValue.length();
    if( offset > len )
        throw DOMException( INDEX_SIZE_ERR );

    if( offset + count > len )
    {
        offset = 0;
        count = len;
    }
    return m_nodeValue.substr( offset, count );
}

/************************************************************************
 * XPCharacterData::append()
 *
 *   Appends the characters specified by arg, of length len, to the
 *   end of the character data of the node.
 ************************************************************************/
void XPCharacterData::append(const char *arg, int len)
{
    m_nodeValue += XPString(arg, len);
}

/************************************************************************
 * XPCharacterData::appendData()                  overrides CharacterData
 *
 *   Appends the specified string to the end of the character data of the node.
 ************************************************************************/
void XPCharacterData::appendData(const XPString& arg)
{
    m_nodeValue += arg;
}

/************************************************************************
 * XPCharacterData::insertData()                  overrides CharacterData
 *
 *   Inserts a string at the specified character offset.
 ************************************************************************/
void XPCharacterData::insertData(unsigned long offset, const XPString& arg)
{
    unsigned long len = m_nodeValue.length();

    if( offset > len )
        throw DOMException( INDEX_SIZE_ERR );

    XPString firstOrigPart = m_nodeValue.substr( 0, offset - 1 );
    XPString remainingOrigPart = m_nodeValue.substr( offset, len - offset );
    m_nodeValue = firstOrigPart;
    m_nodeValue += arg;
    m_nodeValue += remainingOrigPart;
}

/************************************************************************
 * XPCharacterData::deleteData()                  overrides CharacterData
 *
 *   Remove a range of characters from the node. Upon success, data and
 *   length reflect the change.
 *
 *      offset - the offset from which to remove characters.
 *      count  - the number of characters to delete.  If the sum of offset
 *               and count exceeds length then all characters from offset
 *               to the end of the data are deleted.
 ************************************************************************/
void XPCharacterData::deleteData(unsigned long offset, unsigned long count)
{
    unsigned long len = m_nodeValue.length();

    if( offset > len )
        throw DOMException( INDEX_SIZE_ERR );

    if( offset + count > len )
    {
        count = len - offset;
    }

    XPString firstOrigPart = m_nodeValue.substr( 0, offset - 1 );
    XPString remainingOrigPart = m_nodeValue.substr( offset + count, len - count );
    m_nodeValue = firstOrigPart;
    m_nodeValue += remainingOrigPart;
}

/************************************************************************
 * XPCharacterData::replaceData()                 overrides CharacterData
 *
 *   Replaces the characters starting at the specified character offset with
 *   the specified string.
 *
 *      offset - the offset from which to start replacing.
 *      count  - the number of characters to replace.  If the sum of offset
 *               and count exceeds length, then all characters to the end
 *               of the data are replaced (i.e. the effect is the same as
 *               a removeData method call with the same range, followed by
 *               an appendData method invocation.)
 ************************************************************************/
void XPCharacterData::replaceData(unsigned long offset, unsigned long count, const XPString& arg)
{
    deleteData( offset, count );
    insertData( offset, arg );
}


/************************************************************************************
 ********************************** XPComment ***************************************
 ************************************************************************************/

/************************************************************************
 * XPComment::XPComment()
 *
 *   Constructor.
 ************************************************************************/
XPComment::XPComment(XPDocument* ownerDocument, const XPString& arg) :
    XPCharacterData(ownerDocument, COMMENT_NODE, "#comment", arg)
{
}

/************************************************************************
 * XPComment::XPComment()
 *
 *   Copy-constructor.
 ************************************************************************/
XPComment::XPComment(const XPComment &other) :
    XPCharacterData(other)
{
}

/************************************************************************
 * XPComment::~XPComment()
 *
 *   Destructor.
 ************************************************************************/
XPComment::~XPComment()
{
}

/************************************************************************
 * XPComment::cloneNode()                                overrides XPNode
 *
 *   Returns a duplicate of this node, i.e., serves as a generic copy
 *   constructor for nodes.  The duplicate node has no parent (parentNode
 *   returns NULL).
 *
 *     deep  - If true, recursively clone the subtree under the specified
 *              node; if false, clone only the node itself.
 ************************************************************************/
Node* XPComment::cloneNode( bool /* deep */ ) const
{
    // XPNode's copy-constructor will take care of cloning the attributes.
    XPComment* comment = new XMP_Debug_new XPComment(*this);
    assert(comment);
    return comment;
}

/************************************************************************************
 ********************************** XPText ******************************************
 ************************************************************************************/

/************************************************************************
 * XPText::XPText()
 *
 *   Constructor.
 ************************************************************************/
XPText::XPText(XPDocument* ownerDocument, const char *data, int len) :
    XPCharacterData(ownerDocument, TEXT_NODE, "#text", XPString(data, len)), m_displayText(NULL)
{
}

/************************************************************************
 * XPText::XPText()
 *
 *   Constructor.
 ************************************************************************/
XPText::XPText(XPDocument* ownerDocument, const XPString& data, int pos, int len) :
    XPCharacterData(ownerDocument, TEXT_NODE, "#text", XPString(data, pos, len)), m_displayText(NULL)
{
}

/************************************************************************
 * XPText::XPText()
 *
 *   Copy-constructor.
 ************************************************************************/
XPText::XPText(const XPText &other) :
    XPCharacterData(other)
{
    m_displayText = (other.m_displayText ? new XPString(*other.m_displayText) : NULL);
}

/************************************************************************
 * XPText::~XPText()
 *
 *   Destructor.
 ************************************************************************/
XPText::~XPText()
{
    delete m_displayText;
}

void XPText::setData(const XPString &a)
{
    XPCharacterData::setData(a);

    // If there's displayText, then we musta already have decided
    // that we don't preserve spaces
    if (m_displayText)
        updateDisplayText(false);
}

/************************************************************************
 * Update the <m_displayText> member.  If <preserveSpace> is true, make
 * <m_displayText> point to a stripped version of the text.  Otherwise,
 * make sure that <m_displayText> is NULL.
 ************************************************************************/
void XPText::updateDisplayText(bool preserveSpace)
{
    // * FIX ME -- the easiest way is just to rebuild display text as needed
    delete m_displayText;
    m_displayText = NULL;

    if (! preserveSpace)
    {
        DOMString dd = getData();
        m_displayText = new XPString(dd);
        m_displayText->stripWhitespace();
        m_displayText->coalesceWhitespace();
    }
}

/************************************************************************
 * If <m_displayText> is non-NULL, return that string.  Otherwise, return
 * the main string.
 ************************************************************************/
XPString XPText::getDisplayText() const
{
    if (m_displayText)
        return(*m_displayText);

    return(getData());
}

/************************************************************************
 * XPText::splitText()                                     overrides Text           INCOMPLETE
 *
 *   Breaks this node into two XPText nodes at the specified offset, keeping
 *   both in the tree as siblings. This node thenonly contains all the
 *   content up to the offset point. And a new Text node, which is inserted
 *   as the next sibling of this node, contains all the content at and after
 *   the offset point.
 ************************************************************************/
Text* XPText::splitText( unsigned long /* offset */ )
{
    throw DOMException( UNIMPLEMENTED_DOM_ERR );
    return NULL;
}

/************************************************************************
 * XPText::cloneNode()                                  overrides XPNode
 *
 *   Returns a duplicate of this node, i.e., serves as a generic copy
 *   constructor for nodes.  The duplicate node has no parent (parentNode
 *   returns NULL).
 *
 *     deep  - If true, recursively clone the subtree under the specified
 *              node; if false, clone only the node itself.
 ************************************************************************/
Node* XPText::cloneNode( bool /* dee */ ) const
{
    // XPNode's copy-constructor will take care of cloning the attributes.
    XPText* text = new XMP_Debug_new XPText(*this);
    assert(text);
    return text;
}

/************************************************************************************
 ********************************** XPDocument **************************************
 ************************************************************************************/

/************************************************************************
 * XPDocument::XPDocument()                            overrides Document
 *
 *   Constructor.  Both the DocumentType and DOMImplementation objects
 *   are now considered to be owned by XPDocument, so shall be deleted
 *   when the document is deleted.
 ************************************************************************/
XPDocument::XPDocument( DOMImplementation* pDomImpl ) :
    XPNode(NULL, DOCUMENT_NODE, XPString("#document")),
    m_pDocType( NULL ), m_pDomImpl( pDomImpl )
{
}

/************************************************************************
 * XPDocument::~XPDocument()                           overrides Document
 *
 *   Destructor.
 ************************************************************************/
XPDocument::~XPDocument()
{
    delete m_pDocType;
    delete m_pDomImpl;
}

/************************************************************************
 * XPDocument::getDoctype()                            overrides Document
 *
 *   Gets the DocumentType for this document.
 ************************************************************************/
DocumentType* XPDocument::getDoctype() const
{
    return m_pDocType;
}

/************************************************************************
 * XPDocument::setDoctype()
 *
 *   Sets the DocumentType for this document.  To prevent editing of
 *   the docType, this method can only be called once for a document.
 *   When the document is deleted, it will delete the DocumentType object.
 ************************************************************************/
void XPDocument::setDoctype(DocumentType* docType )
{
    if( !m_pDocType )
        m_pDocType = dynamic_cast<XPDocumentType*>(docType);
}

/************************************************************************
 * XPDocument::getImplementation()                     overrides Document
 *
 *   Gets the DOMImplementation object that handles this document.
 *   A DOM application may use objects from multiple implementations.
 ************************************************************************/
DOMImplementation* XPDocument::getImplementation() const
{
    return m_pDomImpl;
}

/************************************************************************
 * XPDocument::getDocumentElement()                    overrides Document
 *
 *   This is a convenience attribute that allows direct access to the child
 *   node that is the root element of the document.  For SVG documents,
 *   this is the element with the tagName "svg".
 ************************************************************************/
Element *XPDocument::getDocumentElement() const
{
    Node* child = getFirstChild();
    while( child != NULL )
    {
        if( child->getNodeType() == ELEMENT_NODE )
        {
            break;
        }
        child = child->getNextSibling();
    }
    return dynamic_cast<Element*>( child );
}

/************************************************************************
 * XPDocument::createElement()                         overrides Document           INCOMPLETE
 *
 *   Creates an element of the type specified.  Note that the instance
 *   returned implements the Element interface, so attributes can be
 *   specified directly on the returned object.
 *
 *      tagName - The name of the element type to instantiate. For XML
 *                this is case-sensitive.
 ************************************************************************/
Element* XPDocument::createElement(const XPString& /* tagName */ ) const
{
    throw DOMException( UNIMPLEMENTED_DOM_ERR );
    return NULL;
}

/************************************************************************
 * XPDocument::createDocumentFragment()                overrides Document           INCOMPLETE
 *
 *   Creates an empty DocumentFragment object.
 ************************************************************************/
DocumentFragment* XPDocument::createDocumentFragment() const
{
    throw DOMException( UNIMPLEMENTED_DOM_ERR );
    return NULL;
}

/************************************************************************
 * XPDocument::createTextNode()                        overrides Document           INCOMPLETE
 *
 *   Creates a Text node given the specified string.
 ************************************************************************/
Text* XPDocument::createTextNode(const XPString& /* data */ ) const
{
    throw DOMException( UNIMPLEMENTED_DOM_ERR );
    return NULL;
}

/************************************************************************
 * XPDocument::createComment()                         overrides Document           INCOMPLETE
 *
 *   Creates a Comment node given the specified string.
 ************************************************************************/
Comment* XPDocument::createComment(const XPString& /* data */ ) const
{
    throw DOMException( UNIMPLEMENTED_DOM_ERR );
    return NULL;
}

/************************************************************************
 * XPDocument::createCDATASection()                    overrides Document           INCOMPLETE
 *
 *   Creates a CDATASection node whose value is the specified string.
 ************************************************************************/
CDATASection* XPDocument::createCDATASection(const XPString& /* data */ ) const
{
    throw DOMException( UNIMPLEMENTED_DOM_ERR );
    return NULL;
}

/************************************************************************
 * XPDocument::createProcessingInstruction()           overrides Document           INCOMPLETE
 *
 *   Creates a ProcessingInstruction node given the specified name and
 *   data strings.
 *
 *      target - The target part of the processing instruction.
 *      data   - The data for the node.
 ************************************************************************/
ProcessingInstruction* XPDocument::createProcessingInstruction(const XPString& /* name */, const XPString& /* data */ ) const
{
    throw DOMException( UNIMPLEMENTED_DOM_ERR );
    return NULL;
}

/************************************************************************
 * XPDocument::createAttribute()                       overrides Document           INCOMPLETE
 *
 *   Creates an Attr of the given name. Note that the Attr instance can
 *   then be set on an Element using the setAttribute() method.
 ************************************************************************/
Attr* XPDocument::createAttribute(const XPString& /* name */ ) const
{
    throw DOMException( UNIMPLEMENTED_DOM_ERR );
    return NULL;
}

/************************************************************************
 * XPDocument::createEntityReference()                 overrides Document           INCOMPLETE
 *
 *   Creates an EntityReference object of the specified name.
 ************************************************************************/
EntityReference* XPDocument::createEntityReference(const XPString& /* name */ ) const
{
    throw DOMException( UNIMPLEMENTED_DOM_ERR );
    return NULL;
}

/************************************************************************
 * XPDocument::getElementsByTagName()                  overrides Document           INCOMPLETE
 *
 *   Returns a NodeList of all the Elements with a given tag name in the
 *   order in which they would be encountered in a preorder traversal of
 *   the Document tree.
 *
 *      tagname - the name of the tag to match on. The special value "*"
 *                matches all tags.
 ************************************************************************/
NodeList* XPDocument::getElementsByTagName(const XPString& /* tagname */) const
{
    throw DOMException( UNIMPLEMENTED_DOM_ERR );
    return NULL;
}

/************************************************************************
 * XPDocument::cloneNode()                               overrides XPNode
 *
 *   Returns a duplicate of this node, i.e., serves as a generic copy
 *   constructor for nodes.  The duplicate node has no parent (parentNode
 *   returns NULL).
 *
 *     deep  - If true, recursively clone the subtree under the specified
 *              node; if false, clone only the node itself.
 ************************************************************************/
Node* XPDocument::cloneNode( bool /* deep */ ) const
{
    // XPNode's copy-constructor will take care of cloning the attributes.
    XPDocument* doc = new XMP_Debug_new XPDocument(*this);
    assert(doc);
    return doc;
}

/************************************************************************************
 ********************************** XPDocumentType **********************************
 ************************************************************************************/

/************************************************************************
 * XPDocumentType::XPDocumentType()
 *
 *   Constructor.
 ************************************************************************/
XPDocumentType::XPDocumentType( XPDocument* ownerDocument, const XPString& name ) :
    XPNode(ownerDocument, DOCUMENT_TYPE_NODE, name),
    m_entities( NULL ),
    m_notations( NULL )
{
}

/************************************************************************
 * XPDocumentType::~XPDocumentType()
 *
 *   Destructor.
 ************************************************************************/
XPDocumentType::~XPDocumentType()
{
    delete m_entities;
    delete m_notations;
}
/*
$Log$
*/
