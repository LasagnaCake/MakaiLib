#ifndef CTL_ASYNC_LOCK_H
#define CTL_ASYNC_LOCK_H

#include "../namespace.hpp"
#include "../templates.hpp"
#include "../typetraits/traits.hpp"
#include "mutex.hpp"

CTL_NAMESPACE_BEGIN

template<Type::Derived<Mutex> T = Mutex>
class BaseLock:
	public Typed<T>,
	public SelfIdentified<BaseLock<T>> {
public:
	using typename Typed<T>::ReferenceType;

	BaseLock(ReferenceType mutex): mutex(mutex) {}

protected:
	ReferenceType mutex;
};

template<Type::Derived<Mutex> T = Mutex>
class ScopeLock:
	public BaseLock<T>,
	public Derived<BaseLock<T>>,
	public SelfIdentified<ScopeLock<T>>{
public:
	using Derived = typename Derived<BaseLock<T>>;

	using BaseType = typename Derived::Bases::FirstType;

	using typename BaseType::ReferenceType;

	ScopeLock(ReferenceType mutex): BaseType(mutex)	{mutex.capture();}
	~ScopeLock()									{mutex.release();}
};

CTL_NAMESPACE_END

#endif // CTL_ASYNC_LOCK_H
