
/* $Header: //xaptk/xaptk/DOMWalker.h#5 $ */
/* DOMWalker.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.  If you have received this file from a 
source other than Adobe, then your use, modification, or distribution of it requires the prior 
written permission of Adobe.
*/


#ifndef DOMWALKER_H
#define DOMWALKER_H /* as nothing */


/*
Interface class for creating objects that can walk a DOM tree
depth-first.
*/

/* ===== Forwards ===== */

/* ===== Includes ===== */

#include "DOM.h"


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


/* ===== Class ===== */

namespace XAPTk {

class DOMWalker {
public:
    /* ===== Public Destructor ===== */
    virtual ~DOMWalker() {}

    /* ===== Public Member Functions ===== */

        /** Found an element node. */
    virtual bool
    enterElement(Element* e) = 0;
        /*^
        Depth-first, first encounter of element before children are recursed.
        Return false if the rest of the element processing should be skipped.
        */

        /** All children of element are done. */
    virtual void
    exitElement(Element* e) = 0;
        /*^
        Depth-first, all children have been recursed, about
        to do next sib, or pop up to parent.
        */

        /** Abort the tree walk. */
    virtual bool
    finished() = 0;
        /*^
        Returns true if the tree walk should be stopped.
        Useful for implementing predicates over tree.
        */

        /** Handle non-Element, non-Text node. */
    virtual void
    handleNode(Node* node) = 0;
        /*^
        Handle comments, PI's, etc., or throw an exception.
        */

        /** Found a character data node. */
    virtual void
    text(CharacterData* cd) = 0;
        /*^
        Text is always a leaf node (ignoring PI, comment, etc.).
        Accumulate until exitElement encountered.
        */

}; // DOMWalker

} // XAPTk


#if macintosh
	#pragma options align=reset
#endif


#endif // DOMWALKER_H

/*
$Log$
*/

