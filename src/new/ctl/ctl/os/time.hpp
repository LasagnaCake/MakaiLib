#ifndef CTL_OS_TIME_H
#define CTL_OS_TIME_H

#include <chrono>
#include "../namespace.hpp"

CTL_NAMESPACE_BEGIN

namespace OS::Time {
	namespace {
		namespace chrono = std::chrono;
	}

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

	class Clock {
	private:
		typedef chrono::system_clock SysClock;
		inline static const auto start = SysClock::now();
	public:

		/// Gets the current time, since the epoch (Jan 1st, 1970).
		template <class T = Micros>
		constexpr static usize sinceEpoch() {
			return chrono::duration_cast<T>(SysClock::now().time_since_epoch()).count();
		}

		/// Gets the current time, since program start.
		template <class T = Micros>
		constexpr static usize sinceStart() {
			return chrono::duration_cast<T>(SysClock::now() - start).count();
		}
	};

	inline usize time() {
		return Clock::sinceEpoch<Millis>();
	}
}

CTL_NAMESPACE_END

#endif // CTL_OS_TIME_H
