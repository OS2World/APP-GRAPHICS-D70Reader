
/* $Header: //xaptk/xmldom/XPTypedPtrList.h#1 $ */ 
//------------------------------------------------------------------------------

//        File:	XPTypedPtrList.h

// Description:	Headerfile for double-linked list template class

//        Date: 1999/05/24

//    ADOBE SYSTEMS INCORPORATED
//    Copyright 2001 Adobe Systems Incorporated
//    All Rights Reserved

//    NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
//    terms of the Adobe license agreement accompanying it.  If you have received this file from a 
//    source other than Adobe, then your use, modification, or distribution of it requires the prior 
//    written permission of Adobe.

//------------------------------------------------------------------------------

#ifndef _H_XPTypedPtrList

#define _H_XPTypedPtrList


#include "XPPtrList.h"


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


//------------------------------------------------------------------------------

#ifdef WIN_ENV
#pragma warning(disable: 4706)	// Assignment within conditional
#endif


//------------------------------------------------------------------------------

template<class TYPE>

class XPTypedPtrList : public XPPtrList

{

public:		// Constructor / destructor

	XPTypedPtrList(bool deleteWhenDone = true) : XPPtrList()

		{ m_deleteWhenDone = deleteWhenDone; }

	virtual ~XPTypedPtrList() { if (m_deleteWhenDone) DeleteItems(); }



	void DeleteItemsOnDestroy(bool doIt = true) { m_deleteWhenDone = doIt; }



	TYPE GetHead() const { return (TYPE)XPPtrList::GetHead(); }

	TYPE GetTail() const { return (TYPE)XPPtrList::GetTail(); }



	TYPE RemoveHead() { return (TYPE)XPPtrList::RemoveHead(); }

	TYPE RemoveTail() { return (TYPE)XPPtrList::RemoveTail(); }



	Position AddHead(TYPE e) { return XPPtrList::AddHead(e); }

	Position AddTail(TYPE e) { return XPPtrList::AddTail(e); }



	void AddHead(XPTypedPtrList<TYPE>* l) { XPPtrList::AddHead(l); }

	void AddTail(XPTypedPtrList<TYPE>* l) { XPPtrList::AddTail(l); }



	TYPE GetNext(Position &p) const { return (TYPE)XPPtrList::GetNext(p); }

	TYPE GetPrev(Position &p) const { return (TYPE)XPPtrList::GetPrev(p); }



	TYPE GetAt(Position p) const { return (TYPE)XPPtrList::GetAt(p); }

	TYPE GetNth(unsigned long idx) const { return (TYPE)XPPtrList::GetNth(idx); }

	void SetAt(Position p, TYPE e) { XPPtrList::SetAt(p, e); }

	TYPE RemoveAt(Position p) { return (TYPE)XPPtrList::RemoveAt(p); }



public:		// Functions

	void	DeleteItems()				// Delete all of the items in the list

	{									// and reset the list

		TYPE		f;

		Position	pos;



		if (m_deleteWhenDone)

		{

			for (pos = GetHeadPosition(); (pos) && (f = GetNext(pos)); )

				delete f;

		}


		RemoveAll();

	}


	void	Remove(TYPE toRemove)		// Remove <toRemove> from the list

	{									// but don't delete it

		TYPE		f;

		Position	pos;



		for (pos = GetHeadPosition(); (pos); GetNext(pos))

		{

			f = GetAt(pos);



			if (f == toRemove)

			{

				RemoveAt(pos);

				return;

			}

		}

	}


protected:	// Data

	bool	m_deleteWhenDone;

};


//------------------------------------------------------------------------------


#if macintosh
	#pragma options align=reset
#endif


#endif

 
/* 
$Log$ 
*/ 

