/*************************************************************************************************/

/*

ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.

*/

/*************************************************************************************************/

#include <memory>
#include <typeinfo>
#include <bitset>

#include "XAPToolkit.h"
#include "UtilityXAP.h"

#include "stdc_utility.h"
#include "stdc_interface.h"
#include "xapc_interface.h"

using namespace adobe;

/*************************************************************************************************/

/*

The opaque object is used to hide the MetaXAP object from the c interface. It also contains the
result string for get operations.

*/

struct xapc_opaque_object : public MetaXAP
{
	public:
		const char* result_string ( const std::string& string ) const;
	private:
		mutable std::string	fResultString;
};

/*************************************************************************************************/

const char *
xapc_opaque_object::result_string ( const std::string& string ) const
{
	fResultString = string;
 	return fResultString.c_str();
}

/*************************************************************************************************/

namespace adobe {
namespace xapc {

/*************************************************************************************************/

class exception_map : public stdc::exception_map
{
public:
	exception_map();
	static stdc_error_t decode ( const std::exception& except ) throw();
};
	
exception_map::exception_map()
{
	stdc::exception_map::value_type table[]	= { { &(typeid (xap_error)),		xapc_error },
												{ &(typeid (xap_bad_xap)),		xapc_bad_xap },
												{ &(typeid (xap_bad_xml)),		xapc_bad_xml },
												{ &(typeid (xap_no_match)),		xapc_no_match },
												{ &(typeid (xap_client_fault)),	xapc_client_fault },
												{ &(typeid (xap_bad_access)),	xapc_bad_access },
												{ &(typeid (xap_bad_feature)),	xapc_bad_feature },
												{ &(typeid (xap_bad_number)),	xapc_bad_number },
												{ &(typeid (xap_bad_path)),		xapc_bad_path },
												{ &(typeid (xap_bad_schema)),	xapc_bad_schema },
												{ &(typeid (xap_bad_type)),		xapc_bad_type } };
	const int	tableCount	= sizeof ( table ) / sizeof ( stdc::exception_map::value_type );
	insert ( &table[0], &table[tableCount] );
}

/*************************************************************************************************/
	
/*
POSTCONDITION: decode() will not throw an exception. If the map cannot be allocated then
stdc_bad_alloc is returned.

NOTE: decode() cannot throw an exception since it is called to decode an exception in flight.

The map is cashed in a static auto_ptr so that it will be deleted when the application quits or
the library is unloaded. The static is local to the function to control creation time. If the map
cannot be allocated stdc_bad_alloc is returned - the original exception may not have been a bad_alloc
but odds are if we don't have enough memory for this, it was.

The map won't be allocated until an exception needs to be decoded.
*/

stdc_error_t
exception_map::decode ( const std::exception& except ) throw()
{
	typedef std::auto_ptr<exception_map> map_t;
	
	static map_t map;
	
	#if 0

		/*
			NOTE : This is the functional equivalent of the below but VC++ didn't have a reset()
			member and GNU would take the assignment without an explicit temp.
		*/
		if ( ! map.get() ) map.reset ( new ( std::nothrow ) exception_map() );
	
	#else
	
		if ( ! map.get() ) {
			map_t temp ( new ( std::nothrow ) exception_map() );
			map = temp;
		}
		
	#endif

	return map.get() ? map->base_decode ( except ) : stdc_bad_alloc;
}

/*************************************************************************************************/
	
} // namespace xapc
} // namespace adobe

/*************************************************************************************************/

extern "C" xapc_object_t
xapc_create ( void )
{
	xapc_object_t result = NULL;
	try {
		result = new xapc_opaque_object();
	} catch ( ... ) {
		/* Do nothing. */
	}
	return result;
}

/*************************************************************************************************/
	
extern "C" void
xapc_destroy (xapc_object_t object )
{
	delete object;
}

/*************************************************************************************************/

extern "C" stdc_error_t
xapc_register_namespace ( xapc_object_t	object,
						  const char *	ns,
						  const char *	prefix )
{
	stdc_error_t error = stdc_no_error;
	
	if ( object == 0 ) return xapc_null_object;
	if ( ns == 0 ) return xapc_client_fault;
	if ( prefix == 0 ) return xapc_client_fault;
	
	try {
		object->RegisterNamespace ( ns, prefix );
	} catch ( const std::exception& except ) {
		error = xapc::exception_map::decode ( except );
	} catch ( ... ) {
		error = stdc_unknown_error;
	}
		
	return error;
}

/*************************************************************************************************/

extern "C" stdc_error_t
xapc_get ( const_xapc_object_t	object,
		   const char * *		result,
		   xapc_features_t *	features_result,
		   const char *			ns,
		   const char *			path )
{
	stdc_error_t error = stdc_no_error;
	
	if ( object == 0 ) return xapc_null_object;
	if ( ns == 0 ) return xapc_client_fault;
	if ( path == 0 ) return xapc_client_fault;
	
	try {
		
		std::string val;
		
		XAPFeatures features = XAP_FEATURE_NONE;
		bool found = object->get ( ns, path, val, features );
		
		if ( ! found ) throw xap_no_match();
		if ( result != 0 ) *result = object->result_string ( val );
		if ( features_result != 0 ) *features_result = static_cast<xapc_features_t>(features);

	} catch ( const std::exception& except ) {
		error = xapc::exception_map::decode ( except );
	} catch ( ... ) {
		error = stdc_unknown_error;
	}
		
	return error;
}

/*************************************************************************************************/

extern "C" stdc_error_t
xapc_set ( xapc_object_t	object,
		   const char *		ns,
		   const char *		path,
		   const char *		value,
		   xapc_features_t	features )
{
	stdc_error_t error = stdc_no_error;
	
	if ( object == 0 ) return xapc_null_object;
	if ( ns == 0 ) return xapc_client_fault;
	if ( path == 0 ) return xapc_client_fault;
	if ( value == 0 ) return xapc_client_fault;
	
	try {
		object->set ( ns, path, value, static_cast <XAPFeatures> ( features ) );
	} catch ( const std::exception& except ) {
		error = xapc::exception_map::decode ( except );
	} catch ( ... ) {
		error = stdc_unknown_error;
	}
		
	return error;
}

/*************************************************************************************************/

extern "C" stdc_error_t
xapc_append ( xapc_object_t		object,
			  const char *		ns,
			  const char *		path,
			  int				placeBefore,	/* Boolean */
			  const char *		value,
			  xapc_features_t	features )
{
	stdc_error_t error = stdc_no_error;
	
	if ( object == 0 ) return xapc_null_object;
	if ( ns == 0 ) return xapc_client_fault;
	if ( path == 0 ) return xapc_client_fault;
	if ( value == 0 ) return xapc_client_fault;
	
	try {
		object->append ( ns, path, value, placeBefore, static_cast<XAPFeatures>(features) );
	} catch ( const std::exception& except ) {
		error = xapc::exception_map::decode ( except );
	} catch ( ... ) {
		error = stdc_unknown_error;
	}
		
	return error;
}

/*************************************************************************************************/

extern "C" stdc_error_t
xapc_create_lang_alt ( xapc_object_t	object,
					   const char *		ns,
					   const char *		path,
					   const char *		value,
					   xapc_features_t	features )
{
	stdc_error_t error = stdc_no_error;
	
	if ( object == 0 ) return xapc_null_object;
	if ( ns == 0 ) return xapc_client_fault;
	if ( path == 0 ) return xapc_client_fault;
	if ( value == 0 ) return xapc_client_fault;
	
	try {
		object->createFirstItem ( ns, path, value, "xml:lang", "x-default",
								  true, static_cast<XAPFeatures>(features) );
	} catch ( const std::exception& except ) {
		error = xapc::exception_map::decode ( except );
	} catch ( ... ) {
		error = stdc_unknown_error;
	}
		
	return error;
}

/*************************************************************************************************/

extern "C" stdc_error_t
xapc_create_container ( xapc_object_t			object,
						const char *			ns,
						const char *			path,
						xapc_rdf_container_t	container,
						const char *			value,
						xapc_features_t			features )
{
	stdc_error_t error = stdc_no_error;
	
	if ( object == 0 ) return xapc_null_object;
	if ( ns == 0 ) return xapc_client_fault;
	if ( path == 0 ) return xapc_client_fault;
	if ( value == 0 ) return xapc_client_fault;
	
	try {
		object->createFirstItem ( ns, path, value, static_cast<XAPStructContainerType>(container),
								  static_cast<XAPFeatures>(features) );
	} catch ( const std::exception& except ) {
		error = xapc::exception_map::decode ( except );
	} catch ( ... ) {
		error = stdc_unknown_error;
	}
		
	return error;
}

/*************************************************************************************************/

extern "C" stdc_error_t
xapc_remove ( xapc_object_t	object,
			  const char *	ns,
			  const char *	path )
{
	stdc_error_t error = stdc_no_error;
	
	if ( object == 0 ) return xapc_null_object;
	if ( ns == 0 ) return xapc_client_fault;
	if ( path == 0 ) return xapc_client_fault;
	
	try {
		object->remove ( ns, path );
	} catch ( const std::exception& except ) {
		error = xapc::exception_map::decode ( except );
	} catch ( ... ) {
		error = stdc_unknown_error;
	}
		
	return error;
}

/*************************************************************************************************/

extern "C" stdc_error_t
xapc_parse ( xapc_object_t		object,
			 const char *		input,
			 stdc(size_t)		n,
			 xapc_parse_last_t	last )
{
	stdc_error_t error = stdc_no_error;
	
	if ( object == 0 ) return xapc_null_object;
	if ( input == 0 ) return xapc_client_fault;
	
	try {
		object->parse ( input, n, last );
	} catch ( const std::exception& except ) {
		error = xapc::exception_map::decode ( except );
	} catch ( ... ) {
		error = stdc_unknown_error;
	}
		
	return error;
}

/*************************************************************************************************/

extern "C" stdc_error_t
xapc_serialize ( xapc_object_t				object,
				 stdc(size_t) *				result,
				 xapc_serialize_options_t	options )
{
	stdc_error_t error = stdc_no_error;
	
	if ( object == 0 ) return xapc_null_object;
	if ( result == 0 ) return xapc_client_fault;
	
	try {

		std::bitset <sizeof ( xapc_serialize_options_t ) * 8> bitField ( options );
		
		int escapeOptions = (bitField.test ( xapc_escape_cr_bit ) ? XAP_ESCAPE_CR : 0) &
							(bitField.test ( xapc_escape_lf_bit ) ? XAP_ESCAPE_LF : 0);
			
		XAPFormatType format =
				bitField.test ( xapc_format_compact_bit ) ? xap_format_compact : xap_format_pretty;
		
		*result = object->serialize ( format, escapeOptions );

	} catch ( const std::exception& except ) {
		error = xapc::exception_map::decode ( except );
	} catch ( ... ) {
		error = stdc_unknown_error;
	}
		
	return error;
}

/*************************************************************************************************/
	
extern "C" stdc_error_t
xapc_extract_serialization ( xapc_object_t	object,
							 stdc(size_t) *	result,
							 char *			buf,
							 stdc(size_t)	nmax )
{
	stdc_error_t error = stdc_no_error;
	
	if ( object == 0 ) return xapc_null_object;
	if ( result == 0 ) return xapc_client_fault;
	if ( buf == 0 ) return xapc_client_fault;
	
	try {
		size_t count = object->extractSerialization ( buf, nmax );
		*result = count;
	} catch ( const std::exception& except ) {
		error = xapc::exception_map::decode ( except );
	} catch ( ... ) {
		error = stdc_unknown_error;
	}
		
	return error;
}

/*************************************************************************************************/

extern "C" stdc_error_t
xapc_init ( void )
{
	stdc_error_t error = stdc_no_error;

	try {
		XAPTk_GeneralInit();
	} catch ( const std::exception& except ) {
		error = xapc::exception_map::decode ( except );
	} catch ( ... ) {
		error = stdc_unknown_error;
	}

	return error;
}

/*************************************************************************************************/

extern "C" stdc_error_t
xapc_terminate ( void )
{
	stdc_error_t error = stdc_no_error;

	try {
		XAPTk_Terminate();
	} catch ( const std::exception& except ) {
		error = xapc::exception_map::decode ( except );
	} catch ( ... ) {
		error = stdc_unknown_error;
	}

	return error;
}

/*************************************************************************************************/

extern "C" stdc_error_t
xapc_make_packet ( xapc_object_t	object,
				   const char * *	packetBuf,
				   int *			packetLen,
				   int				useFixedLen /* Boolean */ )
{
    stdc_error_t		error		= stdc_no_error;
    static std::string	sPacketString;
    const int  			bufferSize	= 4096;
    char				buffer [bufferSize];
    long				xmlLength, ioCount, padLength;
    std::string			packetHeader, packetTrailer;

    if ( object == 0 ) return xapc_null_object;
	if ( packetBuf == 0 ) return xapc_client_fault;
	if ( packetLen == 0 ) return xapc_client_fault;

    try {
 
        sPacketString.erase ( sPacketString.begin(), sPacketString.end() );

        xmlLength = object->serialize ( xap_format_compact );

        if ( xmlLength == 0 ) {

            *packetLen = 0;
            *packetBuf = NULL;

        } else {

            while ( true ) {
                ioCount = object->extractSerialization ( buffer, bufferSize-1 );
                if ( ioCount == 0 ) break;
                sPacketString.insert ( sPacketString.size(), buffer, ioCount );
            }

			if ( useFixedLen ) {
				padLength = - (*packetLen);
			} else {
            	padLength = (xmlLength < 4096) ? 4096 : (xmlLength / 2);
            }

            UtilityXAP::CreateXMLPacket ( "", true, padLength, "\n",
										  packetHeader, packetTrailer, &sPacketString );

            *packetLen = sPacketString.size();
            *packetBuf = sPacketString.data();

        }

	} catch ( const std::exception& except ) {
		error = xapc::exception_map::decode ( except );
	} catch ( ... ) {
		error = stdc_unknown_error;
	}

    return error;
}

/*************************************************************************************************/
