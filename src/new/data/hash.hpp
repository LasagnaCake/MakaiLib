#ifndef MAKAILIB_DATA_HASH_H
#define MAKAILIB_DATA_HASH_H

#include "../compat/ctl.hpp"

namespace Makai::Data {
	enum class HashMode {
		HM_SHA3_224,
		HM_SHA3_256,
		HM_SHA3_384,
		HM_SHA3_512,
	};

	void hash(BinaryData<>& data, HashMode const& mode = HashMode::HM_SHA3_256);
	BinaryData<> hashed(BinaryData<> const& data, HashMode const& mode = HashMode::HM_SHA3_256);
}

#endif // MAKAILIB_DATA_HASH_H
