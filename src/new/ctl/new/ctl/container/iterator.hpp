#ifndef CTL_CONTAINER_ITERATOR_H
#define CTL_CONTAINER_ITERATOR_H

#include <iterator>

#include "../namespace.hpp"
#include "../order.hpp"
#include "../ctypes.hpp"
#include "../templates.hpp"
#include "../cpperror.hpp"
#include "../typetraits/traits.hpp"
#include "../meta/logic.hpp"

CTL_NAMESPACE_BEGIN

/*template<class TData, bool R = false, Type::Integer TIndex = usize>
struct STLIteratorCompatible {
	using iterator_category = Meta::DualType<R, std::bidirectional_iterator_tag>;
    using value_type = TData;
    using difference_type = AsUnsigned<TIndex>;
    using pointer = TData*;
    using reference = TData&;
};*/

template<class TData, bool R = false, Type::Integer TIndex = usize>
struct Iterator:
	//STLIteratorCompatible<TDat, R, TIndex>,
	Typed<AsNonPointer<TData>>,
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

	using Typed				= ::CTL::Typed<AsNonPointer<TData>>;
	using Indexed			= ::CTL::Indexed<TIndex>;
	using Ordered			= ::CTL::Ordered;
	using SelfIdentified	= ::CTL::SelfIdentified<Iterator<TData, REVERSE, TIndex>>;

	using
		typename Typed::DataType,
		typename Typed::ConstantType,
		typename Typed::PointerType,
		typename Typed::ConstPointerType,
		typename Typed::ReferenceType,
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

	using STLForwardIterator	= std::common_iterator<PointerType, PointerType>;
	using STLReverseIterator	= std::reverse_iterator<PointerType>;

	using STLConstForwardIterator	= std::common_iterator<ConstPointerType, ConstPointerType>;
	using STLConstReverseIterator	= std::reverse_iterator<ConstPointerType>;

	constexpr Iterator() {}

	constexpr explicit(REVERSE) Iterator(PointerType const& value): iterand(value)	{}
	constexpr explicit(REVERSE) Iterator(PointerType&& value): iterand(move(value))	{}

	constexpr explicit(REVERSE) Iterator(Iterator const& other): iterand(other.iterand)		{}
	constexpr explicit(REVERSE) Iterator(Iterator&& other): iterand(move(other.iterand))	{}

	constexpr PointerType base() const {return iterand;}

	constexpr ReferenceType operator*()	{return *iter();	}
	constexpr DataType operator->()		{return iter();		}

	constexpr ConstReferenceType operator*() const	{return *iter();	}
	constexpr ConstantType operator->() const		{return iter();		}

	Iterator& operator++()		{step(); return *this;							}
	Iterator operator++(int)	{Iterator copy = *this; step(); return copy;	}

	Iterator& operator--()		{rstep(); return *this;							}
	Iterator operator--(int)	{Iterator copy = *this; rstep(); return copy;	}

	constexpr operator PointerType() const			{return iter();		}
	constexpr operator ReferenceType()				{return *iter();	}
	constexpr operator ConstReferenceType() const	{return *iter();	}

	constexpr bool operator==(Iterator const& other) const			{return iterand == other.iterand;	}
	constexpr OrderType operator<=>(Iterator const& other) const	{return compare(other.iterand);		}

	constexpr IndexType operator-(Iterator const& other) const	{return difference<REVERSE>(other.iterand);	}
	constexpr Iterator operator-(IndexType const& value) const	{return offset<REVERSE>(value);				}
	constexpr Iterator operator+(IndexType const& value) const	{return offset<REVERSE>(value);				}

	constexpr operator STLForwardIterator() requires(!REVERSE)				{return iterand;}
	constexpr operator STLReverseIterator() requires(REVERSE)				{return iterand;}
	constexpr operator STLConstForwardIterator() const requires(!REVERSE)	{return iterand;}
	constexpr operator STLConstReverseIterator() const requires(REVERSE)	{return iterand;}
private:
	constexpr PointerType iter() {
		PointerType it = iterand;
		if constexpr (REVERSE)	return --it;
		else					return it;
	}

	constexpr void step() {
		if constexpr (REVERSE)	--iterand;
		else					++iterand;
	}

	constexpr void rstep() {
		if constexpr (REVERSE)	++iterand;
		else					--iterand;
	}

	constexpr PointerType compare(PointerType const& other) const {
		if constexpr (REVERSE)	return other <=> iterand;
		else					return iterand <=> other;
	}

	constexpr IndexType difference(PointerType const& other) const {
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
	{T::REVERSE} -> Type::Equal<bool>;
} && Type::Derived<T, Iterator<typename T::DataType, T::REVERSE, typename T::IndexType>>;

template<class TData, Type::Integer TIndex>
struct Iteratable: Typed<TData>, Indexed<TIndex> {
	using Indexed	= ::CTL::Indexed<TIndex>;
	using Typed		= ::CTL::Typed<TData>;

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

	using
		Indexed::MAX_SIZE
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
