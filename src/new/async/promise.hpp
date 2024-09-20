#ifndef MAKAILIB_ASYNC_PROMISE_H
#define MAKAILIB_ASYNC_PROMISE_H

#include "core.hpp"

namespace Makai::Async {
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
			return value();
		}

		Nullable<T> value() {
			if (ready() && data())
				return *data;
			return nullptr;
		}

		bool ready() {
			return !(thread() && thread->joinable());
		}

		constexpr Promise(Promise const& other): Promise(other.data, other.thread) {}

	private:
		constexpr Promise(Handle<Nullable<T>> const& v, Handle<Thread> const& t): data(v), thread(t) {}

		Handle<Thread>		thread;
		Handle<Nullable<T>>	data;

		template<typename F> friend class Task;
	};
}

#endif // MAKAILIB_ASYNC_PROMISE_H
