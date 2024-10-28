#ifndef CTL_ASYNC_PROMISE_H
#define CTL_ASYNC_PROMISE_H

#include "../namespace.hpp"
#include "../typetraits/traits.hpp"
#include "../templates.hpp"
#include "../container/pointer.hpp"
#include "atomic.hpp"

CTL_NAMESPACE_BEGIN

template<class F>
class Task;

template<class T>
class Promise;

template<> class Promise<void>:
	SelfIdentified<Promise<void>> {
	using DataType = void;

	void await() {if (!ready()) thread->join();	}
	bool ready() {return !(thread && *thread);	}

	constexpr Promise(Promise const& other): Promise(other.thread) {}

private:
	constexpr Promise(Handle<Thread> const& t): thread(t) {}

	Thread::Handle thread;

	template<typename F> friend class Task;
};

template<Type::Different<void> T> class Promise<T>:
	public Typed<T>,
	public SelfIdentified<Promise<T>> {
public:
	using Typed				= Typed<T>;
	using SelfIdentified	= SelfIdentified<Promise<T>>;

	using
		typename Typed::DataType
	;

	using
		typename SelfIdentified::SelfType
	;

	using NullableType	= Nullable<DataType>;
	using HandleType	= Handle<NullableType>;

	NullableType await() {
		if (!ready())
			thread->join();
		return value();
	}

	NullableType value() {
		if (ready() && data())
			return *data;
		return nullptr;
	}

	bool ready() {
		return !(thread && *thread);
	}

	constexpr Promise(SelfType const& other): Promise(other.data, other.thread) {}

private:
	constexpr Promise(HandleType const& v, Thread::Handle const& t): data(v), thread(t) {}

	Thread::Handle	thread;
	HandleType		data;

	template<typename F> friend class Task;
};

CTL_NAMESPACE_END

#endif // CTL_ASYNC_PROMISE_H
