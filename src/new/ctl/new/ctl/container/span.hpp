#ifndef CTL_CONTAINER_SPAN_H
#define CTL_CONTAINER_SPAN_H

#include "../ctypes.hpp"
#include "../templates.hpp"
#include "../typetraits/traits.hpp"
#include "../namespace.hpp"
#include "error.hpp"
#include "iterator.hpp"

CTL_NAMESPACE_BEGIN

enum class ExtentSize {
	CES_AUTO,
	CES_STATIC,
	CES_DYNAMIC
};

constexpr usize DYNAMIC_SIZE = -1;

template<class TData, usize S = DYNAMIC_SIZE, Type::Integer TIndex = usize, ExtentSize EXTENT = ExtentSize::CES_AUTO>
struct Span:
	Iteratable<TData, TIndex> {

	using Iteratable = ::CTL::Iteratable<TData, TIndex>;

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

	consteval static bool isStatic() {
		return (S != DYNAMIC_SIZE || EXTENT == ExtentSize::CES_DYNAMIC);
	}

	consteval static bool isDynamic() {
		return (S == DYNAMIC_SIZE || EXTENT == ExtentSize::CES_STATIC);
	}

	constexpr Span() noexcept: contents(nullptr), count(0) {}

	constexpr Span(Span const& other) noexcept = default;

	explicit (isDynamic())	constexpr Span(PointerType const& data): contents(data)													{}
	explicit (isDynamic())	constexpr Span(IteratorType const& begin): contents(begin)												{}

	explicit (isStatic())	constexpr Span(PointerType const& data, SizeType const& size): contents(data), count(size)				{}
	explicit (isStatic())	constexpr Span(IteratorType const& begin, IteratorType const& end): contents(begin), count(end - begin)	{}

	constexpr ReferenceType operator[](IndexType index) {
		assertExists();
		wrapBounds(index, count);
		return contents[index];
	}

	constexpr ConstReferenceType operator[](IndexType index) const {
		assertExists();
		wrapBounds(index, count);
		return contents[index];
	}

	constexpr DataType at(IndexType index) const {
		assertExists();
		wrapBounds(index, count);
		return contents[index];
	}

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

private:
	constexpr void assertExists() {
		if (!(contents && count))
			throw Error::NonexistentValue(
				"No data was bound to this span!",
				__FILE__
			);
	}

	using Iteratable::wrapBounds;

	DataType*	contents	= nullptr;
	usize		count		= S;
};

template<usize S = DYNAMIC_SIZE, Type::Integer TIndex = usize, ExtentSize EXTENT = ExtentSize::CES_AUTO>
using ByteSpan = Span<uint8, S, TIndex, EXTENT>;

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_SPAN_H
