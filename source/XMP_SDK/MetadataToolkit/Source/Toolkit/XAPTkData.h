
/* $Header: //xaptk/xaptk/XAPTk_Data.h#16 $ */
/* XAPTk_Data.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.  If you have received this file from a 
source other than Adobe, then your use, modification, or distribution of it requires the prior 
written permission of Adobe.
*/


#ifndef XAPTK_DATA_H
#define XAPTK_DATA_H /* as nothing */


/*
The XAPTk_Data class serves to encapsulate and hide all of the instance variables needed by MetatXAP from clients, while giving access to all implementation classes.
*/

/* ===== Forwards ===== */

class MetaXAP;

/* ===== Includes ===== */

#include "XAPConfigure.h"

#include <string.h>

#include <string>
#include <vector>
#include <map>
#include <sstream>

#include "DOM.h"
#include "DOMGlue.h"

#include "XAPTkInternals.h"
#include "DOMWalker.h"
#include "NormTree.h"


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


/* ===== Types ===== */

XAP_ENUM_TYPE ( XAPTk_PreParseStates ) {
    sInit,
    sStart,
    sBang,
    sPIOrProlog,
    sFinishPI,
    sProcess,
    sFindEnd,
    sDone
};

/* ===== Support Classes ===== */

class PunchCard {
public:
    /*
    This class is used to record a single timestamp for a property.
    */
    XAPDateTime m_lastModified;
    XAPChangeBits m_change;

    PunchCard() : m_change(XAP_CHANGE_NONE) {
        memset(&m_lastModified, 0, sizeof(XAPDateTime));
    }
    ~PunchCard() {}
};

    /*
    XAPTk_PunchCardByPath maps a canonical property path to a PunchCard.
    */
typedef std::map < std::string , PunchCard > XAPTk_PunchCardByPath;
/* [first: propPath, second: PunchCard] */

    /*
    XAPTk_ChangeLog maps a namespace string to a XAPTk_PunchCardByPath map.
    */
typedef std::map < std::string , XAPTk_PunchCardByPath > XAPTk_ChangeLog;
/* [first: ns, second: XAPTk_PunchCardByPath] */

/* ===== "Package" Module Functions ===== */

XAPTk_StringByString*
MetaXAP_GetMasterNSMap();

/* ===== Class ===== */

class XAPTk_Data {
public:
    /* ===== Static Member (Class) Functions ===== */

    static XAPTk_Data*
    New(MetaXAP* owner, XAPTk_Data* orig = NULL);

    /* ===== Instance Variables ===== */
    MetaXAP* m_owner;
    long int m_options;
    XAPClock* m_clock;
    Document* m_domDoc;
    std::istringstream* m_xmlStm;
    bool m_parsePass;
    int m_parseFilter;
    XAPTk_PreParseStates m_preState;
    int m_prePI;
    size_t m_preParseLen;
    char m_preParseBuf[XAPTk::CBUFSIZE];
    std::string m_serialXML;
    XAPTk::NormTreeByStr m_bySchema;
    XAPTk::NormTreeByStr m_byID;
    XAPTk_ChangeLog m_changeLog;
    std::string m_about;

    /* ===== Public Destructor  ===== */

    ~XAPTk_Data();

    /* ===== Public Member Functions ===== */

        /** Append. */
    void
    append(const std::string& ns, const std::string& path, const std::string& value, const bool inFront, const XAPFeatures f);
        /*^
        Support for append.
        */

        /** Count. */
    size_t
    count(const string& ns, const string& path) const;
        /*^
        Path must end with '*'.
        */

        /** Create the first item of a container. */
    void
    createFirstItem(const std::string& ns, const std::string& path, const std::string& value, const XAPStructContainerType type, const std::string* selectorName, const std::string* selectorVal, const bool isAttr, const XAPFeatures f);
        /*^
        Whew, almost forgot this!
        */

        /** Create a new property. */
    void
    createProp(const std::string& ns, const std::string& path, const std::string& value, const XAPFeatures f);
        /*^
        Put the node at the end of the sibling list.
        */

        /** Pass along the call to enumerate. */
    XAPPaths*
    enumerate(const std::string* ns = NULL, const std::string* subPath = NULL, const int steps = 0);
        /*^
        I'll probably get rid of this and move the code up to MetaXAP.
        */

        /** Enumerate by timestamp. */
    XAPPaths*
    enumerate(const XAPTimeRelOp op, const XAPDateTime& dt,
              const XAPChangeBits how);
        /*^
        No additional info.
        */

        /** Evaluate the XPath to an Element. */
    Node*
    evalXPath(const std::string& ns, const std::string& xpath, const bool okNewSchema = false);
        /*^
        If okNewSchema is true, a new schema is created with no properties in it.
        */

        /** With throw mode, evaluate the XPath to an Element. */
    Node*
    evalXPath(const XAPTk_ThrowMode mode, const std::string& ns, const std::string& xpath, const bool okNewSchema = false);
        /*^
        If okNewSchema is true, a new schema is created with no properties in it.
        */

        /** Get the value of this property. */
    bool
    extractPropVal(const std::string& ns, Element* leafProp, std::string& val, XAPFeatures& f) const;
        /*^
        Schema must be valid.
        */

        /** Fixups before serializing. */
    bool
    fixup(XAPTk::NormTree* norm, const XAPFormatType f, const int escnl);
        /*^
        Returns true if this tree was fully handled.
        */

        /** Get actual value. */
    bool
    get(const std::string& ns, const std::string& path, std::string& val, XAPFeatures& f) const;
        /*^
        No additional info.
        */

        /** Get actual container type. */
    XAPStructContainerType
    getContainerType(const std::string& ns, const std::string& subPath) const;
        /*^
        No additional info.
        */

        /** Get actual form. */
    XAPValForm
    getForm(const std::string& ns, const std::string& subPath) const;
        /*^
        No additional info.
        */

        /** Get the timestamp for the property. */
    bool
    getTimestamp(const std::string& ns, const std::string& path, XAPDateTime& dt, XAPChangeBits& how) const;
        /*^
        Returns false if not defined.
        */

        /** Walk DOM tree and load data structures */
    void
    loadFromTree(Node* here = NULL);
        /*^
        Does not require that data structures be cleared before load.
        */

        /** Pass along the call to parse. */
    void
    parse(const char* buf, const size_t n, const bool last = false);
        /*^
        Just like for XAPPathTree.
        */

        /** Update timestamp. */
    void
    punchClock(const std::string& ns, XAPTk::NormTree* norm, Node* propOrAttr, const XAPChangeBits& change);
        /*^
        Checks options and clock.
        */

        /** Purge unwanted timestamps. */
    void
    purgeTimestamps(const XAPChangeBits how, const XAPDateTime* dt);
        /*^
        Purge records that match indicated bits.
        */

        /** Remove property. */
    void
    remove(const std::string& ns, const std::string& subPath);
        /*^
        Remove property.
        */

    /** Replace property. */
    void
    replaceProp(const std::string& ns, Node* prop, const std::string& value, const XAPFeatures f);
        /*^
        Trash the existing value and attributes, set the
        new one.
        */

        /** Select child by name. */
    Element*
    selectChild(const std::string& ns, Element* e, const std::string& expr);
        /*^
        The expression can contain the wild card and a
        predicate filter.
        */

        /** Start a serialization. */
    size_t
    serialize(const XAPFormatType f, const int escnl);
        /*^
        Walk the whole tree and make one giant string.
        */

        /** Get buffers of serialized XML. */
    size_t
    extractSerialization(char* buf, const size_t nmax);
        /*^
        Get buffers from istrstream.
        */

        /** Set actual value. */
    void
    set(const std::string& ns, const std::string& path, const std::string& value, const XAPStructContainerType sct, const XAPFeatures f = XAP_FEATURE_DEFAULT);
        /*^
        No additional info.
        */

        /** Set the timestamp. */
    void
    setTimestamp(const std::string& ns, const std::string& path, const XAPDateTime& dt, const XAPChangeBits& change);
        /*^
        Create if does not yet exist.
        */

        /** Convert path into canonical timestamp path. */
    bool
    toTimestampPath(const std::string& ns, const std::string& path, std::string& canon) const;
        /*^
        Convert path into path used for timestamp for this property.
        Returns false if path does not match any valid property.
        */

protected:
    /* ===== Protected Constructors ===== */

    XAPTk_Data(); // Default

    XAPTk_Data(const XAPTk_Data& rhs); // Copy

    explicit XAPTk_Data(MetaXAP* owner);

    XAPTk_Data*
    clone(MetaXAP* owner) const;

    bool
    getLog(const std::string& ns, const XAPTk_PunchCardByPath** log) const;

    bool
    getPunchCard(const XAPTk_PunchCardByPath& log, const std::string& propPath, const PunchCard** card) const;

    void
    innerParse(const char* buf, const size_t n, const bool last);

    void
    serializeTimestamps(XAPFormatType f, const std::string& nl, std::string& tsd);

}; // XAPTk_Data


#if macintosh
	#pragma options align=reset
#endif


#endif // XAPTK_DATA_H

/*
$Log$
*/

