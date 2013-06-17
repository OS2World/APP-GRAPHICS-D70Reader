
/* $Header: //xaptk/xaptk/XAPTkFuncs.h#9 $ */
/* xapfuncs.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.  If you have received this file from a 
source other than Adobe, then your use, modification, or distribution of it requires the prior 
written permission of Adobe.
*/


#ifndef XAPFUNCS_H
#define XAPFUNCS_H /* as nothing */


/*
General purpose functions for use by the XAPTk implementation.
*/

#include <string>
#include <stdlib.h>

#include "XAPDefs.h"
#include "XAPObjWrapper.h"


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


/* ===== Forwards ===== */

/* ===== Includes ===== */

namespace XAPTk {

/* ===== Forwards ===== */

class NormTree;

/* ===== Types ===== */

/* ===== Functions ===== */

    /** Used when accepting text from the client. */
bool
CDATAToRaw(const std::string& cdata, std::string& raw);
    /*^
    Text is stored in raw, unescaped form in NormTrees.
    Clients may specify text with escapes as property
    values. This function converts them to raw form.
    */

#ifdef XAP_DEBUG_VALIDATE
    /** Debugging function for stack size. */
size_t
CheckStack(const long* base, const long* high);
    /*^
    Test the size of the stack, for recursive functions.
    Call with high=NULL to set the base.
    Call with base=NULL to compute size.
    */
#endif // XAP_DEBUG_VALIDATE

void
ContractName ( const std::string& ns, const std::string& localPart, std::string& qName );
	// Contract full namespace (ns) plus localPart into qName.

    /** Compare A to B. */
XAPTimeRelOp
CompareDateTimes(const XAPDateTime& adt, const XAPDateTime& bdt);
    /*^
    The returned operator has the relationship (A op B).
    */

    /** Language alternative convenience function. */
void
ComposeAltLang(const std::string& contPath, const std::string& lang, std::string& path);
    /*^
    Given a container path and a language, return a path
    that is an alternate selected by language.
    */

    /** Find fixed part of path. */
void
ConformPath(const std::string& orig, std::string& fixed, std::string& var);
    /*^
    Take the original path and divide it into the fixed
    part (no '*'), and variable part.  Scan left to right,
    and stop at the first '*'.
    */

    /** Language alternative convenience function. */
void
DecomposeAltLang(const std::string& path, std::string& contPath, std::string& lang);
    /*^
    Inverse of ComposeAltLang.
    */

    /** Break path into steps. */
void
ExplodePath(const std::string& xpath, XAPTk_VectorOfString& expr);
    /*^
    Each member of the vector is set to a step in the path.
    */

    /** Throw a fit. */
void
NotImplemented(const char* what);
    /*^
    Placeholder for unfinished features.
    */

    /** Used when serializing. */
void
RawToCDATA(const std::string& raw, std::string& cdata, int escnl = XAP_ESCAPE_CR);
    /*^
    Convert raw text to escaped text.  The escnl parameter
    specifies which newline characters to filter.  Set to
    zero for none.
    */

    /** Decompose a Qname. */
void
StripPrefix(const std::string& xmlName, std::string* localPart, std::string* prefix = NULL);
    /*^
    The parameters are pointers so you can pass NULL when you
    don't want one or the other of the parts.
    */

    /** A string scanning function. */
void
ToDateTime(const std::string& s, XAPDateTime& dateTime);
    /*^
    Convert a string into a date/time.
    */

    /** A string formatting function. */
void
ToString(const size_t n, std::string& s);
    /*^
    Pretty-print an unsigned int as a string.
    */

    /** A string formatting function. */
void
ToString(const long int n, std::string& s);
    /*^
    Pretty-print a long int as a string.
    */

    /** A string formatting function. */
void
ToString(const double n, std::string& s);
    /*^
    Pretty-print a double as a string.
    */

    /** A string formatting function. */
void
ToString(const XAPDateTime& dateTime, std::string& s);
    /*^
    Pretty-print a date/time as a string.
    */

} // namespace XAPTk


#if macintosh
	#pragma options align=reset
#endif


#endif // XAPFUNCS_H

/*
$Log$
*/

