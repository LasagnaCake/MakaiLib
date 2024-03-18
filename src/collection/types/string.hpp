#ifndef TYPE_STRING_H
#define TYPE_STRING_H

#include <iostream>
#include "list.hpp"

template <class T>
concept CharacterType = Type::Equal<T, char> || Type::Equal<wchar_t>;

template <CharacterType T>
class BaseString: public List<T> {
	typedef List<T> BaseType;

	typedef std::ostream	OutputStreamType;
	typedef std::istream	InputStreamType;

	constexpr BaseString(const DataType* const& v) {
		size_t len = 0;
		while (v[len++] != '\0');
		reserve(len+1);
		copy(v, data, len);
	}

	template <SizeType S>
	constexpr BaseString(const DataType (const& v)[S]) {
		reserve(S);
		copy(v, data, len);
	}

	OutputStreamType& constexpr operator<<(OutputStreamType& o) {
		o << data;
		return out;
	}
};

#endif // TYPE_STRING_H
