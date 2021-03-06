/************************************************************************/
/*      $Id: XAPDumper.cpp,v 1.1 2005/03/04 16:02:09 joman Exp joman $    */
/*                                                                      */
/*      Include file name:   XAPDumper.cpp                              */
/*                                                                      */
/************************************************************************/
/*      Revisions:                                                      */
/*                                                                      */
/*      Version 0.01 - Fri Mar  4 10:53:34 EST 2005:                    */
/*              First coding.                                           */
/************************************************************************/

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/


// =================================================================================================
// XAPDumper
// =========
//
// Simple dumper of XAP metadata.  Reads a file, parses it, and dumps the internal tree.
//
// =================================================================================================


#include "XAPToolkit.h"
#include "XMLPacketScanner.h"
#include "XMPGizmos.h"
#include "d70reader.h"
#include <sys/stat.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;
using namespace XML;


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

extern "C" int xmp_main(char *argv_filename);
extern "C" struct xmp_data *xmp_display(const char * fileName, unsigned long length, char *data_ptr, int iVerbos);

// =================================================================================================
// ProcessSubstring
// ================

static string *	sSerialPrefix	= 0;
int iGlobalVerbos = 0;

static struct xmp_data *
ProcessSubstring ( string&		xmlString,
				   size_t		offset,
				   size_t		length,
				   const char *	docName,
				   int			iVerbos)
{
	struct xmp_data *xmp_data_ptr = NULL;
	MetaXAP *	localMeta	= NULL;

	localMeta = ParseSubstring ( xmlString, offset, length, docName );
	
	if ( localMeta != NULL ) {

		xmp_data_ptr = DumpMetaXAP ( *localMeta, iVerbos, -1 );
		
		if ( sSerialPrefix != 0 ) {
			string	fullName	= *sSerialPrefix + "_pretty.xmp";
			AppendFile ( *localMeta, fullName.c_str(), xap_format_pretty );
			fullName = *sSerialPrefix + "_compact.xmp";
			AppendFile ( *localMeta, fullName.c_str(), xap_format_compact );
		}

		delete localMeta;

	}

	return xmp_data_ptr;
}	// ProcessSubstring ()


static void
ProcessSubstring ( string&		xmlString,
				   size_t		offset,
				   size_t		length,
				   const char *	docName,
				   struct xmp_data *xmp_data_ptr,
				   int			iVerbos)
{
	MetaXAP *	localMeta	= NULL;

	localMeta = ParseSubstring ( xmlString, offset, length, docName );
	
	if ( localMeta != NULL ) {

		DumpMetaXAP ( *localMeta, iVerbos, -1 );
		
		if ( sSerialPrefix != 0 ) {
			string	fullName	= *sSerialPrefix + "_pretty.xmp";
			AppendFile ( *localMeta, fullName.c_str(), xap_format_pretty );
			fullName = *sSerialPrefix + "_compact.xmp";
			AppendFile ( *localMeta, fullName.c_str(), xap_format_compact );
		}

		delete localMeta;

	}

}	// ProcessSubstring ()


// =================================================================================================
// ProcessFile ()
// ==============

static void
ProcessFile ( const char * fileName, struct xmp_data *xmp_data_ptr  )
{
	string	fileContents;
	int		snipCount;

	PacketScanner *					scanner	= NULL;
	PacketScanner::SnipInfoVector *	snips	= NULL;
	
	// ---------------------------------------------------------------------
	// Use the scanner to find all of the packets then process each of them.
	
	try {
	
		if ( sSerialPrefix != 0 ) {	// Make the ofstream destructor delete existing file contents.
			string		prettyName	= *sSerialPrefix + "_pretty.xmp";
			string		compactName	= *sSerialPrefix + "_compact.xmp";
			ofstream	prettyFile ( prettyName.c_str(), (IOSBase::out | IOSBase::binary) );
			ofstream	compactFile ( compactName.c_str(), (IOSBase::out | IOSBase::binary) );
		}

		InhaleFile ( fileName, fileContents );
		if ( fileContents.size() == 0 ) return;

		scanner = new PacketScanner ( fileContents.size() );
		scanner->Scan ( fileContents.data(), 0, fileContents.size() );
		snipCount = scanner->GetSnipCount();
		
		if ( snipCount == 1 ) {

			string	answer;
			cout << "      No packets, dump all: (y/n) ";
			cin >> answer;

			if ( (answer[0] == 'y') || (answer[0] == 'Y') ) {
				ProcessSubstring ( fileContents, 0, fileContents.size(), fileName, xmp_data_ptr, iGlobalVerbos);
			}

		} else {
		
			snips = new PacketScanner::SnipInfoVector (snipCount);
			scanner->Report ( *snips );
		
			for ( int s = 0; s < snipCount; s++ ) {
				if ( (*snips)[s].fState != PacketScanner::eValidPacketSnip ) continue;
				ProcessSubstring ( fileContents, (*snips)[s].fOffset, (*snips)[s].fLength, fileName, xmp_data_ptr, iGlobalVerbos);
			}
		
		}

	} catch ( ... ) {
	
		cout << endl << "XAPDumper failed for \"" << fileName << '\"' << endl;
	
	}
	
	// ---------------
	// Done, clean up.
	
	delete scanner;
	delete snips;

}	// ProcessFile ()


// =================================================================================================
// main ()
// =======

int xmp_main(char *argv_filename, struct xmp_data *xmp_data_ptr)
{
	struct stat buf;
	
	if(stat(argv_filename, &buf) == (-1))
	{
		perror(argv_filename);
	}
	else
	{
		string	fileName ( argv_filename );

		if ( fileName.length() > 0 ) 
		{
			if ( (fileName[fileName.length()] == '\n') || (fileName[fileName.length()] == '\r') ) 
			{
				fileName.erase ( fileName.length(), 1 );	// Remove eol, allowing for CRLF.

				if ( (fileName[fileName.length()] == '\n') || (fileName[fileName.length()] == '\r') ) 
				{
					fileName.erase ( fileName.length(), 1 );
				}
			}
			
			// Dragging an icon on Windows pastes a quoted path.
			if ( fileName[fileName.length()] == '"' ) 
				fileName.erase ( fileName.length(), 1 );

			if ( fileName[0] == '"' ) 
				fileName.erase ( 0, 1 );

			if ( fileName.length() > 0 ) 
				ProcessFile ( fileName.c_str(), xmp_data_ptr );
		}
	}
	
	return 0;

}

// =================================================================================================
// 
// ==============
struct xmp_data *xmp_display(const char * fileName, unsigned long length, char *data_ptr, int iVerbos)
{
	struct xmp_data *xmp_data_ptr = NULL;
	string	fileContents(data_ptr, length);
	int		snipCount;

	iGlobalVerbos = iVerbos;

	PacketScanner *					scanner	= NULL;
	PacketScanner::SnipInfoVector *	snips	= NULL;
	
	// ---------------------------------------------------------------------
	// Use the scanner to find all of the packets then process each of them.
	
	try {
	
		if ( fileContents.size() == 0 ) 
			return NULL;

		scanner = new PacketScanner ( fileContents.size() );
		scanner->Scan ( fileContents.data(), 0, fileContents.size() );
		snipCount = scanner->GetSnipCount();
		
		if ( snipCount == 1 ) {

			cout << "\nNo XMP packets in image file,\n";
/*
			string	answer;
			cout << "      No packets, dump all: (y/n) ";
			cin >> answer;

			if ( (answer[0] == 'y') || (answer[0] == 'Y') ) {
				ProcessSubstring ( fileContents, 0, fileContents.size(), fileName );
			}
*/
		} else {
		
			snips = new PacketScanner::SnipInfoVector (snipCount);
			scanner->Report ( *snips );
		
			for ( int s = 0; s < snipCount; s++ ) {
				if ( (*snips)[s].fState != PacketScanner::eValidPacketSnip ) continue;
				xmp_data_ptr = ProcessSubstring ( fileContents, (*snips)[s].fOffset, (*snips)[s].fLength, fileName, iVerbos);
			}
		
		}

	} catch ( ... ) {
	
		cout << endl << "XAPDumper failed for \"" << fileName << '\"' << endl;
	
	}
	
	// ---------------
	// Done, clean up.
	
	delete scanner;
	delete snips;

	return xmp_data_ptr;
}
