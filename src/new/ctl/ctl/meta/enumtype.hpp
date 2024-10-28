#ifndef CTL_META_ENUMTYPE_H
#define CTL_META_ENUMTYPE_H

#include "../namespace.hpp"
#include "../typetraits/basictraits.hpp"
#include "../algorithm/bitwise.hpp"
#include "logic.hpp"
#include "pack.hpp"

CTL_NAMESPACE_BEGIN

namespace Meta {
	template<Type::Enumerator T>
	using SignedEnumType = NthType<(bitWidth(sizeof(T))-1), int8, int16, int32, int64>;

	template<Type::Enumerator T>
	using UnsignedEnumType = NthType<(bitWidth(sizeof(T))-1), uint8, uint16, uint32, uint64>;

	template<Type::Enumerator T>
	using EnumType = DualType<Type::Signed<T>, SignedEnumType<T>, UnsignedEnumType<T>>;
}

CTL_NAMESPACE_END

#endif