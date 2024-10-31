#ifndef CTL_TYPETRAITS_CONTAINERS_H
#define CTL_TYPETRAITS_CONTAINERS_H

#include "../ctypes.hpp"
#include "typecontainer.hpp"

CTL_NAMESPACE_BEGIN

template <class...>
using VoidType = void;

template<class T, auto V> struct ValueConstant {
	typedef T DataType;

	constexpr static DataType value = V;
};

namespace Type::Trait {
	template<class C>
	concept ValueConstant =
		requires {
			typename C::DataType;
			C::value;
		} && (reinterpret_cast<typename C::DataType>(C::value) == ::CTL::ValueConstant<typename C::DataType, C::value>::value)
	;
}

template<bool V> struct BooleanConstant: ValueConstant<bool, V> {
		template <typename... Args>
		requires (... && Type::Trait::ValueConstant<Args>)
		struct And: BooleanConstant<(... && Args::value)> {};

		template <typename... Args>
		requires (... && Type::Trait::ValueConstant<Args>)
		struct Or: BooleanConstant<(... || Args::value)> {};
};

template<usize V> struct SizeConstant: ValueConstant<usize, V> {};

struct TrueType:	BooleanConstant<true>	{};
struct FalseType:	BooleanConstant<false>	{};

CTL_NAMESPACE_END

#endif // CTL_TYPETRAITS_CONTAINERS_H
