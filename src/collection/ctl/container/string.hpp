#ifndef CTL_CONTAINER_STRING_H
#define CTL_CONTAINER_STRING_H

#include <istream>
#include <ostream>
#include "list.hpp"
#include <stdlib.h>

template<ASCIIType T, Type::Integer I = size_t>
class BaseString: public List<T, I> {
public:
	// Parent type
	typedef List<T, I> BaseType;
	// Stream types
	typedef std::basic_ostream<DataType>	OutputStreamType;
	typedef std::basic_istream<DataType>	InputStreamType;
	// Self type
	typedef BaseString<DataType, IndexType>	SelfType;

	constexpr BaseString() {}

	constexpr ~BaseString() {
		delete[] strbuf;
	}

	constexpr BaseString(const DataType* const& v) {
		size_t len = 0;
		while (v[len++] != '\0');
		reserve(len+1);
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
		strbuf = new DataType[size()+1];
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
	PointerType mutable strbuf = nullptr;

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

template<Type::Integer I, Type::Integer S = size_t>
constexpr BaseString<char, S> toString(I const& val, uintmax const& base = 10) {
	String result(64, '\0');
	intmax i = itoa(val, result.cbegin(), result.size(), base);
	if (i == -1)
		throw String("ERROR: Invalid number!");
	return result.reserve(i);
}

template<Type::Float F, Type::Integer S = size_t>
constexpr BaseString<char, S> toString(F const& val, uintmax const& precision = 16) {
	String result(64, '\0');
	intmax i = ftoa(val, result.cbegin(), result.size(), base);
	if (i == -1)
		throw String("ERROR: Invalid number!");
	return result.reserve(i);
}

#endif // CTL_CONTAINER_STRING_H
