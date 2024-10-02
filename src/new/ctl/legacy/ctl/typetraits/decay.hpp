#ifndef CTL_TYPETRAITS_DECAY_H
#define CTL_TYPETRAITS_DECAY_H

#include "converter.hpp"
#include "basictraits.hpp"
#include "../algorithm/memory.hpp"
#include "../namespace.hpp"

CTL_NAMESPACE_BEGIN

namespace Decay {
	template<class T>
	using AsArgument = Meta::DualType<
        Type::Array<AsNonReference<T>>,
        AsPointer<AsNonExtent<AsNonReference<T>>>,
        Meta::DualType<
            Type::Function<AsNonReference<T>>,
            AsPointer<AsNonReference<T>>,
            AsNonCV<AsNonReference<T>>
        >
    >;

	template<class T> using AsType = T;

    template<class... Types> struct Pack {};
};

template<typename T> constexpr AsTemporary<T>		forward(AsNonReference<T>&& v)	{return static_cast<T&&>(v);					}
template<typename T> constexpr AsNonReference<T>&&	move(T&& v) noexcept			{return static_cast<AsNonReference<T>&&>(v);	}

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

#endif // CTL_TYPETRAITS_DECAY_H
