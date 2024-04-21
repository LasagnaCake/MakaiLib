#ifndef CTL_CONTAINER_ITERATOR_H
#define CTL_CONTAINER_ITERATOR_H

#include "../order.hpp"
#include "../ctypes.hpp"
#include "../../conceptual.hpp"
#include "../templates.hpp"

template<class T, bool REVERSE = false, Type::Integer I = size_t>
struct Iterator: Typed<T>, Indexed<T>, Ordered, SelfIdentified<T, REVERSE, I> {
public:
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
	constexpr Iterator operator-(IndexType const& value) const	{return offset<REVERSE>(value);				}
	constexpr Iterator operator+(IndexType const& value) const	{return offset<REVERSE>(value);				}

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

	constexpr PointerType offset(IndexType const& value) const {
		if constexpr (REVERSE)	return iterand + value;
		else					return iterand - value;
	}

	PointerType iterand = nullptr;
};

template<class T>
using ForwardIterator = Iterator<T, false>;
template<class T>
using ReverseIterator = Iterator<T, true>;

template<class T>
concept IteratorType = requires {
	typename T::SizeType;
	typename T::IndexType;
	typename T::DataType;
} && Type::Derived<T, Iterator<typename T::DataType, typename T::IndexType>>;

template<class T, Type::Integer I>
struct Iteratable: Typed<T>, Indexed<I> {
	typedef Iterator<DataType, false, SizeType>		IteratorType;
	typedef Iterator<ConstantType, false, SizeType>	ConstIteratorType;
	typedef Iterator<DataType, true, SizeType>		ReverseIteratorType;
	typedef Iterator<ConstantType, true, SizeType>	ConstReverseIteratorType;
}

#endif // CTL_CONTAINER_ITERATOR_H
