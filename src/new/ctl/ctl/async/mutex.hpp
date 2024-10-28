#ifndef CTL_ASYNC_MUTEX_H
#define CTL_ASYNC_MUTEX_H

#include "../namespace.hpp"
#include "../templates.hpp"
#include "../container/nullable.hpp"
#include "thread.hpp"
#include "atomic.hpp"

CTL_NAMESPACE_BEGIN

class Mutex:
	Base::Async::Yieldable,
	SelfIdentified<Mutex>,
	Derived<Base::Async::Yieldable>,
	Ordered {
public:
	using SelfIdentified	= SelfIdentified<Mutex>;
	using Derived			= Derived<Base::Async::Yieldable>;

	using
		typename SelfIdentified::SelfType
	;

	using typename Derived::BaseType;

	Mutex() {}

	SelfType& capture()	{
		Thread::ID const thisThread = Thread::current();
		if (!isCurrentOwner())
			wait();
		current = thisThread;
		return *this;
	}

	bool tryCapture() {
		Thread::ID const thisThread = Thread::current();
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
			while(captured())
				asyncYield();
		return *this;
	}

	bool captured() const							{return current.value().exists();	}
	bool ownedBy(Thread::ID const& id) const		{return equals(id);					}
	bool isCurrentOwner() const						{return equals(Thread::current());	}

	bool operator==(Thread::ID const& id) const			{return ownedBy(id);	}
	OrderType operator<=>(Thread::ID const& id) const	{return compare(id);	}

	operator bool() {return captured();}

	//const Nullable<Thread::ID> currentOwner() const	{return current.value();	}
	Nullable<Thread::ID> currentOwner()					{return current.value();	}

private:
	bool equals(Thread::ID const& id) const {
		if (captured())
			return current.value().value() == id;
		return false;
	}

	OrderType compare(Thread::ID const& id)	const {
		if (captured())
			return current.value().value() <=> id;
		return Order::LESS;
	}

	Atomic<Nullable<Thread::ID>> current;
};

CTL_NAMESPACE_END

#endif // CTL_ASYNC_MUTEX_H
