/* $Header: //xaptk/xmldom/XPPtrList.cpp#1 $ */ 
//------------------------------------------------------------------------------

//        File:	EmbeddedFont.cpp

// Description:	Double-linked list class

//        Date: 1999/05/24

//   ADOBE SYSTEMS INCORPORATED
//	 Copyright 2001 Adobe Systems Incorporated
//   All Rights Reserved

//   NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
//   terms of the Adobe license agreement accompanying it.


//------------------------------------------------------------------------------

#include "XPPtrList.h"
#include "XAP_XPDOMConf.h"

#include <stdlib.h>
#include <string.h>



//------------------------------------------------------------------------------

static int IntCompareCB(const void *a, const void *b)

{

	return(*(long *)a - *(long *)b);

}



//------------------------------------------------------------------------------

XPPtrList::XPPtrList()

{

	m_head = NULL;

	m_tail = NULL;

	m_count = 0;

	m_nextID = 1;

}



XPPtrList::~XPPtrList()

{

	RemoveAll();

}



//------------------------------------------------------------------------------

// Create a new node and link it up.  * FIX ME -- should use block allocator and

// free list

//------------------------------------------------------------------------------

XPPtrList::Node *XPPtrList::NewNode(Node *prev, Node *next)

{

	Node	*node = new XMP_Debug_new Node;



	if (! node)

		return(NULL);



	node->m_prev = NULL;

	node->m_next = NULL;

	node->m_data = NULL;



	LinkNode(node, prev, next);

	m_count++;

	m_nextID++;



	return(node);

}



//------------------------------------------------------------------------------

// Set link/reference to <node> in the list

//------------------------------------------------------------------------------

void XPPtrList::LinkNode(Node *node, Node *prev, Node *next)

{

	if (! prev)

	{

		if (m_head)

		{

			m_head->m_prev = node;

			node->m_next = m_head;

		}



		m_head = node;

	}

	else

	{

		node->m_next = prev->m_next;

		prev->m_next = node;

	}



	if (! next)

	{

		if (m_tail)

		{

			m_tail->m_next = node;

			node->m_prev = m_tail;

		}



		m_tail = node;

	}

	else

	{

		node->m_prev = next->m_prev;

		next->m_prev = node;

	}

}



//------------------------------------------------------------------------------

// Remove any link/reference to <node> in the list

//------------------------------------------------------------------------------

void XPPtrList::UnlinkNode(Node *node)

{

	if (node->m_prev)

		node->m_prev->m_next = node->m_next;



	if (node->m_next)

		node->m_next->m_prev = node->m_prev;



	if (m_head == node)

		m_head = node->m_next;



	if (m_tail == node)

		m_tail = node->m_prev;

}



//------------------------------------------------------------------------------

// * FIX ME -- should use a free list

//------------------------------------------------------------------------------

void XPPtrList::FreeNode(Node *node)

{

	delete node;

}



//------------------------------------------------------------------------------

// Get the data (if any) at the head

//------------------------------------------------------------------------------

void *XPPtrList::GetHead() const

{

	return(m_head ? m_head->m_data : NULL);

}



//------------------------------------------------------------------------------

// Get the data (if any) at the tail

//------------------------------------------------------------------------------

void *XPPtrList::GetTail() const

{

	return(m_tail ? m_tail->m_data : NULL);

}



//------------------------------------------------------------------------------

// Remove a node from the list, updating links and counts

//------------------------------------------------------------------------------

void *XPPtrList::Remove(Node *node)

{

	void	*nodeData = node->m_data;



	UnlinkNode(node);

	--m_count;

	FreeNode(node);



	return(nodeData);

}



//------------------------------------------------------------------------------

void *XPPtrList::RemoveHead()

{

	if (! m_head)

		return(NULL);



	return(Remove(m_head));

}



//------------------------------------------------------------------------------

void *XPPtrList::RemoveTail()

{

	if (! m_tail)

		return(NULL);



	return(Remove(m_tail));

}



//------------------------------------------------------------------------------

// Remove all of the nodes in the list

//------------------------------------------------------------------------------

void XPPtrList::RemoveAll()

{

	while (m_head)

	{

		Node	*last = m_head;



		m_head = m_head->m_next;

		FreeNode(last);

	}



	m_head = NULL;

	m_tail = NULL;

	m_count = 0;

}



//------------------------------------------------------------------------------

// Add a new node with <data> in the list at the beginning

//------------------------------------------------------------------------------

XPPtrList::Position XPPtrList::AddHead(void *data)

{

	Node	*node = NewNode(NULL, m_head);



	if (! node)

		return(NULL);



	node->m_data = data;

	return((Position)node);

}



//------------------------------------------------------------------------------

// Add a list <list> to the beginning of the list

//------------------------------------------------------------------------------

void XPPtrList::AddHead(XPPtrList *list)

{

	Position	pos, addedPos = NULL;

	void		*data;


#ifdef XAP_LIB
	for (pos = list->GetHeadPosition(); (pos) ; ) {
		data = list->GetNext(pos);
		if (data == NULL) break;
#else
	for (pos = list->GetHeadPosition(); (pos) && (data = list->GetNext(pos)); )

	{
#endif

		if (addedPos == NULL)

			addedPos = AddHead(data);

		else

			addedPos = InsertAfter(addedPos, data);

	}

}



//------------------------------------------------------------------------------

// Add a new node with <data> in the list at the end

//------------------------------------------------------------------------------

XPPtrList::Position XPPtrList::AddTail(void *data)

{

	Node	*node = NewNode(m_tail, NULL);



	if (! node)

		return(NULL);



	node->m_data = data;

	return((Position)node);

}



//------------------------------------------------------------------------------

// Add a list <list> to the end of the list

//------------------------------------------------------------------------------

void XPPtrList::AddTail(XPPtrList *list)

{

	Position	pos, addedPos = NULL;

	void		*data;


#ifdef XAP_LIB
	for (pos = list->GetHeadPosition(); (pos) ; )
	{
		data = list->GetNext(pos);
		if (data == NULL) break;
#else
	for (pos = list->GetHeadPosition(); (pos) && (data = list->GetNext(pos)); )
	{
#endif


		if (addedPos == NULL)

			addedPos = AddTail(data);

		else

			addedPos = InsertAfter(addedPos, data);

	}

}



//------------------------------------------------------------------------------

// Add <data> to the end of the list, if it isn't already in the list

//------------------------------------------------------------------------------

void XPPtrList::AddTailIfUnique(void *data)

{

	if (! Find(data))

		AddTail(data);

}



//------------------------------------------------------------------------------

XPPtrList::Position XPPtrList::GetHeadPosition() const

{

	return(m_head ? (Position)m_head : NULL);

}



//------------------------------------------------------------------------------

XPPtrList::Position XPPtrList::GetTailPosition() const

{

	return(m_tail ? (Position)m_tail : NULL);

}



//------------------------------------------------------------------------------

void *XPPtrList::GetNext(Position &pos) const

{

	if (! pos)

		return(NULL);



	Node	*node = (Node *)pos;



	pos = (Position)node->m_next;

	

	return(node->m_data);

}



//------------------------------------------------------------------------------

void *XPPtrList::GetPrev(Position &pos) const

{

	if (! pos)

		return(NULL);



	Node	*node = (Node *)pos;



	pos = (Position)node->m_prev;

	

	return(node->m_data);

}



//------------------------------------------------------------------------------

void *XPPtrList::GetAt(Position pos) const

{

	if (! pos)

		return(NULL);



	return(((Node *)pos)->m_data);

}



//------------------------------------------------------------------------------

void *XPPtrList::GetNth(unsigned long idx) const

{

	Position	pos = FindIndex(idx);



	if (! pos)

		return(NULL);



	return(((Node *)pos)->m_data);

}



//------------------------------------------------------------------------------

void XPPtrList::SetAt(Position pos, void *data)

{

	if (! pos)

		return;



	((Node *)pos)->m_data = data;

}



//------------------------------------------------------------------------------

void *XPPtrList::RemoveAt(Position pos)

{

	if (! pos)

		return(NULL);



	return(Remove((Node *)pos));

}



//------------------------------------------------------------------------------

XPPtrList::Position XPPtrList::InsertBefore(Position pos, void *data)

{

	if (! pos)

		return(AddHead(data));



	Node	*node = (Node *)pos;

	Node	*newNode = NewNode(node->m_prev, node);



	if (! newNode)

		return(NULL);



	newNode->m_data = data;

	

	return((Position)newNode);

}



//------------------------------------------------------------------------------

XPPtrList::Position XPPtrList::InsertAfter(Position pos, void *data)

{

	if (! pos)

		return(AddTail(data));



	Node	*node = (Node *)pos;

	Node	*newNode = NewNode(node, node->m_next);



	if (! newNode)

		return(NULL);



	newNode->m_data = data;

	

	return((Position)newNode);

}



//------------------------------------------------------------------------------

// Search for <data> in the list, starting at node <startAfter>.

//------------------------------------------------------------------------------

XPPtrList::Position XPPtrList::Find(void *data, Position startAfter) const

{

	Node	*node = (startAfter ? (Node *)startAfter : m_head);



	for ( ; node; node = node->m_next)

	{

		if (node->m_data == data)

			return((Position)node);

	}



	return(NULL);

}



//------------------------------------------------------------------------------

// Find the <index>th value in the list.  The first element's index is 1.

//------------------------------------------------------------------------------

XPPtrList::Position XPPtrList::FindIndex(unsigned long index) const

{

	Node			*node;

	unsigned long	idx = 1;



	for (idx = 1, node = m_head; node; ++idx, node = node->m_next)

	{

		if (idx == index)

			return((Position)node);

	}



	return(NULL);

}



//------------------------------------------------------------------------------

// For each element in the list, call <cb> with each node's data, the index,

// and the user supplied data <userDefined>.  If the user function returns false,

// terminate the loop.  If it returns true, continue processing

//------------------------------------------------------------------------------

void XPPtrList::ForEach(ForEachCB cb, void *userDefined) const

{

	Node			*node;

	unsigned long	idx = 1;



	if (! cb)

		return;



	for (idx = 1, node = m_head; node; ++idx, node = node->m_next)

	{

		if (! (cb)(node->m_data, idx, userDefined))

			return;

	}

}



//------------------------------------------------------------------------------

// Dump the data into an array of memory.  Note, caller is responsible for

// deleting memory after call

//------------------------------------------------------------------------------

void **XPPtrList::Dump() const

{

	Node			*node = m_head;

	void			**dumpster = new XMP_Debug_new void*[m_count];

	unsigned long	idx = 0;



	if (! dumpster)

		return(NULL);



	for ( ; node; node = node->m_next, idx++)

	{

		dumpster[idx] = node->m_data;

	}



	return(dumpster);

}



//------------------------------------------------------------------------------

// Sort the list based on the <cb> function.  Since we're really just dealing

// with pointers in this list, the easiest thing is to dump the data into

// an array, quicksort `em, and then re-add the items to the list.

//------------------------------------------------------------------------------

void XPPtrList::Sort(CompareCB cb)

{

	unsigned long	count = m_count;



	// Quick check

	if (! m_head)

		return;



	void	**dumpArray = Dump();



	if (! cb)

		cb = IntCompareCB;



	// Quick sort `em

	qsort(dumpArray, count, sizeof(void *), cb);



	// Reset the list and add `em back

	RemoveAll();



	for (unsigned long i = 0; i < count; i++)

		AddTail(dumpArray[i]);



	// Cleanup

	delete dumpArray;

}



//------------------------------------------------------------------------------

 
/* 
$Log$ 
*/ 
