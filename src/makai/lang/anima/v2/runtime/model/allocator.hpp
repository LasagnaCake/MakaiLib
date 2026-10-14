#ifndef MAKAILIB_ANIMA_V2_RUNTIME_MODEL_ALLOCATOR_H
#define MAKAILIB_ANIMA_V2_RUNTIME_MODEL_ALLOCATOR_H

#include "../../../../../compat/ctl.hpp"

namespace Makai::Anima::V2::Runtime {
	struct Allocator {
		struct Block {
			usize used, free;
			pointer memory;
			usize refs;
		};

		struct Section {
			ref<Block>	block;
			pointer		start;
			pointer		size;
		};
	};
}

#endif
