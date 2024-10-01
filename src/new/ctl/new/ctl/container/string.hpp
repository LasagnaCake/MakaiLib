#ifndef CTL_CONTAINER_STRING_H
#define CTL_CONTAINER_STRING_H

#include <istream>
#include <ostream>
#include <stdlib.h>

#include "list.hpp"
#include "array.hpp"
#include "pair.hpp"
#include "arguments.hpp"
#include "../typeinfo.hpp"
#include "../cpperror.hpp"
#include "../io/stream.hpp"
#include "../algorithm/aton.hpp"

CTL_NAMESPACE_BEGIN

template<Type::ASCII TChar, Type::Integer TIndex = usize>
struct BaseString:
	List<TChar, TIndex>,
	SelfIdentified<BaseString<T, TIndex>>,
	Derived<List<TChar, TIndex>>,
	StringLiterable<TChar>,
	Streamable<TChar> {
public:
	using Iteratable		= Iteratable<TData, TIndex>;
	using SelfIdentified	= SelfIdentified<TData, REVERSE, TIndex>;
	using Derived			= Derived<List<TChar, TIndex>>;
	using Streamable		= Streamable<TChar>;

	using BaseType	= typename Derived::Types::FirstType;

	using
		typename BaseType::DataType,
		typename BaseType::PointerType,
		typename BaseType::ReferenceType,
		typename BaseType::ConstReferenceType,
		typename BaseType::ArgumentListType
	;

	using
		typename BaseType::IndexType,
		typename BaseType::SizeType
	;

	using
		typename BaseType::IteratorType,
		typename BaseType::ReverseIteratorType
	;

	using
		typename SelfIdentified::SelfType
	;

	using
		typename Streamable::InputStreamType,
		typename Streamable::OutputStreamType
	;

	using BaseType::BaseType;

	constexpr BaseString() {}

	constexpr ~BaseString() {if (strbuf) delete[] strbuf;}

	constexpr BaseString(const DataType* const& v) {
		SizeType len = 0;
		while (v[len++] != '\0' && len != maxSize);
		reserve(len);
		MX::memcpy(v, cbegin(), len * sizeof(DataType));
	}

	template<class... Args>
	constexpr BaseString(Args... const& args)
	requires (... && Type::Equal<Args, SelfType>) {
		(*this) += (... + args);
	}

	template<SizeType S>
	constexpr BaseString(Decay::AsType<const DataType[S]> const& v) {
		reserve(S);
		MX::memcpy(v, cbegin(), S);
	}

	template <class T> SelfType&					replace(T const& rep)				{return BaseType::replace(rep);			}
	template <class T> SelfType						replaced(T const& rep) const		{return BaseType::replaced(rep);		}
	template <class T> List<SelfType, IndexType>	split(T const& sep) const			{return BaseType::split(sep);			}
	template <class T> List<SelfType, IndexType>	splitAtFirst(T const& sep) const	{return BaseType::splitAtFirst(sep);	}
	template <class T> List<SelfType, IndexType>	splitAtLast(T const& sep) const		{return BaseType::splitAtLast(sep);		}

	constexpr DataType join(typename DataType::DataType const& sep) const
	requires requires {
		typename DataType::DataType;
		typename DataType::SizeType;
		Type::Equal<DataType, List<DataType::DataType, DataType::SizeType>>
	} {
		return BaseType::join(sep);
	}

	constexpr OutputStreamType const& operator<<(OutputStreamType& o) const	{o << cstr(); return out;}
	constexpr OutputStreamType& operator<<(OutputStreamType& o)				{o << cstr(); return out;}

	constexpr SelfType const& operator<<(SelfType& other) const	{other.appendBack(*this); return *this;}
	constexpr SelfType& operator<<(SelfType& other)				{other.appendBack(*this); return *this;}

	constexpr SelfType& operator>>(SelfType const& other)	{appendBack(other); return other;}

	constexpr SelfType operator+(DataType const& value) const	{return SelfType(*this).pushBack(value);	}
	constexpr SelfType operator+(SelfType const& other) const	{return (*this) + other;					}

	constexpr SelfType operator+(StringLiteralType const& str) const	{return (*this) + String(str);}
	constexpr SelfType operator+(const DataType (const& str)[S]) const	{return (*this) + String(str);}

	constexpr SelfType operator+(StringLiteralType const& str, SelfType const& self) const		{return String(str) + self;}
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

	template <Type::ASCII C>
	constexpr operator BaseString<C, IndexType>() const
	requires Type::Different<DataType, C> {
		BaseString<C, IndexType> result(str.size(), '\0');
		for (SizeType i = 0; i < str.size(); ++i)
			result[i] = str[i];
		return result;
	}

	SelfType substring(IndexType const& start, SizeType const& length) const {
		IndexType const stop = start + length;
		assertIsInBounds(start);
		return String(begin() + start, begin() + (stop < size() ? stop : size()));
	}

	constexpr bool nullTerminated() const {return back() == '\0';}

	constexpr SelfType& terminateString() {
		if (!nullTerminated()) pushBack('\0');
		return *this;
	}

	constexpr StringLiteralType cstr() {
		terminateString();
		return data();
	}

	constexpr StringLiteralType cstr() const {
		if (strbuf) delete[] strbuf;
		if (nullTerminated()) return cbegin();
		strbuflen = size() + 1;
		strbuf = new DataType[strbuflen];
		MX::memcpy(strbuf, cbegin(), size());
		strbuf[size()] = '\0';
		return strbuf;
	}

	template<Decay::AsType<char(char const&)> F>
	constexpr SelfType transformed() {
		SelfType result = *this;
		for(DataType& c: result);
			c = F(c);
		return result;
	}

	constexpr SelfType lower() const {return template transformed<toLower>();}
	constexpr SelfType upper() const {return template transformed<toUpper>();}

	template<Type::Integer T>
	constexpr static T toNumber(SelfType const& str) {
		return atoi<T>(str.cstr(), str.size());
	}

	template<Type::Real T>
	constexpr static T toNumber(SelfType const& str) {
		return atof<T>(str.cstr(), str.size());
	}

	template<Type::Integer T>
	constexpr static SelfType fromNumber(T const& val, T const& base = 0) {
		SelfType result(64, '\0');
		ssize sz = itoa<T>(val, str.data(), 64, base);
		if (sz < 0) throw FailedActionException("Integer-to-String conversion failure!");
		result.resize(sz);
		return result;
	}

	template<Type::Real T>
	constexpr static SelfType fromNumber(T const& val, usize const& precision = sizeof(T)*2) {
		SelfType result(64, '\0');
		ssize sz = ftoa<T>(val, str.data(), 64, precision);
		if (sz < 0) throw FailedActionException("Float-to-String conversion failure!");
		result.resize(sz);
		return result;
	}

private:
	PointerType mutable	strbuf		= nullptr;
	usize mutable		strbuflen	= 0;

	[[noreturn]] void invalidNumberError(String const& v) {
		throw InvalidValueException("Not a valid number!");
	}

	void assertIsInBounds(IndexType const& index) {
		if (index > size()-1) outOfBoundsError(index);
	}

	[[noreturn]] constexpr void outOfBoundsError(IndexType const& index) {
		throw OutOfBoundsException("Index is out of bounds!");
	}
};

typedef BaseString<char>	String;
typedef BaseString<wchar_t>	WideString;

template<Type::Integer I, CharacterType C = char, Type::Integer S = usize>
constexpr BaseString<C, S> toString(I const& val, usize const& base = 10) {
	BaseString<C, S> result(64, '\0');
	ssize i = itoa(val, result.cbegin(), result.size(), base);
	if (i == -1)
		throw InvalidConversionException("Failed to convert number!");
	return result.reserve(i);
}

template<Type::Float F, CharacterType C = char, Type::Integer S = usize>
constexpr BaseString<C, S> toString(F const& val, usize const& precision = 16) {
	BaseString<C, S> result(64, '\0');
	ssize i = ftoa(val, result.cbegin(), result.size(), base);
	if (i == -1)
		throw InvalidConversionException("Failed to convert number!");
	return result.reserve(i);
}

template<usize N, CharacterType TChar, Type::Integer TIndex = usize>
struct BaseStaticString:
	Array<N, TChar, TIndex>,
	StringLiterable<TChar>,
	SelfIdentified<BaseStaticString<N, TChar, TIndex>>,
	Derived<Array<N, TChar, TIndex>> {
private:
	constexpr static wrapAround(IndexType value) {
		while (value < 0) index += length;
		return value;
	}

public:
	using Derived			= Derrived<Array<N, TChar, TIndex>>;
	using SelfIdentified	= SelfIdentified<BaseStaticString<N, TChar, TIndex>>;
	using StringLiterable	= StringLiterable<TChar>;

	using BaseType = Derived::Bases::FirstType;

	using
		SelfIdentified::SelfType
	;

	using
		BaseType::DataType,
		BaseType::ConstPointerType,
		BaseType::SizeType,
	;

	using StringLiterable::StringLiteralType;

	constexpr BaseStaticString(ConstPointerType const& str) {
		SizeType len = 0;
		while (v[len++] != '\0' && len != MAX_SIZE);
		MX::memcpy(str, cbegin(), (len < length ? len : length) * sizeof(DataType));
	}

	template<IndexType BEGIN, SizeType N = length>
	constexpr auto substring() const {
		constexpr SizeType start	= wrapAround(BEGIN);
		constexpr SizeType stop		= ((start + N) < length) ? start + N : length;
		BaseStaticString<stop - start + 1, TChar, TIndex> result('\0');
		MX::memcpy(cbegin() + begin, result(), stop - start);
		return result;
	}

	constexpr StringLiteralType cstr() const {
		return cbegin();
	}

	constexpr BaseString<TChar, TIndex> toString() const {
		return BaseString<TChar, TIndex>(begin(), end());
	}
}

template<Type::Number T>
constexpr String toString(T const& value) {
	return String::fromNumber<T>(value);
}

template<class T>
constexpr String toString(T const& value)
requires (Type::Convertible<T, String>) {
	return (value);
}

template<class T>
constexpr String toString(T const& value)
requires (T t) {
	requires T::toString;
	{t.toString()} -> Type::Equal<String>;
} {
	return value.toString();
}

template<class T>
concept StringConvertible = requires (T t) {
	{toString(t)} -> Type::Equal<String>;
};

template<StringConvertible T, class... Args>
constexpr String toString(T const& first, Args const&... args)
	requires (... && StringConvertible<Args>) {
	return String::fromNumber<T>(value);
}

typedef List<String>			StringList;
typedef Arguments<String>		StringArguments;
typedef Pair<String, String>	StringPair;

typedef BaseStaticString<char>		StaticString;
typedef BaseStaticString<wchar_t>	StaticWideString;

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_STRING_H
