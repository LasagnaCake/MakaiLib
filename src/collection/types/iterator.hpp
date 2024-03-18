#ifndef TYPE_ITERATOR_H
#define TYPE_ITERATOR_H

#include <type_traits>
#include "order.hpp"

template<typename T, bool REVERSE = false, Type::Integer I = size_t>
class Iterator {
public:
	typedef T		DataType;
	typedef T const	ConstantType;

	typedef DataType*		PointerType;
	typedef DataType&		ReferenceType;
	typedef ConstantType*	ConstPointerType;
	typedef ConstantType&	ConstReferenceType;

	typedef I							SizeType;
	typedef std::make_signed<SizeType>	IndexType;

	typedef ValueOrder	OrderType;

	constexpr Iterator() {}

	constexpr Iterator(PointerType const& value): iterand(value)		{}
	constexpr Iterator(PointerType&& value): iterand(std::move(value))	{}

	constexpr Iterator(Iterator const& other): iterand(other.iterand)		{}
	constexpr Iterator(Iterator&& other): iterand(std::move(other.iterand))	{}

	constexpr ReferenceType operator*()	{return *iterand;	}
	constexpr PointerType operator->()	{return iterand;	}

	constexpr ReferenceType operator*() const	{return *iterand;	}
	constexpr PointerType operator->() const	{return iterand;	}

	Iterator& operator++()		{step(); return *this;							}
	Iterator operator++(int)	{Iterator copy = *this; ++(*this); return copy;	}

	constexpr operator PointerType() const			{return iterand;	}
	constexpr operator ReferenceType()				{return *iterand;	}
	constexpr operator ConstReferenceType() const	{return *iterand;	}

	constexpr bool operator==(Iterator const& other) const			{return iterand == other.iterand;	}
	constexpr OrderType operator<=>(Iterator const& other) const	{return compare(other.iterand);		}

	constexpr SizeType operator-(Iterator const& other)	const	{return difference<REVERSE>(other.iterand);	}
	constexpr Iterator operator-(IndexType const& value) const	{return difference<REVERSE>(value);			}
	constexpr Iterator operator+(IndexType const& value) const	{return difference<!REVERSE>(value);		}

private:
	constexpr void step() {
		if constexpr (REVERSE)	--iterand;
		else					++iterand;
	}

	constexpr OrderType compare(PointerType const& other) const {
		if constexpr (REVERSE)	return other <=> iterand;
		else					return iterand <=> other;
	}

	constexpr PointerType difference(PointerType const& other) const {
		if constexpr (REVERSE)	return other - iterand;
		else					return iterand - other;
	}

	template <bool FLIP>
	constexpr PointerType offset(IndexType const& value) const {
		if constexpr (FLIP)	return iterand + value;
		else				return iterand - value;
	}

	PointerType iterand = nullptr;
};

template<typename T>
using ForwardIterator = Iterator<T, false>;
template<typename T>
using ReverseIterator = Iterator<T, true>;

#endif // TYPE_ITERATOR_H
