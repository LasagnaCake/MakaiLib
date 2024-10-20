#ifndef CTL_TYPETRAITS_BITCAST_H
#define CTL_TYPETRAITS_BITCAST_H

#include "../namespace.hpp"
#include "decay.hpp"
#ifdef CTL_DO_NOT_USE_BUILTINS
#include "../memory/memory.hpp"
#endif // CTL_DO_NOT_USE_BUILTINS

CTL_NAMESPACE_BEGIN

template<class TDst, class TSrc>
#ifndef CTL_DO_NOT_USE_BUILTINS
constexpr
#endif // CTL_DO_NOT_USE_BUILTINS
TDst bitcast(TSrc const& v) noexcept {
	static_assert(sizeof(TDst) == sizeof(TSrc), "Sizes of source and target type must match!");
	#ifdef CTL_DO_NOT_USE_BUILTINS
	TDst r;
	MX::memcpy(&r, &v, sizeof(TDst));
	return r;
	#else
	return __builtin_bit_cast(TDst, v);
	#endif // CTL_DO_NOT_USE_BUILTINS
}

CTL_NAMESPACE_END

#endif