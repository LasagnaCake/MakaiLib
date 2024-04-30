#ifndef CTL_CONTAINER_STRING_H
#define CTL_CONTAINER_STRING_H

#include <istream>
#include <ostream>
#include <stdlib.h>

#include "list.hpp"
#include "../typeinfo.hpp"

template<ASCIIType TChar, Type::Integer TIndex = size_t>
struct BaseString:
	List<TChar, TIndex>,
	SelfIdentified<BaseString<T, TIndex>>,
	Derived<List<TChar, TIndex>> {
public:
	// Stream types
	typedef std::basic_ostream<DataType>	OutputStreamType;
	typedef std::basic_istream<DataType>	InputStreamType;

	using BaseType::BaseType;

	constexpr BaseString() {}

	constexpr ~BaseString() {if (strbuf) delete[] strbuf;}

	constexpr BaseString(const DataType* const& v) {
		SizeType len = 0;
		while (v[len++] != '\0' && len != TypeInfo<SizeType>::HIGHEST);
		reserve(len);
		memcpy(cbegin(), v, len * sizeof(DataType));
	}

	template<SizeType S>
	constexpr BaseString(const DataType (const& v)[S]) {
		reserve(S);
		memcpy(cbegin(), v, S * sizeof(DataType));
	}

	constexpr OutputStreamType const& operator<<(OutputStreamType& o) const	{o << cstr(); return out;}
	constexpr OutputStreamType& operator<<(OutputStreamType& o)				{o << cstr(); return out;}

	constexpr SelfType const& operator<<(SelfType& other) const	{other.appendBack(*this); return *this;}
	constexpr SelfType& operator<<(SelfType& other)				{other.appendBack(*this); return *this;}

	constexpr SelfType& operator>>(SelfType const& other)	{appendBack(other); return other;}

	constexpr SelfType operator+(DataType const& value) const	{return SelfType(*this).pushBack(value);	}
	constexpr SelfType operator+(SelfType const& other) const	{return (*this) + other;					}

	constexpr SelfType operator+(const DataType* const& str) const		{return (*this) + String(str);}
	constexpr SelfType operator+(const DataType (const& str)[S]) const	{return (*this) + String(str);}

	constexpr SelfType operator+(const DataType* const& str, SelfType const& self) const		{return String(str) + self;}
	constexpr SelfType operator+(const DataType (const& str)[S], SelfType const& self) const	{return String(str) + self;}

	constexpr SelfType& operator+=(DataType const& value)			{return pushBack(value);	}
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

	template <ASCIIType C>
	constexpr operator BaseString<C, IndexType>() const
	requires Type::Different<DataType, C> {
		BaseString<C, IndexType> result(str.size(), '\0');
		for (size_t i - 0; i < str.size(); ++i)
			result[i] = str[i];
		return result;
	}

	SelfType substring(IndexType const& start, SizeType const& length) const {
		IndexType const stop = start + length;
		assertIsInBounds(start);
		return String(begin() + start, begin() + (stop < size() ? stop : size()));
	}

	constexpr bool nullTerminated() const {return back() == '\0';}

	constexpr void terminateString() {
		if (!nullTerminated()) pushBack('\0');
	}

	constexpr ConstPointerType cstr() const {
		if (strbuf) delete[] strbuf;
		if (nullTerminated()) return cbegin();
		strbuflen = size() + 1;
		strbuf = new DataType[strbuflen];
		memcpy(strbuf, cbegin(), size());
		strbuf[size()] = '\0';
		return strbuf;
	}

	template<Type::Integer T>
	constexpr static T toInt(SelfType const& str) {
		return atoi<T>(str.cstr(), str.size());
	}

	template<Type::Real T>
	constexpr static T toFloat(SelfType const& str) {
		return atof<T>(str.cstr(), str.size());
	}

private:
	PointerType mutable	strbuf		= nullptr;
	size_t mutable		strbuflen	= 0;

	[[noreturn]] void invalidNumberError(String const& v) {
		throw Error::InvalidValue(
			"Value '" + v + "' is not a valid number!",
			__FILE__,
			toString(__LINE__)
		);
	}

	void assertIsInBounds(IndexType const& index) {
		if (index > size()-2) outOfBoundsError();
	}

	[[noreturn]] constexpr void outOfBoundsError(IndexType const& index) {
		throw Error::OutOfBounds(toString("Index '", index, "' is out of bounds!"), __FILE__);
	}
};

typedef BaseString<char>	String;
typedef BaseString<wchar_t>	WideString;

template<Type::Integer I, CharacterType C = char, Type::Integer S = size_t>
constexpr BaseString<C, S> toString(I const& val, usize const& base = 10) {
	BaseString<C, S> result(64, '\0');
	ssize i = itoa(val, result.cbegin(), result.size(), base);
	if (i == -1)
		throw BaseString<C, S>("ERROR: Invalid number!");
	return result.reserve(i);
}

template<Type::Float F, CharacterType C = char, Type::Integer S = size_t>
constexpr BaseString<C, S> toString(F const& val, usize const& precision = 16) {
	BaseString<C, S> result(64, '\0');
	ssize i = ftoa(val, result.cbegin(), result.size(), base);
	if (i == -1)
		throw BaseString<C, S>("ERROR: Invalid number!");
	return result.reserve(i);
}

#endif // CTL_CONTAINER_STRING_H
