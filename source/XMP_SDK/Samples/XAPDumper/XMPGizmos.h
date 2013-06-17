
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


#ifndef __XMPGizmos__
#define __XMPGizmos__


#include "XAPToolkit.h"

#include <string>
#include <ctime>


// =================================================================================================


// ---------------------------------------------------------
// A fake clock that increments 1 second for each timestamp:

class BogusXAPClock : public XAPClock {
public:
	BogusXAPClock();
	void timestamp ( XAPDateTime& dt );
	void reset ();
	XAPDateTime	prevTime;
};

// ---------------------------------------------------
// A basic clock using just the ANSI C time functions:

class BasicXAPClock : public XAPClock {
public:
	BasicXAPClock();
	void timestamp ( XAPDateTime& dt );
	long	prevDaySec;
	long	nextSeq;
private:
};

// ------------------------------------------
// Functions for MetaXAP objects and strings:

extern MetaXAP *
MakeNewMetaXAP ( const char * docName, XAPClock * clock = NULL );

extern MetaXAP *
ParseSubstring ( const std::string& xmlString, size_t offset, size_t length, const char * docName );

extern void
SerializeToString ( MetaXAP& meta, std::string& xmlString, XAPFormatType format, int crlfFilter = -1 );


// -----------------------------
// Functions to display on cout:

extern void
DumpMetaXAP ( MetaXAP& meta, const int steps = -1 );

extern void
DumpMetaXAP ( MetaXAP& meta, const char * ns, const char * root, const int steps = -1 );

extern void
DumpMetaXAP ( MetaXAP&				meta,
			  const XAPTimeRelOp	op,
			  const XAPDateTime&	time,
			  const XAPChangeBits	how		= XAP_CHANGE_MASK );

extern void
SerializeMetaXAP ( MetaXAP& meta, XAPFormatType format = xap_format_pretty, int crlfFilter = -1 );


// ----------------------------------------
// Functions for MetaXAP objects and files:

extern void
InhaleFile ( const char * fileName, std::string& fileContents );

extern MetaXAP *
ParseFile ( const char * fileName, const char * docName );

extern void
WriteFile ( MetaXAP& meta, const char * fileName, XAPFormatType format, int crlfFilter = -1 );

extern void
AppendFile ( MetaXAP& meta, const char * fileName, XAPFormatType format, int crlfFilter = -1 );


// =================================================================================================


#endif	// __XMPGizmos__

