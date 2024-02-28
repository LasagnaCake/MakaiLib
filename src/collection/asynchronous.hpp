#ifndef ASYNCHRONOUS_EXECUTION_H
#define ASYNCHRONOUS_EXECUTION_H

#include "helper.hpp"
#include "dataview.hpp"
#include "referential.hpp"
#include "threaddata.hpp"
#include <thread>
#include <utility>

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

	class Timekeeper {
		typedef StrongPointer<Atomic<size_t>> CounterReference;

		class Waiter {
			constexpr Waiter(CounterReference const& _counter): counter(_counter)	{}
			constexpr Waiter(Waiter const& other): counter(other.counter)			{}
			constexpr Waiter(Waiter&& other): counter(std::move(other.counter))		{}

			constexpr ~Waiter() {counter.unbind();}

			constexpr void wait(size_t const& ticks) {
				counter = ticks;
				while (ticks > 0) {}
			}

		private:
			CounterReference counter;
		};

	public:
		constexpr Timekeeper() {}

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
		class Promise {
		public:
			Nullable<R> await() {
				if (!ready())
					thread->join();
				return data;
			}

			Nullable<R> value() {
				if (ready())
					return data;
				return nullptr;
			}

			bool ready() {
				return !(thread() && thread->joinable());
			}

			constexpr Promise(Promise const& other): Promise(other.data, other.thread) {}

		private:
			constexpr Promise(Atomic<Nullable<T>>& v, WeakPointer<Thread> t): data(v), thread(t) {}

			WeakPointer<Thread>		thread;
			Atomic<Nullable<R>>&	data;

			friend class Task<R(Args...)>;
		}

		constexpr Task() {}

		constexpr Task(Task const& other):
		executor(other.executor),
		result(other.result),
		target(other.target) {
		}

		constexpr Task(Task&& other):
		executor(std::move(other.executor)),
		result(std::move(other.result)),
		target(std::move(other.target)) {
		}

		constexpr Task(FunctorType const& f) {
			target = f;
		}

		constexpr Task(FunctorType const& f, Args... args) {
			target = f;
			run(...args);
		}

		Promise invoke(FunctorType const& f, Args... args) {
			if (running())
				return getPromise();
			executor = f;
			return run(...args);
		}

		Promise run(Args... args) {
			if (!running())
				executor
				.destroy()
				.bind(
					new Thread(
						[&target, &result, ...args = std::forward<Args>(args)] {
							result = target(...args);
						}
					)
				);
			return getPromise();
		}

		Nullable<R> await() {
			if (running())
				executor->join();
			return result;
		}

		Nullable<R> value() {
			if (running())
				return nullptr;
			return result;
		}

		Promise getPromise() {
			return Promise(result, executor);
		}

		bool running() const {
			return executor() && executor->joinable();
		}

		Task& stop() {
			executor.destroy();
			return *this;
		}

	private:
		Atomic<Nullable<T>>		result;
		FunctorType				target;
		StrongPointer<Thread>	executor;
	}

}

#endif // ASYNCHRONOUS_EXECUTION_H
