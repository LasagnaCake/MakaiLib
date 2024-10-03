#ifndef CTL_ASYNC_THREAD_H
#define CTL_ASYNC_THREAD_H

#include <thread>
#include "../namespace.hpp"
#include "../container/pointer.hpp"
#include "../os/time.hpp"

CTL_NAMESPACE_BEGIN

struct Thread: private std::thread {
	using BaseType = std::thread;

	Thread() noexcept:					BaseType(), exect(*this)									{}
	Thread(Thread&& other) noexcept:	BaseType(CTL::move(other)), exect(CTL::move(other.exect))	{}
	Thread(Thread const& other)			= delete;

	~Thread() {exect.requestStop();}

	using Handle	= Handle<Thread>;
	using Instance	= Instance<Thread>;

	struct ExcecutionToken {
		constexpr ExcecutionToken(Thread& source):				stop(false), source(source)				{}
		constexpr ExcecutionToken(ExcecutionToken&& other):		stop(other.stop), source(other.source)	{}
		constexpr ExcecutionToken(ExcecutionToken const& other)	= delete;

		constexpr bool shouldStop() const	{return stop;			}
		constexpr operator bool() const		{return shouldStop();	}

		constexpr ExcecutionToken& requestStop() {
			stop = true;
			return *this;
		}

		constexpr Thread& thread() {return source;}

	private:
		bool	stop;
		Thread&	source;
	};

	struct ID: private std::thread::id {
	private:
		using BaseType = std::thread::id;
		ID(): BaseType() {}
		ID(BaseType&& other): BaseType(CTL::move(other)) {}
		friend class ::CTL::Thread;
		friend std::strong_ordering operator<=>(ID const&, ID const&);
		friend bool operator==(ID const&, ID const&);
	};

	template<class F, class... Args>
	explicit Thread(F&& f, Args&&... args):
		BaseType(forward(f), exect, forward(args)...) {
	}

	static ID current() noexcept {
		return std::this_thread::get_id();
	}

	static void yield() noexcept {
		std::this_thread::yield();
	}

	template<class T = OS::Time::Millis>
	static void wait(usize const& time) {
		std::this_thread::sleep_for<T>(time);
	}

	Thread& join() {
		thread::join();
		return *this;
	}

	Thread& detach() {
		thread::detach();
		return *this;
	}

	ExcecutionToken& token() {return exect;}

	bool running() const {
		return thread::joinable();
	}

	operator bool() const {return running();}

private:
	ExcecutionToken exect;
};

std::strong_ordering operator<=>(Thread::ID const& a, Thread::ID const& b) {
	return a <=> b;
}

bool operator==(Thread::ID const& a, Thread::ID const& b) {
	return a == b;
}

namespace Base::Async {
	struct Yieldable {
	protected:
		static void asyncYield() noexcept {
			Thread::yield();
		}
	};
}

CTL_NAMESPACE_END

#endif // CTL_ASYNC_THREAD_H
