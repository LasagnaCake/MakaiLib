#ifndef ASYNCHRONOUS_EXECUTION_VIA_COROUTINES_H
#define ASYNCHRONOUS_EXECUTION_VIA_COROUTINES_H

#include <coroutine>

namespace Async {
	struct Promise;

	struct Coroutine : std::coroutine_handle<Promise> {
		using promise_type = struct Promise;
		void resume();
		void operator()() {resume();};
	};

	struct Promise {
		Coroutine get_return_object() {
			return {Coroutine::from_promise(*this)};
		}
		std::suspend_always	initial_suspend() noexcept { return {}; }
		std::suspend_never	final_suspend() noexcept { return {}; }
		void return_void() {}
		void unhandled_exception() {}
	};
}

#endif // ASYNCHRONOUS_EXECUTION_VIA_COROUTINES_H
