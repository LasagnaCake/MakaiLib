#ifndef CTL_CONTAINER_TUPLE_H
#define CTL_CONTAINER_TUPLE_H

#include "../templates.hpp"

template<usize N, class T>
struct TupleItem: Typed<T> {
    T value;
};

template<usize N, class... Types>
struct TuplePack;

template<usize N>
struct TuplePack<N>{};



template<usize N, class T, class... Types>
struct TuplePack<N, T, Types...>:
	public TupleLeaf<N, T>,
	public TuplePack<N + 1, Types...>,
	Polyglot<T, Types...>
{
	template<usize INDEX>
	constexpr DataTypes::Types<INDEX>& get()
	requires (INDEX < DataTypes::COUNT) {
		return get<INDEX>(*this);
	}

	template<usize N1, class T1, typename... Types1>
	constexpr static T1& get(TuplePack<N1, T1, Types1...>& tup)
	requires (N1 < TuplePack<N1, T1, Types1...>::DataTypes::COUNT) {
		return tup.TupleItem<N1, T1>::value;
	}
};

template<usize N, class T, typename... Types>
T& get(TuplePack<N, T, Types...>& tup)
requires (N < TuplePack<N, T, Types...>::DataTypes::COUNT) {
    return tup.TupleItem<N, T>::value;
}

template<class... Types>
using Tuple = TuplePack<0, Types...>;

template<class Self, class... MemberTypes>
struct Reflective: SelfIdentified<Self> {
	typedef Tuple<MemberTypes&...>		MemberListType;
	typedef PackInfo<MemberTypes...>	MemberTypes;
};

#define MAKE_REFLECTIVE(__VA_ARGS__) MemberListType members = {__VA_ARGS__}

#endif // CTL_CONTAINER_TUPLE_H
