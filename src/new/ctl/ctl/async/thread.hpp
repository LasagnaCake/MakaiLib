#ifndef CTL_ASYNC_THREAD_H
#define CTL_ASYNC_THREAD_H

#include <thread>
#include "../namespace.hpp"
#include "../os/time.hpp"

CTL_NAMESPACE_BEGIN

/// @brief Execution thread.
struct Thread:
	public SelfIdentified<Thread>,
	public Derived<std::thread>,
	private std::thread {
	using SelfIdentified	= ::CTL::SelfIdentified<Thread>;
	using Derived			= ::CTL::Derived<std::thread>;

	using typename Derived::BaseType;

	using typename SelfIdentified::SelfType;

	using BaseType::swap;

	/// @brief Empty constructor.
	Thread() noexcept:					BaseType(), exect(*this)												{}
	Thread(SelfType&& other) noexcept:	BaseType((BaseType&&)CTL::move(other)), exect(CTL::move(other.exect))	{}
	Thread(SelfType const& other)		= delete;

	~Thread() {exect.requestStop();}

	struct ExecutionSource {
		constexpr ExecutionSource(Thread& source):				stop(false), source(source)				{}
		constexpr ExecutionSource(ExecutionSource&& other):		stop(other.stop), source(other.source)	{}
		constexpr ExecutionSource(ExecutionSource const& other)	= delete;

		constexpr bool shouldStop() const	{return stop;			}
		constexpr operator bool() const		{return shouldStop();	}

		constexpr ExecutionSource& requestStop() {
			stop = true;
			return *this;
		}

		constexpr Thread& thread() {return source;}

	private:
		bool	stop;
		Thread&	source;
	};

	struct ExecutionToken {
		constexpr ExecutionToken(ExecutionSource*&& source):		source(source)	{}
		constexpr ExecutionToken(ExecutionSource& source):			source(&source)	{}
		constexpr ExecutionToken(ExecutionSource* const& source)	= delete;
		constexpr ExecutionToken(ExecutionSource&& source)			= delete;

		constexpr ExecutionSource* operator->()	{return source;		}
		constexpr ExecutionSource& operator*()	{return *source;	}

	private:
		ExecutionSource* const source;
	};

	struct ID:
		private std::thread::id,
		public ::CTL::Ordered,
		public ::CTL::Derived<std::thread::id> {
	private:
		using Derived = ::CTL::Derived<std::thread::id>;

		using typename Derived::BaseType;

	public:
		BaseType const& base() const {return (*this);}
		ID(): BaseType() {}

		OrderType operator<=>(ID const& other) const	{return OrderType(base() <=> other.base());	}
		bool operator==(ID const& other) const			{return base() == other.base();				}

	private:
		ID(BaseType&& other): BaseType(CTL::move(other)) {}

		friend class ::CTL::Thread;
	};

	template<class F, class... Args>
	explicit Thread(F&& f, Args&&... args):
		BaseType(forward(f), token(), forward(args)...) {
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

	SelfType& join() {
		thread::join();
		return *this;
	}

	SelfType& detach() {
		thread::detach();
		return *this;
	}

	ExecutionToken token()		{return exect;}
	ExecutionSource& source()	{return exect;}

	bool running() const {
		return thread::joinable();
	}

	operator bool() const {return running();}

private:
	ExecutionSource exect;
};

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
