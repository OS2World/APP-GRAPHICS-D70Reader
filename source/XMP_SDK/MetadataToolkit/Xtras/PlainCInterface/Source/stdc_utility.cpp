/*************************************************************************************************/

/*

ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.

*/

/*************************************************************************************************/

#include <algorithm>
#include <typeinfo>
#include <stdexcept>

#include "stdc_utility.h"

/*************************************************************************************************/

namespace adobe {
namespace stdc {

class unknown_error : public std::exception
	{ };

/*************************************************************************************************/

exception_map::exception_map() :
	fSorted(false)
	{
	stdc::exception_map::value_type table[] =
		{
			{ &(typeid (unknown_error)),			stdc_unknown_error },
			{ &(typeid (std::exception)),			stdc_exception },
			{ &(typeid (std::bad_alloc)),			stdc_bad_alloc },
			{ &(typeid (std::bad_cast)),			stdc_bad_cast },
			{ &(typeid (std::bad_exception)),		stdc_bad_exception },
			{ &(typeid (std::bad_typeid)),			stdc_bad_typeid },
			{ &(typeid (std::logic_error)),			stdc_logic_error },
			{ &(typeid (std::invalid_argument)),	stdc_invalid_argument },
			{ &(typeid (std::length_error)),		stdc_length_error },
			{ &(typeid (std::out_of_range)),		stdc_out_of_range },
			{ &(typeid (std::runtime_error)),		stdc_runtime_error },
			{ &(typeid (std::overflow_error)),		stdc_overflow_error },
			{ &(typeid (std::range_error)),			stdc_range_error },
			{ &(typeid (std::underflow_error)),		stdc_underflow_error }
		};
		
	insert(&table[0], &table[sizeof(table) / sizeof(stdc::exception_map::value_type)]);
	}

/*************************************************************************************************/

void exception_map::insert(const value_type* first, const value_type* last)
	{

	/*
		REVISIT : We should use Apple's ConditionalMacros.h
		to determine the compiler vendor and version here.
	*/

#if 0
	// Removed for compatibility with Windows
	fStore.insert(fStore.end(), first, last);
#else
	// This is less efficient then the above but the same time order.
	for (const value_type* iter = first; iter != last; ++iter)
		{
		fStore.push_back(*iter);
		}
#endif
	
	fSorted = false;
	}

/*************************************************************************************************/

static bool ExceptionLess(const exception_map::value_type& a, const exception_map::value_type& b)
	{
	return static_cast<bool>(a.first->before(*b.first));
	}

/*************************************************************************************************/

/*
PRECODITION: decode() can _only_ be called when an exception is in flight.
POSTCONDITION: decode() will not throw an exception. If the exception cannot be matched then
stdc_unknown_error is returned.

NOTE: Decode is used to actually decode an exception. A deque of pairs of typeids and error codes is
used as the mapping. If the deque has not been sorted since the last time a pair was inserted it is
sorted now. Then the typeid is taken and the deque is searched using lower_bounds.
*/

stdc_error_t exception_map::base_decode(const std::exception& except) throw()
	{
	if (!fSorted)
		{
		std::sort(fStore.begin(), fStore.end(), ExceptionLess);
		fSorted = true;
		}
		
	const std::type_info* info = &(typeid (except));
		
	value_type key = { info, stdc_no_error };
	store_type::const_iterator iter = std::lower_bound(fStore.begin(), fStore.end(), key, ExceptionLess);
	
	if (*(*iter).first == *info) return (*iter).second;
	
	return stdc_unknown_error;
	}

/*************************************************************************************************/
	
} // namespace xapc
} // namespace adobe

/*************************************************************************************************/
