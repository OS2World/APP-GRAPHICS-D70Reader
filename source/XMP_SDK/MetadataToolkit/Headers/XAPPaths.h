
/* $Header: //xaptk/include/XAPPaths.h#5 $ */
/* XAPPaths.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/


#ifndef XAPPATHS_H
#define XAPPATHS_H /* as nothing */


/*
This class is returned by the XAPPathTree::enumerate function,
and its derivatives.  See doc/XAPPathTree.html for details.
*/

#include <string>

#if defined(WITHIN_PS) && (WITHIN_PS == 0)
/*
This is unfortunate, but necessary in order to make both the Acrobat
Viewer and Distiller happy.
*/
#undef WITHIN_PS
#endif

#ifndef WITHIN_PS
#include "XAPDefs.h"
#endif /* WITHIN_PS */


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


class XAP_API XAPPaths {

public:

    // Destructor
    virtual ~XAPPaths() {};

    // Public Member Functions
    virtual bool
    hasMorePaths() const throw() = 0;

    virtual void
    nextPath(std::string& ns, std::string& path) = 0;
};


#if macintosh
	#pragma options align=reset
#endif


#endif /* XAPPATHS_H */

/*
$Log$
*/

