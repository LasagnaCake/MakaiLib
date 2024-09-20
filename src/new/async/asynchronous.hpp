#ifndef ASYNCHRONOUS_EXECUTION_H
#define ASYNCHRONOUS_EXECUTION_H

#include "../std/helper.hpp"
#include "../std/view.hpp"
#include "../std/pointer.hpp"
#include <utility>
#include "core.hpp"

namespace Async {
	/*struct Promise;

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
	};*/

	class Timekeeper {
		typedef Instance<Atomic<size_t>> CounterReference;

	public:
		class Waiter {
			constexpr Waiter(Waiter const& other): counter(other.counter)			{}
			constexpr Waiter(Waiter&& other): counter(std::move(other.counter))		{}

			constexpr ~Waiter() {counter.unbind();}

			void wait(size_t const& ticks) {
				(*counter) = ticks;
				while (counter && counter->value() > 0) yieldThread();
				// wait(Functor<bool(void)>([this] {return !(counter && counter->value() > 0);}));
			}

		private:

			constexpr Waiter(CounterReference const& _counter): counter(_counter)	{}

			CounterReference counter;

			friend class Timekeeper;
		};

		Timekeeper() {}

		void yield() {
			size_t i = 0;
			for (CounterReference& counter: counters) {
				if (counter.count() < 2)		counters.erase(std::next(counters.begin(), i));
				else if (counter->value() > 0)	(*counter)--;
				++i;
			}
		}

		Waiter getWaiter() {
			counters.push_back(new Atomic<size_t>());
			return Waiter(counters.back());
		}

	private:
		LinkedList<CounterReference> counters;
	};

}

#endif // ASYNCHRONOUS_EXECUTION_H
