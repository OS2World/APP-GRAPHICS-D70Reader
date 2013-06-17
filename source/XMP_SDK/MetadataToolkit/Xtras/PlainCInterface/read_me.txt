/*************************************************************************************************/

/*

ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.  If you have received this file from a 
source other than Adobe, then your use, modification, or distribution of it requires the prior 
written permission of Adobe.
 
*/

/*************************************************************************************************/

This code provides a standard C interface for xaptk. Although this is sample code, what is
provided is of commercial quality.

* This is a sample, additional C wrappers may need to be written for other methods

This release contains the following files:

Headers					// public includes
	stdc_interface.h	// interface file for std C++ exceptions and namespaces in C
	xapc_interface.h	// main interface for xaptk

Source					// private includes and implementations
	stdc_utility.cpp	// some simple utilities for mapping std C++ exceptions
	stdc_utility.h		// interface file for stdc_utility
	xapc_interface.cpp	// implementation of interface mapping for xaptk
	
Build
	xapc_interface.mcp	// Metroworks project to build xaptk with C interface into xapctk.lib

Test
	test_c_only.mcp		// Metrowerks project to build test console app. Built as C only.
	HelloWorld.c		// Test source code