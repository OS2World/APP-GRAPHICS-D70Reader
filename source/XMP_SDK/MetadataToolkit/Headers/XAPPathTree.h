
/* $Header: //xaptk/include/XAPPathTree.h#8 $ */
/* XAPPathTree.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/


#ifndef XAPPATHTREE_H
#define XAPPATHTREE_H /* as nothing */


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
#include "XAPExcep.h"
#include "XAPPaths.h"
#endif /* WITHIN_PS */


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


class XAP_API XAPPathTree {

public:
        /** Destructor */
    virtual ~XAPPathTree() {};
        /*^ Free, live, free! */

    // Public Member Functions

        /** Append value to end of sibling list. */
    virtual void
    append(const std::string& ns, const std::string& path,
           const std::string& value, const bool inFront = false,
           const XAPFeatures f = XAP_FEATURE_DEFAULT) = 0;
        /*^
        Create a new leaf node (character data or attribute) with the
        specified value, and add it after the element or attribute specified
        by namespace ns and path. To add a leaf node to the to the end of the
        sibling list, use the "last()" specifier, e.g., "title/ *[last()]". To
        add an attribute to the end of the list of attributes for a property,
        you must know the path of the last attribute (see enumerate). To add
        a leaf node to the beginning of an attribute or sibling list, pass
        true for inFront.
        */

        /** Count the number of siblings. */
    virtual size_t
    count(const std::string& ns, const std::string& path) const = 0;
        /*^
        Return the number of child element nodes or attributes specified by
        ns and path. To count the attributes, the path must end with '@*'. To
        count the child elements, the path must end with '*'.
        */

        /** List all paths to leaf nodes. */
    virtual XAPPaths*
    enumerate(const int steps = 0) = 0;
        /*^
        Return a pointer to an object that enumerates all of the paths to
        leaf nodes in this XAPPathTree object. Paths are listed in the
        document order of their leaf nodes. Attributes are always listed
        before child properties. It is the responsibility of the caller to
        destroy the XAPPaths object. Changes to XAPPathTree (calls to
        non-const member functions) are not reflected in the XAPPaths
        object.<p>

        The steps parameter limits the number of steps in the path. If 0
        (default), paths to all leaf nodes are enumerated, regardless of the
        number of steps to each leaf. If 1, only the paths with one step (no
        slash) are generated, which correspond to the top-level nodes of the
        XAPPathTree. If 2, paths only have two steps (one slash) or less, and
        generally include the attributes of top-level nodes if any, and
        children of top-level nodes, if any. And so on.
        */

        /** List all paths to leaf nodes from subpath. */
    virtual XAPPaths*
    enumerate(const std::string& ns, const std::string& subPath,
              const int steps = 0) = 0;
        /*^
        Return a pointer to an object that enumerates all of the properties
        in the specified subPath. Children are listed in the order they are
        specified, and attributes are always listed before child properties.
        It is the responsibility of the caller to destroy the XAPPaths
        object. Changes to XAPPathTree (calls to non-const member functions)
        are not reflected in the XAPPaths object. The steps parameter
        is described above.
        */

        /** Get a text value from a leaf node. */
    virtual bool
    get(const std::string& ns, const std::string& path,
        std::string& val, XAPFeatures& f) const = 0;
        /*^
        Get the character data of a leaf
        node, or the value of an attribute, specified by path as a string.
        Specify the namespace ns and path to the value. If any node along the
        path does not exist, return false, otherwise return true and copy the
        string into val. The features of the string value, such as whether or
        not XML markup is preserved.
        */

        /** Parse XML into a path tree. */
    virtual void
    parse(const char* xmlbuf, const size_t n, const bool last = false) = 0;
        /*^
        Parse a buffer of XML and create the corresponding XML tree in
        memory. This function expects to be called in the order that buffers
        occur for a particular XML serialization. The last buffer is
        indicated by passing true for last.</p>
        <p>
        Only one XML document should be parsed per cycle (0 or more calls to
        parse with last==false, 1 call to parse with last==true). To merge
        several different XML files into a single tree, use one cycle for
        all files.</p>
        <p>
        Calling any other functions in XAPPathTree during a parse will yield
        undefined results. Don't do it!
        */


        /** Remove a leaf node from the tree. */
    virtual void
    remove(const std::string& ns, const std::string& path) = 0;
        /*^
        Remove the specified node and all of its children. Throws an
        exception if the path is invalid, or if a syntactically valid path
        matches no node.
        */

        /** Start generating the XML for this path tree.*/
    virtual size_t
    serialize(const XAPFormatType f=xap_format_pretty,
              const int escnl = XAP_ESCAPE_CR) = 0;
        /*^
        Serialize (write out) the <B>XAPPathTree</B> XML tree as XML. Call
        the first version to start a serialization, optionally specifying a
        format and <VAR>escnl</VAR> for filtering linebreaks. The
        <VAR>f</VAR> option xap_format_pretty is pretty-printed for human
        readability, using whitespace and indenting. The <VAR>f</VAR> option
        xap_format_compact minimizes whitespace and uses the most compact
        representation possible. The <VAR>escnl</VAR> bits indicate whether
        line ending characters should be escaped into entity refs (&amp;#xD;
        for CR, &amp;#xA; for LF). This allows a client to post-filter the
        XML to impose line-length limitations: the unescaped version of the
        line-break character can be inserted into the XML, since the XML is
        guaranteed not to contain that character unescaped. Of course, such
        filtering needs to be removed in order to recover valid XML (some
        subclasses of this interface may arrange to do this automatically,
        see the documentation). If <VAR>f</VAR> is xap_format_pretty, lines
        are formatted with a linebreak character as follows: CR if
        <VAR>escnl</VAR> is XAP_ESCAPE_LF only, LF if <VAR>escnl</VAR> is
        XAP_ESCAPE_CR only, CRLF if both bits are set. Returns 0 if there is
        no metadata, &gt;0 otherwise.
        */

        /** Get the next buffer of XML for this path tree. */
    virtual size_t
    extractSerialization(char* buf, const size_t nmax) = 0;
        /*^
        Call the other variation to have data copied into a buffer that you
        provide. You specify the size of your buffer with parameter nmax. The
        function returns the number of bytes (char) that were actually
        copied. When the function returns 0, the serialization is complete.
        Subsequent calls to the buffer version of serialize will result in no
        copies and a return value of 0, until the default parameter version
        is called again.
        */

        /** Set a text value for a leaf node. */
    virtual void
    set(const std::string& ns, const std::string& path,
        const std::string& value,
        const XAPFeatures f = XAP_FEATURE_DEFAULT) = 0;
        /*^
        Set the specified value as the comment, processing instruction,
        character data child of an element, or attribute value specified by
        path, with the optionally specified features. Nodes are created as
        needed to ensure that the path is complete, except for sibling items
        of an element with children (see below xap_bad_number below).
        Existing values are overwritten.
        */

};


#if macintosh
	#pragma options align=reset
#endif


#endif /* XAPPATHTREE_H */

/*
$Log$
*/

