#ifndef CTL_TYPETRAITS_CONTAINERS_H
#define CTL_TYPETRAITS_CONTAINERS_H

#include "../ctypes.hpp"
#include "typecontainer.hpp"

template <class...>
using VoidType = void;

template<class T, auto V> struct ValueConstant {
	typedef T DataType;

	constexpr static DataType value = V;
};

template<class C>
concept ValidValueConstant =
	requires {
		typename C::DataType;
		C::value;
	} && (reinterpret_cast<typename C::DataType>(C::value) == ValueConstant<typename C::DataType, C::value>::value)
;

template<bool V> struct BooleanConstant: ValueConstant<bool, V> {
		template <typename... Args>
		requires (... && ValidValueConstant<Args>)
		struct And: BooleanConstant<(... && Args::value)> {};

		template <typename... Args>
		requires (... && ValidValueConstant<Args>)
		struct Or: BooleanConstant<(... || Args::value)> {};
};

template<usize V> struct SizeConstant: ValueConstant<usize, V> {};

struct TrueType:	BooleanConstant<true>	{};
struct FalseType:	BooleanConstant<false>	{};

#endif // CTL_TYPETRAITS_CONTAINERS_H
