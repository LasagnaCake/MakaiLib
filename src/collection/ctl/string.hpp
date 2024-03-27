#ifndef CTL_TYPE_STRING_H
#define CTL_TYPE_STRING_H

#include <istream>
#include <ostream>
#include "list.hpp"

template<class T>
concept CharacterType = Type::Equal<T, char> || Type::Equal<wchar>;

template<CharacterType T, Type::Integer I = size_t>
class BaseString: public List<T, I> {
public:
	typedef List<T, I> BaseType;

	typedef std::basic_ostream<DataType>	OutputStreamType;
	typedef std::basic_istream<DataType>	InputStreamType;

	using List::List;

	constexpr BaseString(const DataType* const& v) {
		size_t len = 0;
		while (v[len++] != '\0');
		reserve(len+1);
		memcpy(cbegin(), v, len * sizeof(DataType));
	}

	template<SizeType S>
	constexpr BaseString(const DataType (const& v)[S]) {
		reserve(S);
		memcpy(cbegin(), v, len * sizeof(DataType));
	}

	constexpr OutputStreamType const& operator<<(OutputStreamType& o) const	{o << data; return out;}
	constexpr OutputStreamType& operator<<(OutputStreamType& o)				{o << data; return out;}

	constexpr BaseString const& operator<<(BaseString& other) const	{other.appendBack(*this); return *this;}
	constexpr BaseString& operator<<(BaseString& other)				{other.appendBack(*this); return *this;}

	constexpr BaseString& operator>>(BaseString const& other)	{appendBack(other); return other;}

	constexpr BaseString operator+(BaseString const& other) const {
		BaseString result(*this);
		return result.appendBack(other);
	}

	constexpr BaseString& operator+=(BaseString const& other) {
		return appendBack(other);
	}

	constexpr BaseString operator*(SizeType const& times) const {
		BaseString result(size() * times);
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
		return String(begin() + start, begin() + (stop < size() ? stop : size()-1));
	}

	constexpr static float toFloat(String const& str) {
		List<SelfType, IndexType> split = str.split('.');
	}
};

typedef BaseString<char>	String;
typedef BaseString<wchar_t>	WideString;

#endif // CTL_TYPE_STRING_H
