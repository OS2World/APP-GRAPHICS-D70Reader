
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
//
// This sample shows how to use the XMP toolkit to create the properties in the basic schema.
//
// =================================================================================================


#include <XAPToolkit.h>
#include <XAPGizmos.h>

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;


// =================================================================================================
// ConjureMetaXAP_XMP ()
// =====================

static MetaXAP *
ConjureMetaXAP_XMP ()
{

	MetaXAP *	localMeta	= MakeNewMetaXAP ( "Example of basic XMP schema" );
	
    if ( localMeta == NULL ) goto ERROR;
	
	try {

		localMeta->createFirstItem ( XAP_NS_XAP, "Advisory", "http://ns.adobe.com/xap/1.0/ BaseURL", xap_bag );
		localMeta->append ( XAP_NS_XAP, "Advisory/*[last()]", "http://ns.adobe.com/xap/1.0/ Description/*[@xml:lang='en']" );
		localMeta->append ( XAP_NS_XAP, "Advisory/*[last()]", "http://ns.adobe.com/xap/1.0/ Keywords/*[2]" );

		localMeta->createFirstItem ( XAP_NS_XAP, "Authors", "Jane Doe", xap_seq );
		localMeta->append ( XAP_NS_XAP, "Authors/*[last()]", "John Doe" );
		localMeta->append ( XAP_NS_XAP, "Authors/*[last()]", "Jack Doe" );
		
		localMeta->set ( XAP_NS_XAP, "BaseURL", "http://www.acme.com/index.html" );
		localMeta->set ( XAP_NS_XAP, "CreateDate", "2001-09-13T10:42:24Z" );
		localMeta->set ( XAP_NS_XAP, "CreatorTool", "XMP Example Application" );

		localMeta->createFirstItem ( XAP_NS_XAP, "Description",	// xap:Description is an alt by language.
									 "This document is a sample XMP file", "xml:lang", "x-default" );
		localMeta->set ( XAP_NS_XAP, "Description/*[@xml:lang='en']", "This document is a sample XMP file" );
		localMeta->set ( XAP_NS_XAP, "Description/*[@xml:lang='fr']", "Ce document est un fichier d'exemple XMP" );
		localMeta->set ( XAP_NS_XAP, "Description/*[@xml:lang='sp']", "Este documento es una muestra de un archivo XMP" );
		localMeta->set ( XAP_NS_XAP, "Description/*[@xml:lang='de']", "Dieses Dokument ist eine XMP Beispieldatei" );

		localMeta->set ( XAP_NS_XAP, "Format", "text/xml" );

		localMeta->createFirstItem ( XAP_NS_XAP, "Keywords", "XMP", xap_bag );
		localMeta->append ( XAP_NS_XAP, "Keywords/*[last()]", "schema" );
		localMeta->append ( XAP_NS_XAP, "Keywords/*[last()]", "example" );

		localMeta->createFirstItem ( XAP_NS_XAP, "Locale", "en", xap_bag );		// Assuming the content of this
		localMeta->append ( XAP_NS_XAP, "Locale/*[last()]", "fr" );				//	document is in English and French.
		
		localMeta->set ( XAP_NS_XAP, "MetadataDate", "2001-09-13T10:56:37Z" );
		localMeta->set ( XAP_NS_XAP, "ModifyDate", "2001-09-13T10:56:37Z" );
		localMeta->set ( XAP_NS_XAP, "Nickname", "Basic XMP sample" );

		localMeta->createFirstItem ( XAP_NS_XAP, "Title",	// xap:Title is an alt by language.
									 "XMP Basic Schema Example", "xml:lang", "x-default" );
		localMeta->set ( XAP_NS_XAP, "Title/*[@xml:lang='en']", "XMP Basic Schema Example" );

		localMeta->createFirstItem ( XAP_NS_XAP_RIGHTS, "Copyright",	// xapRights:Copyright is an alt by language.
									 "This is a copyright notice", "xml:lang", "x-default" );
		localMeta->set ( XAP_NS_XAP_RIGHTS, "Copyright/*[@xml:lang='en']", "This is a copyright notice" );
		localMeta->set ( XAP_NS_XAP_RIGHTS, "Copyright/*[@xml:lang='fr']", "C'est une notification de copyright" );

		localMeta->set ( XAP_NS_XAP_RIGHTS, "Marked", "False" );

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
	delete localMeta;
	localMeta = NULL;
	goto EXIT;
	
}	// ConjureMetaXAP_XMP ()


// =================================================================================================
// ConjureMetaXAP_PDF ()
// =====================

static MetaXAP *
ConjureMetaXAP_PDF ()
{

	MetaXAP *	localMeta	= MakeNewMetaXAP ( "Example of basic PDF schema" );
	
    if ( localMeta == NULL ) goto ERROR;
	
	try {

		localMeta->set ( XAP_NS_PDF, "Author", "Jane Doe" );
		localMeta->set ( XAP_NS_PDF, "BaseURL", "http://www.acme.com/index.html" );
		localMeta->set ( XAP_NS_PDF, "CreationDate", "2001-09-13T10:42:24Z" );
		localMeta->set ( XAP_NS_PDF, "Creator", "XMP Example Application" );
		localMeta->set ( XAP_NS_PDF, "Keywords", "XMP schema example" );
		localMeta->set ( XAP_NS_PDF, "ModDate", "2001-09-13T10:56:37Z" );
		localMeta->set ( XAP_NS_PDF, "Subject", "This document is a sample XMP file" );
		localMeta->set ( XAP_NS_PDF, "Title", "XMP Basic Schema Example" );

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
	delete localMeta;
	localMeta = NULL;
	goto EXIT;
	
}	// ConjureMetaXAP_PDF ()


// =================================================================================================
// ConjureMetaXAP_DC ()
// ====================

static MetaXAP *
ConjureMetaXAP_DC ()
{

	MetaXAP *	localMeta	= MakeNewMetaXAP ( "Example of basic Dublin Core schema" );
	
    if ( localMeta == NULL ) goto ERROR;
	
	try {

		localMeta->createFirstItem ( XAP_NS_DC, "contributor", "Mary Smith", xap_bag );
		localMeta->append ( XAP_NS_DC, "contributor/*[last()]", "Arnold Jones" );

		localMeta->set ( XAP_NS_DC, "coverage", "Life, the universe, and everything" );

		localMeta->createFirstItem ( XAP_NS_DC, "creator", "Jane Doe", xap_seq );
		localMeta->append ( XAP_NS_DC, "creator/*[last()]", "John Doe" );
		localMeta->append ( XAP_NS_DC, "creator/*[last()]", "Jack Doe" );

		localMeta->createFirstItem ( XAP_NS_DC, "date", "2001-09-13T10:42:24Z", xap_seq );
		localMeta->append ( XAP_NS_DC, "date/*[last()]", "2001-09-13T10:56:37Z" );

		localMeta->createFirstItem ( XAP_NS_DC, "description",	// dc:description is an alt by language.
									 "This document is a sample XMP file", "xml:lang", "x-default" );
		localMeta->set ( XAP_NS_DC, "description/*[@xml:lang='en']", "This document is a sample XMP file" );
		localMeta->set ( XAP_NS_DC, "description/*[@xml:lang='fr']", "Ce document est un fichier d'exemple XMP" );
		localMeta->set ( XAP_NS_DC, "description/*[@xml:lang='sp']", "Este documento es una muestra de un archivo XMP" );
		localMeta->set ( XAP_NS_DC, "description/*[@xml:lang='de']", "Dieses Dokument ist eine XMP Beispieldatei" );
		
		localMeta->set ( XAP_NS_DC, "format", "text/xml" );
		localMeta->set ( XAP_NS_DC, "identifier", "XMP-sample-1" );

		localMeta->createFirstItem ( XAP_NS_DC, "language", "en", xap_alt );	// Assuming the content of this
		localMeta->append ( XAP_NS_DC, "language/*[last()]", "fr" );			//	document is in English and French.

		localMeta->createFirstItem ( XAP_NS_DC, "publisher", "Adobe Systems Incorporated", xap_bag );

		localMeta->createFirstItem ( XAP_NS_DC, "rights",	// dc:rights is an alt by language.
									 "This is a copyright notice", "xml:lang", "x-default" );
		localMeta->set ( XAP_NS_DC, "rights/*[@xml:lang='en']", "This is a copyright notice" );
		localMeta->set ( XAP_NS_DC, "rights/*[@xml:lang='fr']", "C'est une notification de copyright" );

		localMeta->createFirstItem ( XAP_NS_DC, "subject", "XMP", xap_bag );
		localMeta->append ( XAP_NS_DC, "subject/*[last()]", "schema" );
		localMeta->append ( XAP_NS_DC, "subject/*[last()]", "example" );

		localMeta->createFirstItem ( XAP_NS_DC, "title",	// dc:title is an alt by language.
									 "XMP Basic Schema Example", "xml:lang", "x-default" );
		localMeta->set ( XAP_NS_DC, "title/*[@xml:lang='en']", "XMP Basic Schema Example" );

		localMeta->createFirstItem ( XAP_NS_DC, "type", "program", xap_bag );
		localMeta->append ( XAP_NS_DC, "type/*[last()]", "C++" );

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
	delete localMeta;
	localMeta = NULL;
	goto EXIT;
	
}	// ConjureMetaXAP_DC ()


// =================================================================================================
// main ()
// =======

int main ( int /* argc */, const char * /* argv */ [] )
{

	MetaXAP *	localMeta	= NULL;
	
	localMeta = ConjureMetaXAP_XMP();
	if ( localMeta == NULL ) {
		cout << endl << "// *** No output from ConjureMetaXAP_XMP!" << endl;
	} else {
		DumpMetaXAP ( *localMeta );
		SerializeMetaXAP ( *localMeta, xap_format_pretty );
		SerializeMetaXAP ( *localMeta, xap_format_compact );
		delete localMeta;
	}
		
	localMeta = ConjureMetaXAP_PDF();
	if ( localMeta == NULL ) {
		cout << endl << "// *** No output from ConjureMetaXAP_PDF!" << endl;
	} else {
		DumpMetaXAP ( *localMeta );
		SerializeMetaXAP ( *localMeta, xap_format_pretty );
		SerializeMetaXAP ( *localMeta, xap_format_compact );
		delete localMeta;
	}
	
	localMeta = ConjureMetaXAP_DC();
	if ( localMeta == NULL ) {
		cout << endl << "// *** No output from ConjureMetaXAP_DC!" << endl;
	} else {
		DumpMetaXAP ( *localMeta );
		SerializeMetaXAP ( *localMeta, xap_format_pretty );
		SerializeMetaXAP ( *localMeta, xap_format_compact );
		delete localMeta;
	}

	return 0;

}	// main ()

