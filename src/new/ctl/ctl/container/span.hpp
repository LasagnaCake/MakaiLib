#ifndef CTL_CONTAINER_SPAN_H
#define CTL_CONTAINER_SPAN_H

#include "../ctypes.hpp"
#include "../templates.hpp"
#include "../typetraits/traits.hpp"
#include "../namespace.hpp"
#include "../cpperror.hpp"
#include "iterator.hpp"

CTL_NAMESPACE_BEGIN

/// @brief Span extent size deduction.
enum class ExtentSize {
	CES_AUTO,
	CES_STATIC,
	CES_DYNAMIC
};

/// @brief Integer value representing dynamic size.
constexpr usize DYNAMIC_SIZE = -1;

template<class TData, usize S = DYNAMIC_SIZE, Type::Integer TIndex = usize, ExtentSize EXTENT = ExtentSize::CES_AUTO>
struct Span;

/// @brief Fixed-size, or variable-size, view of an array of elements.
/// @tparam TData Element type.
/// @tparam S Array size.
/// @tparam TIndex Index size.
/// @tparam EXTENT Extent size deduction.
/// @note Even if `S` is specified, the actual `Span` size is not guaranteed to be `S`.
template<class TData, usize S, Type::Integer TIndex, ExtentSize EXTENT>
struct Span:
	Iteratable<TData, TIndex>,
	SelfIdentified<Span<TData, S, TIndex, EXTENT>>,
	Ordered {

	using Iteratable		= ::CTL::Iteratable<TData, TIndex>;
	using SelfIdentified	= ::CTL::SelfIdentified<Span<TData, S, TIndex, EXTENT>>;

	using
		typename Iteratable::IteratorType,
		typename Iteratable::ConstIteratorType,
		typename Iteratable::ReverseIteratorType,
		typename Iteratable::ConstReverseIteratorType,
		typename Iteratable::IndexType,
		typename Iteratable::SizeType
	;

	using
		typename Iteratable::DataType,
		typename Iteratable::ReferenceType,
		typename Iteratable::PointerType,
		typename Iteratable::ConstPointerType,
		typename Iteratable::ConstantType,
		typename Iteratable::ConstReferenceType
	;

	using typename SelfIdentified::SelfType;

	using Ordered::OrderType;

	constexpr static bool STATIC	= (S != DYNAMIC_SIZE || EXTENT == ExtentSize::CES_STATIC);
	constexpr static bool DYNAMIC	= (S == DYNAMIC_SIZE && EXTENT != ExtentSize::CES_STATIC);

	using ComparatorType = SimpleComparator<DataType>;

	/// @brief Empty constructor.
	constexpr Span() noexcept: contents(nullptr), count(0) {}

	/// @brief Copy constructor (`Span`).
	/// @param other Other `Span` to copy from.
	constexpr Span(SelfType const& other) noexcept: contents(other.contents), count(other.count)					{}
	/// @brief Move constructor (`Span`).
	/// @param other Other `Span` to move.
	constexpr Span(SelfType&& other) noexcept: contents(CTL::move(other.contents)), count(CTL::move(other.count))	{}

	/// @brief Constructs a `Span` from a pointer to a range of elements.
	/// @param data Elements to view.
	/// @note Becomes explicit if span is dymamic.
	constexpr explicit(DYNAMIC) Span(PointerType const& data): contents(data)													{}
	/// @brief Constructs a `Span` from an iterator to the beginning of a range of elements.
	/// @param data Elements to view.
	/// @note Becomes explicit if span is dymamic.
	constexpr explicit(DYNAMIC) Span(IteratorType const& begin): contents(begin)												{}

	/// @brief Constructs a `Span` from a "c-style" range of elements.
	/// @param data Start of range.
	/// @param size Size of range.
	/// @note Becomes explicit if span is static.
	constexpr explicit(STATIC) Span(PointerType const& data, SizeType const& size): contents(data), count(size)					{}
	/// @brief Constructs a `Span` from a range of elements.
	/// @param begin Iterator to beginning of range.
	/// @param end Iterator to end of range.
	/// @note Becomes explicit if span is static.
	constexpr explicit(STATIC) Span(IteratorType const& begin, IteratorType const& end): contents(begin), count(end - begin)	{}

	/// @brief Constructs a `Span` from a range type.
	/// @tparam T Ranged object type.
	/// @param other Object to view from.
	template<Type::Container::Ranged<IteratorType> T>
	constexpr explicit Span(T const& other): Span(other.begin(), other.end())	{}
	/// @brief Constructs a `Span` from a bounded type.
	/// @tparam T Bounded object type.
	/// @param other Object to view from.
	template<Type::Container::Bounded T>
	constexpr explicit Span(T const& other): Span(other.data(), other.size())	{}

	/// @brief Returns the value of the element at a given index.
	/// @param index Index of the element.
	/// @return Reference to the element.
	/// @throw OutOfBoundsException when index is bigger than `Span` size.
	/// @throw NonexistentValueException when no range is bound, or range is empty.
	constexpr ReferenceType at(IndexType index) {
		assertExists();
		wrapBounds(index, count);
		return contents[index];
	}

	/// @brief Returns the value of the element at a given index.
	/// @param index Index of the element.
	/// @return Const reference to the element.
	/// @throw OutOfBoundsException when index is bigger than `Span` size.
	/// @throw NonexistentValueException when no range is bound, or range is empty
	constexpr ConstReferenceType at(IndexType index) const {
		assertExists();
		wrapBounds(index, count);
		return contents[index];
	}

	/// @brief Returns the value of the element at a given index.
	/// @param index Index of the element.
	/// @return Reference to the element.
	/// @throw OutOfBoundsException when index is bigger than `Span` size.
	/// @throw NonexistentValueException when no range is bound, or range is empty.
	constexpr ReferenceType operator[](IndexType index)				{return at(index);	}

	/// @brief Returns the value of the element at a given index.
	/// @param index Index of the element.
	/// @return Const reference to the element.
	/// @throw OutOfBoundsException when index is bigger than `Span` size.
	/// @throw NonexistentValueException when no range is bound, or range is empty
	constexpr ConstReferenceType operator[](IndexType index) const	{return at(index);	}

	constexpr PointerType		data()			{return contents;	}
	constexpr ConstPointerType	data() const	{return contents;	}

	constexpr IteratorType		begin()			{return contents;		}
	constexpr IteratorType		end()			{return contents+count;	}
	constexpr ConstIteratorType	begin() const	{return contents;		}
	constexpr ConstIteratorType	end() const		{return contents+count;	}

	constexpr ReverseIteratorType		rbegin()		{return contents+count;	}
	constexpr ReverseIteratorType		rend()			{return contents;		}
	constexpr ConstReverseIteratorType	rbegin() const	{return contents+count;	}
	constexpr ConstReverseIteratorType	rend() const	{return contents;		}

	constexpr PointerType		cbegin()		{return contents;		}
	constexpr PointerType		cend()			{return contents+count;	}
	constexpr ConstPointerType	cbegin() const	{return contents;		}
	constexpr ConstPointerType	cend() const	{return contents+count;	}

	constexpr ReferenceType			front()			{return contents[0];		}
	constexpr ReferenceType 		back()			{return contents[count-1];	}
	constexpr ConstReferenceType	front() const	{return contents[0];		}
	constexpr ConstReferenceType	back() const	{return contents[count-1];	}

	constexpr SizeType size() const	{return count;		}
	constexpr bool empty() const	{return count == 0;	}

	/// @brief Equality operator.
	/// @param other Other `Span` to compare with.
	/// @return Whether they're equal.
	/// @note Requires element type to be equally comparable.
	/// @sa Comparator::equals()
	constexpr bool operator==(SelfType const& other) const
	requires Type::Comparator::Equals<DataType, DataType> {
		return equals(other);
	}

	/// @brief Threeway comparison operator.
	/// @param other Other `Span` to compare with.
	/// @return Order between both `Span`s.
	/// @note Requires element type to be three-way comparable.
	/// @sa Comparator::compare()
	constexpr OrderType operator<=>(SelfType const& other) const
	requires Type::Comparator::Threeway<DataType, DataType> {
		return compare(other);
	}

	/// @brief Returns whether it is equal to another `Span`.
	/// @param other Other `Span` to compare with.
	/// @return Whether they're equal.
	/// @note Requires element type to be equally comparable.
	/// @sa Comparator::equals()
	constexpr SizeType equals(SelfType const& other) const
	requires Type::Comparator::Equals<DataType, DataType> {
		bool result = true;
		SizeType i = 0;
		while (result) {
			if (i == count || i == other.count)
				return count == other.count;
			result = ComparatorType::equals(contents[i], other.contents[i]);
			++i;
		}
		return result;
	}

	/// @brief Returns the result of a three-way comparison with another `Span`.
	/// @param other Other `Span` to compare with.
	/// @return Order between both `Span`s.
	/// @note Requires element type to be three-way comparable.
	/// @sa Comparator::compare()
	constexpr OrderType compare(SelfType const& other) const
	requires Type::Comparator::Threeway<DataType, DataType> {
		OrderType result = Order::EQUAL;
		SizeType i = 0;
		while (result == Order::EQUAL) {
			if (i == count || i == other.count)
				return count <=> other.count;
			result = ComparatorType::compare(contents[i], other.contents[i]);
			++i;
		}
	}
	

private:
	constexpr void assertExists() {
		if (!(contents && count))
			throw NonexistentValueException("No element range bound to span!");
	}

	using Iteratable::wrapBounds;

	DataType*	contents	= nullptr;
	usize		count		= S;
};

template<usize S = DYNAMIC_SIZE, Type::Integer TIndex = usize, ExtentSize EXTENT = ExtentSize::CES_AUTO>
using ByteSpan = Span<uint8, S, TIndex, EXTENT>;

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_SPAN_H
