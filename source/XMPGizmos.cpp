/************************************************************************/
/*      $Id: XMPGizmos.cpp,v 1.1 2005/03/04 16:02:43 joman Exp joman $    */
/*                                                                      */
/*      Include file name:  XMPGizmos.cpp				*/
/*                                                                      */
/************************************************************************/
/*      Revisions:                                                      */
/*                                                                      */
/*      Version 0.01 - Fri Mar  4 10:53:34 EST 2005:                    */
/*              First coding.                                           */
/************************************************************************/

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


#include "d70reader.h"
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

extern "C" void free_xmp_data_header_list(struct xmp_data *header_ptr);

// =================================================================================================
// BogusXAPClock::
// ===============


static XAPDateTime	sTimeOrigin	= { 0, 0, 0 , 1, 1, 2000, 0, 0, 0, 0 };
int iGlobalGizmoVerbos = 0;
int iCtr1 = 0;

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

static struct xmp_data *
DumpSubtree ( MetaXAP& meta, const int tabLevel, string& rootNS, string& rootPath )
{
	struct xmp_data *xmp_sub_data_ptr = NULL;
	struct xmp_data *xmp_data_ptr = NULL;
	struct xmp_data *start_xmp_data_ptr = NULL;
	const char *c_str;
	XAPPaths *	paths;
	string		ns, prop, value, childPath;
	XAPFeatures	features;
	int			tabCount;
	
	XAPValForm				form;
	XAPStructContainerType	cont;

	try
	{
		paths = meta.enumerate ( rootNS, rootPath, 1 );
	}
	catch( ... ) { return xmp_data_ptr; }

	if ( paths == NULL ) 
		return xmp_data_ptr;

	while ( paths->hasMorePaths() ) {
	
		paths->nextPath ( ns, prop );
		childPath = rootPath + '/' + prop;
		
		if(xmp_data_ptr == NULL)
		{
			xmp_data_ptr = get_new_xmp_data_ptr();
			xmp_data_ptr->next = NULL;
			xmp_data_ptr->tag = 0;
			start_xmp_data_ptr = xmp_data_ptr;
		}
		else
		{
			xmp_data_ptr->next = get_new_xmp_data_ptr();
			xmp_data_ptr =  xmp_data_ptr->next;
			xmp_data_ptr->next = NULL;
			xmp_data_ptr->tag = 0;
		}

		for ( tabCount = tabLevel; tabCount > 10; tabCount -= 10 ) 
			if(iGlobalGizmoVerbos)
				cout << sTenTabs;

		if(iGlobalGizmoVerbos)
			cout << (sTabEnd - tabCount) << prop;
		
		c_str = prop.c_str();

		xmp_data_ptr->description = (char *) malloc(strlen(c_str) + 1);
		memset(xmp_data_ptr->description, (int) '\0', strlen(c_str) + 1);
		memcpy(xmp_data_ptr->description, c_str, strlen(c_str));

		cont = xap_sct_unknown;
		try {
			form = meta.getForm ( ns, childPath );
			if ( form == xap_container ) cont = meta.getContainerType ( ns, childPath );
		} catch ( ... ) {		}

		if ( cont != xap_sct_unknown ) 
			if(iGlobalGizmoVerbos)
				cout << " : " << sContKinds[cont];
		
		if ( meta.get ( ns, childPath, value, features ) ) 
		{
			if(iGlobalGizmoVerbos)
				cout << " = \"" << value << "\"";

			if(!prop.compare("Function"))
			{
				xmp_data_ptr->tag = 0x0008;

				if(!value.compare("False"))
					value = string("NORMAL");
				else
					value = string("   ");
			}

			if(!prop.compare("Fired"))
			{
				xmp_data_ptr->tag = 0x0009;

				if(!value.compare("True"))
					value = string("Built-in,TTL");
				else
					value = string("   ");
			}

			c_str = value.c_str();

			xmp_data_ptr->value = (char *) malloc(strlen(c_str) + 1);
			memset(xmp_data_ptr->value, (int) '\0', strlen(c_str) + 1);
			memcpy(xmp_data_ptr->value, c_str, strlen(c_str));

			if ( features != 0 ) {
				if(iGlobalGizmoVerbos)
				{
					cout << ", features = 0x";
					cout.setf ( ios_base::hex, ios_base::basefield );
					cout << features;
					cout.setf ( ios_base::dec, ios_base::basefield );
				}
			}

			if ( meta.get ( ns, childPath+"/@xml:lang", value, features ) ) {
				if ( prop != "*[@xml:lang='"+value+"']" ) {		// The lang might be in the XPath.
					if(iGlobalGizmoVerbos)
						cout << ", xml:lang = '" << value << '\'';
				}
			}
		}

		if(iGlobalGizmoVerbos)
			cout << endl;

		xmp_sub_data_ptr = DumpSubtree ( meta, tabLevel+1, ns, childPath );

		if(xmp_sub_data_ptr != NULL)
		{
			xmp_data_ptr->next = xmp_sub_data_ptr;

			do
			{
				xmp_data_ptr =  xmp_data_ptr->next;
			}
			while(xmp_data_ptr != NULL);
		}
	}

	delete paths;

	return start_xmp_data_ptr;
}

static struct xmp_data *
FancyDumpMetaXAP ( MetaXAP& meta, XAPPaths * rootPaths )
{
	struct xmp_data *xmp_sub_data_ptr = NULL;
	struct xmp_data *xmp_data_ptr = NULL;
	struct xmp_data *start_xmp_data_ptr = NULL;
	struct xmp_data *temp_xmp_data_ptr = NULL;
	struct xmp_data *xmp_data_final_ptr = NULL;
	struct xmp_data *start_xmp_data_final_ptr = NULL;
	const char *c_str;

	string		doc, group, ns, prop, value;
	XAPFeatures	features;
	
	XAPValForm				form;
	XAPStructContainerType	cont;

	if ( rootPaths == NULL ) {
		cout << "*** No properties to enumerate!" << endl;
		return xmp_data_ptr;
	}
	
	group = "";
	
	while ( rootPaths->hasMorePaths() ) 
	{
		rootPaths->nextPath ( ns, prop );

		if(xmp_data_ptr == NULL)
		{
			xmp_data_ptr = get_new_xmp_data_ptr();
			xmp_data_ptr->next = NULL;
			xmp_data_ptr->tag = 0;
			start_xmp_data_ptr = xmp_data_ptr;
			temp_xmp_data_ptr = xmp_data_ptr;
		}
		else
		{
			xmp_data_ptr->next = get_new_xmp_data_ptr();
			xmp_data_ptr =  xmp_data_ptr->next;
			xmp_data_ptr->next = NULL;
			xmp_data_ptr->tag = 0;
		}
		
		if ( ns != group ) {
			group = ns;

			if(iGlobalGizmoVerbos)
				cout << "\n\t" << group << endl;
		}
		
		if(iGlobalGizmoVerbos)
			cout << "\t\t" << prop;
		
		if(!prop.compare("WhiteBalance"))
			xmp_data_ptr->tag = 0x0005;

		if(!prop.compare("Lens"))
			xmp_data_ptr->tag = 0x0084;

		c_str = prop.c_str();

		xmp_data_ptr->description = (char *) malloc(strlen(c_str) + 1);
		memset(xmp_data_ptr->description, (int) '\0', strlen(c_str) + 1);
		memcpy(xmp_data_ptr->description, c_str, strlen(c_str));

		cont = xap_sct_unknown;

		try {
			form = meta.getForm ( ns, prop );
			if ( form == xap_container ) cont = meta.getContainerType ( ns, prop );
		} catch ( ... ) {		}

		if ( cont != xap_sct_unknown ) 
			if(iGlobalGizmoVerbos)
				cout << " : " << sContKinds[cont];

		if ( meta.get ( ns, prop, value, features ) ) {
			if(iGlobalGizmoVerbos)
				cout << " = \"" << value << "\"";

			if(!prop.compare("DateTimeOriginal"))
			{
				xmp_data_ptr->tag = 0x9004;

				value = value.replace(value.find_first_of("T", 0), 1, " ");
				value = value.replace(value.find_first_of("-", 0), 1, ":");
				value = value.replace(value.find_first_of("-", 0), 1, ":");
				value = value.substr(0, value.find_first_of("-", 0));
				value = value.substr(0, value.find_first_of("+", 0));
			}

			if(!prop.compare("ColorSpace"))
			{
				xmp_data_ptr->tag = 0x008d;

				if(!value.compare("1"))
					value = string("sRGB");
				else
					value = string("Unknown");
			}

			c_str = value.c_str();

			xmp_data_ptr->value = (char *) malloc(strlen(c_str) + 1);
			memset(xmp_data_ptr->value, (int) '\0', strlen(c_str) + 1);
			memcpy(xmp_data_ptr->value, c_str, strlen(c_str));

			if ( features != 0 ) {
				if(iGlobalGizmoVerbos)
				{
					cout << ", features = 0x";
					cout.setf ( ios_base::hex, ios_base::basefield );
					cout << features;
					cout.setf ( ios_base::dec, ios_base::basefield );
				}
			}
			if ( meta.get ( ns, prop+"/@xml:lang", value, features ) ) {
				if(iGlobalGizmoVerbos)
					cout << ", xml:lang = '" << value << '\'';
			}
		}

		if(iGlobalGizmoVerbos)
			cout << endl;

		if(xmp_data_ptr->value == NULL)
			xmp_data_ptr->value = strdup("xxx");

		xmp_sub_data_ptr = DumpSubtree ( meta, 3, ns, prop );

		if(xmp_sub_data_ptr != NULL)
		{
			xmp_data_ptr->next = xmp_sub_data_ptr;

			do
			{
				if(xmp_data_ptr->next != NULL)
					xmp_data_ptr =  xmp_data_ptr->next;
			}
			while(xmp_data_ptr->next != NULL);
		}
	}

	if(start_xmp_data_ptr != NULL)
	{
		do
		{
			if(start_xmp_data_ptr->tag != 0)
			{
				if(xmp_data_final_ptr == NULL)
				{
					xmp_data_final_ptr = get_new_xmp_data_ptr();
					xmp_data_final_ptr->next = NULL;
					start_xmp_data_final_ptr = xmp_data_final_ptr;
				}
				else
				{
					xmp_data_final_ptr->next = get_new_xmp_data_ptr();
					xmp_data_final_ptr =  xmp_data_final_ptr->next;
					xmp_data_final_ptr->next = NULL;
				}

				xmp_data_final_ptr->tag = start_xmp_data_ptr->tag;

				xmp_data_final_ptr->value = (char *) malloc(strlen(start_xmp_data_ptr->value) + 1);
				memset(xmp_data_final_ptr->value, (int) '\0', strlen(start_xmp_data_ptr->value) + 1);
				memcpy(xmp_data_final_ptr->value, start_xmp_data_ptr->value, strlen(start_xmp_data_ptr->value));

				xmp_data_final_ptr->description = (char *) malloc(strlen(start_xmp_data_ptr->description) + 1);
				memset(xmp_data_final_ptr->description, (int) '\0', strlen(start_xmp_data_ptr->description) + 1);
				memcpy(xmp_data_final_ptr->description, start_xmp_data_ptr->value, strlen(start_xmp_data_ptr->description));
			}

			start_xmp_data_ptr =  start_xmp_data_ptr->next;
		}
		while(start_xmp_data_ptr->next != NULL);

		free_xmp_data_header_list(start_xmp_data_ptr);
	}

	meta.getResourceRef ( doc );

	if(iGlobalGizmoVerbos)
		cout << endl << "// End of tree dump for \"" << doc << "\"" << endl;

	return start_xmp_data_final_ptr;
}	// FancyDumpMetaXAP ()

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
void free_xmp_data_header_list(struct xmp_data *header_ptr)
{
	struct xmp_data *temp_header_ptr = NULL;

	while(header_ptr->next != NULL)
	{
		temp_header_ptr = header_ptr;
		header_ptr = header_ptr->next;
		free(temp_header_ptr);
	}

	free(header_ptr);
}

// =================================================================================================
// DumpMetaXAP ()
// ==============

static void
AnnounceDump ( MetaXAP& meta, const char * rootNS, const char * rootPath, const int steps )
{
	string	doc;

	meta.getResourceRef ( doc );

	if(iGlobalGizmoVerbos)
		if(iGlobalGizmoVerbos)
			cout << endl << "// Dumping MetaXAP tree for \"" << doc << "\"";
	
	if ( rootNS != NULL ) 
		if(iGlobalGizmoVerbos)
			cout << " from " << rootNS << " : " << rootPath;

	if ( steps > 0 ) 
		if(iGlobalGizmoVerbos)
			cout << " to depth " << steps;
	
	if(iGlobalGizmoVerbos)
		cout << endl;

	if ( ! meta.isEnabled ( XAP_OPTION_ALIASING_ON ) ) 
		if(iGlobalGizmoVerbos)
			cout << "*** Aliasing is off! ***" << endl;
	
}	// AnnounceDump ()

struct xmp_data *
DumpMetaXAP( MetaXAP& meta, const int iVerbose, const int steps /*=-1*/ )
{
	struct xmp_data *xmp_data_ptr = NULL;
	iGlobalGizmoVerbos = iVerbose;

	XAPPaths *	paths	= NULL;

	AnnounceDump ( meta, NULL, NULL, steps );
	
	if ( steps < 0 ) {
		paths = meta.enumerate ( 1 );
		xmp_data_ptr = FancyDumpMetaXAP ( meta, paths );
	} else {
		paths = meta.enumerate ( steps );
		SimpleDumpMetaXAP ( meta, paths );
	}

	delete paths;

	return xmp_data_ptr;

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

//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------
struct xmp_data *get_new_xmp_data_ptr()
{
	struct xmp_data *local_header_ptr;
	local_header_ptr = (struct xmp_data *) malloc(sizeof(struct xmp_data));
	memset(local_header_ptr, (int) '\0', sizeof(struct xmp_data));
	return(local_header_ptr);
}

