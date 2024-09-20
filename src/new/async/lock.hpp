#ifndef MAKAILIB_ASYNC_LOCK_H
#define MAKAILIB_ASYNC_LOCK_H

namespace Makai::Async {
	template<Type::Derived<Mutex> T = Mutex>
	class BaseLock {
	public:
		typedef T DataType;

		BaseLock(T& _mutex): mutex(_mutex) {}

	protected:
		T& mutex;
	};

	template<Type::Derived<Mutex> T = Mutex>
	class ScopeLock: public BaseLock<T> {
	public:
		ScopeLock(T& mutex): BaseLock<T>(mutex)	{this->mutex.capture();}
		~ScopeLock()							{this->mutex.release();}
	};
}

#endif // MAKAILIB_ASYNC_LOCK_H
