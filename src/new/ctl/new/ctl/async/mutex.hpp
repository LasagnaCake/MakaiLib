#ifndef CTL_ASYNC_MUTEX_H
#define CTL_ASYNC_MUTEX_H

#include "../namespace.hpp"
#include "../templates.hpp"
#include "../container/nullable.hpp"
#include "thread.hpp"
#include "atomic.hpp"

CTL_NAMESPACE_BEGIN

class Mutex:
	Base::Yieldable,
	SelfIdentified<Mutex>,
	Derived<Base::Yieldable> {
public:
	using SelfIdentified	= SelfIdentified<Mutex>;
	using Derived			= Derived<Base::Yieldable>;

	using
		typename SelfIdentified::SelfType
	;

	using BaseType = Derived::Bases::FirstType;

	Mutex() {}

	SelfType& capture()	{
		Thread::id const thisThread = currentThreadID();
		if (!isCurrentOwner())
			wait();
		current = thisThread;
		return *this;
	}

	bool tryCapture() {
		Thread::id const thisThread = currentThreadID();
		if (!isCurrentOwner()) return false;
		current = thisThread;
		return true;
	}

	SelfType& release()	{
		if (isCurrentOwner())
			current = nullptr;
		return *this;
	}

	SelfType& wait() {
		if (!isCurrentOwner())
			while(isCaptured())
				yield();
		return *this;
	}

	bool isCaptured() const							{return current.value().exists();		}
	bool isOwnedBy(Thread::ID const& thread) const	{return current == thread;				}
	bool isCurrentOwner() const						{return current == currentThreadID();	}

	bool operator==(Thread::ID const& thread)		{return isOwnedBy(thread);	}
	OrderType operator<=>(Thread::ID const& thread)	{return current <=> thread;	}

	constexpr operator bool() {return isCaptured();}

	//const Nullable<Thread::ID> getCurrentOwner() const	{return current.value();	}
	Nullable<Thread::ID> getCurrentOwner()					{return current.value();	}

private:
	Atomic<Nullable<Thread::ID>> current;
};

CTL_NAMESPACE_END

#endif // CTL_ASYNC_MUTEX_H
