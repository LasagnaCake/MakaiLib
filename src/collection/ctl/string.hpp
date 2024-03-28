#ifndef CTL_TYPE_STRING_H
#define CTL_TYPE_STRING_H

#include <istream>
#include <ostream>
#include "list.hpp"
#include <stdlib.h>

template<class T>
concept CharacterType = Type::Equal<T, char> || Type::Equal<wchar>;

template<CharacterType T, Type::Integer I = size_t>
class BaseString: private List<T, I> {
public:
	// Parent type
	typedef List<T, I> BaseType;
	// Stream types
	typedef std::basic_ostream<DataType>	OutputStreamType;
	typedef std::basic_istream<DataType>	InputStreamType;
	// Self type
	typedef BaseString<DataType, IndexType>	SelfType;

	using BaseType::List;

	using BaseType::begin;
	//using BaseType::end;
	//using BaseType::rbegin;
	using BaseType::rend;
	using BaseType::cbegin;
	//using BaseType::cend;
	using BaseType::find;
	using BaseType::reverseFind;
	using BaseType::empty;
	using BaseType::capacity;
	using BaseType::clear;
	using BaseType::dispose;
	using BaseType::front;
	//using BaseType::back;
	using BaseType::operator==;
	using BaseType::operator<=>;
	using BaseType::equals;
	using BaseType::compare;
	using BaseType::disparity;

	constexpr BaseString(const DataType* const& v) {
		size_t len = 0;
		while (v[len++] != '\0');
		reserve(len);
		memcpy(cbegin(), v, len * sizeof(DataType));
	}

	template<SizeType S>
	constexpr BaseString(const DataType (const& v)[S]) {
		reserve(S);
		memcpy(cbegin(), v, len * sizeof(DataType));
		if (v[S-1] != '\0') pushBack('\0');
	}

	constexpr OutputStreamType const& operator<<(OutputStreamType& o) const	{o << data; return out;}
	constexpr OutputStreamType& operator<<(OutputStreamType& o)				{o << data; return out;}

	constexpr SelfType const& operator<<(SelfType& other) const	{other.popBack(); other.appendBack(*this); return *this;}
	constexpr SelfType& operator<<(SelfType& other)				{other.popBack(); other.appendBack(*this); return *this;}

	constexpr SelfType& operator>>(SelfType const& other)	{popBack(); appendBack(other); return other;}

	constexpr SelfType operator+(DataType const& value) const	{SelfType self(*this); self.popBack(); return self.pushBack(value);}

	constexpr SelfType operator+(SelfType const& other) const	{SelfType self(*this); self.popBack(); return self + other;}

	constexpr SelfType operator+(const DataType* const& str) const		{return (*this) + String(str);}
	constexpr SelfType operator+(const DataType (const& str)[S]) const	{return (*this) + String(str);}

	constexpr SelfType operator+(const DataType* const& str, SelfType const& self) const		{return String(str) + self;}
	constexpr SelfType operator+(const DataType (const& str)[S], SelfType const& self) const	{return String(str) + self;}

	// TODO: Write '\0' checking
	constexpr SelfType& operator+=(SelfType const& other)			{popBack(); return appendBack(other);	}
	constexpr SelfType& operator+=(const DataType* const& str)		{popBack(); return appendBack(str);		}
	constexpr SelfType& operator+=(const DataType (const& str)[S])	{popBack(); return appendBack(str);		}

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
		return String(begin() + start, begin() + (stop < size() ? stop : size())) + '\0';
	}

	constexpr ConstPointerType cstr() const {return cbegin();}

	constexpr ReferenceType at(IndexType index) {
		assertIsInBounds(index);
		while (index < 0) index += size();
		return data[index];
	}

	constexpr ConstReferenceType at(IndexType index) const {
		assertIsInBounds(index);
		while (index < 0) index += size();
		return data[index];
	}

	constexpr ReferenceType			operator[](IndexType const& index)			{return at(index);}
	constexpr ConstReferenceType	operator[](IndexType const& index) const	{return at(index);}

	constexpr ReferenceType 		back()			{return data[size()-1];	}
	constexpr ConstReferenceType	back() const	{return data[size()-1];	}

	constexpr IteratorType		end()			{return data+size();	}
	constexpr ConstIteratorType	end() const		{return data+size();	}

	constexpr ReverseIteratorType		rbegin()		{return data+size();	}
	constexpr ConstReverseIteratorType	rbegin() const	{return data+size();	}

	constexpr PointerType		cend()			{return data+size();	}
	constexpr ConstPointerType	cend() const	{return data+size();	}

	constexpr SizeType size() const		{return BaseType::size()-1;		}

private:
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

#endif // CTL_TYPE_STRING_H
