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
#include "../algorithm/transform.hpp"
#include "../algorithm/validate.hpp"
#include "../typetraits/traits.hpp"

CTL_NAMESPACE_BEGIN

template<Type::ASCII TChar, Type::Integer TIndex = usize>
struct BaseString:
	List<TChar, TIndex>,
	SelfIdentified<BaseString<TChar, TIndex>>,
	Derived<List<TChar, TIndex>>,
	StringLiterable<TChar>,
	Streamable<TChar> {
public:
	using Iteratable		= ::CTL::Iteratable<TChar, TIndex>;
	using SelfIdentified	= ::CTL::SelfIdentified<BaseString<TChar, TIndex>>;
	using Derived			= ::CTL::Derived<List<TChar, TIndex>>;
	using Streamable		= ::CTL::Streamable<TChar>;
	using StringLiterable	= ::CTL::StringLiterable<TChar>;

	using BaseType	= typename Derived::Bases::FirstType;

	using
		typename BaseType::DataType,
		typename BaseType::ConstantType,
		typename BaseType::PointerType,
		typename BaseType::ConstPointerType,
		typename BaseType::ReferenceType,
		typename BaseType::ConstReferenceType,
		typename BaseType::ArgumentListType
	;

	using
		typename BaseType::IndexType,
		typename BaseType::SizeType
	;

	using BaseType::MAX_SIZE;

	using
		typename BaseType::IteratorType,
		typename BaseType::ReverseIteratorType
	;

	using
		typename SelfIdentified::SelfType
	;

	using
		typename StringLiterable::StringLiteralType
	;

	using
		typename Streamable::InputStreamType,
		typename Streamable::OutputStreamType
	;

	using
		BaseType::BaseType,
		BaseType::data,
		BaseType::cbegin,
		BaseType::cend,
		BaseType::begin,
		BaseType::end,
		BaseType::rbegin,
		BaseType::rend,
		BaseType::pushBack,
		BaseType::popBack,
		BaseType::appendBack,
		BaseType::front,
		BaseType::back,
		BaseType::insert,
		BaseType::transformed,
		BaseType::validate,
		BaseType::size,
		BaseType::empty,
		BaseType::find,
		BaseType::rfind,
		BaseType::bsearch,
		BaseType::join,
		BaseType::capacity
	;

	constexpr BaseString() {}

	constexpr ~BaseString() {if (strbuf) delete[] strbuf;}

	constexpr BaseString(const DataType* const& v) {
		SizeType len = 0;
		while (v[len++] != '\0' && len <= MAX_SIZE);
		reserve(len);
		MX::memcpy(data(), v, len * sizeof(DataType));
	}

	template<class... Args>
	constexpr BaseString(Args const&... args)
	requires (... && Type::Equal<Args, SelfType>) {
		(*this) += (... + args);
	}

	template<SizeType S>
	constexpr BaseString(Decay::AsType<ConstantType[S]> const& v) {
		reserve(S);
		MX::memcpy(v, cbegin(), S);
	}

	template <class T> SelfType&					replace(T const& rep)				{return BaseType::replace(rep);			}
	template <class T> SelfType						replaced(T const& rep) const		{return BaseType::replaced(rep);		}
	template <class T> List<SelfType, IndexType>	split(T const& sep) const			{return BaseType::split(sep);			}
	template <class T> List<SelfType, IndexType>	splitAtFirst(T const& sep) const	{return BaseType::splitAtFirst(sep);	}
	template <class T> List<SelfType, IndexType>	splitAtLast(T const& sep) const		{return BaseType::splitAtLast(sep);		}

	constexpr OutputStreamType const& operator<<(OutputStreamType& o) const	{o << cstr(); return o;}
	constexpr OutputStreamType& operator<<(OutputStreamType& o)				{o << cstr(); return o;}

	constexpr InputStreamType& readFrom(InputStreamType& i, DataType const& stop) {
		DataType buf[32];
		while(i.getline(buf, 32, stop))
			appendBack(buf, i.gcount());
		return i;
	}
	
	constexpr InputStreamType& readFrom(InputStreamType& i) {
		return readFromStream(i, '\0');
	}

	constexpr SelfType const& operator<<(SelfType& other) const	{other.appendBack(*this); return *this;}
	constexpr SelfType& operator<<(SelfType& other)				{other.appendBack(*this); return *this;}

	constexpr SelfType& operator>>(SelfType const& other)	{appendBack(other); return other;}

	constexpr SelfType operator+(DataType const& value) const	{return SelfType(*this).pushBack(value);	}
	constexpr SelfType operator+(SelfType const& other) const	{return (*this) + other;					}

	constexpr SelfType operator+(StringLiteralType const& str) const				{return (*this) + String(str);}
	template<SizeType S>
	constexpr SelfType operator+(Decay::AsType<ConstantType[S]> const& str) const	{return (*this) + String(str);}

	constexpr SelfType& operator+=(DataType const& value)				{return pushBack(value);	}
	constexpr SelfType& operator+=(SelfType const& other)				{return appendBack(other);	}
	constexpr SelfType& operator+=(StringLiteralType const& str)		{return appendBack(str);	}
	template<SizeType S>
	constexpr SelfType& operator+=(Decay::AsType<ConstantType[S]> str)	{return appendBack(str);	}

	constexpr SelfType operator*(SizeType const& times) const {
		SelfType result(size() * times);
		for (SizeType i = 0; i < times; ++i)
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
		BaseString<C, IndexType> result(size(), '\0');
		for (SizeType i = 0; i < size(); ++i)
			result[i] = data()[i];
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

	constexpr SelfType lower() const {return transformed(toLower);}
	constexpr SelfType upper() const {return transformed(toUpper);}

	constexpr bool isHex() const {return validate(isHexChar);}

	constexpr BaseString<char, SizeType> toString() const
	requires Type::Equal<DataType, wchar> {
		BaseString<char, SizeType> result;
		for (DataType const& c: *this)
			result.pushBack(char(c));
		return result;
	}

	constexpr SelfType toString() const
	requires Type::Equal<DataType, char> {return *this;}

	constexpr BaseString<wchar, SizeType> toWideString() const
	requires Type::Equal<DataType, char> {
		BaseString<wchar, SizeType> result;
		for (DataType const& c: *this)
			result.pushBack(wchar(c));
		return result;
	}

	constexpr SelfType toWideString() const
	requires Type::Equal<DataType, wchar> {return *this;}

	template<Type::Integer T>
	constexpr static T toNumber(SelfType const& str, T const& base = 0) {
		T val;
		if (!atoi<T>(str.data(), str.size(), val, base))
			throw FailedActionException("String-to-Integer conversion failure!");
		return val;
	}

	template<Type::Real T>
	constexpr static T toNumber(SelfType const& str) {
		T val;
		if (!atof<T>(str.data(), str.size(), val))
			throw FailedActionException("String-to-Float conversion failure!");
		return val;
	}

	template<Type::Integer T>
	constexpr static SelfType fromNumber(T const& val, T const& base = 0) {
		SelfType result(sizeof(T)*4, '\0');
		ssize sz = itoa<T>(val, result.data(), result.size(), base);
		if (sz < 0) throw FailedActionException("Integer-to-String conversion failure!");
		result.resize(sz);
		return result;
	}

	template<Type::Real T>
	constexpr static SelfType fromNumber(T const& val, usize const& precision = sizeof(T)*2) {
		SelfType result(sizeof(T)*4, '\0');
		ssize sz = ftoa<T>(val, result.data(), result.size(), precision);
		if (sz < 0) throw FailedActionException("Float-to-String conversion failure!");
		result.resize(sz);
		return result;
	}

private:
	PointerType mutable	strbuf		= nullptr;
	usize mutable		strbuflen	= 0;

	[[noreturn]] void invalidNumberError(StringLiteralType const& v) {
		throw InvalidValueException("Not a valid number!");
	}

	void assertIsInBounds(IndexType const& index) {
		if (index > size()-1) outOfBoundsError(index);
	}

	[[noreturn]] constexpr void outOfBoundsError(IndexType const& index) {
		throw OutOfBoundsException("Index is out of bounds!");
	}
};

template<Type::ASCII TChar, Type::Integer TIndex = usize>
constexpr typename BaseString<TChar, TIndex>::InputStreamType& operator>>(
	typename BaseString<TChar, TIndex>::InputStreamType& stream,
	BaseString<TChar, TIndex>& string
) {
	return string.readFrom(stream);
}



template<Type::ASCII TChar, Type::Integer TIndex = usize>
constexpr BaseString<TChar, TIndex>
operator+(
	typename BaseString<TChar, TIndex>::StringLiteralType const& str,
	BaseString<TChar, TIndex> const& self
) {
	return String(str) + self;
}

template<Type::ASCII TChar, Type::Integer TIndex = usize, AsUnsigned<TIndex> S>
constexpr BaseString<TChar, TIndex>
operator+(
	Decay::AsType<typename BaseString<TChar, TIndex>::ConstantType[S]> const& str,
	BaseString<TChar, TIndex> const& self
) {
	return String(str) + self;
}

typedef BaseString<char>	String;
typedef BaseString<wchar_t>	WideString;

template<Type::Integer I, Type::ASCII C = char, Type::Integer S = usize>
constexpr BaseString<C, S> toString(I const& val, usize const& base = 10) {
	BaseString<C, S> result(64, '\0');
	ssize i = itoa(val, result.cbegin(), result.size(), base);
	if (i == -1)
		throw InvalidConversionException("Failed to convert number!");
	return result.reserve(i);
}

template<Type::Real F, Type::ASCII C = char, Type::Integer S = usize>
constexpr BaseString<C, S> toString(F const& val, usize const& precision = sizeof(F) * 2) {
	BaseString<C, S> result(precision * 4, '\0');
	ssize i = ftoa(val, result.cbegin(), result.size(), precision);
	if (i == -1)
		throw InvalidConversionException("Failed to convert number!");
	return result.reserve(i);
}

template<usize N, Type::ASCII TChar, Type::Integer TIndex = usize>
struct BaseStaticString:
	Array<N, TChar, TIndex>,
	StringLiterable<TChar>,
	SelfIdentified<BaseStaticString<N, TChar, TIndex>>,
	Derived<Array<N, TChar, TIndex>> {
public:
	using Derived			= Derived<Array<N, TChar, TIndex>>;
	using SelfIdentified	= SelfIdentified<BaseStaticString<N, TChar, TIndex>>;
	using StringLiterable	= StringLiterable<TChar>;

	using BaseType = Derived::Bases::FirstType;

	using
		typename SelfIdentified::SelfType
	;

	using
		typename BaseType::DataType,
		typename BaseType::ConstPointerType,
		typename BaseType::SizeType,
		typename BaseType::IndexType
	;

	using
		BaseType::BaseType,
		BaseType::data,
		BaseType::cbegin,
		BaseType::cend,
		BaseType::begin,
		BaseType::end,
		BaseType::rbegin,
		BaseType::rend
	;

	using BaseType::SIZE, BaseType::MAX_SIZE;

	using typename StringLiterable::StringLiteralType;

private:
	constexpr static IndexType wrapAround(IndexType value) {
		while (value < 0) value += SIZE;
		return value;
	}

public:
	constexpr BaseStaticString(StringLiteralType const& str) {
		SizeType len = 0;
		while (str[len++] != '\0' && len <= MAX_SIZE);
		MX::memcpy(str, data(), (len < SIZE ? len : SIZE));
	}

	template<IndexType BEGIN, SizeType S = SIZE>
	constexpr auto substring() const {
		constexpr SizeType start	= wrapAround(BEGIN);
		constexpr SizeType stop		= ((start + S) < SIZE) ? start + S : SIZE;
		BaseStaticString<stop - start + 1, TChar, TIndex> result('\0');
		MX::memcpy(result.data(), data() + start, stop - start);
		return result;
	}

	constexpr StringLiteralType cstr() const {
		return data();
	}

	constexpr BaseString<TChar, TIndex> toString() const {
		return BaseString<TChar, TIndex>(begin(), end());
	}
};

typedef List<String>			StringList;
typedef Arguments<String>		StringArguments;
typedef Pair<String, String>	StringPair;

template<usize N> using StaticString		= BaseStaticString<N, char>;
template<usize N> using StaticWideString	= BaseStaticString<N, wchar_t>;

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_STRING_H
