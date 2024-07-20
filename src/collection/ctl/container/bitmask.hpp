#ifndef CTL_CONTAINER_BITMASK_H
#define CTL_CONTAINER_BITMASK_H

#include "../templates.hpp"
#include "../typetraits/traits.hpp"

template<Type::Integer TData, size_t S, bool I = true>
struct BitMask:
	Typed<TData>,
	SelfIdentified<BitMask<TData, S, I>> {
	constexpr static usize SIZE = S;
	constexpr static bool INITIAL_STATE = I;

	using typename Typed<TData>::DataType;
	using typename SelfIdentified<BitMask<TData, S, I>>::SelfType;

	typedef Decay::AsType<DataType[SIZE]>	MaskType;

	constexpr static DataType ALL_ENABLED = TypeInfo<DataType>::HIGHEST;

	constexpr BitMask() {}

	constexpr BitMask(bool const& state) {
		for SRANGE(i, 0, SIZE)
			mask[i] =  (state ? ALL_ENABLED : 0);
	}

	constexpr BitMask(MaskType const& mask) {
		for SRANGE(i, 0, SIZE)
			this->mask[i] = mask[i];
	}

	constexpr BitMask(SelfType const& other): BitMask(other.mask) {}

	constexpr DataType reduce() const {
		DataType result{1};
		for SRANGE(i, 0, SIZE)
			result &= mask[i];
		return result;
	}

	constexpr DataType overlap() const {
		DataType result{0};
		for SRANGE(i, 0, SIZE)
			result |= mask[i];
		return result;
	}

	constexpr SelfType match(SelfType const& other) const {
		SelfType result;
		for SRANGE(i, 0, SIZE)
			result.mask[i] = other.mask[i] & mask[i];
		return result;
	}

	constexpr SelfType join(SelfType const& other) const {
		SelfType result;
		for SRANGE(i, 0, SIZE)
			result.mask[i] = other.mask[i] | mask[i];
		return result;
	}

	constexpr SelfType exclude(SelfType const& other) const {
		SelfType result;
		for SRANGE(i, 0, SIZE)
			result.mask[i] = other.mask[i] ^ mask[i];
		return result;
	}

	constexpr SelfType operator^(SelfType const& other) const {return exclude(other);	}
	constexpr SelfType operator&(SelfType const& other) const {return match(other);		}
	constexpr SelfType operator|(SelfType const& other) const {return join(other);		}

	MaskType mask = {INITIAL_STATE ? ALL_ENABLED : 0};
};

#endif // CTL_CONTAINER_BITMASK_H
