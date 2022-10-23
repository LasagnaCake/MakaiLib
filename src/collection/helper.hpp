#ifndef HELPING_HAND_H
#define HELPING_HAND_H

#include <vector>
#include "definitions.hpp"

namespace Helper {
	namespace {
		using std::vector;
	}

	template<typename T, typename T2>
	vector<T> convertList(vector<T2> source) {
		vector<T> res;
		for $each(i, source)
			res.push_back((T)i);
		return res;
	}
}

#endif // HELPING_HAND_H
