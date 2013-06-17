/* $Header: //xaptk/xaptk/MetaXAP.cpp#29 $ */
/* MetaXAP.cpp */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/

/*
Implementation of the MetaXAP class.

The MetaXAP class is a "Handle" class.  It defines all the member
functions, but the only visible member variable is "XAPTk_Data* m_data",
where XAPTk_Data is an opaque class. The current implementation uses
this only for information hiding purposes. Most of the member functions
are wrappers that test pre-conditions and then call a member function in
XAPTk_Data. All of the actual instance data is inside of XAPTk_Data.

Because of this "Handle" design and because MetaXAP.h is the public
interface, a new implementation that uses advanced techniques, such
as reference counting or copy-on-write, can be slipped in without any
changes to clients, not even a recompile!

On the other hand, all of the static functions declared
in MetaXAP.h are defined here.
*/

#include "XAPConfigure.h"

#ifndef XAP_FORCE_NORMAL_ALLOC
	#include "XAPTkAlloc.h"
#endif

#include "XAPTkInternals.h" // MetaXAP.h
#include "XAPTkData.h"
#include "XAPAllocator.h"
#include "UtilityXAP.h"
#include "Paths.h"
#include "XMPInitTerm.h"
#include "XPNode.h"		// To implement UpdateAliasFlags.

using namespace std;


/* ===== Forwards ===== */

const string& MetaXAP_CheckSubType(const string& ns, const string& path, string& newPath);

static bool MetaXAP_CollectAliases(const MetaXAP* mx, const string& ns, const string& path, XAPTk::VectorOfProps& props, const bool suppressSingles, XAPStructContainerType* cType = NULL);

static bool CheckAlias(const MetaXAP* mx, const string& ns, const string& path, string& parentPath, string& ans, string& apath, string& aParentPath, string& savedPath, string& topLevel, string& qualifier, XAPStructContainerType* cType = NULL);

static void CopyProp(MetaXAP* src, MetaXAP* dst, const string& ns, const string& path);

static void CopyTimestamp(MetaXAP* src, MetaXAP* dst, const string& ns, const string& path);

static void DupProp(MetaXAP* mx, const string& ns, const string& path, const string& ans, const string& apath, const XAPStructContainerType& sct);

static void PreResolveAlias(const XAPTk_PairOfString& alias, const MetaXAP::Actual& actual);

static bool VerifyAndPopulate ( MetaXAP* mx, bool squashDifs );

static void MetaXAP_InitNSMap();
static void MetaXAP_InitAliasMap();

static void ValidateNS(const string& ns);

/* ===== Inlines ===== */

inline static void
ValidateFeatures(const XAPFeatures f) {
    /*
    Only certain feature bit combinations are supported.  Throw
    an exception if an invalid combination is attempted.
    */
    if (f == XAP_FEATURE_NONE || f == XAP_FEATURE_DEFAULT)
        return;
    if (f == XAP_FEATURE_XML)
        return;
    if (f == XAP_FEATURE_RDF_VALUE)
        return;
    if (f == XAP_FEATURE_RDF_RESOURCE)
        return;
    if (f == (XAP_FEATURE_RDF_VALUE|XAP_FEATURE_RDF_RESOURCE))
        return;
    /* Oh, oh!  Bad features! */
    throw xap_bad_feature ();
}

/* ===== Operators ===== */

#if defined(XAP_DEBUG_CERR)
inline ostream&
operator<<(ostream& os, const XAPTimeRelOp& cmp) {
    string s;
    switch (cmp) {
        case xap_at: s = "xap_at"; break;
        case xap_before: s = "xap_before"; break;
        case xap_after: s = "xap_after"; break;
        case xap_notDef: s = "xap_notDef"; break;
        case xap_noTime: s = "xap_noTime"; break;
        default: s = "unknown";
    }
    return os << s;
}
#endif

/* ===== Constants ===== */

namespace XAPTk {

const long int OPTION_MASK = ( XAP_OPTION_ALIASING_ON		|
							   XAP_OPTION_ALIAS_OUTPUT		|
							   XAP_OPTION_XAPMETA_ONLY		|
							   XAP_OPTION_XAPMETA_OUTPUT	|
							   XAP_OPTION_AUTO_TRACK		|
							   XAP_OPTION_DEBUG );

const long int FEATURE_MASK = (
    XAP_FEATURE_XML);

}

/* ===== Types ===== */

typedef std::map < XAPTk_PairOfString , MetaXAP::Actual > MetaXAP_AliasMap;

typedef struct {
    XAPStructContainerType cType;
    bool actual;
    bool aliasSingle;
    bool actualFirst;
    bool actualLang;
} MetaXAP_AliasInfo;

typedef std::map < XAPTk_PairOfString , MetaXAP_AliasInfo > MetaXAP_InfoMap;
    /*
    [first: <ns,path>, second: MetaXAP_AliasInfo ]
    Maps a property (alias or actual) to alias info.
    */

typedef std::map < XAPTk_PairOfString , MetaXAP_InfoMap > MetaXAP_ResolvedAliases;
    /*
    [first: <ns,path>, second: MetaXAP_InfoMap]
    Maps a property (alias or actual) to a map of all the other
    properties that are linked to it by value.
    */

/* ===== Class Definitions ===== */

class MetaXAP_AVS {
    /* Support class for MetaXAP::Merge */
public:
    string val;
    XAPFeatures f;
};

inline bool operator<(const MetaXAP_AVS& lhs, const MetaXAP_AVS& rhs) {
    return(lhs.val < rhs.val);
}

inline bool operator==(const MetaXAP_AVS& lhs, const MetaXAP_AVS& rhs) {
    return(lhs.val == rhs.val && lhs.f == rhs.f);
}


/* ===== Static (Module) Variables ===== */

static XAPTk_StringByString* MetaXAP_nsMap = NULL; //STATIC_INIT

static MetaXAP_AliasMap* MetaXAP_aliasMap = NULL; //STATIC_INIT

static MetaXAP_ResolvedAliases* MetaXAP_resolvedAliases = NULL; //STATIC_INIT

/* ===== Static Member Functions (Class Methods) ===== */

DECL_EXPORT MetaXAP*
MetaXAP::Clone(MetaXAP* orig) {
    return new XMP_Debug_new MetaXAP(orig);
}

DECL_EXPORT XAPPaths*
MetaXAP::EnumerateAliases() throw() {
    XAPTk::Paths* p = NULL;

	XMP_CheckToolkitInit();
	
    try {
        MetaXAP_InitAliasMap();
        MetaXAP_AliasMap::const_iterator done = MetaXAP_aliasMap->end();
        MetaXAP_AliasMap::const_iterator i = MetaXAP_aliasMap->begin();
        /* [first: <ns,path>, second: Actual] */
        for(; i != done; ++i) {
            if (p == NULL) {
                p = new XMP_Debug_new XAPTk::Paths();
            }
            p->addPath(i->first.first, i->first.second);
        }
    } catch (...) {
        delete p;
        p = NULL;
    }
    return(p);
}

DECL_EXPORT bool
MetaXAP::GetAlias(const string& aliasNS, const string& aliasPath, string& actualNS, string& actualPath, XAPStructContainerType& cType) throw() {

	XMP_CheckToolkitInit();
	
    try {
        MetaXAP_InitAliasMap();
        string fixed;
        string var;
        XAPTk::ConformPath(aliasPath, fixed, var);
        if (fixed.empty())
            return(false);
        XAPTk_PairOfString alias(aliasNS, fixed);
        MetaXAP_AliasMap::const_iterator done = MetaXAP_aliasMap->end();
        MetaXAP_AliasMap::const_iterator i = MetaXAP_aliasMap->find(alias);
        if (i != done) {
            /* [first: alias, second: actual] */
            actualNS = i->second.ns;
            actualPath = i->second.path;
            cType = i->second.cType;
            return(true);
        }
    } catch (...) {
    }
    return(false);
}


DECL_EXPORT void
MetaXAP::DumpAliasMaps ( ostream& outStm )
{

	XMP_CheckToolkitInit();
	
    // Dump state of maps
    
	{
		MetaXAP_AliasMap::const_iterator n		= MetaXAP_aliasMap->begin();
		MetaXAP_AliasMap::const_iterator nDone	= MetaXAP_aliasMap->end();

		outStm << endl;
		outStm << "========================" << endl;
		outStm << "Dump of MetaXAP_aliasMap" << endl << endl;
		
		for ( ; n != nDone; ++n ) {

			outStm << n->first.first << '\t' << n->first.second << "\t => ";
			outStm << n->second.ns << '\t' << n->second.path;

			switch ( n->second.cType ) {
				case xap_alt: outStm << "\t alt"; break;
				case xap_bag: outStm << "\t bag"; break;
				case xap_seq: outStm << "\t seq"; break;
				case xap_sct_unknown: ; break;	// Really means simple
				default: outStm << "\t ?"; break;
			}

			outStm << endl;

		}

	}

	{
		MetaXAP_ResolvedAliases::const_iterator n = MetaXAP_resolvedAliases->begin();
		MetaXAP_ResolvedAliases::const_iterator nDone = MetaXAP_resolvedAliases->end();
		MetaXAP_InfoMap::const_iterator m;
		MetaXAP_InfoMap::const_iterator mDone;

		outStm << endl;
		outStm << "===============================" << endl;
		outStm << "Dump of MetaXAP_resolvedAliases" << endl << endl;
		
		for (; n != nDone; ++n) {

			outStm << n->first.first << '\t' << n->first.second << "\t :" << endl;
			m = n->second.begin();
			mDone = n->second.end();

			for (; m != mDone; ++m) {

				outStm << '\t' << m->first.first << '\t' << m->first.second << " (" ;
				const MetaXAP_AliasInfo& inf = m->second;

				if (inf.actual) outStm << " actual";
				if (inf.aliasSingle) outStm << " single";
				if (inf.actualFirst) outStm << " [1]";
				if (inf.actualLang) outStm << " lang";

				switch (inf.cType) {
					case xap_alt: outStm << " alt"; break;
					case xap_bag: outStm << " bag"; break;
					case xap_seq: outStm << " seq"; break;
					case xap_sct_unknown: break;
					default: outStm << " ?"; break;
				}

				outStm << " )" << endl;

			}

		}

	}

	outStm << endl;

}


    /** Merge properties in two different MetaXAP instances. */
DECL_EXPORT XAPPaths*
MetaXAP::Merge(MetaXAP* a, MetaXAP* b, MetaXAP** merge, const XAPMergePolicy policy, const bool justCheck /*= false*/, XAPTimeRelOp* dontMergeResult /*= NULL */) {
    assert((a != NULL) && (b != NULL));
    if (merge == NULL) {
        assert(policy == xap_policy_dont_merge || policy == xap_policy_ask_user);
    }

    /*
    Strategy: Based on policy, clone a or b into dst, and assign the other to src.  For every timestamp-able path in src, compare with dst.  If not defined and not removed in dst, copy it.  If not defined and removed from dst, or defined in dst, observe policy. Then, for every removed property in src, compare with dst.  Observe policy. Record each conflict.
    */

    MetaXAP* src;
    MetaXAP* dst = NULL;
    XAPTimeRelOp cmp;
    XAPPaths* p;
    XAPDateTime srcdt;
    XAPDateTime dstdt;
    XAPDateTime srcAlt;
    XAPDateTime dstAlt;
    XAPDateTime oldDT;
    bool restoreDT = false;
    bool isMerged = false;
    bool srcAltStamp = false;
    bool dstAltStamp = false;
    XAPTk::Paths* conflicts = XAPTk::Paths::New();
    string origPath;
    XAPTk_PairOfString qPath;
    XAPTk::SetOfQPaths removed;
    XAPTk::SetOfQPaths srcProps;
    bool aAliased;
    bool bAliased;

    // Initialize

    p = NULL;
    if (merge != NULL)
        *merge = NULL;

    // Test preconditions
    if (a == NULL || b == NULL)
        return(NULL);

    // Remember aliasing option
    aAliased = a->isEnabled(XAP_OPTION_ALIASING_ON);
    a->enable(XAP_OPTION_ALIASING_ON, false);
    bAliased = b->isEnabled(XAP_OPTION_ALIASING_ON);
    b->enable(XAP_OPTION_ALIASING_ON, false);

  try {

    //Decide which MetaXAP gets cloned
    if (policy == xap_policy_a) {
        src = b;
        dst = (justCheck) ? a : MetaXAP::Clone(a);
    } else if (policy == xap_policy_b) {
        src = a;
        dst = (justCheck) ? b : MetaXAP::Clone(b);
    } else if (policy == xap_policy_newest) {
        cmp = UtilityXAP::CompareTimestamps(a, b, XAP_NS_XAP, XAPTK_METADATA_DATE);
        if (cmp == xap_before) { // b newer than a
            src = a;
            dst = (justCheck) ? b : MetaXAP::Clone(b);
        } else {
            src = b;
            dst = (justCheck) ? a : MetaXAP::Clone(a);
        }
    } else if (policy == xap_policy_oldest) {
        cmp = UtilityXAP::CompareTimestamps(a, b, XAP_NS_XAP, XAPTK_METADATA_DATE);
        if (cmp == xap_before) { // a older than b
            src = b;
            dst = (justCheck) ? a : MetaXAP::Clone(a);
        } else {
            src = a;
            dst = (justCheck) ? b : MetaXAP::Clone(b);
        }
    } else {
        // Make dst whichever one is bigger
        XAPPaths* allA = a->enumerate();
        XAPPaths* allB = b->enumerate();
        size_t na = 0;
        size_t nb = 0;
        string tmp1;
        string tmp2;

        while(allA->hasMorePaths()) {
            allA->nextPath(tmp1, tmp2);
            ++na;
        }
        delete allA;
        while(allB->hasMorePaths()) {
            allB->nextPath(tmp1, tmp2);
            ++nb;
        }
        delete allB;
        if (na > nb) {
            src = b;
            dst = (justCheck) ? a : MetaXAP::Clone(a);
        } else {
            src = a;
            dst = (justCheck) ? b : MetaXAP::Clone(b);
        }
    }

    //Enumerate src for unique timestamp paths
    p = src->enumerate();
    if (p != NULL) try {
        XAPTk_Data* src_data = src->iWantPackageAccess();
        bool good;
        string tmp;
        while(p->hasMorePaths()) {
            /* [first: ns, second: path] */
            p->nextPath(qPath.first, origPath);
            //We only care about canonical paths.
            good = src_data->toTimestampPath(qPath.first, origPath, qPath.second);
            if (!good) {
                continue; // skip this bogus path!
            }
            srcProps.insert(qPath);
        }
    } catch (...) {
        delete p;
        throw;
    }
    delete p;
    p = NULL;

    /*
    srcProps contains canonical timestamp-able paths.
    */

    //Now enumerate srcProps and compare
    XAPTk::SetOfQPaths::const_iterator i;
    XAPTk::SetOfQPaths::const_iterator done = srcProps.end();
    XAPChangeBits srcHow;
    XAPChangeBits dstHow;
    bool skipSrc;
    bool skipDst;
    bool srcNoTime;
    bool dstNoTime;

    //Get the MetadataDates, just in case.
    srcAltStamp = UtilityXAP::GetDateTime(src, XAP_NS_XAP, XAPTK_METADATA_DATE, srcAlt);
    dstAltStamp = UtilityXAP::GetDateTime(dst, XAP_NS_XAP, XAPTK_METADATA_DATE, dstAlt);

    for(i = srcProps.begin(); i != done; ++i) {
        /* i = [first: ns, second: path], aliases already resolved */
        //Reject props with suspect bits
        skipSrc = false;
        skipDst = false;
        srcHow = XAP_CHANGE_NONE;
        dstHow = XAP_CHANGE_NONE;

#if defined(XAP_DEBUG_CERR) && 0
        cerr << "merge? [" << i->second << ", " << i->first << "]" << endl;
#endif

        /*
        We need to get the timestamp for each property so we can get
        the "how changed" bits, and so that we can substitute the
        MetadataDate if needed.
        */
        srcNoTime = false;
        dstNoTime = false;

        if (!src->getTimestamp(i->first, i->second, srcdt, srcHow)) {
            //Try MetadataDate
            if (srcAltStamp) {
                srcdt = srcAlt;
            } else {
                if (policy == xap_policy_newest || policy == xap_policy_oldest)
                    skipSrc = true;
                memset(&srcdt, 0, sizeof(XAPDateTime));
                srcNoTime = true;
            }
        }

        if (!dst->getTimestamp(i->first, i->second, dstdt, dstHow)) {
            //Try MetadataDate
            if (dstAltStamp) {
                dstdt = dstAlt;
            } else {
                if (policy == xap_policy_newest || policy == xap_policy_oldest)
                    skipDst = true;
                memset(&dstdt, 0, sizeof(XAPDateTime));
                dstNoTime = true;
            }
        }

        if (skipSrc || (srcHow & XAP_CHANGE_SUSPECT) != 0)
            continue; // skip
        if (skipDst || (dstHow & XAP_CHANGE_SUSPECT) != 0)
            continue; // skip

        cmp = UtilityXAP::CompareTimestamps(src, dst, i->first, i->second);

        if (cmp == xap_noTime) {
            if (!srcNoTime && !dstNoTime) {
                cmp = XAPTk::CompareDateTimes(srcdt, dstdt);
            } else
                continue; // skip
        }

#if defined(XAP_DEBUG_CERR) && 0
        cerr << cmp << endl;
#endif

        if (cmp == xap_at)
            continue; // skip

        if (cmp == xap_notDef
        && policy != xap_policy_dont_merge
        && policy != xap_policy_ask_user) {
            //Figure out whether notDef is in src or dst
            XAPValForm form = xap_unknown;
            try {
                form = src->getForm(i->first, i->second);
            } catch (...) {
            }
            if (form != xap_unknown) {
                //Defined in src, but not in dst
                if ((dstHow & XAP_CHANGE_REMOVED) == 0) {
                    //Wasn't removed
                    if (!justCheck) {
                        // Copy it right away
#if defined(XAP_DEBUG_CERR) && 0
                        cerr << "Defined in src, not in dst, not removed from dst." << endl;
#endif
                        CopyProp(src, dst, i->first, i->second);
                        // Copy of not defined is not a conflict
                        CopyTimestamp(src, dst, i->first, i->second);
                        isMerged = true;
                    }
                    continue;
                }
                //Else compare removed timestamp with other
                if (!srcNoTime && !dstNoTime)
                    cmp = XAPTk::CompareDateTimes(srcdt, dstdt);
                //If removed, fall thru to observe policy
            } else
                continue; //Defined in dst, but not in src
        }

        //Observe policy
        switch (policy) {
            case xap_policy_a:
            case xap_policy_b:
                //Already defined in dst
                conflicts->addPath(i->first, i->second);
                continue;
            case xap_policy_newest:
                if (cmp == xap_after) { // src newer than dst
                    if (!justCheck) {
#if defined(XAP_DEBUG_CERR) && 0
                        cerr << "src newer than dst." << endl;
#endif
                        CopyProp(src, dst, i->first, i->second);
                        CopyTimestamp(src, dst, i->first, i->second);
                        isMerged = true;
                    }
                }
                conflicts->addPath(i->first, i->second);
                break;
            case xap_policy_oldest:
                if (cmp == xap_before) { // src older than dst
                    if (!justCheck) {
#if defined(XAP_DEBUG_CERR) && 0
                        cerr << "src older than dst." << endl;
#endif
                        CopyProp(src, dst, i->first, i->second);
                        CopyTimestamp(src, dst, i->first, i->second);
                        isMerged = true;
                    }
                }
                conflicts->addPath(i->first, i->second);
                break;
            case xap_policy_dont_merge:
            case xap_policy_ask_user:
                //Just record the conflict
                conflicts->addPath(i->first, i->second);
                break;
            default:
                continue;
        }
    } // for

    /*
    Now deal with properties removed from src.
    */

    //Gather the props removed from src
    XAPDateTime zeroTime;
    memset(&zeroTime, 0, sizeof(XAPDateTime));
    p = src->enumerate(xap_after, zeroTime, XAP_CHANGE_REMOVED);
    if (p != NULL) try {

        while (p->hasMorePaths()) {
            /* [first: ns, second: path] */
            p->nextPath(qPath.first, qPath.second);
            removed.insert(qPath);
        }
    } catch (...) {
        delete p;
        throw;
    }
    delete p;
    p = NULL;

    /*
    removed contains paths of removed properties.
    */

    //Remove according to policy
    done = removed.end();

    for(i = removed.begin(); i != done; ++i) {
        /* i = [first: ns, second: path], aliases already resolved */
        //Reject props with suspect bits
        srcHow = XAP_CHANGE_NONE;
        dstHow = XAP_CHANGE_NONE;

#if defined(XAP_DEBUG_CERR) && 0
        cerr << "remove? [" << i->second << ", " << i->first << "]" << endl;
#endif

        if (!src->getTimestamp(i->first, i->second, srcdt, srcHow)) {
            continue; // shouldn't happen, but harmless
        }

        if (!dst->getTimestamp(i->first, i->second, dstdt, dstHow)) {
            //Try MetadataDate
            if (!dstAltStamp) {
                continue;
            } else
                dstdt = dstAlt;
        }

        if ((srcHow & XAP_CHANGE_SUSPECT) != 0)
            continue; // skip
        if ((dstHow & XAP_CHANGE_SUSPECT) != 0)
            continue; // skip

        cmp = XAPTk::CompareDateTimes(srcdt, dstdt);

        if (cmp == xap_at)
            continue; // skip

        //Observe policy
        switch (policy) {
            case xap_policy_newest:
                if (cmp == xap_after) { // src newer than dst
                    if (!justCheck) {
#if defined(XAP_DEBUG_CERR) && 0
                        cerr << "remove src newer than dst." << endl;
#endif
                        restoreDT = UtilityXAP::GetDateTime(dst, XAP_NS_XAP, XAPTK_METADATA_DATE, oldDT);
                        dst->remove(i->first, i->second);
                        //Okay if we are blown by exception
                        if (restoreDT)
                            UtilityXAP::SetDateTime(dst, XAP_NS_XAP, XAPTK_METADATA_DATE, oldDT);
                        CopyTimestamp(src, dst, i->first, i->second);
                        isMerged = true;
                    }
                }
                conflicts->addPath(i->first, i->second);
                break;
            case xap_policy_oldest:
                if (cmp == xap_before) { // src older than dst
                    if (!justCheck) {
#if defined(XAP_DEBUG_CERR) && 0
                        cerr << "remove src older than dst." << endl;
#endif
                        restoreDT = UtilityXAP::GetDateTime(dst, XAP_NS_XAP, XAPTK_METADATA_DATE, oldDT);
                        dst->remove(i->first, i->second);
                        //Okay if we are blown by exception
                        if (restoreDT)
                            UtilityXAP::SetDateTime(dst, XAP_NS_XAP, XAPTK_METADATA_DATE, oldDT);
                        CopyTimestamp(src, dst, i->first, i->second);
                        isMerged = true;
                    }
                }
                conflicts->addPath(i->first, i->second);
                break;
            case xap_policy_dont_merge:
            case xap_policy_ask_user:
                //Just record the conflict
                conflicts->addPath(i->first, i->second);
                break;
            default:
                continue;
        }
    } // for
  } catch (...) {
    a->enable(XAP_OPTION_ALIASING_ON, aAliased);
    b->enable(XAP_OPTION_ALIASING_ON, bAliased);
    delete conflicts;
    if ( ! justCheck ) delete dst;
    throw;
  }
    a->enable(XAP_OPTION_ALIASING_ON, aAliased);
    b->enable(XAP_OPTION_ALIASING_ON, bAliased);

    //Finish up
    if (justCheck && dontMergeResult != NULL) {
        if (!srcAltStamp) {
            *dontMergeResult = xap_noTime;
            return(NULL);
        }
        if (!dstAltStamp) {
            *dontMergeResult = xap_noTime;
            return(NULL);
        }
        *dontMergeResult = XAPTk::CompareDateTimes(srcAlt, dstAlt);
        /*
        Don't delete dst because it is the same pointer as a or b.
        */
    }

    if (!justCheck) {
        //Return the merged metadata?
        if (merge != NULL)
            *merge = dst;
        else
            delete dst;
    }

    if (conflicts->m_props.size() == 0
      || (justCheck
          && (policy == xap_policy_dont_merge
            || policy == xap_policy_ask_user))) {
        delete conflicts;
        return(NULL);
    } else
        return(conflicts);
}

#if 0	// *** Disable
DECL_EXPORT void
MetaXAP::ParseAliases(const char* xmlbuf, const size_t n, const bool last /*= false*/) {
    /*
    Parse RDF which represents the alias definitions copied from a
    master schema file.  The properties are arranged as follows:

    a:aliases   bag of:

    a:ns            URI     Namespace of alias
    a:path          XPath   Path of alias
    a:actualNS      URI     Namespace of actual
    a:actualPath    XPath   Path of actual
    a:sct           Choice  {Alt,Bag,Seq} omitted if not container

    Create a MetaXAP to hold these properties, count the number of aliases,
    then iterate over them, calling SetAlias for each entry.  We ignore
    individual errors, and always force an override if the alias already
    exists.

    Since this function may be called multiple times for a single RDF
    serialization, we need to keep static state.
    */

    static MetaXAP* S_as = NULL;
    static bool flushToEnd = false;

    /*
    If an error occurred in a previous call, don't process any
    more XML.  Reset when last buffer is encountered.
    */
    if (flushToEnd) {
        if (last)
            flushToEnd = false;
        return;
    }

    try {
        if (S_as == NULL) {
            S_as = new XMP_Debug_new MetaXAP();
            S_as->enable(XAP_OPTION_XAPMETA_ONLY, false);
            S_as->enable(XAP_OPTION_ALIASING_ON, false);
        }
        S_as->parse(xmlbuf, n, last);
    } catch (...) {
        flushToEnd = true;
        delete S_as;
        S_as = NULL;
        throw;
    }

    if (!last)
        return;  // get more later

#ifdef XAP_DEBUG_CERR
    XAPPaths* p = S_as->enumerate();
    while (p->hasMorePaths()) {
        string a;
        string b;
        p->nextPath(a, b);
        cerr << "[" << b << ", " << a << "]" << endl;
        bool here = true;
    }
    delete p;
#endif

    try {
        const string APROP("aliases/*");
        const string NS_F_ALIAS(XAPTK_NS_F_ALIAS);
        size_t an = S_as->count(NS_F_ALIAS, APROP);
        string ad; // constructed path to alias def by ordinal
        string fp; // constructed path for specific field
        string sOrd; // buffer for string rep of ordinal
        string ns; // namespace of alias
        string path; // path of alias
        string actualNS; // namespace of actual
        string actualPath; // path of actual
        XAPStructContainerType sct; // of actual
        string tmp1;
        string tmp2;
        XAPStructContainerType tmp3;
        XAPFeatures f;

        for (size_t ord = 1; ord <= an; ++ord) {
            try {
                sct = xap_sct_unknown;
                // Construct path to alias def by ordinal
                ad = APROP;
                ad.append("[");
                XAPTk::ToString(ord, sOrd);
                ad.append(sOrd);
                ad.append("]");

                // Get alias ns
                fp = ad;
                fp.append("/ns");
                if (!S_as->get(NS_F_ALIAS, fp, ns, f))
                    continue;

                // Get alias path
                fp = ad;
                fp.append("/path");
                if (!S_as->get(NS_F_ALIAS, fp, path, f))
                    continue;

                // Get actual ns
                fp = ad;
                fp.append("/actualNS");
                if (!S_as->get(NS_F_ALIAS, fp, actualNS, f))
                    continue;

                // Get actual path
                fp = ad;
                fp.append("/actualPath");
                if (!S_as->get(NS_F_ALIAS, fp, actualPath, f))
                    continue;

                // Get sct, okay if not present
                fp = ad;
                fp.append("/sct");
                if (S_as->get(NS_F_ALIAS, fp, tmp1, f)) {
                    if (tmp1 == "Alt") {
                        sct = xap_alt;
                    } else if (tmp1 == "Bag") {
                        sct = xap_bag;
                    } else if (tmp1 == "Seq") {
                        sct = xap_seq;
                    }
                }

                // If alias already defined, remove it
                tmp1 = "";
                if (MetaXAP::GetAlias(ns, path, tmp1, tmp2, tmp3)) {
                    if (tmp1 == actualNS && tmp2 == actualPath && tmp3 == sct)
                        continue;  // Just the same def, no problem
                    // Otherwise, remove old definition
                    MetaXAP::RemoveAlias(ns, path);
                }

                // Define it
                try {
                    MetaXAP::SetAlias(ns, path, actualNS, actualPath, sct);
                } catch (...) {
                    // Restore original, if needed
                    if (!tmp1.empty()) {
                        MetaXAP::SetAlias(ns, path, tmp1, tmp2, tmp3);
                    }
                    throw;
                }
            } catch (...) {
                continue; // Skip this entry
            }
        } // for
    } catch (...) {
        delete S_as;
        S_as = NULL;
    }
    delete S_as;
    S_as = NULL;
}
#endif


DECL_EXPORT void
MetaXAP::RegisterNamespace ( const string& nsName, const string& suggestedPrefix ) {

	XMP_CheckToolkitInit();
    if ( MetaXAP_nsMap == 0 ) MetaXAP_InitNSMap();

	if ( nsName.empty() && suggestedPrefix.empty() ) return;	// Ignore resetting default namespace to be empty.

    /*
    GOTCHA: The nsName and suggestPrefix should be filtered, but here
    they are just used without any checking.  This may lead to XML that
    is not well-formed (suggestedPrefix has bogus character), or to URI's
    that have bogus characters.
    */

    /*
    Generate sequential prefix if suggestion is empty or matches existing
    registered name.
    */

    bool needGen = false;
    long counter = 0;
    string prefix;

    XAPTk_StringByString::const_iterator i;
    XAPTk_StringByString::const_iterator done = MetaXAP_nsMap->end();
    
    // First see if the namespace is already registered.
    
    #if 0
    	cout << "In MetaXAP::RegisterNamespace for " << suggestedPrefix << " = " << nsName << endl;
    #endif
    
    if ( suggestedPrefix.empty() ) {
    	needGen = true;
    } else {
    	i = MetaXAP_nsMap->find ( suggestedPrefix );
    	if ( i != done ) {
    		if ( i->second == nsName ) return;	// Already registered with the suggested prefix.
    		needGen = true;	// This prefix is used with some other namespace URL.
    	}
    }

    for ( i = MetaXAP_nsMap->begin(); i != done; ++i ) {
        if ( i->second == nsName ) return;	// Already registered with some other prefix.
    }

	// The namespace URL is not registered, make sure it has a unique prefix.
	
    prefix = suggestedPrefix;
	while ( needGen ) {
        XAPTk::ToString ( ++counter, prefix );
        prefix = suggestedPrefix + "_" + prefix;
        i = MetaXAP_nsMap->find ( prefix );
        if ( i == done ) needGen = false;
    }

    (*MetaXAP_nsMap)[prefix] = nsName;
    #if 0
    	cout << "   registered as " << prefix << endl;
    #endif

}


DECL_EXPORT void
MetaXAP::GetNamespacePrefix ( const std::string& nsName, std::string& prefix )
{

    XAPTk_StringByString::const_iterator i;
    XAPTk_StringByString::const_iterator done = MetaXAP_nsMap->end();

    for ( i = MetaXAP_nsMap->begin(); i != done; ++i ) {
        if ( i->second == nsName ) {
        	prefix = i->first;
        	return;
        }
    }
    
    prefix = "";

}


DECL_EXPORT void
MetaXAP::GetNamespaceURL ( const std::string& prefix, std::string& nsName )
{

    XAPTk_StringByString::const_iterator i;
    XAPTk_StringByString::const_iterator done = MetaXAP_nsMap->end();

	i = MetaXAP_nsMap->find ( prefix );
	if ( i == done ) {
	    nsName = "";
	} else {
		nsName = i->second;
	}

}


DECL_EXPORT void
MetaXAP::RemoveAlias(const std::string& aliasNS, const std::string& aliasPath) {
    /*
    Each property, whether an alias or an actual, has a single entry in
    the MetaXAP_ResolvedAliases map.  The value of the entry is a pool
    of linked properties, inclusive of the entry itself (e.g., if "foo"
    is linked to "bar" and "bletch", the pool contains "foo", "bar" and
    "bletch").  MetaXAP_InfoMap defines the pool.  The names in the pool
    may be adjusted according to the flavor of the alias.  See PreResolveAlias.

    To remove an alias, we first lookup the entry in MetaXAP_ResolvedAliases.
    Using the MetaXAP_InfoMap result, we iterate over each pool member,
    which gives us a new set of target entries.  We look up each target
    entry in MetaXAP_ResolvedAliases.  For each MetaXAP_InfoMap result,
    we find and remove the original alias property.  If the MetaXAP_InfoMap
    result is empty (only contains itself), we remove the target entry from
    MetaXAP_ResolvedAliases, and we also remove the property from the original
    MetaXAP_InfoMap pool.

    Finally, we remove the original alias entry from MetaXAP_ResolvedAliases,
    and from MetaXAP_AliasMap.
    */

    // Prepare parameters, check pre-conditions
    string fixed;
    string var;

	XMP_CheckToolkitInit();
	
    XAPTk::ConformPath(aliasPath, fixed, var);
    if (fixed.empty()) {
        throw xap_bad_path ();
    }
    const XAPTk_PairOfString alias(aliasNS, fixed);
    if (MetaXAP_aliasMap->find(alias) == MetaXAP_aliasMap->end())
        return; // Not found, no harm done

#if defined (XAP_DEBUG_CERR) && 0
    // Before state of maps
    if (MetaXAP_aliasMap->size() > 0) {
        cerr << "BEFORE:" << endl;
        MetaXAP_ResolvedAliases::const_iterator n = MetaXAP_resolvedAliases->begin();
        MetaXAP_ResolvedAliases::const_iterator nDone = MetaXAP_resolvedAliases->end();
        MetaXAP_InfoMap::const_iterator m;
        MetaXAP_InfoMap::const_iterator mDone;
        for (; n != nDone; ++n) {
            cerr << n->first.second << ", " << n->first.first << ":" << endl;
            m = n->second.begin();
            mDone = n->second.end();
            for (; m != mDone; ++m) {
                cerr << "  " << m->first.second << "(" ;
                const MetaXAP_AliasInfo& inf = m->second;
                if (inf.actual)
                    cerr << "A";
                if (inf.aliasSingle)
                    cerr << "s";
                if (inf.actualFirst)
                    cerr << "1";
                if (inf.actualLang)
                    cerr << "L";
                switch (inf.cType) {
                    case xap_alt: cerr << ":alt"; break;
                    case xap_bag: cerr << ":bag"; break;
                    case xap_seq: cerr << ":seq"; break;
                    default: cerr << "?"; break;
                }
                cerr << "), " << m->first.first << endl;
            }
        int xxx = 1;
        }
        cerr << "=======================" << endl;
    }
#endif

    // Look up the original entry, get the original pool of props
    MetaXAP_ResolvedAliases::iterator origEntry = MetaXAP_resolvedAliases->find(alias);
    assert(origEntry != MetaXAP_resolvedAliases->end());
    MetaXAP_InfoMap& origPool = origEntry->second;

    // Iterate over the target entries
    MetaXAP_InfoMap::iterator target = origPool.begin();
    for(; target != origPool.end(); ++target) {
        // For each target, look up the pool of props
        XAPTk::ConformPath(target->first.second, fixed, var);
        const XAPTk_PairOfString targetName(target->first.first, fixed);
        if (targetName == alias)
            continue; // Skip self
        MetaXAP_ResolvedAliases::iterator tEntry = MetaXAP_resolvedAliases->find(targetName);
        assert(tEntry != MetaXAP_resolvedAliases->end());
        MetaXAP_InfoMap& targetPool = tEntry->second;
        // Remove the original property (back pointer)
        targetPool.erase(alias);
        if (targetPool.size() == 1) {
            // Also remove the target entry from the resolved aliases map
            MetaXAP_resolvedAliases->erase(targetName);
        }
    }

    // Remove original alias
    MetaXAP_resolvedAliases->erase(alias);
    MetaXAP_aliasMap->erase(alias);

#if defined (XAP_DEBUG_CERR) && 0
    // Before state of maps
    if (MetaXAP_aliasMap->size() > 0) {
        cerr << "AFTER:" << endl;
        MetaXAP_ResolvedAliases::const_iterator n = MetaXAP_resolvedAliases->begin();
        MetaXAP_ResolvedAliases::const_iterator nDone = MetaXAP_resolvedAliases->end();
        MetaXAP_InfoMap::const_iterator m;
        MetaXAP_InfoMap::const_iterator mDone;
        for (; n != nDone; ++n) {
            cerr << n->first.second << ", " << n->first.first << ":" << endl;
            m = n->second.begin();
            mDone = n->second.end();
            for (; m != mDone; ++m) {
                cerr << "  " << m->first.second << "(" ;
                const MetaXAP_AliasInfo& inf = m->second;
                if (inf.actual)
                    cerr << "A";
                if (inf.aliasSingle)
                    cerr << "s";
                if (inf.actualFirst)
                    cerr << "1";
                if (inf.actualLang)
                    cerr << "L";
                switch (inf.cType) {
                    case xap_alt: cerr << ":alt"; break;
                    case xap_bag: cerr << ":bag"; break;
                    case xap_seq: cerr << ":seq"; break;
                    default: cerr << "?"; break;
                }
                cerr << "), " << m->first.first << endl;
            }
        int xxx = 1;
        }
        cerr << "=======================" << endl;
    }
#endif
}


static void
setAlias(const string& nsAlias, const string& pAlias, const string& nsActual, const string& pActual, const XAPStructContainerType cType) {
    /* [first: alias, second: actual] */
    XAPTk_PairOfString alias(nsAlias, pAlias);
    MetaXAP::Actual actual;
    actual.ns = nsActual;
    actual.path = pActual;
    actual.cType = cType;
    (*MetaXAP_aliasMap)[alias] = actual;
    PreResolveAlias(alias, actual);
}


DECL_EXPORT void
MetaXAP::SetStandardAliases ( const string & aliasNS )
{
	const bool	doAll	= (aliasNS == "");

	if ( doAll || (aliasNS == XAP_NS_PDF) ) {
		// Aliases from PDF to DC and XMP.
	    setAlias ( XAP_NS_PDF, "Author",       XAP_NS_DC, "creator/*[1]", xap_seq );
	    setAlias ( XAP_NS_PDF, "BaseURL",      XAP_NS_XAP, "BaseURL", xap_sct_unknown );
	    setAlias ( XAP_NS_PDF, "CreationDate", XAP_NS_XAP, "CreateDate", xap_sct_unknown );
	    setAlias ( XAP_NS_PDF, "Creator",      XAP_NS_XAP, "CreatorTool", xap_sct_unknown );
	    setAlias ( XAP_NS_PDF, "ModDate",      XAP_NS_XAP, "ModifyDate", xap_sct_unknown );
	    setAlias ( XAP_NS_PDF, "Subject",      XAP_NS_DC,  "description/*[@xml:lang='x-default']", xap_alt );
	    setAlias ( XAP_NS_PDF, "Title",        XAP_NS_DC,  "title/*[@xml:lang='x-default']", xap_alt );
    }
    
	if ( doAll || (aliasNS == XAP_NS_PHOTOSHOP) ) {
	    // Aliases from PHOTOSHOP to DC and XMP.
	    setAlias ( XAP_NS_PHOTOSHOP, "Author",       XAP_NS_DC, "creator/*[1]", xap_seq );
	    setAlias ( XAP_NS_PHOTOSHOP, "Caption",      XAP_NS_DC, "description/*[@xml:lang='x-default']", xap_alt );
	    setAlias ( XAP_NS_PHOTOSHOP, "Copyright",    XAP_NS_DC, "rights/*[@xml:lang='x-default']", xap_alt );
	    // *** bug found by Photoshop?	setAlias ( XAP_NS_PHOTOSHOP, "JobName",      XAP_NS_XAP_BJ, "JobRef/*[1]/stJob:name", xap_sct_unknown );
	    setAlias ( XAP_NS_PHOTOSHOP, "Keywords",     XAP_NS_DC, "subject", xap_bag );
	    setAlias ( XAP_NS_PHOTOSHOP, "Marked",       XAP_NS_XAP_RIGHTS, "Marked", xap_sct_unknown );
	    setAlias ( XAP_NS_PHOTOSHOP, "Title",        XAP_NS_DC, "title/*[@xml:lang='x-default']", xap_alt );
	    setAlias ( XAP_NS_PHOTOSHOP, "WebStatement", XAP_NS_XAP_RIGHTS, "WebStatement", xap_sct_unknown );
    }
	    
	if ( doAll || (aliasNS == XAP_NS_TIFF) || (aliasNS == XAP_NS_EXIF) ) {
	    // Aliases from TIFF and EXIF to DC and XMP.
	    setAlias ( XAP_NS_TIFF, "Artist",           XAP_NS_DC, "creator/*[1]", xap_seq );
	    setAlias ( XAP_NS_TIFF, "Copyright",        XAP_NS_DC, "rights", xap_alt );
	    setAlias ( XAP_NS_TIFF, "DateTime",         XAP_NS_XAP, "CreateDate", xap_sct_unknown );
	    setAlias ( XAP_NS_TIFF, "ImageDescription", XAP_NS_DC, "title", xap_alt );
	    setAlias ( XAP_NS_TIFF, "Software",         XAP_NS_XAP, "CreatorTool", xap_sct_unknown );
	    setAlias ( XAP_NS_EXIF, "UserComment",      XAP_NS_DC, "description", xap_alt );
    }

}


DECL_EXPORT void
MetaXAP::SetAlias(const string& aliasNS, const string& aliasPath,
         const string& actualNS, const string& actualPath,
         const XAPStructContainerType cType /*= xap_sct_unknown */) {
    MetaXAP_InitAliasMap();
    /*
    Although the API describes an alias model, where a single actual
    property can have multiple names, with certain constraints, such
    as no actual may be an alias of another actual, the underlying
    implementation is a less constrainted linked value model. In a linked
    value model, no property need be distinguished as "actual" or not,
    all property names and values are kept in a common pool. Therefore,
    we provide additional data structures, such as MetaXAP_aliasMap, to
    provide the additional constraints for the API specification.
    */
    // First make sure that the actual is not itself an alias.
    string a;
    string b;
    XAPStructContainerType sct;

	XMP_CheckToolkitInit();
	
    if (MetaXAP::GetAlias(actualNS, actualPath, a, b, sct)) {
        throw xap_bad_path ();
    }
    /*
    Second make sure that the alias is a clean container name or simple
    property (contains no variable part in the path).
    */
    string fixed;
    string var;
    XAPTk::ConformPath(aliasPath, fixed, var);
    if (!var.empty()) {
        throw xap_bad_path ();
    }

    /*
    Third make sure that the alias is not the actual of another alias.
    */
    XAPTk_PairOfString alias(aliasNS, fixed);
    MetaXAP_AliasMap::const_iterator done = MetaXAP_aliasMap->end();
    MetaXAP_AliasMap::const_iterator i = MetaXAP_aliasMap->begin();
    for (; i != done; ++i) {
        /* [first: alias, second: actual] */
        if (aliasNS == i->second.ns && aliasPath == i->second.path) {
            throw xap_bad_path ();
        }
    }
    // Good to go!
    MetaXAP::Actual actual;
    actual.ns = actualNS;
    actual.path = actualPath;
    actual.cType = cType;
    (*MetaXAP_aliasMap)[alias] = actual;

    PreResolveAlias(alias, actual);
}



/* ====== Constructors ===== */

    /* Default */
DECL_EXPORT MetaXAP::MetaXAP() : m_data(NULL) {

	XMP_CheckToolkitInit();
	
    m_data = XAPTk_Data::New(this);
    if (m_data == NULL) {
        throw bad_alloc ();
    }
}

    /* Construct empty, specify a clock for timestamps. */
DECL_EXPORT 
MetaXAP::MetaXAP(XAPClock* clock) {
    XAPDateTime dt;

	XMP_CheckToolkitInit();
	
    clock->timestamp(dt);
    if (dt.tzHour != 0 || dt.tzMin != 0) {
        throw xap_bad_number ();
    }
    m_data = XAPTk_Data::New(this);
    if (m_data == NULL) {
        throw bad_alloc ();
    }
    m_data->m_clock = clock;
}

    /* Initialize with XML */
DECL_EXPORT MetaXAP::MetaXAP(const char* xmlbuf, const long int len, const long int opt /*= XAP_OPTION_DEFAULT*/, XAPClock* clock /*=NULL*/) {

	XMP_CheckToolkitInit();
	
    m_data = XAPTk_Data::New(this);
    if (m_data == NULL) {
        throw bad_alloc ();
    }
    m_data->m_clock = clock; // NULL okay
    m_data->m_options = opt & XAPTk::OPTION_MASK;
    this->parse(xmlbuf, len, true); // all in 1 buffer
}

/* ===== Destructor ===== */

DECL_EXPORT
MetaXAP::~MetaXAP() {
    assert(m_data != NULL);
    try {
        delete m_data;
    } catch (...) {}
}


/* ===== Public Member Functions ===== */

DECL_EXPORT void
MetaXAP::append(const string& _ns, const string& _path, const string& value, const bool inFront /*= false*/, const XAPFeatures f /*= XAP_FEATURE_DEFAULT*/) {
    ValidateFeatures(f);
    ValidateNS(_ns);
    XAPTk::VectorOfProps props;
    const bool ckAlias = MetaXAP_CollectAliases(this, _ns, _path, props, true);

    string filtered;
    const string* val = &value;

    if (XAPTk::CDATAToRaw(value, filtered))
        val = &filtered;

    if (!ckAlias) {
        m_data->append(_ns, _path, *val, inFront, f);
    } else {
        XAPTk::VectorOfProps::const_iterator i = props.begin();
        XAPTk::VectorOfProps::const_iterator done = props.end();
        for (; i != done; ++i) {
            try {
                m_data->append(i->first, i->second, *val, inFront, f);
            } catch (...) {
                // Only throw for original path, suppress for aliases
                if (i->first == _ns && i->second == _path) {
                    throw;
                }
            }
        }
    }
}

DECL_EXPORT size_t
MetaXAP::count(const string& _ns, const string& _path) const {
    string::const_reverse_iterator r = _path.rbegin();
    if (*r != '*') {
        throw xap_bad_path ();
    }
    return m_data->count(_ns, _path);
}

DECL_EXPORT void
MetaXAP::createFirstItem(const string& _ns, const string& _path, const string& value, const XAPStructContainerType type /*= xap_bag*/, const XAPFeatures f /*= XAP_FEATURES_DEFAULT*/) {
    ValidateFeatures(f);
    ValidateNS(_ns);
    XAPTk::VectorOfProps props;
    const bool ckAlias = MetaXAP_CollectAliases(this, _ns, _path, props, true);

    string filtered;
    const string* val = &value;

    if (XAPTk::CDATAToRaw(value, filtered))
        val = &filtered;

    if (!ckAlias) {
        m_data->createFirstItem(_ns, _path, *val, type, NULL, NULL, false, f);
    } else {
        XAPTk::VectorOfProps::const_iterator i = props.begin();
        XAPTk::VectorOfProps::const_iterator done = props.end();
        for (; i != done; ++i) {
            try {
                m_data->createFirstItem(i->first, i->second, *val, type, NULL, NULL, false, f);
            } catch (...) {
                // Only throw for original path, suppress for aliases
                if (i->first == _ns && i->second == _path) {
                    throw;
                }
            }
        }
        /*
        Sigh, in order to guarantee that alias targets that are single get set,
        we have to call set, which redundantly sets the non-single targets. We only
        do this if there is an alias target that is single, but we have to search
        for such in the resolved aliases map.
        */
        i = props.begin();
        MetaXAP_ResolvedAliases::const_iterator j;
        MetaXAP_InfoMap::const_iterator k;
        string fixedPath;
        string varPart;
        XAPTk::ConformPath(i->second, fixedPath, varPart);
        XAPTk_PairOfString test(i->first, fixedPath);
        j = MetaXAP_resolvedAliases->find(test);
        if (j != MetaXAP_resolvedAliases->end()) {
            const MetaXAP_InfoMap& iMap = j->second;
            for (k = iMap.begin(); k != iMap.end(); ++k) {
                const MetaXAP_AliasInfo& info = k->second;
                if (info.aliasSingle && !info.actual) {
                    string dup(_path);
                    dup.append("/*[1]");
                    try {
                        this->set(_ns, dup, *val, f);
                    } catch (...) {
                        ; // Don't throw for aliases
                    }
                    break;
                }
            }
        }
    }
}

DECL_EXPORT void
MetaXAP::createFirstItem(const string& _ns, const string& _path, const string& value, const string& selectorName, const string& selectorVal, const bool isAttr /*=true*/, const XAPFeatures f /*= XAP_FEATURES_DEFAULT*/) {
    ValidateFeatures(f);
    ValidateNS(_ns);
    XAPTk::VectorOfProps props;
    const bool ckAlias = MetaXAP_CollectAliases(this, _ns, _path, props, true);

    string filtered;
    const string* val = &value;

    if (XAPTk::CDATAToRaw(value, filtered))
        val = &filtered;

    if (!ckAlias) {
        m_data->createFirstItem(_ns, _path, *val, xap_alt, &selectorName, &selectorVal, isAttr, f);
    } else {
        XAPTk::VectorOfProps::const_iterator i = props.begin();
        XAPTk::VectorOfProps::const_iterator done = props.end();
        for (; i != done; ++i) {
            try {
                m_data->createFirstItem(i->first, i->second, *val, xap_alt, &selectorName, &selectorVal, isAttr, f);
            } catch (...) {
                // Only throw for original path, suppress for aliases
                if (i->first == _ns && i->second == _path) {
                    throw;
                }
            }
        }
        /*
        Sigh, in order to guarantee that alias targets that are single get set,
        we have to call set, which redundantly sets the non-single targets. We only
        do this if there is an alias target that is single, but we have to search
        for such in the resolved aliases map.
        */
        i = props.begin();
        MetaXAP_ResolvedAliases::const_iterator j;
        MetaXAP_InfoMap::const_iterator k;
        string fixedPath;
        string varPart;
        XAPTk::ConformPath(i->second, fixedPath, varPart);
        XAPTk_PairOfString test(i->first, fixedPath);
        j = MetaXAP_resolvedAliases->find(test);
        if (j != MetaXAP_resolvedAliases->end()) {
            const MetaXAP_InfoMap& iMap = j->second;
            for (k = iMap.begin(); k != iMap.end(); ++k) {
                const MetaXAP_AliasInfo& info = k->second;
                if (info.aliasSingle && !info.actual) {
                    string dup(_path);
                    dup.append("/*[@");
                    dup.append(selectorName);
                    dup.append("='");
                    dup.append(selectorVal);
                    dup.append("']");
                    try {
                        this->set(_ns, dup, *val, f);
                    } catch (...) {
                        ; // Don't throw for aliases
                    }
                    break;
                }
            }
        }
    }
}

DECL_EXPORT void
MetaXAP::enable ( const long int opt, const bool pleaseEnable ) throw ()
{
    long int good = (opt & XAPTk::OPTION_MASK);

    // Pre-process
    if ( pleaseEnable && (good & XAP_OPTION_ALIASING_ON) &&
   		 (! (m_data->m_options & XAP_OPTION_ALIASING_ON)) ) {

        /*
        Transition from aliasing off to aliasing on requires verification
        of the metadata, to make sure that aliased names that have values
        don't have different values.  If metadata checks out okay, populate
        the linked values for all the aliased names.  Disable aliasing
        if verification fails.
        */

		// *** [462404] The parsing code turns aliasing off then back on to get the
		// *** aliases populated.  It is not good to silently leave aliasing off if
		// *** there are inconsistent aliases, better to reconcile the values.  For
		// *** now this is by calling VerifyAndPopulate and populate twice, squashing
		// *** differences both times.  The first call makes sure the base of the
		// *** alias chain (the actual) has a value, the second call propagates it.
		
		(void) VerifyAndPopulate ( this, true );
        if ( ! VerifyAndPopulate ( this, true ) ) good &= ~(XAP_OPTION_ALIASING_ON);

    }

    if ( pleaseEnable ) {
        m_data->m_options |= good;
    } else {
        m_data->m_options &= ~good;
    }

    // No Post-process

}

DECL_EXPORT XAPPaths*
MetaXAP::enumerate(const int steps /*=0*/) {
    XAPPaths* list = m_data->enumerate(NULL, NULL, steps);
    return(list);
}

DECL_EXPORT XAPPaths*
MetaXAP::enumerate(const string& _ns, const string& _subPath, const int steps /*=0*/) {
    XAPPaths* list = m_data->enumerate(&_ns, &_subPath, steps);
    return(list);
}

DECL_EXPORT XAPPaths*
MetaXAP::enumerate(const XAPTimeRelOp op, const XAPDateTime& dt, const XAPChangeBits how /*= XAP_CHANGE_MASK*/) {
    assert(op != xap_notDef);
    if (op == xap_notDef)
        return(NULL);

    XAPPaths* list = m_data->enumerate(op, dt, how);
    return(list);
}

DECL_EXPORT bool
MetaXAP::get(const string& _ns, const string& _path, string& val, XAPFeatures& f) const {
    /*
    We catch all the XAP exceptions here and turn them into "not found".
    Clients should use getForm() if they want to get the exceptions instead.
    Clients who are debugging can set breakpoints in the catch clauses to
    see if a "false" really means not found, or was an error.
    Notice that other exceptions, like bad_alloc and assert() violations
    still get through.
    */
    bool found;
    try {
        found = m_data->get(_ns, _path, val, f);
    } catch (xap_error&) {
        found = false;
    } catch (xap_client_fault&) {
        found = false;
    }
    return(found);
}

DECL_EXPORT XAPStructContainerType
MetaXAP::getContainerType(const string& _ns, const string& _path) const {
    return m_data->getContainerType(_ns, _path);
}

DECL_EXPORT XAPValForm
MetaXAP::getForm(const string& _ns, const string& _path) const {
    return m_data->getForm(_ns, _path);
}

DECL_EXPORT void
MetaXAP::getResourceRef(string& resRef) const {
    // Return text in raw form
    resRef = m_data->m_about;
}

DECL_EXPORT bool
MetaXAP::getTimestamp(const string& _ns, const string& _path, XAPDateTime& dt, XAPChangeBits& how) const {
    return m_data->getTimestamp(_ns, _path, dt, how);
}

DECL_EXPORT bool
MetaXAP::isEnabled(const long int opt) const
    throw () {
    return(((opt & XAPTk::OPTION_MASK) & m_data->m_options) != 0);
}


DECL_EXPORT void
MetaXAP::parse ( const char* xmlbuf, const size_t n, const bool last /*= false*/ ) {

    assert(xmlbuf != NULL);
    m_data->parse(xmlbuf, n, last);

    if ( last ) {
    
    	#if 0
    	{
		    XAPTk_StringByString::const_iterator regNS;
		    cout << endl << "Registered namespace map @ 0x" << ios::hex << (int)MetaXAP_nsMap << ios::dec << endl;
		    for ( regNS = MetaXAP_nsMap->begin(); regNS != MetaXAP_nsMap->end(); ++regNS ) {
		    	cout << '\t' << regNS->first << ":\t " << regNS->second << endl;
		    }
		}
    	#endif

        m_data->loadFromTree();

        // Optionally, cause all aliased values to be duplicated
        if ( this->isEnabled ( XAP_OPTION_ALIASING_ON ) ) {
            // The transition from aliasing off to on forces a verify
            // and populate of the aliases in the metadata.
            this->enable ( XAP_OPTION_ALIASING_ON, false );
            this->enable ( XAP_OPTION_ALIASING_ON, true );
        }

    }

}

DECL_EXPORT void
MetaXAP::purgeTimestamps(const XAPChangeBits how /*= XAP_CHANGE_REMOVED*/, const XAPDateTime* dt /*= NULL*/) {
    m_data->purgeTimestamps(how, dt);
}


DECL_EXPORT void
MetaXAP::remove ( const string & _ns, const string & _path )
{
    XAPTk::VectorOfProps props;
    const bool haveAliases = MetaXAP_CollectAliases ( this, _ns, _path, props, false );

    if ( ! haveAliases ) {

        m_data->remove ( _ns, _path );

    } else {
    
    	// =========================================================================================
    	// V477166:  Gack!  Another problem from replicated aliases!  Suppose we have two pairs of
    	// aliases, A -> B[1], and B -> C, and then we try to remove C.  MetaXAP_CollectAliases will
    	// only return B and C, it won't detect A!  If all references translated down to the base
    	// and there was only one copy of the value this would not be a problem.  Sigh.
    	//
    	// As a hack for now we'll check if the property is a container.  If it is, we'll try to
    	// remove the first item, or the x-default item for an alt-by-lang container.  We have to do
    	// this for every top level alias.
		
        XAPTk::VectorOfProps::const_iterator i = props.begin();
        XAPTk::VectorOfProps::const_iterator done = props.end();
        
        bool	isContainer	= false;
        bool	isAlt		= false;
        
        try {
        	if ( this->getForm ( _ns, _path ) == xap_container ) {
        		isContainer = true;
	        	if ( this->getContainerType ( _ns, _path ) == xap_alt ) isAlt = true;
	        }
        } catch ( ... ) { /* Do nothing */ }

        for ( ; i != done; ++i ) {
        
        	try {
				if ( isContainer ) this->remove ( _ns, _path + "/*[1]" );
			} catch ( ... ) { /* Do nothing */ }
        
        	try {
				if ( isAlt ) this->remove ( _ns, _path + "/*[@xml:lang='x-default']" );
			} catch ( ... ) { /* Do nothing */ }

            try {
                m_data->remove ( i->first, i->second );
            } catch ( ... ) {
                // Only throw for original path, suppress for aliases.
                if ( (i->first == _ns) && (i->second == _path) ) throw;
            }

        }

    }

}


// ============================================================================
// *** Rather a heavy hammer, but the quick and easy way to limit alias output.

static void UpdateAliasSubtree ( MetaXAP* meta, XAPTk_Data* m_data, string& rootNS, string& rootPath )
{
	string	aliasNS, aliasPart, aliasPath, actualNS, actualPath, value;
	XAPStructContainerType	sct;

	XPElement *	elem	= NULL;
	XAPPaths *	paths	= NULL;

	paths = meta->enumerate ( rootNS, rootPath, 1 );
	if ( paths == NULL ) return;
	
	while ( paths->hasMorePaths() ) {
	
		paths->nextPath ( aliasNS, aliasPart );
		aliasPath = rootPath + '/' + aliasPart;
		// cout << "Checking alias for " << aliasPath << endl;
		
		if ( MetaXAP::GetAlias ( aliasNS, aliasPath, actualNS, actualPath, sct )  ) {

			elem = dynamic_cast<XPElement*> (m_data->evalXPath ( aliasNS, aliasPath ));
			// *** assert ( elem != NULL );		see V442300
			if ( elem != NULL ) elem->setIsAnAlias();

			elem = dynamic_cast<XPElement*> (m_data->evalXPath ( actualNS, actualPath ));
			// *** assert ( elem != NULL );		see V442300
			if ( elem != NULL ) elem->setHasAliases();

		}
		
		UpdateAliasSubtree ( meta, m_data, aliasNS, aliasPath );

	}

	delete paths;
   
}	// UpdateAliasSubtree

static void UpdateAliasFlags ( MetaXAP* meta, XAPTk_Data* m_data )
{
	string	aliasNS, aliasPath, actualNS, actualPath, value;
	XAPStructContainerType	sct;

	XPElement *	elem	= NULL;
	XAPPaths *	paths	= NULL;

	paths = meta->enumerate ( 1 );
	if ( paths == NULL ) return;
	
	while ( paths->hasMorePaths() ) {
	
		paths->nextPath ( aliasNS, aliasPath );
		// cout << "Checking alias for " << aliasPath << endl;
		
		if ( MetaXAP::GetAlias ( aliasNS, aliasPath, actualNS, actualPath, sct )  ) {

			elem = dynamic_cast<XPElement*> (m_data->evalXPath ( aliasNS, aliasPath ));
			// *** assert ( elem != NULL );		see V442300
			if ( elem != NULL ) elem->setIsAnAlias();

			elem = dynamic_cast<XPElement*> (m_data->evalXPath ( actualNS, actualPath ));
			// *** assert ( elem != NULL );		see V442300
			if ( elem != NULL ) elem->setHasAliases();
			
		}
	
		UpdateAliasSubtree ( meta, m_data, aliasNS, aliasPath );

	}

	delete paths;
   
}	// UpdateAliasFlags ()


DECL_EXPORT size_t
MetaXAP::serialize(const XAPFormatType f /*=xap_format_pretty*/, const int escnl /*= XAP_ESCAPE_CR*/) {
	UpdateAliasFlags ( this, m_data );
    return m_data->serialize(f, escnl);
}

DECL_EXPORT size_t
MetaXAP::extractSerialization(char* buf, const size_t nmax) {
    assert(buf != NULL);
    assert(nmax > 0);
	UpdateAliasFlags ( this, m_data );
    return m_data->extractSerialization(buf, nmax);
}


DECL_EXPORT void
MetaXAP::set ( const string& _ns, const string& _path, const string& value, const XAPFeatures f /*= XAP_FEATURE_DEFAULT*/ )
{

    ValidateFeatures ( f );
    ValidateNS ( _ns );
    XAPStructContainerType sct = xap_sct_unknown;
    XAPTk::VectorOfProps props;
    const bool isAliased = MetaXAP_CollectAliases ( this, _ns, _path, props, false, &sct );

    // No need to filter value from CDATAToRaw, since XAPTk_Data::set does this for us.

    if ( ! isAliased ) {
 
        m_data->set ( _ns, _path, value, sct, f );
 
    } else {
 
        XAPTk::VectorOfProps::const_iterator	currAlias	= props.begin();
		XAPTk::VectorOfProps::const_iterator	endAlias	= props.end();
        for ( ; currAlias != endAlias; ++currAlias ) {
            try {
                m_data->set ( currAlias->first, currAlias->second, value, sct, f );
            } catch (...) {	// Only throw for original path, suppress for aliases.
                if ( (currAlias->first == _ns) && (currAlias->second == _path) ) throw;
            }
        }
	
	}
	
}


DECL_EXPORT void
MetaXAP::setTimestamp(const string& _ns, const string& _path, const XAPDateTime& dt) {
    if (dt.tzHour != 0 || dt.tzMin != 0) {
        throw xap_bad_number ();
    }
    XAPTk::VectorOfProps props;
    const bool ckAlias = MetaXAP_CollectAliases(this, _ns, _path, props, false);

    if (!ckAlias) {
        m_data->setTimestamp(_ns, _path, dt, XAP_CHANGE_FORCED);
    } else {
        XAPTk::VectorOfProps::const_iterator i = props.begin();
        XAPTk::VectorOfProps::const_iterator done = props.end();
        for (; i != done; ++i) {
            try {
                m_data->setTimestamp(i->first, i->second, dt, XAP_CHANGE_FORCED);
            } catch (...) {
                // Only throw for original path, suppress for aliases
                if (i->first == _ns && i->second == _path) {
                    throw;
                }
            }
        }
    }
}

DECL_EXPORT void
MetaXAP::setup(const string& /* key */, const string& /* val */ ) {
    //PENDING: but allow calls to complete without error
}


DECL_EXPORT void
MetaXAP::setResourceRef(const string& ref) {
    if (!XAPTk::CDATAToRaw(ref, m_data->m_about))
        m_data->m_about = ref;
}

/* ===== Undocumented Public Methods ===== */

XAPTk_Data*
MetaXAP::iWantPackageAccess() const {
    return(m_data);
}

void
MetaXAP::shortCutSerialize(string& serialXML) {
    serialXML = this->m_data->m_serialXML;
    this->m_data->m_serialXML = "";
}

/* ===== Protected ===== */

    /* Clone */
MetaXAP::MetaXAP(MetaXAP* orig) : m_data(NULL) {
    this->m_data = XAPTk_Data::New(this, orig->m_data);
}

#ifdef XXX
    /* Copy */
MetaXAP::MetaXAP(const MetaXAP& rhs) : m_data(NULL) {
    XAPTk::NotImplemented(XAP_SAY_WHAT("MetaXAP::MetaXAP"));
}

// Operators
// MetaXAP& operator=(const MetaXAP& rhs);
#endif


/* ===== "Package" Module Functions ===== */


static bool
Do_CollectAliases ( const MetaXAP* mx, const string& ns, const string& path,
					XAPTk::VectorOfProps& props, const bool /* suppressSingles */, XAPStructContainerType* cType /*=NULL*/ )
{

    // Short-circuit if no alias
    if ( ! mx->isEnabled ( XAP_OPTION_ALIASING_ON ) ) return false;

    const char *const LI_ORD_1 = "*[1]";
    string fixedPath;
    string ans;
    string apath;
    string aParentPath;
    string savedPath;
    string qualifier;
    string topLevel;
    XAPStructContainerType sct;
    MetaXAP_ResolvedAliases::iterator entry;
    bool isActual = false;
    bool needsFixin = false;
    bool isLang = false;
    bool isSingle = false;
    string lang;
    string origNS = ns;
    string origPath = path;

    props.clear();

    if ( CheckAlias ( mx, origNS, origPath, fixedPath, ans, apath, aParentPath, savedPath, topLevel, qualifier, &sct ) ) {

        XAPTk_PairOfString test ( origNS, fixedPath );
        entry = MetaXAP_resolvedAliases->find ( test );
        if ( (entry == MetaXAP_resolvedAliases->end()) && (! qualifier.empty()) ) {
            // Might work without the qualifier
            test.second = topLevel;
            entry = MetaXAP_resolvedAliases->find ( test );
        }
        assert ( entry != MetaXAP_resolvedAliases->end() );

    } else {

        // Might be an actual
        ans = origNS;
        apath = origPath;
        XAPTk::ConformPath ( apath, aParentPath, savedPath );
        XAPTk_PairOfString test ( ans, aParentPath );
        entry = MetaXAP_resolvedAliases->find ( test );

        if ( entry == MetaXAP_resolvedAliases->end() ) {

            /*
            Maybe this is a simple path with a qualifier, like
                <dc:format rdf:value="plain/text" dcq:FormatType="MIME"/>
            Need to make sure that dcq:FormatType gets copied to
            xap:Format (the alias).
            */
            const string::size_type firstSlash = aParentPath.find ( "/" );
            if ( firstSlash == string::npos ) {
            	return false;	// No aliases at all!
            } else {
                topLevel = aParentPath.substr ( 0, firstSlash );
                test.second = topLevel;
                entry = MetaXAP_resolvedAliases->find ( test );
                if ( entry == MetaXAP_resolvedAliases->end() ) return false;  // No aliases at all!
                // else, aParentPath has aliases and a qualifier
                qualifier = aParentPath.substr ( firstSlash, aParentPath.size()-firstSlash );
            } 

        } else if ( ! savedPath.empty() ) {

            // Handle case of qualified member of container.
            string::size_type ix;
            for ( ix = savedPath.find ( "/" ); ix != string::npos; ix = savedPath.find ( "/", ix+1 ) ) {
                if ( ix+1 >= savedPath.size() ) continue; // skip terminal slash
                if ( savedPath[ix+1] != '*' ) {
                    // Qualifier or subprop found!
                    topLevel = aParentPath;
                    assert ( topLevel.find ( "/" ) == string::npos );
                    qualifier = savedPath.substr ( ix, savedPath.size()-ix );
                    savedPath = savedPath.substr ( 0, ix );
                    break;
                }
            }

        }

        isActual = true;
        MetaXAP_InfoMap::iterator i = entry->second.find ( test );
        /* i: [first: <ns,path>, second: AliasInfo] */
        if ( i == entry->second.end() ) {
            //Try full path
            test.second = apath;
            i = entry->second.find ( test );
            if ( i == entry->second.end() ) i = entry->second.begin();	//Try first entry
        }
        sct = i->second.cType;

    }

    if ( cType != NULL ) *cType = sct;
    /* entry: [first: <ns,path>, second: InfoMap */
    needsFixin = !savedPath.empty();
    isLang = UtilityXAP::IsAltByLang ( mx, origNS, origPath, &lang );

    if ( sct == xap_sct_unknown ) {

        isSingle = true;

    } else if ( !isActual ) {

        // Aliases single only if actual is *[1] or lang
        string t1;
        string t2;
        string t3;
        XAPStructContainerType st;
        if ( MetaXAP::GetAlias ( origNS, fixedPath, t1, t2, st ) ) {
            bool ll = UtilityXAP::IsAltByLang ( mx, t1, t2 );
            XAPTk::ConformPath ( t2, t1, t3 );
            bool ss = (t3 == LI_ORD_1);
            isSingle = (ss || ll);
        }
    }

    // Check for *[last()], convert savedPath to ord or lang if possible
    if ( savedPath.find ( "*[last()]" ) != string::npos ) {
	    try {

	        XAPTk::NormTree* norm = XAPTk::NormTree::GetNormTree ( mx->iWantPackageAccess()->m_bySchema, origNS );

	        if ( norm != NULL ) {

	            string canon;
	            string t1;
	            XAPTk::NormTree::CanonicalPath(norm, origPath, canon);
	            XAPTk::ConformPath(canon, t1, savedPath);
	            origPath = fixedPath;
	            origPath.append("/");
	            origPath.append(savedPath);

	            if ( isActual ) {
	                apath = origPath;
	            } else {
	                XAPTk::ConformPath ( apath, aParentPath, t1 );
	                if ( t1 == "*[last()]" ) {
	                    apath = aParentPath;
	                    apath.append ( "/" );
	                    apath.append ( savedPath );
	                }
	            }

	        }

	    } catch (...) {
	        ;
	    }
    }

    // Use entry to build list
    MetaXAP_InfoMap& iMap = entry->second;
    MetaXAP_InfoMap::const_iterator item = iMap.begin();
    /* item: [first: <ns,path>, second: AliasInfo] */

    // Pre-select actual and original for first and second slots
    XAPTk_PairOfString matchOrig;
    XAPTk_PairOfString fullOrig;
    XAPTk_PairOfString matchActual;
    XAPTk_PairOfString fullActual;
    bool okToFix;
    bool foundActual = false;
    bool foundAlias = false;

	if ( ! isActual ) {

        matchOrig.first = origNS;
        matchOrig.second = (!topLevel.empty()) ? topLevel : fixedPath;
        fullOrig.first = origNS;
        fullOrig.second = origPath;
        matchActual.first = ans;
        if ( topLevel.empty() ) {
            matchActual.second = aParentPath;
        } else {
            string::size_type firstSlash = aParentPath.find ( "/" );
            assert ( firstSlash != string::npos );
            matchActual.second = aParentPath.substr ( 0, firstSlash );
        }
        fullActual.first = ans;
        fullActual.second = apath;
        
	} else {

        for ( ; item != iMap.end(); ++item ) {

            if ( item->second.actual ) {
                matchActual = item->first;
                fullActual = matchActual;
                if ( needsFixin ) {
                    fullActual.second.append ( "/" );
                    fullActual.second.append ( savedPath );
                }
                if ( ! qualifier.empty() ) fullActual.second.append ( qualifier );
                foundActual = true;
                continue;
            }

            const MetaXAP_AliasInfo& info = item->second;
            okToFix = ( needsFixin && (! info.aliasSingle) );

            if ( ! needsFixin ) {

				if ( (! isSingle) && info.aliasSingle ) continue; // skip this, doesn't match

            } else {

                if ( info.actualFirst && savedPath != LI_ORD_1 ) continue; // skip this, doesn't match

                if ( info.actualLang ) {
                    string tmpLang;
                    const XAPTk_PairOfString& link = item->first;
                    bool is = UtilityXAP::IsAltByLang ( mx, link.first, link.second, &tmpLang );
                    if ( is ) {
                        if ( lang != tmpLang ) continue; // skip this, doesn't match
                        // Else don't fix this path, it's already correct
                        okToFix = false;
                    } else if ( info.aliasSingle && (! info.actual) ) {
                        // Look up alias to compare language
                        string targetNS;
                        string targetPath;
                        XAPStructContainerType s;
                        if ( MetaXAP::GetAlias ( link.first, link.second, targetNS, targetPath, s ) ) {
                            is = UtilityXAP::IsAltByLang ( mx, targetNS, targetPath, &tmpLang );
                            if (is && (lang != tmpLang) ) continue;
                        }
                    }
                    // Fall thru, okToFix
                }

            }

            // Found non-actual for orig
            matchOrig = item->first;
            fullOrig = matchOrig;
            if ( okToFix ) {
                fullOrig.second.append ( "/" );
                fullOrig.second.append ( savedPath );
            }
            if (! qualifier.empty() ) fullOrig.second.append ( qualifier );
            foundAlias = true;
            if ( foundActual ) break;
 
        }
 
        item = iMap.begin();

    }

    bool skip = false;
    if ( isActual && (! (foundActual && foundAlias)) ) {
        if ( ! foundActual ) return false;
        /*
        No alias found.  This can sometimes happen when the actuals only have
        single targets, but the current call is using the actual as a container
        path. In this case the first slot will contain the actual, the second
        will contain nothing, i.e., the length of the vector will only be 1,
        which is how the caller can detect this case.
        */
        props.push_back ( fullActual );
        skip = true;
    } else {
        // Load first and second slots
        props.push_back ( fullActual );
        props.push_back ( fullOrig );
    }

    string candidatePath;

    // Loop thru, skipping matches for orig and actual (already loaded)
    if ( ! skip ) {
    
	    for ( ; item != iMap.end(); ++item ) {

	        const MetaXAP_AliasInfo& info = item->second;
	        okToFix = ( needsFixin && (! info.aliasSingle) );
	        if ( ! needsFixin ) {
	        
	        	if ( (! isSingle) && info.aliasSingle ) continue; // skip this, doesn't match
	        
	        } else {
	        
	            if ( info.actualFirst && savedPath != LI_ORD_1 ) continue; // skip this, doesn't match

	            if ( info.actualLang ) {

	                string tmpLang;
	                const XAPTk_PairOfString& link = item->first;
	                bool is = UtilityXAP::IsAltByLang ( mx, link.first, link.second, &tmpLang );

	                if ( is ) {
	                    if ( lang != tmpLang ) continue; // skip this, doesn't match
	                    // Else don't fix this path, it's already correct
	                    okToFix = false;
	                } else if ( info.aliasSingle && !info.actual ) {
	                    // Look up alias to compare language
	                    string targetNS;
	                    string targetPath;
	                    XAPStructContainerType s;
	                    if ( MetaXAP::GetAlias ( link.first, link.second, targetNS, targetPath, s ) ) {
	                        is = UtilityXAP::IsAltByLang ( mx, targetNS, targetPath, &tmpLang );
	                        if ( is && (lang != tmpLang) ) continue;
	                    }
	                }

	                // Fall thru, okToFix

	            }

	        }
	        
	        // Skip items that are already loaded in the list
	        if ( (item->first == matchOrig) || (item->first == matchActual) ) continue;
	        // Load the item
	        candidatePath = item->first.second;
	        if ( okToFix ) {
	            candidatePath.append ( "/" );
	            candidatePath.append ( savedPath );
	        }
	        if ( ! qualifier.empty() ) candidatePath.append ( qualifier );
	        XAPTk_PairOfString lVal ( item->first.first, candidatePath );
	        props.push_back ( lVal );

	    }
    
    }
    

#if defined(XAP_DEBUG_CERR) && 0
    XAPTk::VectorOfProps::iterator vn = props.begin();
    cerr << "Orig: " << path << ", " << ns << endl;
    while (vn != props.end()) {
        cerr << "----: " << vn->second << ", " << vn->first << endl;
        ++vn;
    }
    cerr << endl;
#endif

    return true;

}


// V436108: There is a problem where a simple property is aliased to an item in a container and another container is
// aliased to the entire container.  E.g. A -> B/*[1] and C -> B.  When setting via the simple property (A) the second
// container is written as simple (just C) instead of to the correct item (C/*[1]).  This can be avoid by the trick of
// collecting the aliases again, but through the base.  Luckily MetaXAP_CollectAliases always puts the base first.

static bool
MetaXAP_CollectAliases ( const MetaXAP* mx, const string& ns, const string& path,
						 XAPTk::VectorOfProps& props, const bool suppressSingles, XAPStructContainerType* cType /*=NULL*/ )
{
    const bool isAliased = Do_CollectAliases ( mx, ns, path, props, suppressSingles, cType );

    if ( isAliased ) {

         XAPTk::VectorOfProps::const_iterator	firstProp	= props.begin();
       
        if ( (props.size() > 1) && ((firstProp->first != ns) || (firstProp->second != path)) ) {
        	// There are aliases and the original path is not the base.  Collect the aliases again.
        	(void) Do_CollectAliases ( mx, firstProp->first, firstProp->second, props, suppressSingles, cType );
        }
        
	}
	
	return isAliased;

}

XAPTk_StringByString*
MetaXAP_GetMasterNSMap() {
    MetaXAP_InitNSMap();
    return MetaXAP_nsMap;
}


/* ===== Private Module Methods ===== */


static bool
CheckAlias ( const MetaXAP* mx, const string& ns, const string& path,
			 string& parentPath, string& ans, string& apath, string& aParentPath,
			 string& savedPath, string& topLevel, string& qualifier, XAPStructContainerType* cType /*= NULL */ )
{

    if ( ! mx->isEnabled ( XAP_OPTION_ALIASING_ON ) ) return false;
    XAPTk::ConformPath ( path, parentPath, savedPath );	 // Clean up the path before we check.
    bool needsFixin = ! savedPath.empty();

#if defined(XAP_DEBUG_CERR) && 0
    cerr << "Orig: " << path << endl;
    cerr << "Fixd: " << parentPath << endl;
#endif

    // Check alias of cleaned path
    XAPStructContainerType sct;
    bool ok = MetaXAP::GetAlias ( ns, parentPath, ans, aParentPath, sct );
    if ( !ok ) {
        /*
        Maybe this is a simple path with a qualifier, like
            <dc:format rdf:value="plain/text" dcq:FormatType="MIME"/>
        Need to make sure that dcq:FormatType gets copied to
        xap:Format (the alias).
        */
        const string::size_type firstSlash = parentPath.find("/");
        if (firstSlash != string::npos) {
            topLevel = parentPath.substr(0, firstSlash);
            ok = MetaXAP::GetAlias(ns, topLevel, ans, aParentPath, sct);
            if (ok) {
                // Restore qualifier
                qualifier = parentPath.substr(firstSlash, parentPath.size()-firstSlash);
                aParentPath.append(qualifier);
            } else topLevel = XAP_NO_VALUE;
        }
        if (!savedPath.empty() && !topLevel.empty()) {
            // Handle case of qualified member of container.
            string::size_type ix;
            for (ix = savedPath.find("/"); ix != string::npos; ix = savedPath.find("/", ix+1)) {
                if (ix+1 >= savedPath.size())
                    continue; // skip terminal slash
                if (savedPath[ix+1] != '*') {
                    // Qualifier or subprop found!
                    qualifier = savedPath.substr(ix, savedPath.size()-ix);
                    savedPath = savedPath.substr(0, ix);
                    break;
                }
            }
        }
    }

    if (ok && cType != NULL)
        *cType = sct;

#if defined(XAP_DEBUG_CERR) && 0
    if (ok) {
        cerr << "Actu: " << aParentPath << endl;
    }
#endif

    // If path was cleaned, restore its tail
    apath = aParentPath;
    if (ok && needsFixin) {
        apath.append("/");
        apath.append(savedPath);
    }

#if defined(XAP_DEBUG_CERR) && 0
        cerr << endl;
#endif

    return(ok);
}


static void
CreateAltTextItem	( MetaXAP *				meta,		// ! Does MUCH less than UtilityXAP::SetLocalizedText!
					  const std::string&	ns,
					  const std::string&	path,
					  const std::string&	lang,
					  const std::string&	val,
					  const XAPFeatures		f /*= XAP_FEATURE_DEFAULT*/ )
{

    try {
        XAPValForm	form	= meta->getForm ( ns, path );
        if ( form != xap_container ) throw xap_bad_type();
    } catch ( xap_no_match& ) {
        meta->createFirstItem ( ns, path, val, XAPTK_ATTR_XML_LANG, lang, true, f );
        return;
    }

    std::string	tmp;
    XAPTk::ComposeAltLang ( path, lang, tmp );
    meta->set ( ns, tmp, val, f );

}


static void
CopyProp(MetaXAP* src, MetaXAP* dst, const string& ns, const string& path) {
    bool ok;
    string val;
    XAPFeatures f;
    XAPDateTime savedDT;

    bool restoreDT = UtilityXAP::GetDateTime(dst, XAP_NS_XAP, XAPTK_METADATA_DATE, savedDT);

    //Simple and nested values are easy
    if (src->getForm(ns, path) != xap_container) {
        ok = src->get(ns, path, val, f);
        assert(ok);
        if (ok) {
            dst->set(ns, path, val, f);
            //Okay if we are blown by exception
            //Restore metadata date
            if (restoreDT)
                UtilityXAP::SetDateTime(dst, XAP_NS_XAP, XAPTK_METADATA_DATE, savedDT);
        }
#if defined(XAP_DEBUG_CERR) && 0
        cerr << "Copied {" << path << ", " << ns << "}" << endl;
#endif
        return;
    }

    //Container in src: Start with a clean slate
    XAPValForm fdst;
    try {
        /*
        If not defined, getForm will raise exception before remove executes,
        which is okay.  Also okay if remove raises an exception.
        */
        fdst = dst->getForm(ns, path);
        dst->remove(ns, path);
    } catch (...) {
        fdst = xap_unknown;
    }
    assert(fdst == xap_container || fdst == xap_unknown);

    //Containers are enumerated and copied
    XAPPaths* p = NULL;
    string srcNS;
    string srcPath;
    bool firstTime = true;

    //Static analysis of source
    XAPStructContainerType cType = src->getContainerType(ns, path);
    bool isAltByLang;
    string parentPath;
    string langVal;
    string tmp;
    string::size_type last;

    //Now copy
    try {
        p = src->enumerate(ns, path);
        while(p->hasMorePaths()) {
            p->nextPath(srcNS, srcPath);
            srcPath = path + '/' + srcPath;
            ok = src->get(srcNS, srcPath, val, f);
            if (!ok)
                continue;

            //Need the parent element path
            last = srcPath.rfind(XAPTk::SLSH);
            if (last != string::npos) {
                parentPath = srcPath.substr(0, last);
            } else
                parentPath = srcPath;

            if (firstTime) { //Create first item
                //Is it alt by lang?
                isAltByLang = UtilityXAP::IsAltByLang(src, srcNS, srcPath, &langVal);
                if (isAltByLang) {
                    //Create alt by lang
                    dst->createFirstItem(
                        srcNS,
                        parentPath,
                        val,
                        XAPTK_ATTR_XML_LANG,
                        langVal,
                        true,
                        f);
                } else {
                    //Create member element
                    dst->createFirstItem(
                        srcNS,
                        parentPath,
                        val,
                        cType,
                        f);
                }
                //Okay if we are blown by exception
                //Restore metadata date
                if (restoreDT)
                    UtilityXAP::SetDateTime(dst, XAP_NS_XAP, XAPTK_METADATA_DATE, savedDT);
#if defined(XAP_DEBUG_CERR) && 0
                cerr << "Copied first {" << srcPath << ", " << srcNS << "}";
                cerr << endl;
#endif
                firstTime = false;
            } else { //Append next item
                //Is it alt by lang?
                XAPTk::DecomposeAltLang(srcPath, tmp, langVal);
                if (langVal != "") {
                    CreateAltTextItem ( dst, srcNS, tmp, langVal, val, f );
                } else {
                    tmp = parentPath;
                    tmp.append("/*[last()]");
                    dst->append(srcNS, tmp, val, false, f);
                }
                //Okay if we are blown by exception
                //Restore metadata date
                if (restoreDT)
                    UtilityXAP::SetDateTime(dst, XAP_NS_XAP, XAPTK_METADATA_DATE, savedDT);
#if defined(XAP_DEBUG_CERR) && 0
                cerr << "Appended {" << srcPath << ", " << srcNS << "}";
                cerr << endl;
#endif
            }
        }
        delete p;
    } catch (...) {
        delete p;
        p = NULL;
    }
}

static void
CopyTimestamp(MetaXAP* src, MetaXAP* dst, const string& ns, const string& path) {
    bool ok;
    XAPDateTime dt;
    XAPChangeBits how;

    // Set timestamp
    ok = src->getTimestamp(ns, path, dt, how);
    if (!ok) {
        ok = UtilityXAP::GetDateTime(src, XAP_NS_XAP, XAPTK_METADATA_DATE, dt);
        how = XAP_CHANGE_FORCED;
    }
    if (ok) {
        dst->iWantPackageAccess()->setTimestamp(ns, path, dt, how);
    } else
        return;

    // Update MetadataDate
    XAPDateTime oldDT;
    ok = UtilityXAP::GetDateTime(dst, XAP_NS_XAP, XAPTK_METADATA_DATE, oldDT);
    if (ok) {
        XAPTimeRelOp cmp = XAPTk::CompareDateTimes(dt, oldDT);
        if (cmp == xap_after)
            UtilityXAP::SetDateTime(dst, XAP_NS_XAP, XAPTK_METADATA_DATE, dt);
    }
}



static void
DupProp(MetaXAP* mx, const string& ns, const string& path, const string& dstNS, const string& dstPath, const XAPStructContainerType& sct) {
    bool ok = true;
    string val;
    XAPFeatures f;
    string fixed;
    string var;
    string pathFixed;
    string pathVar;
    string srcNS = ns;
    string srcPath = path;
    string parentPath;
    bool firstTime;
    bool dstSingle;
    string langVal;
    string selName;
    string selVal;
    string lastStep;
    string tv;
    bool valFound;
    XAPFeatures tf;
    long int ord;
    XAPValForm form;
    XAPPaths* p = NULL;

    assert(!mx->isEnabled(XAP_OPTION_ALIASING_ON));

    while (ok) try {
        assert(mx->getForm(ns, path) != xap_container);
        XAPTk::ConformPath(dstPath, fixed, var);
        XAPTk::ConformPath(path, pathFixed, pathVar);

        // Determine if destination is a single (non-container) value.  If there is a variable part then
        // obviously it is not simple.  If the destination has no variable part it might still be a direct
        // copy of a container, so check the source.
        
   		dstSingle = false;
        if ( var.empty() ) {
        	form = mx->getForm ( ns, path );
        	if ( form != xap_container ) dstSingle = true;
        }

        // Determine if destination exists yet
        try {
            if (!var.empty())
                form = mx->getForm(dstNS, fixed);
            else
                form = mx->getForm(dstNS, dstPath);
            firstTime = false;
        } catch (...) {
            firstTime = true;
            form = (var.empty()) ? xap_simple : xap_container;
        }

        if (mx->get(ns, path, val, f)) {
            // Just a single value
            // Check destination, might need to create first item
            if (sct != xap_sct_unknown && !dstSingle) {
                if (firstTime) {
                    if (sct == xap_alt) {
                        // Need the lang value, try original path first
                        UtilityXAP::AnalyzeStep(path, parentPath, lastStep, ord, selName, selVal);
                        if (selVal.empty()) {
                            // Try the destination
                            UtilityXAP::AnalyzeStep(dstPath, parentPath, lastStep, ord, selName, selVal);
                        }
                        if (selVal.empty()) {
                            // An xap_alt, but not by xml:lang
                            mx->createFirstItem(
                                dstNS,
                                fixed,
                                val,
                                sct,
                                f);
                        } else {
                            mx->createFirstItem(
                                dstNS,
                                fixed,
                                val,
                                XAPTK_ATTR_XML_LANG,
                                selVal,
                                true,
                                f);
                        }
                    } else {
                        mx->createFirstItem(
                            dstNS,
                            fixed,
                            val,
                            sct,
                            f);
                    }
                } else {
                    // Don't set the same value twice!
                    tf = XAP_FEATURE_NONE;
                    tv = "";
                    try {
                        valFound = mx->get(dstNS, dstPath, tv, tf);
                    } catch (...) {
                        valFound = false;
                    }
                    if (!valFound || (tv != val || tf != f)) {
                        // Need to append
                        //Is it alt by lang?
                        XAPTk::DecomposeAltLang(srcPath, parentPath, langVal);
                        if (!langVal.empty()) {
		                    CreateAltTextItem ( mx, dstNS, fixed, langVal, val, f );
                        } else {
                            parentPath = fixed;
                            parentPath.append("/*[last()]");
                            mx->append(dstNS, parentPath, val, false, f);
                        }
                    }
                }
            } else {
                // Don't set the same value twice!
                tf = XAP_FEATURE_NONE;
                tv = "";
                try {
                    valFound = mx->get(dstNS, dstPath, tv, tf);
                } catch (...) {
                    valFound = false;
                }
                if (!valFound || (tv != val || tf != f)) {
                    mx->set(dstNS, dstPath, val, f);
                }
            }
        }
        break;
    } catch (...) {
        ok = false;
#if defined(XAP_DEBUG_CERR) && 0
        delete p;
        throw;
#endif
    }

    // Restore state
    delete p;
}

inline static bool
operator==(const MetaXAP_AliasInfo& lhs, const MetaXAP_AliasInfo& rhs) {
    return(lhs.cType == rhs.cType
     && lhs.actual == rhs.actual
     && lhs.aliasSingle == rhs.aliasSingle
     && lhs.actualFirst == rhs.actualFirst
     && lhs.actualLang == rhs.actualLang);
}

inline static bool	// Not presently used, causes UNIX warning. Keep in case it becomes used!
operator!=(const MetaXAP_AliasInfo& lhs, const MetaXAP_AliasInfo& rhs) {
    return(!(lhs == rhs));
}

inline static void
initInfo(MetaXAP_AliasInfo& info, const XAPStructContainerType& sct) {
    info.cType = sct;
    info.actual = false;
    info.aliasSingle = false;
    info.actualFirst = false;
    info.actualLang = false;
}

// Aliases come in four flavors. When I say "single", I mean a path whose last step is
// a xap_simple form, which has no parent or which has a xap_description parent, and is
// a terminal (text) node.  A "container" is a path whose last step has form
// xap_container.
// 
// Flavors  Alias       Actual                      Example
// -------  -----       ------                      -------
// 1        single      single                      Date => date
// 2        container   container                   Keywords => topics
// 3        single      container/*[1]              Title => title/*[1]
// 4        single      container/*[@xml:lang='xx'] Title => title/*[@xml:lang='en-us']
// 
// The <ns,path> passed in may be in one of the alias path types listed above,
// or may be a variation, such that the "fixed" portion of the conformed path
// (a path broken up into a its fixed and variable parts, the variable part
// being the latest step that contains '*') matches one of the alias path types.

static void
PreResolveAlias(const XAPTk_PairOfString& alias, const MetaXAP::Actual& arec) {
    /*
    Each property, whether an alias or an actual, has a single entry in
    the MetaXAP_ResolvedAliases map.  The value of the entry is a pool
    of linked properties, inclusive of the entry itself (e.g., if "foo"
    is linked to "bar" and "bletch", the pool contains "foo", "bar" and
    "bletch").  MetaXAP_InfoMap defines the pool.  Each property in
    the pool (left hand side of pair in MetaXAP_InfoMap) is also an
    entry in the MetaXAP_ResolvedAliases map.
    */

    // Alias
    //XXX const string& ns = alias.first;
    const string& path = alias.second;
    string parentPath;
    string var;
    // Actual
    //XXX const string& ans = arec.ns;
    const string& apath = arec.path;
    string aParentPath;
    string aVar;
    // Other variables
    MetaXAP_AliasInfo infoAlias;
    MetaXAP_AliasInfo infoActual;
    const XAPTk_PairOfString actual(arec.ns, arec.path);
    string lang;

    // Prep info for alias and actual entries
    XAPTk::ConformPath(apath, aParentPath, aVar);
    const XAPTk_PairOfString fixedActual(arec.ns, aParentPath);
    XAPTk::ConformPath(path, parentPath, var);
    initInfo(infoAlias, arec.cType);
    initInfo(infoActual, arec.cType);
    infoActual.actual = true;
    if (aVar.empty()) {
        if (arec.cType == xap_sct_unknown) {
            infoAlias.aliasSingle = true; // flavor 1
        }
        // else, info.aliasSingle remains false, flavor 2
    } else {
        if (aVar == "*[1]") {
            // flavor 3
            infoAlias.aliasSingle = true;
            infoAlias.actualFirst = true;
            infoActual.actualFirst = true;
        } else if (aVar.find("[@xml:lang") != string::npos) {
            // flavor 4
            infoAlias.aliasSingle = true;
            infoAlias.actualLang = true;
            infoActual.actualLang = true;
            // Derive lang value
            string t1, t2, t3;
            long int ord;
            UtilityXAP::AnalyzeStep(apath, t1, t2, ord, t3, lang);
        } else {
            //CantHappen
            throw xap_bad_path ();
        }
    }


    // Work on alias first
    MetaXAP_ResolvedAliases::iterator iAlias = MetaXAP_resolvedAliases->find(alias);
    if (iAlias != MetaXAP_resolvedAliases->end()) {
        // Alias should not already be defined
        throw xap_bad_path ();
    } else {
        // Create new entry
        MetaXAP_InfoMap tmpMap;
        tmpMap[alias] = infoAlias; // Insert self (alias) first
        (*MetaXAP_resolvedAliases)[alias] = tmpMap;
        iAlias = MetaXAP_resolvedAliases->find(alias);
    }
    MetaXAP_InfoMap& aliasIMap = iAlias->second;

    // Lookup actual entry, create if new
    MetaXAP_ResolvedAliases::iterator iActual = MetaXAP_resolvedAliases->find(fixedActual);
    if (iActual == MetaXAP_resolvedAliases->end()) {
        // Create new entry
        MetaXAP_InfoMap tmpMap;
        MetaXAP_AliasInfo tmpInfo;
        initInfo(tmpInfo, arec.cType);
        tmpInfo.actual = true;
        tmpMap[fixedActual] = tmpInfo;
        (*MetaXAP_resolvedAliases)[fixedActual] = tmpMap;
        iActual = MetaXAP_resolvedAliases->find(fixedActual);
    }
    MetaXAP_InfoMap& actualIMap = iActual->second;

    // Add actual map to alias map
    MetaXAP_InfoMap::const_iterator i = actualIMap.begin();
    MetaXAP_InfoMap::const_iterator done = actualIMap.end();
    for (; i != done; ++i) {
        /* [first: <ns,path>, second: AliasInfo] */
        // Skip self
        if (i->first == alias)
            continue;
        // Any other item needs to be filtered
        MetaXAP_AliasInfo item = i->second; // copy, might be changed
        const string& itemNS = i->first.first;
        string itemPath(i->first.second); // copy, might be changed
        if (!item.aliasSingle && infoAlias.aliasSingle) {
            if (infoActual.actualFirst && !item.actualFirst) {
                assert(itemPath.find("*[1]") == string::npos);
                itemPath.append("/*[1]");
                item.actualFirst = true;
            } else if (infoActual.actualLang && !item.actualLang) {
                assert(itemPath.find("[@xml:lang") == string::npos);
                itemPath.append("/*[@xml:lang='");
                itemPath.append(lang);
                itemPath.append("']");
                item.actualLang = true;
            }
        }
        // Already entered?
        XAPTk_PairOfString test(itemNS, itemPath);
        if (aliasIMap.find(test) != aliasIMap.end()) {
            continue;
        }
        // Add item
        aliasIMap[test] = item;
    }

    // Add alias to all maps listed in actual map
    i = actualIMap.begin();
    done = actualIMap.end();
    for (; i != done; ++i) {
        /* [first: <ns,path>, second: AliasInfo] */
        // Skip the same actual
        if (i->first == fixedActual)
            continue;
        // Skip self (shouldn't happen)
        assert(i->first != alias);
        // Look up map and check
        MetaXAP_ResolvedAliases::iterator j = MetaXAP_resolvedAliases->find(i->first);
        /* [first: <ns,path>, second: InfoMap] */
        MetaXAP_InfoMap& link = j->second;
        assert(link.find(alias) == link.end());
        // Add alias
        link[alias] = infoAlias;
    }

    // Finally, add self to actual map
    actualIMap[alias] = infoAlias;

#if defined (XAP_DEBUG_CERR) && 0
    // Dump state of maps
    MetaXAP_ResolvedAliases::const_iterator n = MetaXAP_resolvedAliases->begin();
    MetaXAP_ResolvedAliases::const_iterator nDone = MetaXAP_resolvedAliases->end();
    MetaXAP_InfoMap::const_iterator m;
    MetaXAP_InfoMap::const_iterator mDone;
    for (; n != nDone; ++n) {
        cerr << n->first.second << ", " << n->first.first << ":" << endl;
        m = n->second.begin();
        mDone = n->second.end();
        for (; m != mDone; ++m) {
            cerr << "  " << m->first.second << "(" ;
            const MetaXAP_AliasInfo& inf = m->second;
            if (inf.actual)
                cerr << "A";
            if (inf.aliasSingle)
                cerr << "s";
            if (inf.actualFirst)
                cerr << "1";
            if (inf.actualLang)
                cerr << "L";
            switch (inf.cType) {
                case xap_alt: cerr << "alt"; break;
                case xap_bag: cerr << "bag"; break;
                case xap_seq: cerr << "seq"; break;
                default: cerr << "?"; break;
            }
            cerr << "), " << m->first.first << endl;
        }
    int xxx = 1;
    }
    cerr << "=======================" << endl;
#endif

}

static void
ValidateNS ( const string& ns ) {
    // If this namespace has not been registered, throw an exception.
    XAPTk_StringByString::const_iterator i;
    XAPTk_StringByString::const_iterator done = MetaXAP_nsMap->end();

    for ( i = MetaXAP_nsMap->begin(); i != done; ++i ) {
        if ( i->second == ns )  return;
    }
    
    #if 0
	    cout << "#ERROR: Namespace not registered: " << ns << endl;
	    for ( i = MetaXAP_nsMap->begin(); i != done; ++i ) {
	        cout << '\t' << i->first << ":\t" << i->second << endl;
	    }
    #endif
   
    throw xap_bad_schema ();

}


// *** VerifyAndPopulate is a mess.  The verify and populate parts ought to be broken up into a
// *** few static functions.  It would also be nice to avoid all of the ns/path type calls, we
// *** are in the middle of the tree, why constantly walk from the top?

// *** [462404] If squashDifs is set we don't bother doing any comparisons, we just delete the
// *** alias form (keep the actual) any time both are present.  The populate the actual's value.

#ifndef	Trace_VerifyAndPopulate
	#define Trace_VerifyAndPopulate	0
#endif

static bool
VerifyAndPopulate ( MetaXAP* mx, bool squashDifs )
{
    /*
    Make sure that aliased names that have values
    don't have different values.  If metadata checks out okay, populate
    the linked values for all the aliased names.  Return false if
    verification fails.
    */
    string val;
    XAPFeatures f;
    string srcNS;
    string srcPath;
    string ns;
    string path;
    XAPTk::VectorOfProps props;
    XAPStructContainerType sct;
    bool ret;
    XAPValForm aliasForm = xap_unknown; // ** init just to silence gcc warnings.
    XAPValForm actualForm = xap_unknown; // ** init just to silence gcc warnings.
    bool noAlias;
    bool noActual;
    bool alFound;
    bool acFound;
    string actualVal;
    XAPFeatures actualF;
    bool perfect = true; // Skip populate, perfect copy

    /*
    Verify by checking if alias and actual both have values, and that
    both values are the same.  If either has no value, no problem,
    we'll just copy it.
    */

    assert(!mx->isEnabled(XAP_OPTION_ALIASING_ON));

    try {
        MetaXAP_AliasMap::const_iterator i = MetaXAP_aliasMap->begin();
        MetaXAP_AliasMap::const_iterator done = MetaXAP_aliasMap->end();
        /* [first: alias<ns,path>, second: actual(MetaXAP::Actual)] */
        for (; i != done; ++i) {
            const string& aliasNS = i->first.first;
            const string& aliasPath = i->first.second;
            const string& actualNS = i->second.ns;
            const string& actualPath = i->second.path;
            // Check for existence
            noAlias = true;
            try {
                if (mx->iWantPackageAccess()->evalXPath(xaptk_no_throw, aliasNS, aliasPath) != NULL) {
                   aliasForm = mx->getForm(aliasNS, aliasPath);
                   noAlias = false;
                }
            } catch (...) {
                ;
            }
            noActual = true;
            try {
                if (mx->iWantPackageAccess()->evalXPath(xaptk_no_throw, actualNS, actualPath) != NULL) {
                    actualForm = mx->getForm(actualNS, actualPath);
                    noActual = false;
                }
            } catch (...) {
                ;
            }
            if ( (noAlias && !noActual) || (!noAlias && noActual) ) perfect = false;
            if ( noAlias || noActual ) continue;
	        if ( squashDifs ) {
	        	#if Trace_VerifyAndPopulate
		        	cout << "VerifyAndPopulate: Removing " << aliasPath << " from " << aliasNS << endl;
		        #endif
	        	mx->remove ( aliasNS, aliasPath );
	        	perfect = false;
	        	continue;
	        }
            if (aliasForm == xap_description || actualForm == xap_description) {
                //CantHappen!
                throw xap_bad_xap (); //Actually, the alias might be bad
            }
            // Compare values
            if (aliasForm == xap_simple && actualForm == xap_simple) {
                alFound = mx->get(aliasNS, aliasPath, val, f);
                acFound = mx->get(actualNS, actualPath, actualVal, actualF);
                if ((alFound && !acFound) || (!alFound && acFound))		// *** Why check found again?
                    perfect = false;									// *** Didn't the earlier call
                if ( !alFound || !acFound )								// ***	to evalXPath do enough?
                    continue;
                if (val != actualVal || f != actualF) {
                    return(false);
                }
            } else if (aliasForm == xap_container && actualForm == xap_container) {
                // Enumerate and compare
                // *** These compares only work for containers of simple items, not for containers
                // *** of structures or containers.  It also isn't clear if this works for alt by
                // *** language containers (needs closer examination and testing).
                XAPPaths* lhs = NULL;
                XAPPaths* rhs = NULL;
                string lns;
                string lp;
                string rns;
                string rp;
                bool lMore;
                bool rMore;
                bool ok = true;
                XAPStructContainerType lsct = mx->getContainerType(aliasNS, aliasPath);
                XAPStructContainerType rsct = mx->getContainerType(actualNS, actualPath);
                if (lsct != rsct)
                    return(false);
                try {
                    lhs = mx->enumerate(aliasNS, aliasPath);
                    rhs = mx->enumerate(actualNS, actualPath);

                    if (lsct == xap_seq) {
                        // Items and order must match
                        while (ok) {
                            lMore = lhs->hasMorePaths();
                            rMore = rhs->hasMorePaths();
                            if ((lMore && !rMore) || (!lMore && rMore)) {
                                ok = false;
                                break;
                            }
                            if (!lMore && !rMore)
                                break;
                            lhs->nextPath(lns, lp);
                            lp = aliasPath + '/' + lp;
                            rhs->nextPath(rns, rp);
                            rp = actualPath + '/' + rp;
                            if (!mx->get(lns, lp, val, f)) {
                                ok = false;
                                break;
                            }
                            if (!mx->get(rns, rp, actualVal, actualF)) {
                                ok = false;
                                break;
                            }
                            if (val != actualVal || f != actualF) {
                                ok = false;
                                break;
                            }
                        }
                    } else {
                        // Items must match
                        vector<MetaXAP_AVS> avs;
                        MetaXAP_AVS tmpAVS;
                        string n;
                        string p;
                        vector<MetaXAP_AVS>::iterator vx;
                        // Put all the alias values in a vector
                        while (lhs->hasMorePaths()) {
                            lhs->nextPath(n, p);
                            p = aliasPath + '/' + p;
                            if (!mx->get(n, p, val, f)) {
                                ok = false;
                                break;
                            }
#if defined(XAP_DEBUG_CERR) && 0
                            cerr << "alias: {" << p << ", " << n << "}" << endl;
                            cerr << "       [" << val << "]" << endl;
#endif
                            tmpAVS.val = val;
                            tmpAVS.f = f;
                            avs.push_back(tmpAVS);
                        }
                        // Compare with values from actuals
                        if (ok) while (rhs->hasMorePaths()) {
                            rhs->nextPath(n, p);
                            p = actualPath + '/' + p;
                            if (!mx->get(n, p, val, f)) {
                                ok = false;
                                break;
                            }
                            if (avs.size() == 0) {
                                // More actuals than aliases
                                ok = false;
                                break;
                            }
#if defined(XAP_DEBUG_CERR) && 0
                            cerr << "actul: {" << p << ", " << n << "}" << endl;
                            cerr << "       [" << val << "]" << endl;
#endif
                            // Find by value, and remove
                            bool found = false;
                            vx = avs.begin();
                            while (vx != avs.end()) {
                                if (vx->val == val && vx->f == f) {
                                    found = true;
                                    vx = avs.erase(vx);
                                    break;
                                }
                                ++vx;
                            }
                            if (!found) {
                                // Actual not present in aliases
                                ok = false;
                                break;
                            }
                        }
                        if (ok && avs.size() != 0) {
                            // Alias not present in actual, or more aliases
                            ok = false;
                        }
                    }
                } catch (...) {
                    delete lhs;
                    delete rhs;
                    throw;
                }
                delete lhs;
                delete rhs;
                if (!ok)
                    return(false);
#if defined(XAP_DEBUG_CERR) && 0
                cerr << endl;
#endif

            } else {
                //CantHappen
                throw xap_bad_xap (); //Bad alias?
            }
        }
    } catch (...) {
        //XXX throw;
        return(false);
    }

    // If we get this far with perfect true, we can skip the population
    if (perfect)
        return(true);

    ret = true;

    // Populate aliases by enumerating everything
    XAPPaths* p = NULL;
    try {
        XAPTk_Data* m_data = mx->iWantPackageAccess();
        p = mx->enumerate();
        while (p->hasMorePaths()) {
            p->nextPath(ns, path);
            bool ckAlias = false;
            // Manually turn on aliasing
            assert(!mx->isEnabled(XAP_OPTION_ALIASING_ON));
            m_data->m_options |= XAP_OPTION_ALIASING_ON;
            try {
                ckAlias = MetaXAP_CollectAliases(mx, ns, path, props, false, &sct);
            } catch (...) {
                ;
            }
            m_data->m_options &= ~(XAP_OPTION_ALIASING_ON);
            if (!ckAlias)
                continue;

            srcNS = "";
            srcPath = "";

            // Use the first form with a value as the source.  Since MetaXAP_CollectAliases puts the base first
            // this will tend to propagate the base.
            #if 0
            try {
                if (m_data->get(ns, path, val, f)) {
                    srcNS = ns;
                    srcPath = path;
                }
            } catch (...) {
                ;
            }
            #endif
            if (srcPath.empty()) {
                // No current value, so use the first match
                XAPTk::VectorOfProps::const_iterator d = props.begin();
                XAPTk::VectorOfProps::const_iterator ex = props.end();
                for(; d != ex; ++d) {
                    try {
                        if (m_data->get(d->first, d->second, val, f)) {
                            srcNS = d->first;
                            srcPath = d->second;
                            break;
                        }
                    } catch (...) {
                    }
                }
            }
            if ( ! srcPath.empty() ) {
               XAPTk::VectorOfProps::const_iterator d = props.begin();
               XAPTk::VectorOfProps::const_iterator ex = props.end();
               for ( d = props.begin(); d != ex; ++d ) {
                    if ( (d->first == srcNS) && (d->second == srcPath) ) continue;
                    #if Trace_VerifyAndPopulate
                    	cout << "VerifyAndPopulate: Duplicating " << srcPath << " to " << d->second << " sct:" << (int)sct << endl;
                    	cout << "                   namespaces: " << srcNS << " to " << d->first << endl;
                    	cout << "                   origination: " << path << " in " << ns << endl;
                    #endif
                    DupProp ( mx, srcNS, srcPath, d->first, d->second, sct );
                }
            }
        }
    } catch (...) {
        ret = false;
#if defined(XAP_DEBUG_CERR) && 0
        delete p;
        throw;
#endif
    }
    delete p;
    return(ret);
}


#ifndef UseXMPPrefix
	#define UseXMPPrefix	0
#endif

static void
MetaXAP_InitNSMap()
{
    if ( MetaXAP_nsMap != NULL ) return;
    MetaXAP_nsMap = new XMP_Debug_new XAPTk_StringByString;

    XAPTk_InitConstData(); //STATIC_INIT		// *** Redundant?

    (*MetaXAP_nsMap)[XAP_PREFIX_RDF] = XAP_NS_RDF;

	#if UseXMPPrefix
	
		// *** Don't switch to the xmp prefixes until all top apps have shipped with at least a
		// *** version 2.9 XMP toolkit. There are bugs in handling multiple namespace prefixes
		// *** in earlier versions of the XMP toolkit.
		
		#error "Are we ready for xmp prefixes?"
	
	    (*MetaXAP_nsMap)["xmp"]        = XAP_NS_XAP;
	    (*MetaXAP_nsMap)["xmpG"]       = XAP_NS_XAP_G;
	    (*MetaXAP_nsMap)["xmpGImg"]    = XAP_NS_XAP_G_IMG;
	    (*MetaXAP_nsMap)["xmpDyn"]     = XAP_NS_XAP_DYN;
	    (*MetaXAP_nsMap)["xmpDynA"]    = XAP_NS_XAP_DYN_A;
	    (*MetaXAP_nsMap)["xmpDynV"]    = XAP_NS_XAP_DYN_V;
	    (*MetaXAP_nsMap)["xmpT"]       = XAP_NS_XAP_T;
	    (*MetaXAP_nsMap)["xmpTPg"]     = XAP_NS_XAP_T_PG;
	    (*MetaXAP_nsMap)["xmpRights"]  = XAP_NS_XAP_RIGHTS;
	    (*MetaXAP_nsMap)["xmpMM"]      = XAP_NS_XAP_MM;
	    (*MetaXAP_nsMap)["xmpS"]       = XAP_NS_XAP_S;
	    (*MetaXAP_nsMap)["xmpBJ"]      = XAP_NS_XAP_BJ;
	
	#else
	
	    (*MetaXAP_nsMap)["xap"]        = XAP_NS_XAP;
	    (*MetaXAP_nsMap)["xapG"]       = XAP_NS_XAP_G;
	    (*MetaXAP_nsMap)["xapGImg"]    = XAP_NS_XAP_G_IMG;
	    (*MetaXAP_nsMap)["xapDyn"]     = XAP_NS_XAP_DYN;
	    (*MetaXAP_nsMap)["xapDynA"]    = XAP_NS_XAP_DYN_A;
	    (*MetaXAP_nsMap)["xapDynV"]    = XAP_NS_XAP_DYN_V;
	    (*MetaXAP_nsMap)["xapT"]       = XAP_NS_XAP_T;
	    (*MetaXAP_nsMap)["xapTPg"]     = XAP_NS_XAP_T_PG;
	    (*MetaXAP_nsMap)["xapRights"]  = XAP_NS_XAP_RIGHTS;
	    (*MetaXAP_nsMap)["xapMM"]      = XAP_NS_XAP_MM;
	    (*MetaXAP_nsMap)["xapS"]       = XAP_NS_XAP_S;
	    (*MetaXAP_nsMap)["xapBJ"]      = XAP_NS_XAP_BJ;
	
	#endif
	
    (*MetaXAP_nsMap)["pdf"]        = XAP_NS_PDF;
    (*MetaXAP_nsMap)["photoshop"]  = XAP_NS_PHOTOSHOP;
    (*MetaXAP_nsMap)["exif"]       = XAP_NS_EXIF;
    (*MetaXAP_nsMap)["tiff"]       = XAP_NS_TIFF;

    (*MetaXAP_nsMap)["dc"]         = XAP_NS_DC;

    /*
    The following are sub-property namespaces reserved for a future
    version where we actually partition sub-properties into their own schemas.
    The version 1.0 XAP library does not do this partitioning: all sub-properties
    are defined in the namespace of their top-level ancestor, meaning that
    the schema tables are organized by namespace, and the lookup of
    paths is flat.

    Defining these here makes it possible for clients to use unqualified
    localPath names for sub-properties in the absence of a schema object.
    For example, to properly create the width of an image, I should use
    the path "Dimensions/stDim:w".  The API also allows "Dimensions/w".
    However, in the case of creating a property for the first time,
    such as set(..., "Dimensions/w", ...), the library needs to know what
    namespace to put "w" in.  It uses a lookup function based on
    MetaXAP_nsMap, and the following hard-coded definitions.
    */

    (*MetaXAP_nsMap)["stDim"]       = XAP_NS_ST_DIMENSIONS;
    (*MetaXAP_nsMap)["stRes"]       = XAP_NS_ST_RESOLUTION;
    (*MetaXAP_nsMap)["stTrk"]       = XAP_NS_ST_TRACK_DESC;
    (*MetaXAP_nsMap)["stFnt"]       = XAP_NS_ST_FONT;
    (*MetaXAP_nsMap)["stRef"]       = XAP_NS_ST_RESOURCE_REF;
    (*MetaXAP_nsMap)["stVer"]       = XAP_NS_ST_VERSION;
    (*MetaXAP_nsMap)["stEvt"]       = XAP_NS_ST_RESOURCE_EVENT;
    (*MetaXAP_nsMap)["stDsp"]       = XAP_NS_ST_FILE_DISPOSITION;
    (*MetaXAP_nsMap)["stJob"]       = XAP_NS_ST_JOB;
    (*MetaXAP_nsMap)["stRit"]       = XAP_NS_ST_RIGHT;

    /*
    Other namespaces used in XAP RDF.
    */

    (*MetaXAP_nsMap)[XAP_PREFIX_INFO_XAP] = XAPTK_NS_INFO_XAP;


#ifdef XXX
    /*
    We also need to have a map of the unqualified patterns that
    need to be substituted.
    */
    if (MetaXAP_subTypeMap != NULL)
        return;
    MetaXAP_subTypeMap = new XMP_Debug_new XAPTk::SubTypeMap;
    /* [first: <ns,path>, second: map by top level path step] */
    XAPTk_PairOfString p;
    XAPTk_StringByString stDim;
    XAPTk_StringByString stRes;
    XAPTk_StringByString stTrk;
    XAPTk_StringByString stFnt;
    XAPTk_StringByString stRef;
    XAPTk_StringByString stVer;
    XAPTk_StringByString stEvt;
    XAPTk_StringByString stDsp;

    stDim["w"] = "stDim:w";
    stDim["h"] = "stDim:h";
    stDim["unit"] = "stDim:unit";

    stRes["x"] = "stRes:x";
    stRes["y"] = "stRes:x";
    stRes["unit"] = "stRes:x";

    stTrk["bits"] = "stTrk:bits";
    stTrk["codec"] = "stTrk:codec";
    stTrk["description"] = "stTrk:description";
    stTrk["name"] = "stTrk:name";
    stTrk["rate"] = "stTrk:rate";
    stTrk["type"] = "stTrk:type";

    stFnt["name"] = "stFnt:name";
    stFnt["type"] = "stFnt:type";
    stFnt["version"] = "stFnt:version";
    stFnt["encoding"] = "stFnt:encoding";
    stFnt["actualName"] = "stFnt:actualName";
    stFnt["actualType"] = "stFnt:actualType";
    stFnt["actualVersion"] = "stFnt:actualVersion";
    stFnt["language"] = "stFnt:language";
    stFnt["tsume"] = "stFnt:tsume";

    stRef["documentID"] = "stRef:DocumentID";
    stRef["versionID"] = "stRef:VersionID";
    stRef["renditionClass"] = "stRef:RenditionClass";

    stVer["comments"] = "stVer:comments";
    stVer["event"] = "stVer:event";
    stVer["modifyDate"] = "stVer:modifyDate";
    stVer["modifier"] = "stVer:modifier";
    stVer["version"] = "stVer:version";

    stEvt["action"] = "stEvt:action";
    stEvt["parameters"] = "stEvt:parameters";
    stEvt["softwareAgent"] = "stEvt:softwareAgent";
    stEvt["when"] = "stEvt:when";

    stDsp["os"] = "stDsp:os";
    stDsp["path"] = "stDsp:path";
    stDsp["name"] = "stDsp:name";

    p.first = XAP_NS_XAP_G;
    p.second = "NaturalDimensions";
    (*MetaXAP_subTypeMap)[p] = stDim;

    p.first = XAP_NS_XAP_G_IMG;
    p.second = "Dimensions";
    (*MetaXAP_subTypeMap)[p] = stDim;
    p.second = "Resolution";
    (*MetaXAP_subTypeMap)[p] = stRes;

    p.first = XAP_NS_XAP_DYN;
    p.second = "Tracks";
    (*MetaXAP_subTypeMap)[p] = stTrk;

    p.first = XAP_NS_XAP_DYN_V;
    p.second = "Dimensions";
    (*MetaXAP_subTypeMap)[p] = stDim;

    p.first = XAP_NS_XAP_T;
    p.second = "FontList";
    (*MetaXAP_subTypeMap)[p] = stFnt;

    p.first = XAP_NS_XAP_T_PG;
    p.second = "MaxPageSize";
    (*MetaXAP_subTypeMap)[p] = stDim;

    p.first = XAP_NS_XAP_MM;
    p.second = "Versions";
    (*MetaXAP_subTypeMap)[p] = stVer;
    p.second = "RenditionOf";
    (*MetaXAP_subTypeMap)[p] = stRef;
    p.second = "ContainedResources";
    (*MetaXAP_subTypeMap)[p] = stRef;
    p.second = "ContributorResources";
    (*MetaXAP_subTypeMap)[p] = stRef;
    p.second = "History";
    (*MetaXAP_subTypeMap)[p] = stEvt;

    p.first = XAP_NS_XAP_S;
    p.second = "FileDisposition";
    (*MetaXAP_subTypeMap)[p] = stDsp;
#endif
    
	#if 0
	{
	    XAPTk_StringByString::const_iterator regNS;
	    cout << endl << "Initial namespace map @ 0x" << ios::hex << (int)MetaXAP_nsMap << ios::dec << endl;
	    for ( regNS = MetaXAP_nsMap->begin(); regNS != MetaXAP_nsMap->end(); ++regNS ) {
	    	cout << '\t' << regNS->first << ":\t " << regNS->second << endl;
	    }
	}
	#endif

}


#ifndef XMP_SetAllAliases
	#define XMP_SetAllAliases	0
#endif

static void
MetaXAP_InitAliasMap ()
{

    if (MetaXAP_aliasMap != NULL) return;
    
    MetaXAP_aliasMap = new XMP_Debug_new MetaXAP_AliasMap;
    /* [first: alias<ns,path>, second: actual<MetaXAP::Actual>] */
    MetaXAP_resolvedAliases = new XMP_Debug_new MetaXAP_ResolvedAliases;
    /* [first: alias<ns,path>, second: MetaXAP_InfoMap] */

    /*
    The following aliases are established by the format (human text) specification of the schemas, 
    so we go ahead and hard-code them here. This makes them available to MetaXAP objects in the
    absence of schema objects. They are redundantly defined in the schema files as well.
    */

	// Aliases from XMP to DC.
    setAlias ( XAP_NS_XAP, "Author",       XAP_NS_DC, "creator/*[1]", xap_seq );
    setAlias ( XAP_NS_XAP, "Authors",      XAP_NS_DC, "creator", xap_seq );
    setAlias ( XAP_NS_XAP, "Description",  XAP_NS_DC, "description", xap_alt );
    setAlias ( XAP_NS_XAP, "Format",       XAP_NS_DC, "format", xap_sct_unknown );
    setAlias ( XAP_NS_XAP, "Keywords",     XAP_NS_DC, "subject", xap_bag );
    setAlias ( XAP_NS_XAP, "Locale",       XAP_NS_DC, "language", xap_bag );
    setAlias ( XAP_NS_XAP, "Title",        XAP_NS_DC, "title", xap_alt );
    setAlias ( XAP_NS_XAP_RIGHTS, "Copyright", XAP_NS_DC, "rights", xap_alt );

	#if XMP_SetAllAliases
		MetaXAP::SetStandardAliases ( "" );
	#else
		MetaXAP::SetStandardAliases ( XAP_NS_PDF );
    #endif


#ifdef XXX
    ofstream* f = new ofstream("alias.map", ios_base::out | ios_base::trunc/* | ios_base::binary*/);
    if (!(f)) {
        cerr << "Error: alias.map" << endl;
        exit(-1);
    }
    ostream err = *f;

    // Dump state of maps
    MetaXAP_ResolvedAliases::const_iterator n = MetaXAP_resolvedAliases->begin();
    MetaXAP_ResolvedAliases::const_iterator nDone = MetaXAP_resolvedAliases->end();
    MetaXAP_InfoMap::const_iterator m;
    MetaXAP_InfoMap::const_iterator mDone;
    for (; n != nDone; ++n) {
        err << n->first.second << ", " << n->first.first << ":" << endl;
        m = n->second.begin();
        mDone = n->second.end();
        for (; m != mDone; ++m) {
            err << "  " << m->first.second << "(" ;
            const MetaXAP_AliasInfo& inf = m->second;
            if (inf.actual)
                err << "A";
            if (inf.aliasSingle)
                err << "s";
            if (inf.actualFirst)
                err << "1";
            if (inf.actualLang)
                err << "L";
            switch (inf.cType) {
                case xap_alt: err << "alt"; break;
                case xap_bag: err << "bag"; break;
                case xap_seq: err << "seq"; break;
                default: err << "?"; break;
            }
            err << "), " << m->first.first << endl;
        }
    int xxx = 1;
    }
    err << "=======================" << endl;

    *f << flush;
    delete f;
#endif
}


void
XAPTk_InitMetaXAP()
{

    MetaXAP_InitNSMap();
    MetaXAP_InitAliasMap();

}


void
XAPTk_KillMetaXAP()
{
    // From MetaXAP_InitAliasMap
    delete MetaXAP_resolvedAliases;
    MetaXAP_resolvedAliases = NULL;
    delete MetaXAP_aliasMap;
    MetaXAP_aliasMap = NULL;
    // From MetaXAP_InitNSMap
    delete MetaXAP_nsMap;
    MetaXAP_nsMap = NULL;
}


#if ! XAP_DEBUG_BUILD
	void
	XMPAssertFailure()
	{
		throw xap_assert_failure();
	}
#endif


/*
$Log$
*/
