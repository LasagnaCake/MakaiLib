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
		using Threaded::Atomic, Threaded::Thread;
	}

	DEFINE_ERROR_TYPE(OccupiedError);

	void yieldThread() {
		std::this_thread::yield();
	}

	template<class T = Time::Millis>
	void wait(size_t const& millis) {
		std::this_thread::sleep_for<T>(millis);
	}

	void wait(Atomic<bool>& condition) {
		while (condition.value()) yieldThread();
	}

	void wait(Atomic<Functor<bool(void)>>& predicate) {
		while (predicate.value()()) yieldThread();
	}

	void wait(Functor<bool(void)> const& predicate) {
		while (predicate()) yieldThread();
	}

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

	template<typename T>
	class Task;

	template<class T>
	class Promise;

	template<> class Promise<void> {
		void await() {if (!ready()) thread->join();				}
		bool ready() {return !(thread() && thread->joinable());	}

		constexpr Promise(Promise const& other): Promise(other.thread) {}

	private:
		constexpr Promise(Handle<Thread> const& t): thread(t) {}

		Handle<Thread> thread;

		template<typename F> friend class Task;
	};

	template<Type::Different<void> T> class Promise<T> {
	public:
		Nullable<T> await() {
			if (!ready())
				thread->join();
			return data;
		}

		Nullable<T> value() {
			if (ready())
				return data;
			return nullptr;
		}

		bool ready() {
			return !(thread() && thread->joinable());
		}

		constexpr Promise(Promise const& other): Promise(other.data, other.thread) {}

	private:
		constexpr Promise(Atomic<Nullable<T>>& v, Handle<Thread> const& t): data(v), thread(t) {}

		Handle<Thread>			thread;
		Atomic<Nullable<T>>&	data;

		template<typename F> friend class Task;
	};

	template<typename R, typename... Args>
	class Task<R(Args...)> {
	public:
		typedef R ReturnType;
		typedef ReturnType FunctionType(Args...);
		typedef Functor<FunctionType>	FunctorType;
		typedef Promise<R>				PromiseType;
		typedef Nullable<R>				NullableType;

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

		constexpr Task(FunctorType const& f): target(f)					{				}
		constexpr Task(FunctorType const& f, Args... args): Task(f)		{run(args...);	}
		constexpr Task(Task const& other, Args... args): Task(other)	{run(args...);	}
		constexpr Task(Task&& other, Args... args): Task(other)			{run(args...);	}

		PromiseType invoke(FunctorType const& f, Args... args) {
			if (running())
				return getPromise();
			executor = f;
			return run(args...);
		}

		PromiseType run(Args... args) requires Type::Different<R, void> {
			if (!running()) {
				rebindResult();
				executor
				.unbind()
				.bind(
					new Thread(
						[this, ...args = std::forward<Args>(args)] {
							result = nullptr;
							result = target.value()(args...);
						}
					)
				);
			}
			return getPromise();
		}

		PromiseType run(Args... args) requires Type::Equal<R, void> {
			if (!running()) {
				rebindResult();
				executor
				.unbind()
				.bind(
					new Thread(
						target.value(),
						args...
					)
				);
			}
			return getPromise();
		}

		NullableType await() requires Type::Different<R, void> {
			if (running())
				executor->join();
			return result;
		}

		void await() requires Type::Equal<R, void> {
			if (running())
				executor->join();
		}

		NullableType value() requires Type::Different<R, void> {
			if (running())
				return nullptr;
			return result->value();
		}

		PromiseType getPromise() requires Type::Different<R, void>	{return Promise(*result, executor);	}
		PromiseType getPromise() requires Type::Equal<R, void>		{return Promise(executor);			}

		bool running() const {
			return executor() && executor->joinable();
		}

		Task& stop() {
			executor.destroy();
			return *this;
		}

	private:

		void rebindResult() {
			result.unbind().bind(new Atomic<NullableType>());
		}

		Instance<Atomic<NullableType>>	result = new Atomic<NullableType>();
		Atomic<FunctorType>				target;
		Instance<Thread>				executor;
	};

}

#endif // ASYNCHRONOUS_EXECUTION_H
