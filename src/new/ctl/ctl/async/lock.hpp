#ifndef CTL_ASYNC_LOCK_H
#define CTL_ASYNC_LOCK_H

#include "../namespace.hpp"
#include "../templates.hpp"
#include "../typetraits/traits.hpp"
#include "mutex.hpp"

CTL_NAMESPACE_BEGIN

template<Type::Derived<Mutex> TMutex = Mutex>
class BaseLock:
	public Typed<TMutex>,
	public SelfIdentified<BaseLock<TMutex>> {
public:
	using Typed	= ::CTL::Typed<TMutex>;

	using typename Typed::ReferenceType;

	BaseLock(ReferenceType mutex): mutex(mutex) {}

protected:
	ReferenceType mutex;
};

template<Type::Derived<Mutex> TMutex = Mutex>
class ScopeLock:
	public BaseLock<TMutex>,
	public Derived<BaseLock<TMutex>>,
	public SelfIdentified<ScopeLock<TMutex>> {
public:
	using Derived = ::CTL::Derived<BaseLock<TMutex>>;

	using typename Derived::BaseType;

	using typename BaseType::ReferenceType;

	ScopeLock(ReferenceType mutex): BaseType(mutex)	{mutex.capture();}
	~ScopeLock()									{mutex.release();}
	
protected:
	using BaseType::mutex;
};

CTL_NAMESPACE_END

#endif // CTL_ASYNC_LOCK_H
