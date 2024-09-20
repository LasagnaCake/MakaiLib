#ifndef MAKAILIB_ASYNC_MUTEX_H
#define MAKAILIB_ASYNC_MUTEX_H

#include "core.hpp"

namespace Makai::Async {
	class Mutex: Yieldable {
	public:
		constexpr Mutex() noexcept {}

		Mutex& capture();

		bool tryCapture();

		Mutex& release();

		Mutex& wait();

		bool isCaptured();
		bool isOwnedBy(Thread::id const& thread);
		bool isCurrentOwner();

		Nullable<Thread::id> getCurrentOwner();

	private:
		Atomic<Nullable<Thread::id>> current;
	};
}

#endif // MAKAILIB_ASYNC_MUTEX_H
