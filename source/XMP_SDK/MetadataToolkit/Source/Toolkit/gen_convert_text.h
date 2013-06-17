/*************************************************************************************************/

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/

/*************************************************************************************************/

#ifndef __gen_convert_text
#define __gen_convert_text

#include <stdexcept>
#include <string>
#include <utility>

/*************************************************************************************************/

namespace adobe {
namespace gen {

/*************************************************************************************************/

class illegal_utf_sequence : public std::runtime_error
	{
public:
	explicit illegal_utf_sequence(const char* what_arg) : runtime_error(what_arg) {}
	explicit illegal_utf_sequence(const std::string& what_arg) : runtime_error(what_arg) {}
	};

/*************************************************************************************************/

std::pair<const char*, wchar_t*> convert_utf (
	const char* sourceBegin, const char* sourceEnd,
	wchar_t* targetBegin, wchar_t* targetEnd, bool lenient = true)
		throw (illegal_utf_sequence);
	
std::pair<const wchar_t*, char*> convert_utf (
	const wchar_t* sourceBegin, const wchar_t* sourceEnd,
	char* targetBegin, char* targetEnd, bool lenient = true)
		throw (illegal_utf_sequence);

template<class Result, class Source>
Result convert_utf (const Source& source, bool lenient = true);

//template <> std::string convert_utf (const std::wstring& source);
//template <> std::wstring convert_utf (const std::string& source);

/*
	NOTE (sparent) : Other forms of convert UTF that would be of value:
	
	basic_string convert_utf (const charT*);
	basic_string convert_utf (const charT*, size_type n);
	basic_string convert_utf (InputIterator begin, InputIterator end);
*/

#if 0	// Not written yet	
bool is_legal_utf8 (const char* sourceBegin, const char* sourceEnd) throw();
bool is_legal_utf8 (const std::string& source) throw();
#endif

/*************************************************************************************************/

} // namespace adobe
} // namespace gen

/*************************************************************************************************/

#endif	// __gen_convert_text
