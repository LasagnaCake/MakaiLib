#ifndef ASYNCHRONOUS_EXECUTION_H
#define ASYNCHRONOUS_EXECUTION_H

#include "helper.hpp"
#include "dataview.hpp"
#include "referential.hpp"
#include "threaddata.hpp"
#include <thread>

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

	namespace {
		using Threaded::Atomic;
	}

	DEFINE_ERROR_TYPE(OccupiedError);

	template<typename T>
	class Promise {
	public:
		constexpr Promise(Atomic<T>& v, WeakPointer<Thread> t): data(v), thread(t) {}

		T awaitValue() {
			if ((!thread()) || thread->joinable())
				thread->join();
			return data;
		}

		Nullable<T> value() {
			if ((!thread()) || thread->joinable())
				return nullptr;
			return data;
		}

	public:
		WeakPointer<Thread>	thread;
		Atomic<T>&			data;
	}

	template<class T = Time::Millis>
	void wait(size_t const& millis) {
		std::this_thread::sleep_for<T>(millis);
	}

	template<class T = Time::Millis>
	void wait(Atomic<bool>& condition) {
		while (condition) {};
	}

	template<class T = Time::Millis>
	void wait(Atomic<Functor<bool(void)>>& predicate) {
		while (predicate.value()()) {};
	}

	typedef StrongPointer<Atomic<size_t>> CounterReference;

	class Waiter {
		constexpr Waiter(CounterReference const& _counter): counter(_counter) {}

		constexpr ~Waiter() {counter.unbind();}

		constexpr void wait(size_t const& ticks) {
			counter = ticks;
			while (ticks > 0) {}
		}

	private:
		CounterReference counter;
	};

	class Processor {
		constexpr Processor() {}

		void yield() {
			size_t i = 0;
			for (CounterReference& counter: counters) {
				if (counter.count() < 2)	counters.erase(counters.begin() + i);
				else if (counter > 0)		(*counter)--;
				++i;
			}
		}

		Waiter getWaiter() {
			counters.push_back(new Atomic<size_t>());
			return Waiter(counter.back());
		}

	private:
		LinkedList<CounterReference> counters;
	};

	template<typename T>
	class Task;

	template<typename R, typename... Args>
	class Task<R(Args...)> {
		typedef R FunctionType(Args...);
		typedef Functor<FunctionType> FunctorType;

	public:
		constexpr Task(FunctorType const& f) {
			target = f;
		}

		constexpr operator=(FunctorType const& f) {
		}

		Promise run(Args... args) {
			executor
			.destroy()
			.bind(
				new Thread(
					[&result, ...args] {
						result = target(...args);
					}
				)
			);
			return getPromise();
		}

		T await() {
			if (running())
				executor->join();
			return result;
		}

		Nullable<T> result() {
			if (running())
				return nullptr;
			return result;
		}

		Promise getPromise() {
			return Promise<R>(result, executor);
		}

		bool running() const {
			return executor() && executor->joinable();
		}

		void stop() {
			executor.destroy();
		}

	private:
		Atomic<T>				result;
		FunctorType				target;
		StrongPointer<Thread>	executor;
	}

}

#endif // ASYNCHRONOUS_EXECUTION_H
