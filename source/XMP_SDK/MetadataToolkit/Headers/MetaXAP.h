
/* $Header: //xaptk/include/MetaXAP.h#13 $ */
/* MetaXAP.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/


#ifndef METAXAP_H
#define METAXAP_H /* as nothing */


/*
MetaXAP class.
See doc/MetaXAP.html for details.
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
#include "XAPExcep.h"
#include "XAPAllocator.h"
#include "XAPPathTree.h"
#include "XAPPaths.h"
#endif /* WITHIN_PS */


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


class XAPTk_Data;

// Option Constants

const long int XAP_OPTION_NONE = 0L;

const long int XAP_OPTION_ALIASING_ON = 1L<<0;
const long int XAP_OPTION_ALIAS_OUTPUT = 1L<<1;

const long int XAP_OPTION_XAPMETA_ONLY = 1L<<4;
const long int XAP_OPTION_XAPMETA_OUTPUT = 1L<<5;

const long int XAP_OPTION_AUTO_TRACK = 1L<<16;
const long int XAP_OPTION_DEBUG = 1L<<31;

// Old, rather unclear names for options
#define XAP_OPTION_ALIAS	XAP_OPTION_ALIASING_ON
#define XAP_OPTION_TAG_ONLY	XAP_OPTION_XAPMETA_ONLY

const long int XAP_OPTION_DEFAULT = ( XAP_OPTION_ALIASING_ON	|
									  XAP_OPTION_XAPMETA_OUTPUT	|
									  XAP_OPTION_AUTO_TRACK );

// Clock support class

class XAPClock {
public:
    /*
    Generate GMT(UTC) timestamps only. The dt.tzHour and
    dt.tzMin fields must be zero or a xap_bad_number will
    be thrown by the calling code.
    */
    virtual void timestamp(XAPDateTime& dt) = 0;
protected:
    virtual ~XAPClock() {}
};

/* ===== MetaXAP ===== */

class XAP_API MetaXAP : public XAPPathTree {

public:
    // Static Member Functions (Class Methods)
    static MetaXAP*
    Clone ( MetaXAP* orig );

    static XAPPaths*
    EnumerateAliases() throw();

    static bool
    GetAlias ( const std::string& aliasNS, const std::string& aliasPath,
               std::string& actualNS, std::string& actualPath,
               XAPStructContainerType& cType ) throw();

	static void
	DumpAliasMaps ( std::ostream& outStm );
	
    static XAPPaths*
    Merge ( MetaXAP* a, MetaXAP* b, MetaXAP** merge,
            const XAPMergePolicy policy, const bool justCheck = false,
            XAPTimeRelOp* dontMergeResult = NULL );

    static void
    RegisterNamespace ( const std::string& nsName,
                        const std::string& suggestedPrefix );

	static void
	GetNamespacePrefix ( const std::string& nsName, std::string& prefix );

	static void
	GetNamespaceURL ( const std::string& prefix, std::string& nsName );

    static void
    RemoveAlias ( const std::string& aliasNS,
                  const std::string& aliasPath );

    static void
    SetAlias ( const std::string& aliasNS, const std::string& aliasPath,
               const std::string& actualNS, const std::string& actualPath,
               const XAPStructContainerType cType = xap_sct_unknown );

    static void
    SetStandardAliases ( const std::string & aliasNS );


    // Constructors
    MetaXAP(); // default
    explicit MetaXAP(XAPClock* clock);
    MetaXAP(const char* xmlbuf, const long int len,
            const long int opt = XAP_OPTION_DEFAULT, XAPClock* clock = NULL);

    // Destructor
    virtual ~MetaXAP();

    // Public Member Functions
    virtual void
    append(const std::string& ns, const std::string& path,
           const std::string& value, const bool inFront = false,
           const XAPFeatures f = XAP_FEATURE_DEFAULT);

    virtual size_t
    count(const std::string& ns, const std::string& path) const;

    virtual void
    createFirstItem(const std::string& ns, const std::string& path,
                    const std::string& value,
                    const XAPStructContainerType type = xap_bag,
                    const XAPFeatures f = XAP_FEATURE_DEFAULT);

    virtual void
    createFirstItem(const std::string& ns, const std::string& path,
                    const std::string& value,
                    const std::string& selectorName,
                    const std::string& selectorVal,
                    const bool isAttr = true,
                    const XAPFeatures f = XAP_FEATURE_DEFAULT);

    virtual void
    enable(const long int opt, const bool pleaseEnable)
        throw ();

    virtual XAPPaths*
    enumerate(const int steps = 0);

    virtual XAPPaths*
    enumerate(const std::string& ns, const std::string& subPath,
              const int steps = 0);

    virtual XAPPaths*
    enumerate(const XAPTimeRelOp op, const XAPDateTime& dt,
              const XAPChangeBits how = XAP_CHANGE_MASK);

    virtual bool
    get(const std::string& ns, const std::string& path, std::string& val,
        XAPFeatures& f) const;

    virtual XAPStructContainerType
    getContainerType(const std::string& ns,
                     const std::string& subPath) const;

    virtual XAPValForm
    getForm(const std::string& ns, const std::string& subPath) const;

    virtual void
    getResourceRef(std::string& resRef) const;

        /** Get the timestamp for a property. */
    virtual bool
    getTimestamp(const std::string& ns, const std::string& path,
                 XAPDateTime& dt, XAPChangeBits& how) const;

    virtual bool
    isEnabled(const long int opt) const
        throw ();

    virtual void
    parse(const char* xmlbuf, const size_t n, const bool last = false);

    virtual void
    purgeTimestamps(const XAPChangeBits how = XAP_CHANGE_REMOVED,
                    const XAPDateTime* dt = NULL);

    virtual void
    remove(const std::string& ns, const std::string& subPath);

    virtual size_t
    serialize(const XAPFormatType f=xap_format_pretty,
              const int escnl = 0);

    virtual size_t
    extractSerialization(char* buf, const size_t nmax);

    virtual void
    set(const std::string& ns, const std::string& path,
        const std::string& value, const XAPFeatures f = XAP_FEATURE_DEFAULT);

    virtual void
    setTimestamp(const std::string& ns, const std::string& path,
                 const XAPDateTime& dt);

    virtual void
    setup(const std::string& key, const std::string& val);

    virtual void
    setResourceRef(const std::string& ref);

public: /* Undocumented */
        /** Undocumented. */
    XAPTk_Data*
    iWantPackageAccess() const;
        /*^
        This function is not for use by client programs.
        The internal implementation modules of this library need access
        to this MetaXAP. Since C++ doesn't have Java-style package access,
        and since I don't want to declare every implementation model of
        the library as a friend of this class, I have to give public
        access to the pointer.
        */

        /** Undocumented. */
    void
    shortCutSerialize(std::string& serialXML);
        /*^
        This function is not for use by client programs. The internal
        implementation modules of this library need access to the serialized
        XML string directly. Clears the this->m_serialXML after being called,
        in order to save memory.
        */

    // Alias support structure

    typedef struct {
        std::string ns;
        std::string path;
        XAPStructContainerType cType;
    } Actual;

protected:
    // Copy constructor
    MetaXAP(const MetaXAP& rhs);
    // Clone constructor
    MetaXAP(MetaXAP* orig);
    // Operators
    MetaXAP& operator=(const MetaXAP& rhs);

private:
    XAPTk_Data* m_data;
};


#if macintosh
	#pragma options align=reset
#endif


#endif /* METAXAP_H */

/*
$Log$
*/

