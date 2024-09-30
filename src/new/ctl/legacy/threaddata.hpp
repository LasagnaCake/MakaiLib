#ifndef THREADED_DATA_MANIPULATION_H
#define THREADED_DATA_MANIPULATION_H

#include <thread>
#include "dataview.hpp"

namespace Threaded {
	typedef std::thread Thread;

	inline Thread::id currentThreadID() noexcept {
		return std::this_thread::get_id();
	}

	inline void yieldThread() noexcept {
		std::this_thread::yield();
	}

	struct Yieldable {
		inline void yield() noexcept {
			yieldThread();
		}
	};

	template <class T>
	class Atomic: Yieldable {
	public:
		typedef T DataType;

		constexpr Atomic()											{}
		constexpr Atomic(T const& _data): data(_data)				{}
		constexpr Atomic(T&& _data):		data(std::move(_data))	{}

		inline bool isLocked() {return locked;}

		inline Atomic& operator=(T const& val) {
			capture();
			data = val;
			return release();
		}

		inline Atomic& operator=(T&& val) {
			capture();
			data = std::move(val);
			return release();
		}

		inline Atomic& modify(Operation<T> op) {
			capture();
			data = op(data);
			return release();
		}

		inline Atomic& operator()(Operation<T> op) {return modify(op);}

		inline DataView<Atomic<T>> view() {
			capture();
			DataView<Atomic<T>> v = DataView<Atomic<T>>(*this);
			release();
			return v;
		}

		inline operator T() {return value();}

		inline T value() {
			capture();
			T v = data;
			release();
			return v;
		}

		inline bool operator==(T const& other)		{return value() == other;			}
		inline bool operator==(Atomic const& other)	{return value() == other.value();	}

		inline Helper::PartialOrder operator<=>(T const& other)			{return value() <=> other;			}
		inline Helper::PartialOrder operator<=>(Atomic const& other)	{return value() <=> other.value();	}

		Atomic<T>& operator++() requires Type::PreIncrementable<T>						{capture(); ++data; return release();			}
		T operator++(int) requires Type::PostIncrementable<T>							{capture(); T v = data++; release(); return v;	}
		Atomic<T>& operator--() requires Type::PreDecrementable<T>						{capture(); --data; return release();			}
		T operator--(int) requires Type::PostDecrementable<T>							{capture(); T v = data--; release(); return v;	}
		Atomic<T>& operator+=(T const& v) requires Type::AddAssignable<T, T>			{capture(); data += v; return release();		}
		Atomic<T>& operator-=(T const& v) requires Type::SubAssignable<T, T>			{capture(); data -= v; return release();		}
		Atomic<T>& operator*=(T const& v) requires Type::MulAssignable<T, T>			{capture(); data *= v; return release();		}
		Atomic<T>& operator/=(T const& v) requires Type::DivAssignable<T, T>			{capture(); data /= v; return release();		}
		Atomic<T>& operator^=(T const& v) requires Type::Bitwise::XorAssignable<T, T>	{capture(); data ^= v; return release();		}
		Atomic<T>& operator&=(T const& v) requires Type::Bitwise::AndAssignable<T, T>	{capture(); data &= v; return release();		}
		Atomic<T>& operator|=(T const& v) requires Type::Bitwise::OrAssignable<T, T>	{capture(); data |= v; return release();		}
		Atomic<T>& operator%=(T const& v) requires Type::ModAssignable<T, T>			{capture(); data %= v; return release();		}
		Atomic<T>& operator<<=(T const& v) requires Type::Stream::InsAssignable<T, T>	{capture(); data <<= v; return release();		}
		Atomic<T>& operator>>=(T const& v) requires Type::Stream::ExtAssignable<T, T>	{capture(); data >>= v; return release();		}

	private:
		inline Atomic& capture()	{return acquireLock();	}
		inline Atomic& release()	{return releaseLock();	}

		inline Atomic& acquireLock() {
			do {
				while (getLock())
					yield();
			} while (__atomic_test_and_set(&locked, __ATOMIC_ACQUIRE));
			return *this;
		}

		inline Atomic& releaseLock() {
			__atomic_clear(&locked, __ATOMIC_RELEASE);
			return *this;
		}

		inline bool getLock() {
			return __atomic_load_n(&locked, __ATOMIC_RELAXED);
		}

		T data;
		volatile bool locked = false;
	};

	class Mutex: Yieldable {
	public:
		Mutex() {}

		inline Mutex& capture()	{
			Thread::id const thisThread = currentThreadID();
			if (!isCurrentOwner())
				wait();
			current = thisThread;
			return *this;
		}

		inline bool tryCapture() {
			Thread::id const thisThread = currentThreadID();
			if (!isCurrentOwner()) return false;
			current = thisThread;
			return true;
		}

		inline Mutex& release()	{
			if (isCurrentOwner())
				current = nullptr;
			return *this;
		}

		inline Mutex& wait() {
			if (!isCurrentOwner())
				while(isCaptured())
					yield();
			return *this;
		}

		inline bool isCaptured()						{return current.value().exists();		}
		inline bool isOwnedBy(Thread::id const& thread)	{return current == thread;				}
		inline bool isCurrentOwner()					{return current == currentThreadID();	}

		//inline const Nullable<Thread::id> getCurrentOwner() const	{return current.value();	}
		inline Nullable<Thread::id> getCurrentOwner()				{return current.value();	}

	private:
		Atomic<Nullable<Thread::id>> current;
	};

	template<class T, Type::Derived<Mutex> M = Mutex>
	class Box: public M {
	public:
		typedef T ValueType;
		typedef M MutexType;

		constexpr Box()											{}
		constexpr Box(T const& _data):	data(_data)				{}
		constexpr Box(T&& _data):		data(std::move(_data))	{}

		inline Box& operator=(T const& val) {
			this->wait();
			data = val;
			return *this;
		}

		inline operator T() {return value();}

		inline T value() {
			this->wait();
			return data;
		}

		inline bool operator==(T const& other) const	{return value() == other;			}
		inline bool operator==(Box const& other) const	{return value() == other.value();	}

		inline Helper::PartialOrder operator<=>(T const& other) const	{return value() <=> other;			}
		inline Helper::PartialOrder operator<=>(Box const& other) const	{return value() <=> other.value();	}

		Box<T>& operator++() requires Type::PreIncrementable<T>						{this->wait(); ++data;					}
		T operator++(int) requires Type::PostIncrementable<T>						{this->wait(); T v = data++; return v;	}
		Box<T>& operator--() requires Type::PreDecrementable<T>						{this->wait(); --data;					}
		T operator--(int) requires Type::PostDecrementable<T>						{this->wait(); T v = data--; return v;	}
		Box<T>& operator+=(T const& v) requires Type::AddAssignable<T, T>			{this->wait(); data += v;				}
		Box<T>& operator-=(T const& v) requires Type::SubAssignable<T, T>			{this->wait(); data -= v;				}
		Box<T>& operator*=(T const& v) requires Type::MulAssignable<T, T>			{this->wait(); data *= v;				}
		Box<T>& operator/=(T const& v) requires Type::DivAssignable<T, T>			{this->wait(); data /= v;				}
		Box<T>& operator^=(T const& v) requires Type::Bitwise::XorAssignable<T, T>	{this->wait(); data ^= v;				}
		Box<T>& operator&=(T const& v) requires Type::Bitwise::AndAssignable<T, T>	{this->wait(); data &= v;				}
		Box<T>& operator|=(T const& v) requires Type::Bitwise::OrAssignable<T, T>	{this->wait(); data |= v;				}
		Box<T>& operator%=(T const& v) requires Type::ModAssignable<T, T>			{this->wait(); data %= v;				}
		Box<T>& operator<<=(T const& v) requires Type::Stream::InsAssignable<T, T>	{this->wait(); data <<= v;				}
		Box<T>& operator>>=(T const& v) requires Type::Stream::ExtAssignable<T, T>	{this->wait(); data >>= v;				}

	private:
		T data;
	};

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

#endif // THREADED_DATA_MANIPULATION_H
