#ifndef CTL_TYPETRAITS_DECAY_H
#define CTL_TYPETRAITS_DECAY_H

#include "converter.hpp"
#include "basictraits.hpp"
#include "../algorithm/memory.hpp"

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
template<typename T> constexpr AsNonReference<T>&&	move(T&& v)						{return static_cast<AsNonReference<T>&&>(v);	}

template<class TDst, class TSrc>
#ifndef _DO_NOT_USE_BUILTINS_
constexpr
#endif // _DO_NOT_USE_BUILTINS_
bitcast(TSrc const& v) noexcept {
	static_assert(sizeof(TDst) <= sizeof(TSrc), "Sizes of target type must not be bigger than the source type!");
	#ifdef _DO_NOT_USE_BUILTINS_
	TDst r;
	MX::memcpy(&r, &v, sizeof(TDst));
	return r;
	#else
	return __builtin_bit_cast(TDst, v);
	#endif // _DO_NOT_USE_BUILTINS_
}

#endif // CTL_TYPETRAITS_DECAY_H
