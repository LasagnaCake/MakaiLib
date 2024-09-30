#ifndef CTL_ASYNC_THREAD_H
#define CTL_ASYNC_THREAD_H

#include <thread>
#include "../namespace.hpp"
#include "../container/pointer.hpp"
#include "../os/time.hpp"

CTL_NAMESPACE_BEGIN

struct Thread: private std::thread {
	Thread() noexcept:					thread()									{}
	Thread(Thread&& other) noexcept:	thread(move(other)), stop(move(other.stop))	{}
	Thread(Thread const& other)			= delete;

	~Thread() {stop.requestStop();}

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
		using id::operator==;
		using id::operator<=>;
		friend class Thread;
	};

	template<class F, class... Args>
	explicit Thread(F&& f, Args&&... args):
		thread<F, ExecutionToken&, Args...>(forward(f), excec, forward(args)...) {
	}

	static ID current() noexcept {
		return std::this_thread::get_id();
	}

	static void yield() noexcept {
		std::this_thread::yield();
	}

	template<class T = Time::Millis>
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

	ExcecutionToken& token() {return excec;}

	bool running() const {
		return thread::joinable();
	}

	operator bool() const {return running();}

private:
	ExcecutionToken excec;
};

namespace Base::Async {
	struct Yieldable {
	protected:
		static void asyncYield() noexcept const {
			yield();
		}
	};
}

CTL_NAMESPACE_END

#endif // CTL_ASYNC_THREAD_H
