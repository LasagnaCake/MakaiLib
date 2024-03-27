#ifndef CTL_TYPE_STRING_H
#define CTL_TYPE_STRING_H

#include <istream>
#include <ostream>
#include "list.hpp"
#include <stdlib.h>

template<class T>
concept CharacterType = Type::Equal<T, char> || Type::Equal<wchar>;

template<CharacterType T, Type::Integer I = size_t>
class BaseString: public List<T, I> {
public:
	// Parent type
	typedef List<T, I> BaseType;
	// Stream types
	typedef std::basic_ostream<DataType>	OutputStreamType;
	typedef std::basic_istream<DataType>	InputStreamType;
	// Self type
	typedef BaseString<DataType, IndexType>	SelfType;

	using BaseType::List;

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

	constexpr SelfType const& operator<<(SelfType& other) const	{other.appendBack(*this); return *this;}
	constexpr SelfType& operator<<(SelfType& other)				{other.appendBack(*this); return *this;}

	constexpr SelfType& operator>>(SelfType const& other)	{appendBack(other); return other;}

	constexpr SelfType operator+(SelfType const& other) const	{return SelfType(*this) + result.appendBack(other);}

	constexpr SelfType operator+(const DataType* const& str) const		{return (*this) + String(str);}
	constexpr SelfType operator+(const DataType (const& str)[S]) const	{return (*this) + String(str);}

	constexpr SelfType operator+(const DataType* const& str, SelfType const& self) const		{return String(str) + self;}
	constexpr SelfType operator+(const DataType (const& str)[S], SelfType const& self) const	{return String(str) + self;}

	constexpr SelfType& operator+=(SelfType const& other)			{return appendBack(other);	}
	constexpr SelfType& operator+=(const DataType* const& str)		{return appendBack(str);	}
	constexpr SelfType& operator+=(const DataType (const& str)[S])	{return appendBack(str);	}

	constexpr SelfType operator*(SizeType const& times) const {
		SelfType result(size() * times);
		for SSRANGE(i, 0, times)
			result += (*this);
		return result;
	}

	constexpr SelfType& operator*=(SizeType const& times) {
		*this = (*this) * times;
	}

	SelfType substring(IndexType const& start, SizeType const& length) const {
		IndexType const stop = start + length;
		assertIsInBounds(start);
		return String(begin() + start, begin() + (stop < size() ? stop : size()-1));
	}

	constexpr static floatmax toFloat(String const& str) try {
		return atod(str.cbegin());
	}

	constexpr static uintmax toUnsignedInt(String const& str) {
		return atoull(str.cbegin());
	}

	constexpr static intmax toInt(String const& str) {
		return atoll(str.cbegin());
	}

private:
	[[noreturn]] void invalidNumberError(String const& v) {
		throw Error::InvalidValue(
			"Value '" + v + "' is not a valid number!",
			__FILE__,
			toString(__LINE__)
		);
	}
};

typedef BaseString<char>	String;
typedef BaseString<wchar_t>	WideString;

#endif // CTL_TYPE_STRING_H
