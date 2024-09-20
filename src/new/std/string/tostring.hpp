#ifndef MAKAILIB_STD_STRING_TOSTRING_H
#define MAKAILIB_STD_STRING_TOSTRING_H

#include "cpptypes.hpp"
#include "ctl/meta/fold.hpp"

namespace std {
	constexpr string	to_string(string const& s)		{return s;	}
	constexpr wstring	to_wstring(wstring const& ws)	{return ws;	}

	template<typename T>
	constexpr string	to_string(T const& val)	requires Type::Convertible<T, string>	{return static_cast<string>(val);	}
	template<typename T>
	constexpr wstring	to_wstring(T const& val) requires Type::Convertible<T, wstring>	{return static_cast<wstring>(val);	}

	constexpr string	to_string(char const& c)	{return string() + c;	}
	constexpr wstring	to_wstring(wchar const& wc)	{return wstring() + wc;	}
}

#endif // MAKAILIB_STD_TOSTRING_H
