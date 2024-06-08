#ifndef CTL_TYPETRAITS_DECAY_H
#define CTL_TYPETRAITS_DECAY_H

#include "traits.hpp"

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
};

#endif // CTL_TYPETRAITS_DECAY_H
