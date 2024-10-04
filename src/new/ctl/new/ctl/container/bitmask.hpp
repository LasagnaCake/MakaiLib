#ifndef CTL_CONTAINER_BITMASK_H
#define CTL_CONTAINER_BITMASK_H

#include "../templates.hpp"
#include "../typetraits/traits.hpp"

CTL_NAMESPACE_BEGIN

template<Type::Integer TData, usize S, bool I = true>
struct BitMask:
	Typed<TData>,
	SelfIdentified<BitMask<TData, S, I>> {
	constexpr static usize SIZE = S;
	constexpr static bool INITIAL_STATE = I;

	using Typed				= ::CTL::Typed<TData>;
	using SelfIdentified	= ::CTL::SelfIdentified<BitMask<TData, S, I>>;

	using typename Typed::DataType;
	using typename SelfIdentified::SelfType;

	using MaskType = Decay::AsType<DataType[SIZE]>;

	constexpr static DataType ALL_ENABLED = TypeInfo<DataType>::HIGHEST;

	constexpr BitMask() {}

	constexpr BitMask(bool const& state) {
		for (usize i = 0; i < SIZE; ++i)
			mask[i] =  (state ? ALL_ENABLED : 0);
	}

	constexpr BitMask(MaskType const& mask) {
		for (usize i = 0; i < SIZE; ++i)
			this->mask[i] = mask[i];
	}

	constexpr BitMask(SelfType const& other): BitMask(other.mask) {}

	constexpr DataType reduce() const {
		DataType result{1};
		for (usize i = 0; i < SIZE; ++i)
			result &= mask[i];
		return result;
	}

	constexpr DataType overlap() const {
		DataType result{0};
		for (usize i = 0; i < SIZE; ++i)
			result |= mask[i];
		return result;
	}

	constexpr SelfType match(SelfType const& other) const {
		SelfType result;
		for (usize i = 0; i < SIZE; ++i)
			result.mask[i] = other.mask[i] & mask[i];
		return result;
	}

	constexpr SelfType join(SelfType const& other) const {
		SelfType result;
		for (usize i = 0; i < SIZE; ++i)
			result.mask[i] = other.mask[i] | mask[i];
		return result;
	}

	constexpr SelfType exclude(SelfType const& other) const {
		SelfType result;
		for (usize i = 0; i < SIZE; ++i)
			result.mask[i] = other.mask[i] ^ mask[i];
		return result;
	}

	constexpr SelfType operator^(SelfType const& other) const {return exclude(other);	}
	constexpr SelfType operator&(SelfType const& other) const {return match(other);		}
	constexpr SelfType operator|(SelfType const& other) const {return join(other);		}

	MaskType mask = {INITIAL_STATE ? ALL_ENABLED : 0};
};

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_BITMASK_H
