#ifndef CTL_CONTAINER_ITERATOR_H
#define CTL_CONTAINER_ITERATOR_H

#include <iterator>

#include "../namespace.hpp"
#include "../order.hpp"
#include "../ctypes.hpp"
#include "../templates.hpp"
#include "../cpperror.hpp"
#include "../typetraits/traits.hpp"

CTL_NAMESPACE_BEGIN

template<class TData, bool R = false, Type::Integer TIndex = usize>
struct Iterator:
	Typed<TData>,
	Indexed<TIndex>,
	Ordered,
	SelfIdentified<Iterator<TData, R, TIndex>> {
public:
	static_assert(
		Type::PostDecrementable<TData>
	&&	Type::PostIncrementable<TData>
	&&	Type::PreDecrementable<TData>
	&&	Type::PreIncrementable<TData>
	, "Type is not a valid iteratable type!"
	);

	constexpr static bool REVERSE = R;

	using Typed				= Typed<TData>;
	using Indexed			= Indexed<TIndex>;
	using Ordered			= Ordered;
	using SelfIdentified	= SelfIdentified<Iterator<TData, REVERSE, TIndex>>;

	using
		typename Typed::DataType,
		typename Typed::PointerType,
		typename Typed::ReferenceType,
		typename Typed::ConstantType,
		typename Typed::ConstReferenceType
	;

	using
		typename Indexed::IndexType,
		typename Indexed::SizeType
	;

	using
		typename SelfIdentified::SelfType
	;

	using typename Ordered::OrderType;

	constexpr Iterator() {}

	constexpr explicit(REVERSE) Iterator(DataType const& value): iterand(value)	{}
	constexpr explicit(REVERSE) Iterator(DataType&& value): iterand(move(value))	{}

	constexpr explicit(REVERSE) Iterator(Iterator const& other): iterand(other.iterand)		{}
	constexpr explicit(REVERSE) Iterator(Iterator&& other): iterand(move(other.iterand))	{}

	constexpr Iterator(std::forward_iterator<DataType> const& other) requires (!REVERSE): iterand(other.base())			{}
	constexpr Iterator(std::reverse_iterator<DataType> const& other) requires (REVERSE): iterand(other.base())	{}

	constexpr DataType base() const {return base;}

	constexpr ReferenceType operator*()	{return *iter();	}
	constexpr DataType operator->()		{return iter();	}

	constexpr ConstReferenceType operator*() const	{return *iter();	}
	constexpr ConstantType operator->() const		{return iter();	}

	Iterator& operator++()		{step(); return *this;							}
	Iterator operator++(int)	{Iterator copy = *this; ++(*this); return copy;	}

	constexpr operator PointerType() const			{return iter();		}
	constexpr operator ReferenceType()				{return *iter();	}
	constexpr operator ConstReferenceType() const	{return *iter();	}

	constexpr bool operator==(Iterator const& other) const			{return iterand == other.iterand;	}
	constexpr OrderType operator<=>(Iterator const& other) const	{return compare(other.iterand);		}

	constexpr SizeType operator-(Iterator const& other)	const	{return difference<REVERSE>(other.iterand);	}
	constexpr Iterator operator-(IndexType const& value) const	{return offset<REVERSE>(value);				}
	constexpr Iterator operator+(IndexType const& value) const	{return offset<REVERSE>(value);				}

	constexpr operator std::forward_iterator<DataType>() const requires(!REVERSE)			{return iterand;}
	constexpr operator std::reverse_iterator<DataType>() const requires(!REVERSE)	{return iterand;}
private:
	constexpr DataType iter() {
		DataType it = iterand;
		if constexpr (REVERSE)	return --it;
		else					return it;
	}

	constexpr void step() {
		if constexpr (REVERSE)	--iterand;
		else					++iterand;
	}

	constexpr OrderType compare(DataType const& other) const {
		if constexpr (REVERSE)	return other <=> iterand;
		else					return iterand <=> other;
	}

	constexpr DataType difference(DataType const& other) const {
		if constexpr (REVERSE)	return other - iterand;
		else					return iterand - other;
	}

	constexpr DataType offset(IndexType const& value) const {
		if constexpr (REVERSE)	return iterand + value;
		else					return iterand - value;
	}

	DataType iterand = nullptr;
};

template<class TData, Type::Integer TIndex = usize>
using ForwardIterator = Iterator<TData, false, TIndex>;
template<class TData, Type::Integer TIndex = usize>
using ReverseIterator = Iterator<TData, true, TIndex>;

template<class T>
concept IteratorType = requires {
	typename T::SizeType;
	typename T::IndexType;
	{T::REVERSE} -> Type::Equal<bool>;
} && Type::Derived<T, Iterator<typename T::DataType, T::REVERSE, typename T::IndexType>>;

template<class TData, Type::Integer TIndex>
struct Iteratable: Typed<TData>, Indexed<TIndex> {
	using Indexed	= Indexed<TIndex>;
	using Typed		= Typed<TData>;

	using
		typename Typed::DataType,
		typename Typed::ConstantType,
		typename Typed::PointerType,
		typename Typed::ConstPointerType
	;

	using
		typename Indexed::IndexType,
		typename Indexed::SizeType
	;

	typedef ForwardIterator<PointerType, SizeType>		IteratorType;
	typedef ForwardIterator<ConstPointerType, SizeType>	ConstIteratorType;
	typedef ReverseIterator<PointerType, SizeType>		ReverseIteratorType;
	typedef ReverseIterator<ConstPointerType, SizeType>	ConstReverseIteratorType;

protected:
	constexpr static void wrapBounds(IndexType& index, SizeType const& count) {
		while (index < 0) index += count;
	}

	[[noreturn]] constexpr static void outOfBoundsError() {
		throw OutOfBoundsException("Index is bigger than array size!");
	}
};

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_ITERATOR_H
