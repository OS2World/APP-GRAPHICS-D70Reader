/*************************************************************************************************/

/*

ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.

*/

/*************************************************************************************************/

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "xapc_interface.h"

int main ( void )
{

	const char* xap_test =
		"<?adobe-xap-filters esc='CR'?>"
		"<x:xapmeta xmlns:x='adobe:ns:meta/'>"
		"<rdf:RDF xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'"
		" xmlns:iX='http://ns.adobe.com/iX/1.0/'>"
		"<rdf:Description about='' "
		"  xmlns='a/test/name/space/'"
		"  xmlns:test='a/test/name/space/'>"
		"  <test:Number>123</test:Number>"
		" </rdf:Description>"
		"</rdf:RDF>"
		"</x:xapmeta>";
		
	stdc_error_t error = stdc_no_error;
	char buffer[1024];
	size_t count = 0;
	const char* string_result = "";
	xapc_features_t features = xapc_feature_none;
	
	xapc_object_t xap = NULL;
	
	error = xapc_init();
	assert ( error == stdc_no_error );
	
	xap = xapc_create();
	assert ( xap );

	/* This get should fail, going through the exception path. */
	error = xapc_get ( xap, &string_result, &features, "a/bogus/name/space/", "Nothing" );
	assert ( error == -97 );
	
	error = xapc_parse ( xap, xap_test, strlen ( xap_test ), xapc_parse_last );
	assert ( error == stdc_no_error );
	
	error = xapc_get ( xap, &string_result, &features, "a/test/name/space/", "Number" );
	assert ( error == stdc_no_error );
	printf ( "Result: %s\n", string_result );
	
	error = xapc_register_namespace ( xap, "a/test/name/space/", "test" );
	assert ( error == stdc_no_error );
	
	error = xapc_set ( xap, "a/test/name/space/", "Number", "987654", features );
	assert ( error == stdc_no_error );
	
	error = xapc_create_lang_alt ( xap, "a/test/name/space/", "LangAlt", "default", features );
	assert ( error == stdc_no_error );
	error = xapc_set ( xap, "a/test/name/space/", "LangAlt/*[@xml:lang='fr']", "french", features );
	assert ( error == stdc_no_error );
	
	error = xapc_create_container ( xap, "a/test/name/space/", "BagOfWords", xapc_rdf_bag, "first", features );
	assert ( error == stdc_no_error );
	error = xapc_append ( xap, "a/test/name/space/", "BagOfWords/*[last()]", 0, "second", features );
	assert ( error == stdc_no_error );
	
	error = xapc_serialize ( xap, &count, xapc_escape_cr );
	assert ( error == stdc_no_error );
	
	while  ( count > 0 ) {
		error = xapc_extract_serialization ( xap, &count, buffer, sizeof ( buffer ) - 1 );
		assert ( error == stdc_no_error );
		buffer[count] = 0;
		printf ( "%s", buffer );
	}
	
	error = xapc_remove ( xap, "a/test/name/space/", "Number" );
	assert ( error == stdc_no_error );
	
	error = xapc_serialize ( xap, &count, xapc_escape_cr );
	assert ( error == stdc_no_error );
	
	while  ( count > 0 ) {
		error = xapc_extract_serialization ( xap, &count, buffer, sizeof ( buffer ) - 1 );
		assert ( error == stdc_no_error );
		buffer[count] = 0;
		printf ( "%s", buffer );
	}
	
	xapc_destroy ( xap );
		
	error = xapc_terminate();
	assert ( error == stdc_no_error );

	return 0;

}
