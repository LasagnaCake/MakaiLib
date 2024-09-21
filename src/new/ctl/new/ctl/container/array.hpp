#ifndef CTL_CONTAINER_ARRAY_H
#define CTL_CONTAINER_ARRAY_H

#include "../templates.hpp"
#include "../typeinfo.hpp"
#include "../ctypes.hpp"
#include "../cpperror.hpp"
#include "../typetraits/traits.hpp"
#include "iterator.hpp"
#include "../algorithm/sort.hpp"
#include "../algorithm/reverse.hpp"
#include "../algorithm/memory.hpp"

CTL_NAMESPACE_BEGIN

template<usize N, class TData, Type::Integer TIndex = usize>
struct Array:
	Iteratable<TData, TIndex>,
	SelfIdentified<Array<N, TData, TIndex>>,
	Ordered {
private:
public:
	constexpr SizeType SIZE = N;

	typedef Decay::AsType<DataType[SIZE]> ArrayType;

	static_assert(N <= MAX_SIZE, "Array size must not be bigger than highest SizeType!");

	constexpr Array() requires(Type::Constructible<DataType>)	{for (auto& e: data) e = DataType();	}
	constexpr Array(ArrayType const& data)						{MX::memcpy(arr, data, SIZE);			}
	constexpr Array(DataType const& v)							{for (auto& e: data) e = v;				}

	constexpr DataType& operator[](IndexType index) {
		if (index >= SIZE)
			throw Exception("Index is bigger than array size!");
		wrapBounds(index, SIZE);
		return data[index];
	}

	constexpr DataType operator[](IndexType index) const {
		if (index >= SIZE)
			throw Exception("Index is bigger than array size!");
		wrapBounds(index, SIZE);
		return data[index];
	}

	constexpr bool empty()	const {return SIZE;}
	constexpr bool size()	const {return SIZE;}

	constexpr IteratorType		begin()			{return data;		}
	constexpr IteratorType		end()			{return data+SIZE;	}
	constexpr ConstIteratorType	begin() const	{return data;		}
	constexpr ConstIteratorType	end() const		{return data+SIZE;	}

	constexpr ReverseIteratorType		rbegin()		{return data+SIZE;	}
	constexpr ReverseIteratorType		rend()			{return data;		}
	constexpr ConstReverseIteratorType	rbegin() const	{return data+SIZE;	}
	constexpr ConstReverseIteratorType	rend() const	{return data;		}

	constexpr PointerType		cbegin()		{return data;		}
	constexpr PointerType		cend()			{return data+SIZE;	}
	constexpr ConstPointerType	cbegin() const	{return data;		}
	constexpr ConstPointerType	cend() const	{return data+SIZE;	}

	constexpr ReferenceType			front()			{return data[0];		}
	constexpr ReferenceType 		back()			{return data[SIZE-1];	}
	constexpr ConstReferenceType	front() const	{return data[0];		}
	constexpr ConstReferenceType	back() const	{return data[SIZE-1];	}

private:
	ArrayType data;
};

namespace Impl {
	template<typename T, Type::Integer TIndex = usize>
	struct FromCArray;

	template<usize N, class TData, Type::Integer TIndex>
	struct FromCArray<TData[N], TIndex> {
		typedef Array<N, TData, TIndex> Type;
	};
}

template<typename T, Type::Integer TIndex = usize>
using FromCArray = Impl::FromCArray<T, TIndex>::Type;

CTL_NAMESPACE_BEGIN

#endif // CTL_CONTAINER_ARRAY_H
