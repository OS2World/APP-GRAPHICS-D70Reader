
/* $Header: //xaptk/xaptk/Paths.h#3 $ */
/* Paths.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.  If you have received this file from a 
source other than Adobe, then your use, modification, or distribution of it requires the prior 
written permission of Adobe.
*/


#ifndef XAPTK_PATHS_H
#define XAPTK_PATHS_H /* as nothing */


/* ===== Forwards ===== */

/* ===== Includes ===== */

#include "XAPTkInternals.h"


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


/* ===== Class ===== */

namespace XAPTk {

class Paths : public XAPPaths {
public:
    /* ===== Static Member (Class) Functions ===== */
    static Paths*
    New();

    /* ===== Instance Variables ===== */
    VectorOfProps m_props;
    VectorOfProps::iterator m_here;

    /* ===== Public Constructors ===== */
    Paths() {
        m_here = m_props.end();
    }

    /* ===== Public Destructor ===== */
    ~Paths() {}

    /* ===== Public Member Functions ===== */
    virtual bool
    hasMorePaths() const throw();

    virtual void
    nextPath(std::string& ns, std::string& path);

    void
    addPath(const std::string& ns, const std::string& path);

protected:
    Paths(Paths& rhs); // Copy
    Paths& operator=(Paths& rhs);
}; // Paths

} // XAPTk


#if macintosh
	#pragma options align=reset
#endif


#endif // XAPTK_PATHS_H

/*
$Log$
*/

