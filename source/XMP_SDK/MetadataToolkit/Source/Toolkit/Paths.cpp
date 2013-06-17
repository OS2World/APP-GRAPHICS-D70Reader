/* $Header: //xaptk/xaptk/Paths.cpp#7 $ */
/* Paths.cpp */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/

#ifndef XAP_FORCE_NORMAL_ALLOC
	#include "XAPTkAlloc.h"
#endif

#include "XMPInitTerm.h"
#include "Paths.h"

#define DECL_VIRTUAL /* as nothing */
#define DECL_STATIC /* as nothing */

namespace XAPTk {

/* ===== Static Member (Class) Functions ===== */

DECL_STATIC Paths*
Paths::New() {
    return new XMP_Debug_new Paths();
}

/* ===== Default Constructor ===== */

/* ===== Destructor ===== */

/* ===== Public Member Functions ===== */

DECL_VIRTUAL bool
Paths::hasMorePaths() const throw() {
    return(m_here != m_props.end());
}

DECL_VIRTUAL void
Paths::nextPath(std::string& ns, std::string& path) {
    ns = m_here->first;
    path = m_here->second;
    ++m_here;
}

void
Paths::addPath(const std::string& ns, const std::string& path) {
    XAPTk_PairOfString pp(ns, path);
    m_props.push_back(pp);
    m_here = m_props.begin();
#if defined(XAP_DEBUG_CERR) && 0
    std::cerr << "EN[" << path << ", " << ns << "] " << std::endl;
#endif
}

} // XAPTk

void
XAPTk_InitPaths() {
    //No-op
}

void
XAPTk_KillPaths() {
    //No-op
}

/*
$Log$
*/
