/*************************************************************************************************/

/*

ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.

*/

/*************************************************************************************************/

#ifndef STDC_INTERFACE
#define STDC_INTERFACE

/*************************************************************************************************/

/*
	NOTES:

	* stdc is used to denote items in the std:: namespace.
	* index is 0 based.
	* count is 1 based.
*/

/*************************************************************************************************/

#ifdef __cplusplus
//#define stdc(ref) std::ref

//ashah
#define stdc(ref) ::ref

#else
#define stdc(ref) ref
#endif

/*************************************************************************************************/

enum
	{
	stdc_no_error = 0,
	stdc_unknown_error,				/* returned if lookup fails */
	stdc_exception,
	stdc_bad_alloc,
	stdc_bad_cast,
	stdc_bad_exception,
	stdc_bad_typeid,
	stdc_logic_error,
	stdc_domain_error,
	stdc_invalid_argument,
	stdc_length_error,
	stdc_out_of_range,
	stdc_runtime_error,
	stdc_overflow_error,
	stdc_range_error,
	stdc_underflow_error
	};
	
typedef long stdc_error_t;

#define STDC_ERROR_BLOCK(index) (-100 * (index + 1))

/*************************************************************************************************/

#endif
