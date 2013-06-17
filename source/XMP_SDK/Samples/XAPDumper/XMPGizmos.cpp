
// =================================================================================================
//
// ADOBE SYSTEMS INCORPORATED
// Copyright 2001 Adobe Systems Incorporated
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
// terms of the Adobe license agreement accompanying it.
//
// =================================================================================================


#include "XMPGizmos.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;


// ------------------------------------------------------------------------------------------------
// Some systems have pre-standard STL implementations, using ios instead of basic_ios and ios_base.

#ifndef UseDefunctIOSClass
	#define UseDefunctIOSClass	0
#endif

#if UseDefunctIOSClass
	#define IOSBase	ios
#else
	#define IOSBase	ios_base
#endif


// =================================================================================================
// BogusXAPClock::
// ===============


static XAPDateTime	sTimeOrigin	= { 0, 0, 0 , 1, 1, 2000, 0, 0, 0, 0 };


BogusXAPClock::BogusXAPClock() :
	prevTime ( sTimeOrigin )
{
}	// BogusXAPClock::BogusXAPClock ()

// -------------------------------------------------------------------------------------------------

void
BogusXAPClock::reset()
{
	prevTime = sTimeOrigin;	// Start at midnight of January 1, 2000.
}	// BogusXAPClock::reset ()

// -------------------------------------------------------------------------------------------------

void
BogusXAPClock::timestamp ( XAPDateTime& dt )
{

	prevTime.sec++;	// Increment by one second from the prvious time.

	if ( prevTime.sec == 60 ) {
		prevTime.sec = 0;
		prevTime.min++;
		if ( prevTime.min == 60 ) {
			prevTime.min = 0;
			prevTime.hour++;
			if ( prevTime.hour == 24 ) {
				prevTime.hour = 0;
				prevTime.mday++;
				if ( prevTime.mday == 31 ) {
					prevTime.mday = 1;
					prevTime.month++;
					if ( prevTime.month == 13 ) {
						prevTime.month = 1;
						prevTime.year++;
					}
				}
			}
		}
	}

	dt = prevTime;

}	// BogusXAPClock::timestamp ()


// =================================================================================================
// BasicXAPClock::
// ===============


BasicXAPClock::BasicXAPClock() :
	prevDaySec ( 0 ),
	nextSeq ( 0 )
{
}	// BasicXAPClock::BasicXAPClock ()

// -------------------------------------------------------------------------------------------------

void
BasicXAPClock::timestamp ( XAPDateTime& dt )
{
	time_t		now		= time ( NULL );	// Get the current "binary" time.
	struct tm *	tmPtr	= gmtime ( &now );	// Break up the binary time.
	if ( tmPtr == NULL ) tmPtr = localtime ( &now );
	
	if ( tmPtr == NULL ) {
	
		dt = sTimeOrigin;
		dt.year	= 0;

	} else {

		dt.sec		= tmPtr->tm_sec;			// ! ANSI allows 2 leap seconds, pass them through.
		dt.min		= tmPtr->tm_min;
		dt.hour		= tmPtr->tm_hour;
		dt.mday		= tmPtr->tm_mday;
		dt.month	= tmPtr->tm_mon + 1;		// ! ANSI counts months as 0..11.
		dt.year		= tmPtr->tm_year + 1900;	// ! ANSI counts years since 1900.
		dt.tzHour	= 0;
		dt.tzMin	= 0;
		dt.nano		= 0;

	}

	long	daySec	= (((dt.hour * 60) + dt.min) * 60) + dt.sec;
	if ( daySec != prevDaySec ) {
		nextSeq = 0;			// This avoids problems with prevSeq wrapping around within
		prevDaySec = daySec;	//	the same second, and thus making time go backwards.
	}
	dt.seq = nextSeq;
	nextSeq++;

}	// BasicXAPClock::timestamp ()


// =================================================================================================
// MakeNewMetaXAP ()
// =================

MetaXAP *
MakeNewMetaXAP ( const char * docName, XAPClock * clock )
{

	MetaXAP *	localMeta;
	
	if ( clock == NULL ) {
		localMeta = new MetaXAP();
	} else {
		localMeta = new MetaXAP ( clock );
	}
    if ( localMeta == NULL ) goto AllocateError;

	try {

	    localMeta->enable ( XAP_OPTION_XAPMETA_ONLY, false );
		localMeta->setResourceRef ( docName );
		localMeta->enable ( XAP_OPTION_XAPMETA_OUTPUT, true );

	} catch ( ... ) {
	
		goto InitializeError;
	
	}

EXIT:
	return localMeta;

ERROR:
	delete localMeta;
	localMeta = NULL;
	goto EXIT;

AllocateError:
	cout << endl << "*** MakeNewMetaXAP failed to allocate for \"" << docName << "\"" << endl;
	goto ERROR;

InitializeError:
	cout << endl << "*** MakeNewMetaXAP failed to initialize for \"" << docName << "\"" << endl;
	goto ERROR;
	
}	// MakeNewMetaXAP ()


// =================================================================================================
// ParseSubstring ()
// =================

MetaXAP *
ParseSubstring ( const std::string& xmlString, size_t offset, size_t length, const char * docName )
{
	MetaXAP *	localMeta	= NULL;
	
    localMeta = MakeNewMetaXAP ( docName );
    if ( localMeta == NULL ) goto ERROR;
	
	try {
		localMeta->parse ( (xmlString.data() + offset), length, true );
	} catch ( const std::logic_error& lerr ) {
		cout << "Caught logic error " << lerr.what() << endl;
		goto ERROR;
	} catch ( const std::runtime_error& lerr ) {
		cout << "Caught runtime error " << lerr.what() << endl;
		goto ERROR;
    } catch (...) {
		cout << "Caught unexpected exception" << endl;
		goto ERROR;
	}

EXIT:
	return localMeta;

ERROR:
	cout << endl << "*** ParseSubstring failed" << endl;
	delete localMeta;
	localMeta = NULL;
	goto EXIT;
	
}	// ParseSubstring ()


// =================================================================================================
// SerializeToString ()
// ====================

void
SerializeToString ( MetaXAP& meta, std::string& xmlString, XAPFormatType format, int crlfFilter )
{
	const int	bufferSize	= 4096;
	char 		buffer [bufferSize];
	long		ioCount;

	xmlString.erase ( xmlString.begin(), xmlString.end() );
	
	if ( crlfFilter == -1 ) {
		ioCount = meta.serialize ( format );		// Passing -1 means use the default.
	} else {
		ioCount = meta.serialize ( format, crlfFilter );
	}
	
    if ( ioCount == 0 ) {
    	cout << "*** SerializeToString failed" << endl;
    } else {
		while ( true ) {
			ioCount = meta.extractSerialization ( buffer, bufferSize-1 );
			if ( ioCount == 0 ) break;
			xmlString.insert ( xmlString.size(), buffer, ioCount );
		}
    }
   
}	// SerializeToString ()


// =================================================================================================
// SimpleDumpMetaXAP ()
// ====================
//
// This lets enumerate do everything.  Since enumerate only visits leaf nodes, we won't get any
// information about containers.

static void
SimpleDumpMetaXAP ( MetaXAP& meta, XAPPaths * paths )
{
	string		doc, group, ns, prop, value, propTail, langItem;
	XAPFeatures	features;

	if ( paths == NULL ) {
		cout << "*** No properties to enumerate!" << endl;
		return;
	}
	
	group = "";
	
	while ( paths->hasMorePaths() ) {
	
		paths->nextPath ( ns, prop );
		
		if ( ns != group ) {
			group = ns;
			cout << "\n\t" << group << endl;
		}
		
		cout << "\t\t" << prop;

		if ( meta.get ( ns, prop, value, features ) ) {
			cout << " = \"" << value << "\"";
			if ( features != 0 ) {
				cout << ", features = 0x";
				cout.setf ( ios_base::hex, ios_base::basefield );
				cout << features;
				cout.setf ( ios_base::dec, ios_base::basefield );
			}
			if ( meta.get ( ns, prop+"/@xml:lang", value, features ) ) {
				langItem = "/*[@xml:lang='" + value + "']";		// The lang might be in the XPath.
				propTail = prop;
				int	liSize = langItem.size();
				int	ptSize = propTail.size();
				if ( ptSize > liSize ) propTail.erase ( 0, ptSize-liSize );
				if ( propTail != langItem ) cout << ", xml:lang = '" << value << '\'';
			}
		}
		cout << endl;

	}

	meta.getResourceRef ( doc );
	cout << endl << "// End of tree dump for \"" << doc << "\"" << endl;
   
}	// SimpleDumpMetaXAP ()


// =================================================================================================
// FancyDumpMetaXAP ()
// ===================

static const char *	sTenTabs	= "\t\t\t\t\t\t\t\t\t\t";
static const char *	sTabEnd		= sTenTabs + strlen ( sTenTabs );

static const char *	sContKinds[]	= { "", "alt", "bag", "seq" };

static void
DumpSubtree ( MetaXAP& meta, const int tabLevel, string& rootNS, string& rootPath )
{
	XAPPaths *	paths;
	string		ns, prop, value, childPath;
	XAPFeatures	features;
	int			tabCount;
	
	XAPValForm				form;
	XAPStructContainerType	cont;

	paths = meta.enumerate ( rootNS, rootPath, 1 );
	if ( paths == NULL ) return;
	
	while ( paths->hasMorePaths() ) {
	
		paths->nextPath ( ns, prop );
		childPath = rootPath + '/' + prop;
		
		for ( tabCount = tabLevel; tabCount > 10; tabCount -= 10 ) cout << sTenTabs;
		cout << (sTabEnd - tabCount) << prop;
		
		cont = xap_sct_unknown;
		try {
			form = meta.getForm ( ns, childPath );
			if ( form == xap_container ) cont = meta.getContainerType ( ns, childPath );
		} catch ( ... ) {
		}
		if ( cont != xap_sct_unknown ) cout << " : " << sContKinds[cont];
		
		if ( meta.get ( ns, childPath, value, features ) ) {
			cout << " = \"" << value << "\"";
			if ( features != 0 ) {
				cout << ", features = 0x";
				cout.setf ( ios_base::hex, ios_base::basefield );
				cout << features;
				cout.setf ( ios_base::dec, ios_base::basefield );
			}
			if ( meta.get ( ns, childPath+"/@xml:lang", value, features ) ) {
				if ( prop != "*[@xml:lang='"+value+"']" ) {		// The lang might be in the XPath.
					cout << ", xml:lang = '" << value << '\'';
				}
			}
		}
		cout << endl;

		DumpSubtree ( meta, tabLevel+1, ns, childPath );

	}

	delete paths;
   
}

static void
FancyDumpMetaXAP ( MetaXAP& meta, XAPPaths * rootPaths )
{
	string		doc, group, ns, prop, value;
	XAPFeatures	features;
	
	XAPValForm				form;
	XAPStructContainerType	cont;

	if ( rootPaths == NULL ) {
		cout << "*** No properties to enumerate!" << endl;
		return;
	}
	
	group = "";
	
	while ( rootPaths->hasMorePaths() ) {
	
		rootPaths->nextPath ( ns, prop );
		
		if ( ns != group ) {
			group = ns;
			cout << "\n\t" << group << endl;
		}
		
		cout << "\t\t" << prop;
		
		cont = xap_sct_unknown;
		try {
			form = meta.getForm ( ns, prop );
			if ( form == xap_container ) cont = meta.getContainerType ( ns, prop );
		} catch ( ... ) {
		}
		if ( cont != xap_sct_unknown ) cout << " : " << sContKinds[cont];

		if ( meta.get ( ns, prop, value, features ) ) {
			cout << " = \"" << value << "\"";
			if ( features != 0 ) {
				cout << ", features = 0x";
				cout.setf ( ios_base::hex, ios_base::basefield );
				cout << features;
				cout.setf ( ios_base::dec, ios_base::basefield );
			}
			if ( meta.get ( ns, prop+"/@xml:lang", value, features ) ) {
				cout << ", xml:lang = '" << value << '\'';
			}
		}
		cout << endl;

		DumpSubtree ( meta, 3, ns, prop );

	}

	meta.getResourceRef ( doc );
	cout << endl << "// End of tree dump for \"" << doc << "\"" << endl;
   
}	// FancyDumpMetaXAP ()


// =================================================================================================
// DumpMetaXAP ()
// ==============

static void
AnnounceDump ( MetaXAP& meta, const char * rootNS, const char * rootPath, const int steps )
{
	string	doc;

	meta.getResourceRef ( doc );
	cout << endl << "// Dumping MetaXAP tree for \"" << doc << "\"";
	
	if ( rootNS != NULL ) cout << " from " << rootNS << " : " << rootPath;
	if ( steps > 0 ) cout << " to depth " << steps;
	
	cout << endl;
	if ( ! meta.isEnabled ( XAP_OPTION_ALIASING_ON ) ) cout << "*** Aliasing is off! ***" << endl;
	
}	// AnnounceDump ()

void
DumpMetaXAP ( MetaXAP& meta, const int steps /*=-1*/ )
{
	XAPPaths *	paths	= NULL;

	AnnounceDump ( meta, NULL, NULL, steps );
	
	if ( steps < 0 ) {
		paths = meta.enumerate ( 1 );
		FancyDumpMetaXAP ( meta, paths );
	} else {
		paths = meta.enumerate ( steps );
		SimpleDumpMetaXAP ( meta, paths );
	}

	delete paths;

}	// DumpMetaXAP ()

void
DumpMetaXAP ( MetaXAP& meta, const char * rootNS, const char * rootPath, const int steps /*=-1*/ )
{
	string		ns		= string ( rootNS );
	string		root	= string ( rootPath );
	XAPPaths *	paths	= NULL;

	AnnounceDump ( meta, rootNS, rootPath, steps );
	
	if ( steps < 0 ) {
		paths = meta.enumerate ( ns, root, 1 );
		FancyDumpMetaXAP ( meta, paths );
	} else {
		paths = meta.enumerate ( ns, root, steps );
		SimpleDumpMetaXAP ( meta, paths );
	}

	delete paths;

}	// DumpMetaXAP ()

void
DumpMetaXAP ( MetaXAP&				meta,
			  const XAPTimeRelOp	op,
			  const XAPDateTime&	time,
			  const XAPChangeBits	how		/*=XAP_CHANGE_MASK*/ )
{
	XAPPaths *	paths	= NULL;

	AnnounceDump ( meta, NULL, NULL, 0 );
	
	paths = meta.enumerate ( op, time, how );
	SimpleDumpMetaXAP ( meta, paths );

	delete paths;

}	// DumpMetaXAP ()


// =================================================================================================
// AnnounceSerialize ()
// ====================

static void
AnnounceSerialize ( MetaXAP& meta, const char * dest, XAPFormatType format, int crlfFilter )
{
	string	docName;

	meta.getResourceRef ( docName );

	cout << endl << "// Serializing MetaXAP tree for \"" << docName << "\"";
	if ( dest != "" ) cout << " to " << dest;
	
	if ( format == xap_format_pretty ) {
		cout << " in pretty form";
	} else if ( format == xap_format_compact ) {
		cout << " in compact form";
	} else {
		cout << " in unknown form";
	}
	
	switch ( crlfFilter ) {
		case -1 :
			cout << " with default cr/lf filtering";
			break;
		case 0 :
			cout << " with no cr/lf filtering";
			break;
		case XAP_ESCAPE_CR :
			cout << " with cr filtering";
			break;
		case XAP_ESCAPE_LF :
			cout << " with lf filtering";
			break;
		case (XAP_ESCAPE_CR | XAP_ESCAPE_LF) :
			cout << " with both cr/lf filtering";
			break;
		default :
			cout << " with unknown cr/lf filtering";
			break;
	}

	cout << endl << endl;
   
}	// AnnounceSerialize ()


// =================================================================================================
// SerializeMetaXAP ()
// ===================

void
SerializeMetaXAP ( MetaXAP& meta, XAPFormatType format, int crlfFilter )
{
	string	xmlString;

	AnnounceSerialize ( meta, "", format, crlfFilter );
	SerializeToString ( meta, xmlString, format, crlfFilter );
	cout << xmlString;
   
}	// SerializeMetaXAP ()


// =================================================================================================
// InhaleFile ()
// =============

void
InhaleFile ( const char * fileName, std::string& fileContents )
{
	enum { kBufLen = 4096 };
	
	char		fileBuf [kBufLen];
	long		ioCount;
	ifstream	file ( fileName, (IOSBase::in | IOSBase::binary) );

    if ( file.fail() ) goto EXIT;
	fileContents.erase ( fileContents.begin(), fileContents.end() );

	while ( true ) {
		ioCount = kBufLen;
		file.read ( fileBuf, ioCount );
		ioCount = file.gcount();
		if ( ioCount < 1 ) break;
		fileContents.insert ( fileContents.size(), fileBuf, ioCount );
	}
	
EXIT:
	file.close();
	
}	// InhaleFile ()


// =================================================================================================
// ParseFile ()
// ============

MetaXAP *
ParseFile ( const char * fileName, const char * docName )
{
	string	fileBuf;
	
	InhaleFile ( fileName, fileBuf );
    return ParseSubstring ( fileBuf, 0, fileBuf.size(), docName );
	
}	// ParseFile ()


// =================================================================================================
// WriteFile ()
// ============

void
WriteFile ( MetaXAP& meta, const char * fileName, XAPFormatType format, int crlfFilter )
{
	string		xmlString;
	ofstream	file ( fileName, (IOSBase::out | IOSBase::binary) );

    if ( file.fail() ) goto EXIT;

	SerializeToString ( meta, xmlString, format, crlfFilter );
	file.write ( xmlString.data(), xmlString.size() );
    
EXIT:
    file.close();
   
}	// WriteFile ()


// =================================================================================================
// AppendFile ()
// =============

void
AppendFile ( MetaXAP& meta, const char * fileName, XAPFormatType format, int crlfFilter )
{
	string		xmlString;
	ofstream	file ( fileName, (IOSBase::app | IOSBase::binary) );

    if ( file.fail() ) goto EXIT;

	SerializeToString ( meta, xmlString, format, crlfFilter );
	file.write ( xmlString.data(), xmlString.size() );
    
EXIT:
    file.close();
   
}	// AppendFile ()


// =================================================================================================

