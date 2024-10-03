#ifndef CTL_ASYNC_TASK_H
#define CTL_ASYNC_TASK_H

#include "../namespace.hpp"
#include "../container/functor.hpp"
#include "promise.hpp"

CTL_NAMESPACE_BEGIN

template<class F>
class Task;

template<typename R, typename... Args>
class Task<R(Args...)>:
	Functional<Promise<R>(Thread::ExecutionToken, Args...)>,
	Returnable<Promise<R>>,
	Argumented<Args...>,
	SelfIdentified<Tast<R, Args...>> {
public:
	using Functional = ::CTL::Functional<Promise<R>(Thread::ExcecutionToken, Args...)>;
	using Returnable = ::CTL::Returnable<Promise<R>>;

	using
		Functional::FunctionType
	;

	using
		Returnable::ReturnType
	;

	using ExecutionTokenType = Thread::ExecutionToken;

	using FunctorType	= Functor<FunctionType>;
	using PromiseType	= Promise<ReturnType>;
	using NullableType	= Nullable<ReturnType>;

	constexpr Task() {}

	constexpr Task(SelfType const& other):
	executor(other.executor),
	result(other.result),
	target(other.target) {
	}

	constexpr Task(SelfType&& other):
	executor(CTL::move(other.executor)),
	result(CTL::move(other.result)),
	target(CTL::move(other.target)) {
	}

	constexpr Task(FunctorType const& f): target(f)							{				}
	constexpr Task(FunctorType const& f, Args... args): SelfType(f)			{run(args...);	}
	constexpr Task(SelfType const& other, Args... args): SelfType(other)	{run(args...);	}
	constexpr Task(SelfType&& other, Args... args): SelfType(other)			{run(args...);	}

	PromiseType invoke(FunctorType const& f, Args... args) {
		if (running())
			return getPromise();
		executor = f;
		return run(args...);
	}

	PromiseType run(Args... args) requires Type::Different<ReturnType, void> {
		if (!running()) {
			rebindResult();
			executor
			.unbind()
			.bind(
				new Thread(
					[this, ...args = forward<Args>(args)] (ExecutionTokenType& exect) {
						result = nullptr;
						result = target.value()(exect, args...);
					}
				)
			);
		}
		return getPromise();
	}

	PromiseType run(Args... args) requires Type::Equal<ReturnType, void> {
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

	NullableType await() requires Type::Different<ReturnType, void> {
		if (running())
			executor->join();
		return result;
	}

	void await() requires Type::Equal<ReturnType, void> {
		if (running())
			executor->join();
	}

	NullableType value() requires Type::Different<ReturnType, void> {
		if (running())
			return nullptr;
		return result->value();
	}

	PromiseType getPromise() requires Type::Different<ReturnType, void>	{return Promise(result.asWeak(), executor);	}
	PromiseType getPromise() requires Type::Equal<ReturnType, void>		{return Promise(executor);					}

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
	Thread::Instance				executor;
};

CTL_NAMESPACE_END

#endif // CTL_ASYNC_TASK_H
