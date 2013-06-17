
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


// =================================================================================================
// ScannerTest
// ===========
//
// Test application for the XML packet scanner.  A list of input files can be given on the command
// line.  If the command line is empty, it reads file names from standard input, one per line,
// quitting when an empty line is encountered.
//
// Each recognizable packet in the file should have start and end markers for the test driver.  The
// tags are a 5 character string of the form ["+++" | "---"] ['h' | 't'] ['G' | 'B'].  The first
// part tells the driver which way to look for the packet header or trailer.  The "+++" says to look
// forward, the "---" says look backwards.  The 'h' or 't' says whether to look for the header or
// trailer.  Looking for a header means looking for the '<' at the start of the header.  Looking for
// a trailer means looking for the '>' at the end of the trailer.  The 'G' or 'B' says if the packet
// should be a good or a bad packet.  A bad packet is syntactically legal but has a bad value for the
// "bytes" attribute, or some other semantic error.
//
// The test tool uses the marks to independantly determine where the packets ought to be, so it can
// verify the scanner's report.
//
// =================================================================================================


#include "XMLPacketScanner.h"
#include "XMPGizmos.h"

#include <string>
#include <iostream>
#include <vector>

#include <cstdlib>
#include <stddef.h>
#include <stdio.h>

#include <ctime>


#ifndef UseStringPushBack	// VC++ 6.x does not provide push_back for strings!
	#define UseStringPushBack	0
#endif


using namespace std;
using namespace XML;


// =================================================================================================


static char *	snipStateName [6]	= { "not-seen", "raw-data", "good-packet", "partial", "bad-packet" };
static char *	charFormName [6]	= { "8 bit", "?", "16 bit BE", "16 bit LE", "32 bit BE", "32 bit LE" };


// =================================================================================================
// DumpSnipInfo ()
// ===============

static void
DumpSnipInfo ( PacketScanner::SnipInfoVector& packets, string title )
{
	const int	snipCount	= packets.size();
	
	PacketScanner::SnipInfoVector::iterator	currSnip	= packets.begin();
	PacketScanner::SnipInfoVector::iterator	endSnip		= packets.end();
	
	cout << endl << title << ", " << snipCount << " snips   (kind  start..end, length)" << endl;

	for ( /* currSnip = currSnip */; currSnip != endSnip; currSnip++ ) {

		cout << '\t' << snipStateName[currSnip->fState] << '\t'
			 << currSnip->fOffset << ".." << (currSnip->fOffset + currSnip->fLength - 1)
			 << " (" << currSnip->fLength << ')';
		
		if ( (currSnip->fState == PacketScanner::eValidPacketSnip) ||
			 (currSnip->fState == PacketScanner::ePartialPacketSnip) ||
			 (currSnip->fState == PacketScanner::eBadPacketSnip) ) {
			cout << ", access = " << currSnip->fAccess;
			if ( currSnip->fBytesAttr != -1 ) cout << ", bytes = " << currSnip->fBytesAttr;
			if ( currSnip->fEncodingAttr != string() ) cout << ", encoding = " << currSnip->fEncodingAttr;
			if ( currSnip->fCharForm != PacketScanner::eChar8Bit ) cout << ", " << charFormName[currSnip->fCharForm];
			if ( currSnip->fOutOfOrder ) cout << ", out-of-order";
		}
		
		cout << endl;

	}

}	// DumpSnipInfo ()


// =================================================================================================
// AtStartingMarker () and AtEndingMarker ()
// =========================================

static inline bool
AtStartingMarker ( const char * currChar )
{
	if ( *currChar == '+' ) {
		if ( (*(currChar+1) == '+') && (*(currChar+2) == '+') &&
			 (*(currChar+3) == 'h') && ((*(currChar+4) == 'G') || (*(currChar+4) == 'B')) ) return true;
	} else if ( *currChar == '-' ) {
		if ( (*(currChar+1) == '-') && (*(currChar+2) == '-') &&
			 (*(currChar+3) == 'h') && ((*(currChar+4) == 'G') || (*(currChar+4) == 'B')) ) return true;
	}
	return false;	 
}	// AtStartingMarker ()

// -------------------------------------------------------------------------------------------------

static inline bool
AtEndingMarker ( const char * currChar )
{
	if ( *currChar == '+' ) {
		if ( (*(currChar+1) == '+') && (*(currChar+2) == '+') &&
			 ((*(currChar+3) == 't') || (*(currChar+3) == 'p')) &&
			  ((*(currChar+4) == 'G') || (*(currChar+4) == 'B')) ) return true;
	} else if ( *currChar == '-' ) {
		if ( (*(currChar+1) == '-') && (*(currChar+2) == '-') &&
			 ((*(currChar+3) == 't') || (*(currChar+3) == 'p')) &&
			  ((*(currChar+4) == 'G') || (*(currChar+4) == 'B')) ) return true;
	}
	return false;	 
}	// AtEndingMarker ()


// =================================================================================================
// FindExpectedPackets ()
// ======================
//
// Find the bounds of the expected packets from the markers.  Only works for 8 bit characters!

static void
FindExpectedPackets ( string title, string rawFile, PacketScanner::SnipInfoVector& snips )
{
	const char *	rawChars	= rawFile.data();
	const int		rawLength	= rawFile.size();
	const char *	tagLimit	= rawChars + rawLength - 5;
	
	const char *	headPtr		= rawChars;
	const char *	tailPtr		= NULL;
	
	int		currStart;				// Offset in the string of the current packet.
	int		nextStart	= 0;		// Offset 1 byte beyond the end of the previous packet.
	int		scanDir;				// Direction of tag scanning, -1 is backwards, +1 is forwards.
	char	stateChar, finishChar;	// The head tag's G/B char (state) and tail tag's t/p char (finish).
	
	PacketScanner::SnipInfoVector::iterator	currSnip;
	
	snips.erase ( snips.begin(), snips.end() );
	
	while ( headPtr <= tagLimit ) {

		while ( (! AtStartingMarker ( headPtr )) && (headPtr <= tagLimit) ) headPtr++;

		if ( headPtr > tagLimit ) {	// At EoF, add final raw data snip if necessary, exit the main loop.
			if ( nextStart < rawLength ) {
				snips.push_back ( PacketScanner::SnipInfo ( PacketScanner::eRawInputSnip, nextStart, (rawLength - nextStart) ) );
			}
			break;
		}

		stateChar = *(headPtr+4);	// Save the G/B flag to double check the tail tag.
		
		scanDir = 1;
		if ( *headPtr == '-' ) scanDir = -1;
		while ( *headPtr != '<' ) headPtr += scanDir;	// Scan for the '<' of the packet header.
		currStart = headPtr - rawChars;
		
		if ( currStart > nextStart ) {	// Add a raw data snip if there is a gap between packets.
			snips.push_back ( PacketScanner::SnipInfo ( PacketScanner::eRawInputSnip, nextStart, (currStart - nextStart) ) );
		}

		currSnip = snips.insert ( snips.end(), PacketScanner::SnipInfo ( PacketScanner::ePartialPacketSnip, currStart, 0 ) );

		for ( tailPtr = headPtr; tailPtr <= tagLimit; tailPtr++ ) {
			if ( AtEndingMarker ( tailPtr ) ) break;
		}
		
		if ( tailPtr > tagLimit ) {	// At EoF, no tail tag, last snip is a partial packet, exit the main loop.
			currSnip->fLength = rawLength - currStart;
			break;
		}

		if ( stateChar != *(tailPtr+4) ) cout << "Mismatched G/B mode at offset " << (tailPtr - rawChars) << endl;
		if ( stateChar == 'G' ) {
			currSnip->fState = PacketScanner::eValidPacketSnip;
		} else {
			currSnip->fState = PacketScanner::eBadPacketSnip;
		}
		currSnip->fAccess = '.';	// Just to make the dump look a bit better.
		
		finishChar = *(tailPtr+3);
		
		scanDir = 1;
		if ( *tailPtr == '-' ) scanDir = -1;
		while ( *tailPtr != '>' ) tailPtr += scanDir;	// Scan for the '>' of the packet trailer.
		tailPtr++;
		
		if ( finishChar == 'p' ) {
			while ( tailPtr < (rawChars + rawLength) ) {	// Scan for whitespace padding after the packet trailer.
				const char	currChar	= *tailPtr;
				if ( (currChar != ' ') && (currChar != '\t') && (currChar != '\n') && (currChar != '\r') ) break;
				tailPtr++;
			}
		}

		currSnip->fLength = tailPtr - headPtr;
		nextStart = currSnip->fOffset + currSnip->fLength;
		headPtr = tailPtr;		

	}
	
	DumpSnipInfo ( snips, title );

}	// FindExpectedPackets ()


// =================================================================================================
// DoPacketSnipChecks ()
// =====================

static int
DoPacketSnipChecks	( string title, int	snipIndex,
					  PacketScanner::SnipInfoVector::iterator currActual,
					  PacketScanner::SnipInfoVector::iterator currExpected )
{
	int	errorCount	= 0;
	
	if ( (currActual->fAccess != 'r') && (currActual->fAccess != 'w') ) {
		errorCount++;
		cout << "### " << title << " : invalid access in snip " << snipIndex << endl;
	}

	if ( currActual->fCharForm != PacketScanner::eChar8Bit ) {
		if ( (currActual->fCharForm < PacketScanner::eChar16BitBig) ||
			 (currActual->fCharForm > PacketScanner::eChar32BitLittle) ) {
			errorCount++;
			cout << "### " << title << " : invalid char form in snip " << snipIndex << endl;
		} else {
			const int bytesPerChar = (int)currActual->fCharForm & 6;
			if ( (currActual->fLength % bytesPerChar) != 0 ) {
				errorCount++;
				cout << "### " << title << " : invalid length (mod bpc) in snip " << snipIndex << endl;
			}
		}
	}

	if ( (currActual->fBytesAttr != -1) && (currActual->fBytesAttr != currActual->fLength) ) {
		errorCount++;
		cout << "### " << title << " : invalid bytes attr in snip " << snipIndex << endl;
	}

	for ( int c = 0; c < currActual->fEncodingAttr.size(); c++ ) {	// Make sure the encoding is reduced to UTF-8.
		if ( currActual->fEncodingAttr[c] == 0 ) {
			errorCount++;
			cout << "### " << title << " : invalid encoding attr in snip " << snipIndex << endl;
		}
	}

	if ( currExpected->fAccess != '.' ) {

		if ( currActual->fOutOfOrder != currExpected->fOutOfOrder ) {
			errorCount++;
			cout << "### " << title << " : out-of-order error in snip " << snipIndex << endl;
		}

		if ( currActual->fAccess != currExpected->fAccess ) {
			errorCount++;
			cout << "### " << title << " : access error in snip " << snipIndex << endl;
		}

		if ( currActual->fCharForm != currExpected->fCharForm ) {
			errorCount++;
			cout << "### " << title << " : char form error in snip " << snipIndex << endl;
		}

		if ( currActual->fBytesAttr != currExpected->fBytesAttr ) {
			errorCount++;
			cout << "### " << title << " : bytes attribute error in snip " << snipIndex << endl;
		}

		if ( currActual->fEncodingAttr != currExpected->fEncodingAttr ) {
			errorCount++;
			cout << "### " << title << " : encoding attribute error in snip " << snipIndex << endl;
		}

	}
	
	return errorCount;

}	// DoPacketSnipChecks ()

// =================================================================================================
// VerifyReport ()
// ===============

static int
VerifyReport ( string title, PacketScanner::SnipInfoVector& actual, PacketScanner::SnipInfoVector& expected )
{
	const int	actualCount		= actual.size();
	const int	expectedCount	= expected.size();
	
	int	errorCount	= 0;
	int	nextOffset	= 0;

	DumpSnipInfo ( actual, title );
	
	if ( actualCount != expectedCount ) {

		errorCount++;
		cout << "### " << title << " : snip count error, actual = " << actualCount
			 << ", expected = " << expectedCount << endl;

	} else {
		
		PacketScanner::SnipInfoVector::iterator	currActual		= actual.begin();
		PacketScanner::SnipInfoVector::iterator	currExpected	= expected.begin();
		
		for ( int s = 0; s < actualCount; s++ ) {
		
			if ( currActual->fState != currExpected->fState ) {
				errorCount++;
				cout << "### " << title << " : state error in snip " << s << endl;
			}

			if ( currActual->fOffset != currExpected->fOffset ) {
				errorCount++;
				cout << "### " << title << " : offfset error in snip " << s << endl;
			}

			if ( currActual->fLength != currExpected->fLength ) {
				errorCount++;
				cout << "### " << title << " : length error in snip " << s << endl;
			}

			if ( currActual->fOffset != nextOffset ) {
				errorCount++;
				cout << "### " << title << " : invalid offset in snip " << s << endl;
			}
			nextOffset = currActual->fOffset + currActual->fLength;
			
			if ( currExpected->fState == PacketScanner::eValidPacketSnip ) {
				errorCount += DoPacketSnipChecks ( title, s, currActual, currExpected );
			}
			
			++currActual;
			++currExpected;
		
		}
	
	}
	
	return errorCount;
	
}	// VerifyReport ()


// =================================================================================================
// ScanForPackets ()
// =================
//
// Try the scanner.

static int
ScanForPackets ( string title, string rawFile, PacketScanner::SnipInfoVector& expected, bool update = false )
{
	int		errorCount	= 0;
	long	pos, len;
	
	const int	rawlength	= rawFile.size();

	PacketScanner *					scanner	= NULL;
	PacketScanner::SnipInfoVector	actual;
	
	// --------------------------------
	// Try the scanner using one chunk.

	#if 1

		scanner = new PacketScanner ( rawlength );
		scanner->Scan ( rawFile.data(), 0, rawlength );

		scanner->Report ( actual );
		errorCount += VerifyReport ( (title + ", one chunk"), actual, expected );

		delete scanner;
		scanner = NULL;
		
	#endif
	
	// -----------------------------
	// Try using single byte chunks.

	#if 1

		char	tempBuf [9]	= { -1, -1, -1, -1, 0, -1, -1, -1, -1 };
		
		scanner = new PacketScanner ( rawlength );
		for ( pos = 0; pos < rawlength; pos++ ) {
			#if DebugBuild
				scanner->Scan ( &rawFile[pos], pos, 1 );
			#else
				tempBuf[4] = rawFile[pos];	// Use a local copy so that out of bounds references fail.
				scanner->Scan ( &tempBuf[4], pos, 1 );
			#endif
		}

		scanner->Report ( actual );
		errorCount += VerifyReport ( (title + ", single byte chunks"), actual, expected );

		delete scanner;
		scanner = NULL;

	#endif
	
	// --------------------------------------
	// Try using random lengths from 0 to 63.

	#if 1

		srand ( clock() );	// Try to get a unique set of random numbers.
	
		scanner = new PacketScanner ( rawlength );
		pos = 0;
		while ( pos < rawlength ) {
			len = rand() & 0x3F;
			if ( len > (rawlength - pos) ) len = rawlength - pos;
			scanner->Scan ( &rawFile[pos], pos, len );
			pos += len;
		}

		scanner->Report ( actual );
		errorCount += VerifyReport ( (title + ", random length chunks"), actual, expected );

		delete scanner;
		scanner = NULL;
		
	#endif

	// -----
	// Done.
	
	if ( update && (errorCount == 0) ) expected = actual;	// Save the good actual snip report.
	return errorCount;
	
}	// ScanForPackets ()


// =================================================================================================
// AddTweakedData ()
// =================

static const string	sHeaderPrefix		= "<?xpacket begin='";
static const int	sHeaderPrefixSize	= sHeaderPrefix.size();

static void
AddTweakedData ( string& tweakedFile, const char * rawDataPtr, unsigned long rawLength, 
				 int bytesPerChar, bool isBigEndian )
{
	int	c, i;
	
	#if UseStringPushBack
		
		for ( c = rawLength; c > 0; c--, rawDataPtr++ ) {
			if ( isBigEndian ) for ( i = bytesPerChar-1; i > 0; i-- ) tweakedFile.push_back ( 0 );
			tweakedFile.push_back ( *rawDataPtr );
			if ( ! isBigEndian ) for ( i = bytesPerChar-1; i > 0; i-- ) tweakedFile.push_back ( 0 );
		}

	#else
		
		for ( c = rawLength; c > 0; c--, rawDataPtr++ ) {
			if ( isBigEndian ) for ( i = bytesPerChar-1; i > 0; i-- ) tweakedFile.insert ( tweakedFile.end(), 0 );
			tweakedFile.insert ( tweakedFile.end(), *rawDataPtr );
			if ( ! isBigEndian ) for ( i = bytesPerChar-1; i > 0; i-- ) tweakedFile.insert ( tweakedFile.end(), 0 );
		}

	#endif

}	// AddTweakedData ()

// =================================================================================================
// ExpandChars ()
// ==============

static void
ExpandChars ( string rawFile, PacketScanner::SnipInfoVector& rawSnips,
			  string& tweakedFile, PacketScanner::SnipInfoVector& tweakedSnips,
			  PacketScanner::CharacterForm cForm )
{
	int		bytesPerChar	= cForm & (PacketScanner::eChar16BitMask | PacketScanner::eChar32BitMask);
	bool	isBigEndian		= CharFormIsBigEndian ( cForm );
	int		s;
	
	if ( cForm == PacketScanner::eChar8Bit ) bytesPerChar = 1;	// The eChar8Bit code is zero.
	
	// --------------------------------------------------
	// Create the tweaked data stream and expected snips.
	
	tweakedFile.erase ( tweakedFile.begin(), tweakedFile.end() );
	tweakedSnips.erase ( tweakedSnips.begin(), tweakedSnips.end() );
	
	for ( s = 0; s < rawSnips.size(); s++ ) {
	
		const PacketScanner::SnipState	rawState	= rawSnips[s].fState;
		
		const unsigned long	rawOffset	= rawSnips[s].fOffset;
		const unsigned long	rawLength	= rawSnips[s].fLength;
		const char *		rawDataPtr	= &rawFile[rawOffset];
		
		unsigned long	tweakedOffset	= tweakedFile.size();
		unsigned long	tweakedLength	= rawLength * bytesPerChar;	// Ignores BOM insertion for packet snips.
		
		if ( (rawState == PacketScanner::eRawInputSnip) ||
			 ((rawState == PacketScanner::ePartialPacketSnip) && (rawLength < sHeaderPrefixSize)) ) {
		
			// ------------------------------------------------------------------------------
			// A raw data snip, or a partial packet that ends before the BOM, copy all of it.
			
			AddTweakedData ( tweakedFile, rawDataPtr, rawLength, bytesPerChar, isBigEndian );
			tweakedSnips.push_back ( PacketScanner::SnipInfo ( rawState, tweakedOffset, tweakedLength ) );
			tweakedSnips[s].fBytesAttr = -1;
		
		} else {
		
			// ----------------------------------------------------------------------------
			// A full packet (valid or bad), or a partial packet that has room for the BOM.
			// Copy the prefix, insert the BOM, update the bytes attribute, copy the rest.
			
			AddTweakedData ( tweakedFile, rawDataPtr, sHeaderPrefixSize, bytesPerChar, isBigEndian );

			switch ( cForm ) {
				case PacketScanner::eChar8Bit :
					AddTweakedData ( tweakedFile, "\xEF\xBB\xBF", 3, 1, isBigEndian );
					break;
				case PacketScanner::eChar16BitBig :
					AddTweakedData ( tweakedFile, "\xFE\xFF", 2, 1, isBigEndian );
					break;
				case PacketScanner::eChar16BitLittle :
					AddTweakedData ( tweakedFile, "\xFF\xFE", 2, 1, isBigEndian );
					break;
				case PacketScanner::eChar32BitBig :
					AddTweakedData ( tweakedFile, "\x00\x00\xFE\xFF", 4, 1, isBigEndian );
					break;
				case PacketScanner::eChar32BitLittle :
					AddTweakedData ( tweakedFile, "\xFF\xFE\x00\x00", 4, 1, isBigEndian );
					break;
			}
			
			const char *		suffixDataPtr	= rawDataPtr + sHeaderPrefixSize;
			const unsigned long	suffixLength	= rawLength - sHeaderPrefixSize;
			AddTweakedData ( tweakedFile, suffixDataPtr, suffixLength, bytesPerChar, isBigEndian );

			tweakedLength = tweakedFile.size() - tweakedOffset;	// Include the inserted BOM.
			tweakedSnips.push_back ( PacketScanner::SnipInfo ( rawState, tweakedOffset, tweakedLength ) );

		}
		
		tweakedSnips[s].fOutOfOrder		= rawSnips[s].fOutOfOrder;
		tweakedSnips[s].fAccess			= rawSnips[s].fAccess;
		tweakedSnips[s].fCharForm		= cForm;
		tweakedSnips[s].fBytesAttr		= rawSnips[s].fBytesAttr;
		tweakedSnips[s].fEncodingAttr	= rawSnips[s].fEncodingAttr;

	}
	
	// ---------------------------------------------------------------------------------------
	// Go back and touch up the boundaries for packet snips that have a valid bytes attribute.
	
	for ( s = 1; s < tweakedSnips.size(); s++ ) {
		if ( rawSnips[s-1].fBytesAttr != rawSnips[s-1].fLength ) continue;
		const int	delta	= tweakedSnips[s-1].fBytesAttr - tweakedSnips[s-1].fLength;
		tweakedSnips[s-1].fLength	+= delta;
		tweakedSnips[s].fOffset		+= delta;
		tweakedSnips[s].fLength		-= delta;
	}
	
	// --------------------------------------------------------------------------------------
	// Go back and touch up the boundaries for partial snips in big endian mode.  The scanner
	// does not move the leading nulls in until the whole packet is recognized.  The first
	// snip must be ignored, there is nothing before it to give the nulls back to!
	
	if ( isBigEndian ) {
		const int	nullCount	= bytesPerChar - 1;
		for ( s = 1; s < tweakedSnips.size(); s++ ) {
			if ( rawSnips[s].fState != PacketScanner::ePartialPacketSnip ) continue;
			tweakedSnips[s-1].fLength	+= nullCount;
			tweakedSnips[s].fOffset		+= nullCount;
			tweakedSnips[s].fLength		-= nullCount;
		}
	}
	
	DumpSnipInfo ( tweakedSnips, "Tweaked expected snips" );

}	// ExpandChars ()


// =================================================================================================
// ProcessFile ()
// ==============
//
// Drive the testing for a single input file.

static bool	sTweakInput	= true;

static int
ProcessFile ( string fileName )
{
	int		errorCount	= 0;
	int		cForm;
	string	title, rawFile, tweakedFile;
	
	PacketScanner::SnipInfoVector	rawSnips, tweakedSnips;
	
	InhaleFile ( fileName.c_str(), rawFile );
	
	title = "Expected packets for " + fileName;
	FindExpectedPackets ( title, rawFile, rawSnips );
	
	title = "Actual packets for " + fileName;
	errorCount += ScanForPackets ( title, rawFile, rawSnips, true );
	
	if ( errorCount > 0 ) goto ERROR;	// Don't continue if the raw input can't be handled.

	if ( sTweakInput ) {

		ExpandChars ( rawFile, rawSnips, tweakedFile, tweakedSnips, PacketScanner::eChar8Bit );
		title = "Actual packets for " + fileName + " with BOM";
		errorCount += ScanForPackets ( title, tweakedFile, tweakedSnips );
		
		for ( cForm = 2; cForm <= 5; cForm++ ) {
			ExpandChars ( rawFile, rawSnips, tweakedFile, tweakedSnips, (PacketScanner::CharacterForm)cForm );
			title = "Actual packets for " + fileName + " @ " + charFormName[cForm];
			errorCount += ScanForPackets ( title, tweakedFile, tweakedSnips );
		}
	
	}

EXIT:
	cout << endl << errorCount << " errors for " << fileName << endl << endl;
	return errorCount;

ERROR:
	goto EXIT;
	
}	// ProcessFile ()


// =================================================================================================
// main ()
// =======

#if XMPTestingBuild

	int main ( int argc, const char * argv [] )
	{
		int		totalErrors	= 0;
		int	i;	// ** Thanks to MS VC++ for not implementing the language!
		
		cout << endl << "Starting XMP packet scanner test" << endl << endl;

		for ( i = 1; i < argc; i++ ) {
			if ( strcmp ( "-U", argv[i] ) == 0 ) {
				sTweakInput = false;
			} else if ( strcmp ( "-i", argv[i] ) == 0 ) {
				++i;	// Move to the file name.
				totalErrors += ProcessFile ( string ( argv[i] ) );
			}
		}
		
		cout << "Finished XMP packet scanner test, " << totalErrors << " errors" << endl;
		return totalErrors;

	}	// main ()

#else

	int main ()
	{
		int		totalErrors	= 0;
		string	fileName;
		
		cout << endl << "Starting XMP packet scanner test" << endl << endl;

		while ( true ) {
			cout << "File: ";
			cin >> fileName;
			if ( fileName == "." ) break;
			totalErrors += ProcessFile ( fileName );
		}
		
		cout << "Finished XMP packet scanner test, " << totalErrors << " errors" << endl;
		return totalErrors;

	}	// main ()

#endif

