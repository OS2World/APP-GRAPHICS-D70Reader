
/* $Header: //xaptk/xaptk/xapowrapper.h#7 $ */
/* xapowrapper.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.  If you have received this file from a 
source other than Adobe, then your use, modification, or distribution of it requires the prior 
written permission of Adobe.
*/


#ifndef __XAPObjWrapper__
#define __XAPObjWrapper__	/* as nothing */


/*
STL container structures want to be able to construct and
destruct their member objects.  Objects created by factory
methods (such as the DOM) cannot be used in STL containers
directly.  Thus, I provide a wrapper class that holds the
pointer to the object, and provides default constructors
and destructors (which do nothing interesting) for STL containers.

This header also defines some useful combinations of primitive
STL containers.
*/

/* ===== Forwards ===== */

/* ===== Includes ===== */

#include <utility>
#include <vector>
#include <map>
#include <stack>
#include <set>


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


/* ===== Template Definitions ===== */

/* ~~~ Wrapper based on pointer values ~~~ */

template <class Obj>
class XAPObjWrapper {
public:
    Obj* m_obj;

    XAPObjWrapper() : m_obj(NULL) {} // Default
    explicit XAPObjWrapper(Obj* obj) : m_obj(obj) {}
    ~XAPObjWrapper() {} // Destructor
};

template <class Obj> inline
bool operator<(const XAPObjWrapper<Obj>& lhs, const XAPObjWrapper<Obj>& rhs) {
    return(lhs.m_obj < rhs.m_obj);
}

template <class Obj> inline
bool operator==(const XAPObjWrapper<Obj>& lhs, const XAPObjWrapper<Obj>& rhs) {
    return(lhs.m_obj == rhs.m_obj);
}

template <class Obj> inline
bool operator>(const XAPObjWrapper<Obj>& lhs, const XAPObjWrapper<Obj>& rhs) {
    return(lhs.m_obj > rhs.m_obj);
}

template <class Obj> inline
bool operator!=(const XAPObjWrapper<Obj>& lhs, const XAPObjWrapper<Obj>& rhs) {
    return(lhs.m_obj != rhs.m_obj);
}

/* ===== Global Data Structures ===== */

typedef std::pair< std::string , std::string > XAPTk_PairOfString;

typedef std::vector< std::string > XAPTk_VectorOfString;

typedef std::map < std::string , std::string > XAPTk_StringByString;

typedef std::stack<std::string> XAPTk_StackOfString;

inline bool
operator<(const XAPTk_PairOfString& lhs, const XAPTk_PairOfString& rhs) {
    if (lhs.first == rhs.first)
        return(lhs.second < rhs.second);
    else
        return(lhs.first < rhs.first);
}

inline bool
operator==(const XAPTk_PairOfString& lhs, const XAPTk_PairOfString& rhs) {
    return(lhs.first == rhs.first && lhs.second == rhs.second);
}

typedef std::map < XAPTk_PairOfString , XAPTk_PairOfString > XAPTk_PairByPair;

/* ===== XAPTk Data Structures ===== */


namespace XAPTk {

typedef std::vector< XAPTk_PairOfString > VectorOfProps;

typedef std::stack < XAPTk_StringByString > StackOfNSDefs;

typedef std::set < XAPTk_PairOfString > SetOfQPaths;

typedef std::map < XAPTk_PairOfString , XAPTk_StringByString> SubTypeMap;

}


#if macintosh
	#pragma options align=reset
#endif


#endif // __XAPObjWrapper__

/*
$Log$
*/

