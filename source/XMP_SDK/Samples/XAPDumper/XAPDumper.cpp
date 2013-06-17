
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


// =================================================================================================
// ProcessSubstring
// ================

static string *	sSerialPrefix	= 0;

static void
ProcessSubstring ( string&		xmlString,
				   size_t		offset,
				   size_t		length,
				   const char *	docName )
{
	MetaXAP *	localMeta	= NULL;
	
	cout << endl << "// Dumping raw input for \"" << docName
		 << "\" (" << offset << ".." << (offset + length - 1) << ')' << endl << endl;
	cout << xmlString.substr ( offset, length ) << endl;
	
	localMeta = ParseSubstring ( xmlString, offset, length, docName );
	
	if ( localMeta != NULL ) {

		DumpMetaXAP ( *localMeta );
		SerializeMetaXAP ( *localMeta, xap_format_pretty );
		SerializeMetaXAP ( *localMeta, xap_format_compact );
		
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
ProcessFile ( const char * fileName  )
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
				ProcessSubstring ( fileContents, 0, fileContents.size(), fileName );
			}

		} else {
		
			snips = new PacketScanner::SnipInfoVector (snipCount);
			scanner->Report ( *snips );
		
			for ( int s = 0; s < snipCount; s++ ) {
				if ( (*snips)[s].fState != PacketScanner::eValidPacketSnip ) continue;
				ProcessSubstring ( fileContents, (*snips)[s].fOffset, (*snips)[s].fLength, fileName );
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

int main(int argc, char *argv[], char *env[])
{
/*
    while(filename = *++argv)
    {
		if(stat(filename, &buf) == (-1))
		{
			perror(filename);
		}
		else
		{
			if(iThumbnail)
				ProcessFileJPG(argc, filename);
			else
				ProcessFile(argc, filename);
		}
	}
*/
	struct stat buf;
	char *argv_filename;

    while(argv_filename = *++argv)
    {
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
					ProcessFile ( fileName.c_str() );
			}
		}
	}
	
	return 0;

}	// main ()

