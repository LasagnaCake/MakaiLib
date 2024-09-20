#ifndef MAKAILIB_ASYNC_CORE_H
#define MAKAILIB_ASYNC_CORE_H

#include <thread>

namespace Makai::Async {
	DEFINE_ERROR_TYPE(OccupiedError);

	typedef std::thread Thread;

	Thread::id currentThreadID() noexcept;

	void yieldThread() noexcept;

	struct Yieldable {
		void yield() noexcept;
	};

	template<class T = Time::Millis>
	inline void wait(usize const& millis) {
		std::this_thread::sleep_for<T>(millis);
	}

	inline void wait(Atomic<bool>& condition);
	inline void wait(Atomic<Functor<bool(void)>>& predicate);
	inline void wait(Functor<bool(void)> const& predicate);
}

#endif // MAKAILIB_ASYNC_CORE_H
