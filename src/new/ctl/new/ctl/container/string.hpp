#ifndef CTL_CONTAINER_STRING_H
#define CTL_CONTAINER_STRING_H

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

	using typename Derived::BaseType;

	using typename BaseType::OrderType;

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
		BaseType::clear,
		BaseType::reserve,
		BaseType::resize,
		BaseType::tight,
		BaseType::tighten,
		BaseType::data,
		BaseType::cbegin,
		BaseType::cend,
		BaseType::begin,
		BaseType::end,
		BaseType::rbegin,
		BaseType::rend,
		BaseType::front,
		BaseType::back,
		BaseType::transformed,
		BaseType::validate,
		BaseType::size,
		BaseType::empty,
		BaseType::find,
		BaseType::rfind,
		BaseType::bsearch,
		BaseType::capacity,
		BaseType::appendBack,
		BaseType::pushBack,
		BaseType::popBack
	;

	constexpr BaseString(): BaseType() {}

	constexpr ~BaseString() {if (strbuf) MX::free(strbuf);}

	constexpr BaseString(StringLiteralType const& v) {
		SizeType len = 0;
		while (v[len++] != '\0' && len <= MAX_SIZE);
		reserve(len);
		appendBack(BaseType(v, v+len-1));
	}

	constexpr BaseString(BaseType const& other):	BaseType(other)				{}
	constexpr BaseString(BaseType&& other):			BaseType(CTL::move(other))	{}

	constexpr BaseString(SelfType const& other):	BaseType(other)				{}
	constexpr BaseString(SelfType&& other):			BaseType(CTL::move(other))	{}

	template<class T>
	constexpr BaseString(T const& other)
	requires requires (T t) {
		{t.begin()} -> Type::Equal<IteratorType>;
		{t.end()} -> Type::Equal<IteratorType>;
	}:
		BaseType(other.begin(), other.end()) {
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

	constexpr List<SelfType, IndexType> split(DataType const& sep) const {
		List<SelfType, IndexType> res;
		SelfType buf;
		for (ConstReferenceType v : *this) {
			if (v == sep) {
				res.pushBack(buf);
				buf.clear();
				continue;
			}
			buf += v;
		}
		if (res.empty()) res.pushBack(*this);
		return res;
	}

	constexpr List<SelfType, IndexType> split(BaseType const& seps) const {
		List<SelfType, IndexType> res;
		SelfType buf;
		for (ConstReferenceType v : *this) {
			if (seps.find(v)) {
				res.pushBack(buf);
				buf.clear();
				continue;
			}
			if (res.empty()) res.pushBack(*this);
			buf.pushBack(v);
		}
		return res;
	}

	/*constexpr List<SelfType, IndexType> split(ArgumentListType const& seps) const {
		List<SelfType, IndexType> res;
		SelfType buf;
		for (ConstReferenceType v : *this) {
			bool isSeparator = false;
			for (DataType& s: seps)
				if (v == s) {
					res.pushBack(buf);
					buf.clear();
					isSeparator = true;
					break;
				}
			if (isSeparator)
				continue;
			buf.pushBack(v);
		}
	}*/

	constexpr SelfType sliced(IndexType const& start) const							{return BaseType::sliced(start);		}
	constexpr SelfType sliced(IndexType const& start, SizeType const& count) const	{return BaseType::sliced(start, count);	}

	constexpr List<SelfType, IndexType> splitAtFirst(DataType const& sep) const {
		List<SelfType, IndexType> res;
		IndexType idx = find(sep);
		if (idx < 0)
			res.pushBack(*this);
		else {
			res.pushBack(sliced(0, idx-1));
			res.pushBack(sliced(idx+1));
		}
		return res;
	}

	constexpr List<SelfType, IndexType> splitAtFirst(BaseType const& seps) const {
		List<SelfType, IndexType> res;
		IndexType idx = -1;
		for(ConstReferenceType sep: seps) {
			IndexType i = find(sep);
			if (i > -1 && i < idx)
				idx = i;
		}
		if (idx < 0)
			res.pushBack(*this);
		else {
			res.pushBack(sliced(0, idx-1));
			res.pushBack(sliced(idx+1));
		}
		return res;
	}

	/*constexpr List<SelfType, IndexType> splitAtFirst(ArgumentListType const& seps) const {
		List<SelfType, IndexType> res;
		IndexType idx = -1;
		for(ConstReferenceType sep: seps) {
			IndexType i = find(sep);
			if (i > -1 && i < idx)
				idx = i;
		}
		if (idx < 0)
			res.pushBack(*this);
		else {
			res.pushBack(sliced(0, idx-1));
			res.pushBack(sliced(idx+1));
		}
		return res;
	}*/

	constexpr List<SelfType, IndexType> splitAtLast(DataType const& sep) const {
		List<SelfType, IndexType> res;
		IndexType idx = rfind(sep);
		if (idx < 0)
			res.pushBack(*this);
		else {
			res.pushBack(sliced(0, idx-1));
			res.pushBack(sliced(idx+1));
		}
		return res;
	}

	constexpr List<SelfType, IndexType> splitAtLast(BaseType const& seps) const {
		List<SelfType, IndexType> res;
		IndexType idx = -1;
		for(ConstReferenceType sep: seps) {
			IndexType i = rfind(sep);
			if (i > -1 && i > idx)
				idx = i;
		}
		if (idx < 0)
			res.pushBack(*this);
		else {
			res.pushBack(sliced(0, idx-1));
			res.pushBack(sliced(idx+1));
		}
		return res;
	}

	/*constexpr List<SelfType, IndexType> splitAtLast(ArgumentListType const& seps) const {
		List<SelfType, IndexType> res;
		IndexType idx = -1;
		for(ConstReferenceType sep: seps) {
			IndexType i = rfind(sep);
			if (i > -1 && i > idx)
				idx = i;
		}
		if (idx < 0)
			res.pushBack(*this);
		else {
			res.pushBack(sliced(0, idx-1));
			res.pushBack(sliced(idx+1));
		}
		return res;
	}*/
	
	constexpr SelfType& replace(DataType const& val, DataType const& rep) {
		for (DataType& v: *this)
			if (v == val) v = rep;
		return *this;
	}

	constexpr SelfType& replace(BaseType const& values, DataType const& rep) {
		for (DataType const& val: values)
			replace(val, rep);
		return *this;
	}

	/*constexpr SelfType& replace(ArgumentListType const& values, DataType const& rep) {
		for (DataType const& val: values)
			replace(val, rep);
		return *this;
	}*/

	struct Replacement {
		BaseType	targets;
		DataType	replacement;
	};

	constexpr SelfType& replace(Replacement const& rep) {
		replace(rep.targets, rep.replacement);
		return *this;
	}

	constexpr SelfType& replace(List<Replacement, SizeType> const& reps) {
		for (Replacement const& rep: reps)
			replace(rep);
		return *this;
	}

	/*constexpr SelfType& replace(Arguments<Replacement> const& reps) {
		for (Replacement const& rep: reps)
			replace(rep);
		return *this;
	}*/

	constexpr SelfType replaced(DataType const& val, DataType const& rep) const				{return SelfType(*this).replace(val, rep);		}
	constexpr SelfType replaced(BaseType const& values, DataType const& rep) const			{return SelfType(*this).replace(values, rep);	}
//	constexpr SelfType replaced(ArgumentListType const& values, DataType const& rep) const	{return SelfType(*this).replace(values, rep);	}

	constexpr SelfType replaced(Replacement const& rep) const					{return SelfType(*this).replace(rep);	}
	constexpr SelfType replaced(List<Replacement, SizeType> const& reps) const	{return SelfType(*this).replace(reps);	}
//	constexpr SelfType replaced(Arguments<Replacement> const& reps) const		{return SelfType(*this).replace(reps);	}

	constexpr OutputStreamType& operator<<(OutputStreamType& o) const	{if (!empty()) o << cstr(); return o;}
	constexpr OutputStreamType& operator<<(OutputStreamType& o)			{if (!empty()) o << cstr(); return o;}

	friend constexpr OutputStreamType& operator<<(OutputStreamType& o, SelfType& self)			{if (!self.empty()) o << self.cstr(); return o;}
	friend constexpr OutputStreamType& operator<<(OutputStreamType& o, SelfType const& self)	{if (!self.empty()) o << self.cstr(); return o;}

	constexpr InputStreamType& readFrom(InputStreamType& i, DataType const& stop) {
		DataType buf[32];
		while(i.getline(buf, 32, stop))
			appendBack(buf, i.gcount());
		return i;
	}
	
	constexpr InputStreamType& readFrom(InputStreamType& i) {
		return readFromStream(i, '\0');
	}

	constexpr SelfType& operator=(SelfType const& other)			{BaseType::operator=(other); return *this;				}
	constexpr SelfType& operator=(SelfType&& other)					{BaseType::operator=(CTL::move(other)); return *this;	}
	constexpr SelfType& operator=(StringLiteralType const& other)	{BaseType::operator=(SelfType(other)); return *this;	}

	constexpr SelfType const& operator<<(SelfType& other) const	{other.appendBack(*this); return *this;}
	constexpr SelfType& operator<<(SelfType& other)				{other.appendBack(*this); return *this;}

	constexpr SelfType& operator>>(SelfType const& other)	{appendBack(other); return other;}

	constexpr SelfType operator+(DataType const& value) const	{return SelfType(*this).pushBack(value);	}
	constexpr SelfType operator+(SelfType const& other) const	{return SelfType(*this).appendBack(other);	}

	constexpr SelfType operator+(StringLiteralType const& str) const				{return (*this) + SelfType(str);}
	template<SizeType S>
	constexpr SelfType operator+(Decay::AsType<ConstantType[S]> const& str) const	{return (*this) + SelfType(str);}

	friend constexpr SelfType operator+(DataType const& value, SelfType const& self)	{return SelfType().pushBack(value) + self;	}

	friend constexpr SelfType operator+(StringLiteralType const& str, SelfType const& self)					{return SelfType(str) + (self);}
	template<SizeType S>
	friend constexpr SelfType operator+(Decay::AsType<ConstantType[S]> const& str, SelfType const& self)	{return SelfType(str) + (self);}

	constexpr SelfType& operator+=(DataType const& value)				{pushBack(value); return *this;				}
	constexpr SelfType& operator+=(SelfType const& other)				{appendBack(other); return *this;			}
	constexpr SelfType& operator+=(StringLiteralType const& str)		{appendBack(SelfType(str)); return *this;	}
	template<SizeType S>
	constexpr SelfType& operator+=(Decay::AsType<ConstantType[S]> str)	{appendBack(str); return *this;		}

	constexpr SelfType operator*(SizeType const& times) const {
		SelfType result(size() * times);
		for (SizeType i = 0; i < times; ++i)
			result += (*this);
		return result;
	}

	constexpr SelfType& operator*=(SizeType const& times) {
		SelfType copy = *this;
		for (SizeType i = 0; i < times; ++i)
			appendBack(copy);
		return *this;
	}

	template<SizeType S>
	constexpr bool operator==(Decay::AsType<ConstantType[S]> const& str) const	{return *this == SelfType(str);			}
	constexpr bool operator==(StringLiteralType const& str) const				{return *this == SelfType(str);			}
	constexpr bool operator==(SelfType const& other) const						{return BaseType::operator==(other);	}
	template<SizeType S>
	constexpr OrderType operator<=>(Decay::AsType<ConstantType[S]> const& str) const	{return *this <=> SelfType(str);		}
	constexpr OrderType operator<=>(StringLiteralType const& str) const					{return *this <=> SelfType(str);		}
	constexpr OrderType operator<=>(SelfType const& other) const						{return BaseType::operator<=>(other);	}

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
		return sliced(start, stop);
	}

	constexpr bool nullTerminated() const {return back() == '\0';}
	
	constexpr StringLiteralType cstr() const {
		if (nullTerminated()) return cbegin();
		strbuflen = size() + 1;
		strbuf = MX::realloc<DataType>(strbuf, strbuflen);
		MX::memcpy<DataType>(strbuf, cbegin(), size());
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
	return BaseString<TChar, TIndex>(str) + self;
}

template<Type::ASCII TChar, Type::Integer TIndex = usize, AsUnsigned<TIndex> S>
constexpr BaseString<TChar, TIndex>
operator+(
	Decay::AsType<typename BaseString<TChar, TIndex>::ConstantType[S]> const& str,
	BaseString<TChar, TIndex> const& self
) {
	return BaseString<TChar, TIndex>(str) + self;
}

typedef BaseString<char>	String;
typedef BaseString<wchar_t>	WideString;

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

	using typename Derived::BaseType;

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
