
/* $Header: //xaptk/xmldom/XPPtrList.h#1 $ */ 
//------------------------------------------------------------------------------

//        File:	XPPtrList.h

// Description:	Headerfile for double-linked list class

//        Date: 1999/05/24

//    ADOBE SYSTEMS INCORPORATED
//    Copyright 2001 Adobe Systems Incorporated
//    All Rights Reserved

//    NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
//    terms of the Adobe license agreement accompanying it.  If you have received this file from a 
//    source other than Adobe, then your use, modification, or distribution of it requires the prior 
//    written permission of Adobe.

//------------------------------------------------------------------------------

#ifndef _H_XPPtrList

#define _H_XPPtrList


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif



//------------------------------------------------------------------------------

class XPPtrList

{

protected:	// Definitions

	struct Node

	{

		Node	*m_next;

		Node	*m_prev;

		void	*m_data;

	};

	




public:		// Definitions

	typedef struct _Node *Position;	// Opaque reference for iteration
	// Callback function definition.  <listItem> is the pointer to the <idx>th item

	// in the list (1 based index).  <userDefined> is a user defined pointer that is

	// returned with each callback.

	typedef bool (*ForEachCB)(void *listItem, unsigned long idx, void *userDefined);



	// Comparison Callback comparison

	typedef int (*CompareCB)(const void *itemA, const void *itemB);



public:		// Constructor / destructor

	XPPtrList();

	virtual ~XPPtrList();



public:		// Functions

	unsigned long	GetCount() const { return (m_count); };

	bool	IsEmpty() const { return (m_count ? false : true); };



	void	*GetHead() const;

	void	*GetTail() const;

	

	unsigned long	GetNextID() const { return(m_nextID); }



	void	*RemoveHead();				// Return pointer to data at head or tail

	void	*RemoveTail();

	void	RemoveAll();				// Remove all elements (doesn't delete data)



	Position	AddHead(void *);		// Add element before head

	void		AddHead(XPPtrList *);	// Add list before head

	Position	AddTail(void *);		// Add element after tail

	void		AddTail(XPPtrList *);	// Add list after tail

	

	void		AddTailIfUnique(void *);// Add element after tail, if it isn't already

										// in the list

	Position	GetHeadPosition() const;

	Position	GetTailPosition() const;

	void		*GetNext(Position &) const;

	void		*GetPrev(Position &) const;



	void	*GetAt(Position) const;

	void	*GetNth(unsigned long idx) const;	// return Nth (1-based index) element

	void	SetAt(Position, void *);

	void	*RemoveAt(Position);



	Position InsertBefore(Position, void *);

	Position InsertAfter(Position, void *);



	Position Find(void *search, Position startAfter = 0) const;

	Position FindIndex(unsigned long oneBasedIndexValue) const;



	void	ForEach(ForEachCB, void *userDefined = 0) const;



	// Sort the list.  If <cb> is null, an integer comparison is used

	void	Sort(CompareCB cb = 0);



	// Dump the data into an array.  Note, caller MUST delete memory

	void	**Dump() const;



protected:	// Data

	Node			*m_head;

	Node			*m_tail;

	unsigned long	m_count;

	unsigned long	m_nextID;



protected:	// Functions

	XPPtrList::Node	*NewNode(Node *prev, Node *next);

	void			LinkNode(Node *, Node *prev, Node *next);

	void			UnlinkNode(Node *);

	void			*Remove(Node *);

	void			FreeNode(Node *);



	// FIX ME * Could add a block allocator for Node objects

};



//------------------------------------------------------------------------------


#if macintosh
	#pragma options align=reset
#endif


#endif

 
/* 
$Log$ 
*/ 

