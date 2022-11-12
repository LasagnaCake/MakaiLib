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

namespace Time {
	namespace {
		$getspace(std, chrono)
		typedef chrono::system_clock _clock;
		auto start = _clock::now();
	}

	typedef _clock	Clock;

	typedef chrono::years			Years;
	typedef chrono::months			Months;
	typedef chrono::weeks			Weeks;
	typedef chrono::days			Days;
	typedef chrono::hours			Hours;
	typedef chrono::minutes			Minutes;
	typedef chrono::seconds			Seconds;
	typedef chrono::milliseconds	Millis;
	typedef chrono::microseconds	Micros;
	typedef chrono::nanoseconds		Nanos;

	/// Gets the current time, since the epoch (Jan 1st, 1970).
	template <class T = Micros>
	size_t sinceEpoch() {
		return chrono::duration_cast<T>(Clock::now().time_since_epoch()).count();
	}

	/// Gets the current time, since program start.
	template <class T = Micros>
	size_t sinceStart() {
		return chrono::duration_cast<T>(Clock::now() - start).count();
	}
}

#endif // HELPING_HAND_H
