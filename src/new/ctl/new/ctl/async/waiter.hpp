#ifndef CTL_ASYNC_WAITABLE_H
#define CTL_ASYNC_WAITABLE_H

#include "../namespace.hpp"
#include "../container/functor.hpp"
#include "thread.hpp"
#include "atomic.hpp"

CTL_NAMESPACE_BEGIN

DEFINE_ERROR_TYPE(OccupiedError);

struct Waiter {
	template<class T = OS::Time::Millis>
	static void wait(usize const& time) {
		Thread::wait<T>(time);
	}

	static void wait(Atomic<bool>& condition) {
		while (condition.value()) yieldThread();
	}

	static void wait(Atomic<Trigger<>>& predicate) {
		while (predicate.value()()) yieldThread();
	}

	static void wait(Trigger<> const& predicate) {
		while (predicate()) yieldThread();
	}
};

CTL_NAMESPACE_END

#endif // CTL_ASYNC_WAITABLE_H
