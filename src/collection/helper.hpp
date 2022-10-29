#ifndef HELPING_HAND_H
#define HELPING_HAND_H

#include <vector>
#include "definitions.hpp"

namespace Helper {
	namespace {
		using
			std::vector,
			std::map;
	}

	template<typename T, typename T2>
	vector<T> convertList(vector<T2> source) {
		vector<T> res;
		for $each(i, source)
			res.push_back((T)i);
		return res;
	}

	template<typename T, typename T2>
	vector<T> getKeys(map<T, T2> lst) {
		vector<T> keys;
		for (auto i = lst.begin(); i != lst.end(); i++) {
			keys.push_back(i->first);
		}
		return keys;
	}

	template<typename T, typename T2>
	vector<T2> getValues(map<T, T2> lst) {
		vector<T2> values;
		for (auto i = lst.begin(); i != lst.end(); i++) {
			values.push_back(i->second);
		}
		return values;
	}
}

#endif // HELPING_HAND_H
