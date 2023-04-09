#ifndef HELPING_HAND_H
#define HELPING_HAND_H

#include <vector>
#ifdef _USE_CPP20_FORMAT_
#include <format>
#endif // _USE_CPP20_FORMAT_
#include "conceptual.hpp"
#include "definitions.hpp"

namespace Helper {
	namespace {
		using
			std::vector,
			std::map,
			std::unordered_map,
			std::string;
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

	template<typename T, typename T2>
	vector<T> getKeys(unordered_map<T, T2> lst) {
		vector<T> keys;
		for (auto i = lst.begin(); i != lst.end(); i++) {
			keys.push_back(i->first);
		}
		return keys;
	}

	template<typename T, typename T2>
	vector<T2> getValues(unordered_map<T, T2> lst) {
		vector<T2> values;
		for (auto i = lst.begin(); i != lst.end(); i++) {
			values.push_back(i->second);
		}
		return values;
	}
	// If you're using a compiler that supports it, please enable it.
	// God fucking dammit, MSYS2.
	#ifdef _USE_CPP20_FORMAT_
	template<Type::Number T>
	string floatString(T val, size_t precision = 2) {
		return std::format(std::format("{{:.{}f}}", precision), val);
	}
	#else

	template<Type::Number T>
	string floatString(T val, size_t precision = 1) {
		if (!precision)
			return std::to_string((long long)val);
		std::stringstream ss;
		ss << std::fixed << std::setprecision(precision) << val;
		return ss.str();
	}

	template<Type::Number T>
	using FloatFormat = std::pair<T, size_t>;

	template<Type::Number T>
	string floatString(FloatFormat<T> fmt) {
		return floatString(fmt->first, fmt->second);
	}
	#endif // _USE_CPP20_FORMAT_
}

namespace Time {
	namespace {namespace chrono {using namespace std::chrono;}}

	typedef chrono::system_clock	Clock;

	namespace {auto start = Clock::now();}

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
	constexpr inline size_t sinceEpoch() {
		return chrono::duration_cast<T>(Clock::now().time_since_epoch()).count();
	}

	/// Gets the current time, since program start.
	template <class T = Micros>
	constexpr inline size_t sinceStart() {
		return chrono::duration_cast<T>(Clock::now() - start).count();
	}
}

#endif // HELPING_HAND_H
