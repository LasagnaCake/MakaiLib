#ifndef CTL_CONTAINER_ITERATOR_H
#define CTL_CONTAINER_ITERATOR_H

#include "../namespace.hpp"
#include "../order.hpp"
#include "../ctypes.hpp"
#include "../templates.hpp"
#include "../cpperror.hpp"
#include "../typetraits/traits.hpp"

CTL_NAMESPACE_BEGIN

template<class TData, bool REVERSE = false, Type::Integer TIndex = usize>
struct Iterator: Typed<TData>, Indexed<TIndex>, Ordered, SelfIdentified<TData, REVERSE, TIndex> {
public:
	using Typed				= Typed<TData>;
	using Indexed			= Indexed<TIndex>;
	using Ordered			= Ordered;
	using SelfIdentified	= SelfIdentified<TData, REVERSE, TIndex>;

	using
		DataType			= typename Typed::DataType,
		PointerType			= typename Typed::PointerType,
		ReferenceType		= typename Typed::ReferenceType,
		ConstReferenceType	= typename Typed::ConstReferenceType
	;

	using
		IndexType	= typename Indexed::IndexType,
		SizeType	= typename Indexed::SizeType
	;

	using
		SelfType	= typename SelfIdentified::SelfType
	;

	using typename Ordered::OrderType;

	constexpr Iterator() {}

	constexpr Iterator(PointerType const& value): iterand(value)	{}
	constexpr Iterator(PointerType&& value): iterand(move(value))	{}

	constexpr Iterator(Iterator const& other): iterand(other.iterand)	{}
	constexpr Iterator(Iterator&& other): iterand(move(other.iterand))	{}

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

template<class TData, Type::Integer TIndex = usize>
using ForwardIterator = Iterator<TData, false, TIndex>;
template<class TData, Type::Integer TIndex = usize>
using ReverseIterator = Iterator<TData, true, TIndex>;

template<class T>
concept IteratorType = requires {
	typename T::SizeType;
	typename T::IndexType;
	typename T::DataType;
} && Type::Derived<T, Iterator<typename T::DataType, typename T::IndexType>>;

template<class TData, Type::Integer TIndex>
struct Iteratable: Typed<TData>, Indexed<TIndex> {
	using Indexed	= Indexed<TIndex>;
	using Typed		= Typed<TData>;

	using
		Typed::DataType,
		Typed::ConstantType
	;

	using
		Indexed::IndexType,
		Indexed::SizeType
	;

	typedef ForwardIterator<DataType, SizeType>		IteratorType;
	typedef ForwardIterator<ConstantType, SizeType>	ConstIteratorType;
	typedef ReverseIterator<DataType, SizeType>		ReverseIteratorType;
	typedef ReverseIterator<ConstantType, SizeType>	ConstReverseIteratorType;

protected:
	constexpr static void wrapBounds(IndexType& index, SizeType const& count) {
		while (index < 0) index += count;
	}

	[[noreturn]] constexpr static void outOfBoundsError() {
		throw Exception("Index is bigger than array size!");
	}
}

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_ITERATOR_H
