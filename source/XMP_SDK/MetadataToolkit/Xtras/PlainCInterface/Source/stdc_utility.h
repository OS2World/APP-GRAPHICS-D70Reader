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

#ifndef STDC_UNTILITIES
#define STDC_UNTILITIES

#include <deque>
#include <typeinfo>
#include <utility>

#include "stdc_interface.h"

/*************************************************************************************************/

namespace adobe {
namespace stdc {

/*************************************************************************************************/

class exception_map
	{
public:
	struct p_v
		{
		const std::type_info*	first;
		stdc_error_t			second;
		};
	typedef p_v value_type;
		
	exception_map();
	void insert(const value_type* first, const value_type* last); 
	stdc_error_t base_decode(const std::exception& except) throw();
private:
	typedef std::deque<value_type> store_type;

	bool		fSorted;
	store_type	fStore;
	};

/*************************************************************************************************/

} // namespace stdc
} // namespace adobe

/*************************************************************************************************/

#endif
