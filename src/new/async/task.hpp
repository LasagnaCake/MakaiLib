#ifndef MAKAILIB_ASYNC_TASK_H
#define MAKAILIB_ASYNC_TASK_H

#include "core.hpp"
#include "promise.hpp"

namespace Makai::Async {
	template<typename T>
	class Task;

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

		PromiseType getPromise() requires Type::Different<R, void>	{return Promise(result.asWeak(), executor);	}
		PromiseType getPromise() requires Type::Equal<R, void>		{return Promise(executor);					}

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

#endif // MAKAILIB_ASYNC_TASK_H
