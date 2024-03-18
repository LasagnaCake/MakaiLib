#ifndef TYPE_STRING_H
#define TYPE_STRING_H

#include <istream>
#include <ostream>
#include "list.hpp"

template <class T>
concept CharacterType = Type::Equal<T, char> || Type::Equal<wchar>;

template <CharacterType T>
class BaseString: public List<T> {
public:
	typedef List<T> BaseType;

	typedef std::basic_ostream<DataType>	OutputStreamType;
	typedef std::basic_istream<DataType>	InputStreamType;

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

	constexpr OutputStreamType& operator<<(OutputStreamType& o) {
		o << data;
		return out;
	}

	constexpr BaseString& operator<<(BaseString const& other) {
		appendBack(other);
		return *this;
	}

	constexpr BaseString& operator>>(BaseString& other) const {
		other.appendBack(*this);
		return other;
	}

	constexpr BaseString operator+(BaseString const& other) const {
		BaseString result(*this);
		return result.appendBack(other);
	}

	constexpr BaseString& operator+=(BaseString const& other) {
		return appendBack(other);
	}

	constexpr BaseString operator*(SizeType const& times) const {
		BaseString result(count * times);
		for SSRANGE(i, 0, times)
			result += (*this);
		return result;
	}

	constexpr BaseString& operator*=(SizeType const& times) {
		*this = (*this) * times;
	}

	BaseString substring(IndexType const& start, SizeType const& length) const {
		IndexType const stop = start + length;
		assertIsInBounds(start);
		return String(begin() + start, begin() + (stop < count ? stop : count-1));
	}
};

typedef BaseString<char>	String;
typedef BaseString<wchar_t>	WideString;

#endif // TYPE_STRING_H
