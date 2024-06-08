#ifndef CTL_TYPETRAITS_CONTAINERS_H
#define CTL_TYPETRAITS_CONTAINERS_H

#include "../ctypes.hpp"

template <class...>
using VoidType = void;

template<class T, auto V> struct ValueConstant {constexpr static T value = V;};

template<bool V> struct BooleanConstant: ValueConstant<bool, V> {
		template <typename... Args>
		struct And: BooleanConstant<(... && Args::value)> {};

		template <typename... Args>
		struct Or: BooleanConstant<(... || Args::value)> {};
};

template<usize V> struct SizeConstant: ValueConstant<usize, V> {};

struct TrueType:	BooleanConstant<true>	{};
struct FalseType:	BooleanConstant<false>	{};

template<class T> struct TypeContainer {typedef T Type;};

#endif // CTL_TYPETRAITS_CONTAINERS_H
