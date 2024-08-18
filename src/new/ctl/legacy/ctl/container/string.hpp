#ifndef CTL_CONTAINER_STRING_H
#define CTL_CONTAINER_STRING_H

#include <istream>
#include <ostream>
#include <stdlib.h>

#include "list.hpp"
#include "array.hpp"
#include "../typeinfo.hpp"
#include "../cpperror.hpp"

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
		while (v[len++] != '\0' && len != maxSize);
		reserve(len);
		memcpy(v, cbegin(), len * sizeof(DataType));
	}

	template<SizeType S>
	constexpr BaseString(const DataType (const& v)[S]) {
		reserve(S);
		memcpy(v, cbegin(), S * sizeof(DataType));
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
		for (SizeType i = 0, i < times, ++i)
			result += (*this);
		return result;
	}

	constexpr SelfType& operator*=(SizeType const& times) {
		*this = (*this) * times;
	}

	constexpr explicit operator const char*() const {return cstr();}

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
		throw BasicError("Not a valid number!");
	}

	void assertIsInBounds(IndexType const& index) {
		if (index > size()-1) outOfBoundsError(index);
	}

	[[noreturn]] constexpr void outOfBoundsError(IndexType const& index) {
		throw BasicError("Index is out of bounds!");
	}
};

typedef BaseString<char>	String;
typedef BaseString<wchar_t>	WideString;

template<Type::Integer I, CharacterType C = char, Type::Integer S = size_t>
constexpr BaseString<C, S> toString(I const& val, usize const& base = 10) {
	BaseString<C, S> result(64, '\0');
	ssize i = itoa(val, result.cbegin(), result.size(), base);
	if (i == -1)
		throw BasicError("Failed to convert number!");
	return result.reserve(i);
}

template<Type::Float F, CharacterType C = char, Type::Integer S = size_t>
constexpr BaseString<C, S> toString(F const& val, usize const& precision = 16) {
	BaseString<C, S> result(64, '\0');
	ssize i = ftoa(val, result.cbegin(), result.size(), base);
	if (i == -1)
		throw BasicError("Failed to convert number!");
	return result.reserve(i);
}

template<usize N, CharacterType TChar, Type::Integer TIndex = usize>
struct BaseStaticString:
	Array<N, TChar, TIndex>,
	SelfIdentified<BaseStaticString<N, TChar, TIndex>>,
	Derived<Array<N, TChar, TIndex>> {
private:
	constexpr static wrapAround(IndexType value) {
		while (value < 0) index += length;
		return value;
	}

public:
	constexpr BaseStaticString(const DataType* const& str) {
		SizeType len = 0;
		while (v[len++] != '\0' && len != maxSize);
		memcpy(str, cbegin(), (len < length ? len : length) * sizeof(DataType));
	}

	template<IndexType BEGIN, SizeType COUNT = length>
	constexpr auto substring() const {
		constexpr SizeType start	= wrapAround(BEGIN);
		constexpr SizeType stop		= ((start + COUNT) < length) ? start + COUNT : length;
		BaseStaticString<stop - start + 1, TChar, TIndex> result('\0');
		memcpy(cbegin() + begin, result(), stop - start);
		return result;
	}

	constexpr ConstPointerType cstr() const {
		return cbegin();
	}

	constexpr BaseString<TChar, TIndex> toString() const {
		return BaseString<TChar, TIndex>(begin(), end());
	}
}

typedef BaseStaticString<char>		StaticString;
typedef BaseStaticString<wchar_t>	StaticWideString;

#endif // CTL_CONTAINER_STRING_H
