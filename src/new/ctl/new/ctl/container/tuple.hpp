#ifndef CTL_CONTAINER_TUPLE_H
#define CTL_CONTAINER_TUPLE_H

#include "../namespace.hpp"
#include "../templates.hpp"

CTL_NAMESPACE_BEGIN

// Based off off: https://stackoverflow.com/a/52208842

namespace Impl {
	template<usize N, class T>
	struct TupleItem: Typed<T> {
		using Typed = ::CTL::Typed<T>;

		using typename Typed::DataType;

		DataType value;
	};

	template<usize N, class... Types>
	struct TuplePack;

	template<usize N>
	struct TuplePack<N>{};

	template<usize N, class T, class... Types>
	struct TuplePack<N, T, Types...>:
		public TupleItem<N, T>,
		public TuplePack<N + 1, Types...>,
		Polyglot<T, Types...>
	{
		using Polyglot = Polyglot<T, Types...>;

		using typename Polyglot::DataTypes;

		template<usize INDEX>
		constexpr auto& get()
		requires (INDEX < DataTypes::COUNT) {
			return get<INDEX>(*this);
		}

		template<usize N1, class T1, class... Types1>
		constexpr static T1& get(TuplePack<N1, T1, Types1...>& tup)
		requires (N1 < TuplePack<N1, T1, Types1...>::DataTypes::COUNT) {
			return tup.TupleItem<N1, T1>::value;
		}
	};

	template<usize N, class T, class... Types>
	T& get(TuplePack<N, T, Types...>& tup)
	requires (N < TuplePack<N, T, Types...>::DataTypes::COUNT) {
		return tup.TupleItem<N, T>::value;
	}
}

template<class... Types>
using Tuple = Impl::TuplePack<0, Types...>;

template<class Self, class... TMembers>
struct Reflective: SelfIdentified<Self> {
	typedef Tuple<TMembers&...>		MemberListType;
	typedef PackInfo<TMembers...>	MemberTypes;
};

CTL_NAMESPACE_END

//#define MAKE_REFLECTIVE(__VA_ARGS__) MemberListType members = {__VA_ARGS__}

#endif // CTL_CONTAINER_TUPLE_H
