#ifndef CTL_ASYNC_WAITABLE_H
#define CTL_ASYNC_WAITABLE_H

#include "../namespace.hpp"
#include "../container/functor.hpp"
#include "thread.hpp"
#include "atomic.hpp"

CTL_NAMESPACE_BEGIN

DEFINE_ERROR_TYPE(OccupiedError);

namespace Async {
	template<class T = OS::Time::Millis>
	inline void wait(usize const& time) {
		Thread::wait<T>(time);
	}

	inline void wait(Atomic<bool>& condition) {
		while (condition.value()) Thread::yield();
	}

	inline void wait(Atomic<Trigger<>>& predicate) {
		while (predicate.value()()) Thread::yield();
	}

	inline void wait(Trigger<> const& predicate) {
		while (predicate()) Thread::yield();
	}
};

CTL_NAMESPACE_END

#endif // CTL_ASYNC_WAITABLE_H
