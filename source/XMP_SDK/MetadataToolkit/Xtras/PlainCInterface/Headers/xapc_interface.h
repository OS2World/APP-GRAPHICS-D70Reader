/*************************************************************************************************/

/*

ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.

*/

/*************************************************************************************************/

#ifndef XAPC_INTERFACE
#define XAPC_INTERFACE

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#include "stdc_interface.h"

/*************************************************************************************************/

/*
	NOTES:

	* xapc_ is used to denote items in the xap namespace.
	* All strings are assumed to be encoded UTF-8.
	* All strings are assumed to be NULL terminated unless a length parameter is provided.
	* NULL passed for a string is considered equivalent to an empty string.
	* String results are valid until subsequent call yielding a result to the same object.
	* stdc_ errors are positive. xapc_ errors are negative falling into blocks of 100.
	* Parameter order is always (object, out..., in...) except for create and destroy.
*/

/*************************************************************************************************/

enum {
	xapc_runtime_error_index,	/* 0 */
	xapc_client_error_index,	/* 1 */
	xapc_cinterface_index		/* 2 */
};
	
/*
NOTE: These error codes are returned corresponding to the exceptions thrown by xaptk. The
xapc_null_object is returned if a null object is passed to any of the functions except destroy.
*/

enum {
	xapc_error			= STDC_ERROR_BLOCK(xapc_runtime_error_index),	/* -100 */
	xapc_bad_xap,														/*  -99 */
	xapc_bad_xml,														/*  -98 */
	xapc_no_match,														/*  -97 */
	
	xapc_client_fault	= STDC_ERROR_BLOCK(xapc_client_error_index),	/* -200 */
	xapc_bad_access,													/* -199 */
	xapc_bad_feature,													/* -198 */
	xapc_bad_number,													/* -197 */
	xapc_bad_path,														/* -196 */
	xapc_bad_schema,													/* -195 */
	xapc_bad_type,														/* -194 */
	
	xapc_null_object	= STDC_ERROR_BLOCK(xapc_cinterface_index)		/* -300 */
};

/*************************************************************************************************/

/* Namespace String Constants */

#ifdef __cplusplus
extern "C" {
#endif

extern const char *const XAP_NS_XAP;
extern const char *const XAP_NS_XAP_G;
extern const char *const XAP_NS_XAP_G_IMG;
extern const char *const XAP_NS_XAP_DYN;
extern const char *const XAP_NS_XAP_DYN_A;
extern const char *const XAP_NS_XAP_DYN_V;
extern const char *const XAP_NS_XAP_T;
extern const char *const XAP_NS_XAP_T_PG;
extern const char *const XAP_NS_XAP_RIGHTS;
extern const char *const XAP_NS_XAP_MM;
extern const char *const XAP_NS_XAP_BJ;

extern const char *const XAP_NS_PHOTOSHOP;
extern const char *const XAP_NS_EXIF;
extern const char *const XAP_NS_TIFF;
extern const char *const XAP_NS_PDF;
extern const char *const XAP_NS_DC;

extern const char *const XAP_NS_RDF;
extern const char *const XAP_NS_META;

extern const char *const XAP_NS_ST_DIMENSIONS;
extern const char *const XAP_NS_ST_RESOLUTION;
extern const char *const XAP_NS_ST_TRACK_DESC;
extern const char *const XAP_NS_ST_FONT;
extern const char *const XAP_NS_ST_RESOURCE_REF;
extern const char *const XAP_NS_ST_VERSION;
extern const char *const XAP_NS_ST_RESOURCE_EVENT;
extern const char *const XAP_NS_ST_FILE_DISPOSITION;
extern const char *const XAP_NS_ST_JOB;
extern const char *const XAP_NS_ST_RIGHT;

#ifdef __cplusplus
}
#endif

/*************************************************************************************************/

/*
REVISIT : I can't believe it is 2001 and C doesn't yet have bool as a standard feature?
Rather than adding yet another bool type I just added an enum for last and not_last.
*/

enum {
	xapc_parse_not_last,
	xapc_parse_last
};

typedef unsigned char xapc_parse_last_t;

/*
REVISIT : We should probably change the C++ options to work in a similar manner. They
would be stored in a std::bitset<> though.
*/

enum {
	xapc_escape_cr_bit,
	xapc_escape_lf_bit,
	xapc_format_compact_bit
};

enum {
	xapc_escape_cr		= 1UL << xapc_escape_cr_bit,
	xapc_escape_lf		= 1UL << xapc_escape_lf_bit,
	xapc_format_compact = 1UL << xapc_format_compact_bit		/* pretty if zero */
};
	
typedef unsigned long xapc_serialize_options_t;

/*
REVIST : The values xapc_features_t have to be kept in sync with the values for C++. We
should change the interface so the declaration file can be shared between the two interfaces.
*/

enum {
	xapc_feature_xml_bit,
	xapc_feature_reserved_bit,
	xapc_feature_rdf_resource_bit,
	xapc_feature_rdf_value_bit
};

enum {
	xapc_feature_none,
	xapc_feature_xml			= 1UL << xapc_feature_xml_bit,
	xapc_feature_reserved		= 1UL << xapc_feature_reserved_bit,
	xapc_feature_rdf_resource	= 1UL << xapc_feature_rdf_resource_bit,
	xapc_feature_rdf_value		= 1UL << xapc_feature_rdf_value_bit
};
	
typedef unsigned long xapc_features_t;

enum {	/* This matches the XAPStructContainerType enumeration. */
	xapc_rdf_simple,
	xapc_rdf_alt,
	xapc_rdf_bag,
	xapc_rdf_seq
};

typedef unsigned char	xapc_rdf_container_t;

/*************************************************************************************************/
	
/*
The xapc_object is an opaque pointer. All operations take an xapc_object as the first parameter.
const_xapc_object is used for const method calls. The object cooresponds to a MetaXAP object.
*/
typedef struct xapc_opaque_object* xapc_object_t;
typedef const struct xapc_opaque_object* const_xapc_object_t;

/*************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************************************/

stdc_error_t xapc_init ( void );

stdc_error_t xapc_terminate ( void );

xapc_object_t xapc_create ( void );

void xapc_destroy ( xapc_object_t object ); /* Will ignore NULL objects */

stdc_error_t xapc_register_namespace ( xapc_object_t object, const char * ns, const char * prefix );

stdc_error_t xapc_get ( const_xapc_object_t	object,
						const char * *		result,
						xapc_features_t *	features_result,
						const char *		ns,
						const char *		path );

stdc_error_t xapc_set ( xapc_object_t	object,
						const char *	ns,
						const char *	path,
						const char *	value,
						xapc_features_t	features );

stdc_error_t xapc_append ( xapc_object_t	object,
						   const char *		ns,
						   const char *		path,
						   int				placeBefore,	/* Boolean */
						   const char *		value,
						   xapc_features_t	features );

stdc_error_t xapc_create_lang_alt ( xapc_object_t	object,
									const char *	ns,
									const char *	path,
									const char *	value,
									xapc_features_t	features );

stdc_error_t xapc_create_container ( xapc_object_t			object,
									 const char *			ns,
									 const char *			path,
									 xapc_rdf_container_t	container,
									 const char *			value,
									 xapc_features_t		features );

stdc_error_t xapc_remove ( xapc_object_t object, const char * ns, const char * path);

stdc_error_t xapc_parse ( xapc_object_t		object,
						  const char *		input,
						  stdc(size_t)		n,
						  xapc_parse_last_t last );

stdc_error_t xapc_serialize ( xapc_object_t				object,
							  stdc(size_t) *			result,
							  xapc_serialize_options_t	options );

stdc_error_t xapc_extract_serialization ( xapc_object_t		object,
										  stdc(size_t) *	result,
										  char	*			buf,
										  stdc(size_t)		nmax );

stdc_error_t xapc_make_packet ( xapc_object_t	object,
								const char * *	packetBuf,
								int *			packetLen,
								int				useFixedLen /* Boolean */ );

/*************************************************************************************************/

#ifdef __cplusplus
}
#endif

/*************************************************************************************************/

#endif
