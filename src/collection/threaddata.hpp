#ifndef THREADED_DATA_MANIPULATION_H
#define THREADED_DATA_MANIPULATION_H

#include <thread>
#include "dataview.hpp"

namespace Threaded {
	typedef std::thread Thread;

	Thread::id currentThreadID() {
		return std::this_thread::get_id();
	}

	template <Viewable T>
	class Atomic {
	public:
		typedef T DataType;

		constexpr Atomic()											{}
		constexpr Atomic(T const& _data): data(_data)				{}
		constexpr Atomic(T&& _data):		data(std::move(_data))	{}

		inline bool isLocked() {return locked;}

		inline Atomic& wait() {while (locked) {}; return (*this);}

		inline Atomic& operator=(T const& val) {
			capture();
			data = val;
			return release();
		}

		inline Atomic& operator=(T&& val) {
			capture();
			data = std::move(val);
			release();
			return *this;
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
		inline Atomic& capture()	{wait(); locked = true;	return *this;}
		inline Atomic& release()	{locked = false; return *this;}

		T data;
		bool locked = false;
	};

	class Mutex {
	public:
		constexpr Mutex() {}

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
				while(isCaptured()) {}
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

	template<Type::Derived<Mutex> T = Mutex>
	class BaseLock {
	public:
		typedef T DataType;

		BaseLock(T& mutex): mtx(mutex) {}

	protected:
		T& mtx;
	};

	template<Type::Derived<Mutex> T = Mutex>
	class ScopeLock: public BaseLock<T> {
	public:
		ScopeLock(T& mutex): BaseLock<T>(mutex)	{this->mtx.capture();}
		~ScopeLock()							{this->mtx.release();}
	};

	template<class T>
	concept LockType =
		requires {typename T::DataType;}
	&&	Type::Derived<T, BaseLock<typename T::DataType>>
	;

	template<Viewable T, LockType Lock>
	class BoxLock;

	template<Viewable T>
	class Box {
	public:
		typedef T DataType;

		constexpr Box()											{}
		constexpr Box(T const& _data):	data(_data)				{}
		constexpr Box(T&& _data):		data(std::move(_data))	{}

		inline Box& operator=(T const& val) {
			mutex.wait();
			data = val;
			return *this;
		}

		inline DataView<Box<T>> view() {
			mutex.wait();
			return *this;
		}

		inline operator T() {return value();}

		inline T value() {
			mutex.wait();
			return data;
		}

		inline bool operator==(T const& other) const	{return value() == other;			}
		inline bool operator==(Box const& other) const	{return value() == other.value();	}

		inline Helper::PartialOrder operator<=>(T const& other) const	{return value() <=> other;			}
		inline Helper::PartialOrder operator<=>(Box const& other) const	{return value() <=> other.value();	}

		Box<T>& operator++() requires Type::PreIncrementable<T>						{mutex.wait(); ++data;					}
		T operator++(int) requires Type::PostIncrementable<T>						{mutex.wait(); T v = data++; return v;	}
		Box<T>& operator--() requires Type::PreDecrementable<T>						{mutex.wait(); --data;					}
		T operator--(int) requires Type::PostDecrementable<T>						{mutex.wait(); T v = data--; return v;	}
		Box<T>& operator+=(T const& v) requires Type::AddAssignable<T, T>			{mutex.wait(); data += v;				}
		Box<T>& operator-=(T const& v) requires Type::SubAssignable<T, T>			{mutex.wait(); data -= v;				}
		Box<T>& operator*=(T const& v) requires Type::MulAssignable<T, T>			{mutex.wait(); data *= v;				}
		Box<T>& operator/=(T const& v) requires Type::DivAssignable<T, T>			{mutex.wait(); data /= v;				}
		Box<T>& operator^=(T const& v) requires Type::Bitwise::XorAssignable<T, T>	{mutex.wait(); data ^= v;				}
		Box<T>& operator&=(T const& v) requires Type::Bitwise::AndAssignable<T, T>	{mutex.wait(); data &= v;				}
		Box<T>& operator|=(T const& v) requires Type::Bitwise::OrAssignable<T, T>	{mutex.wait(); data |= v;				}
		Box<T>& operator%=(T const& v) requires Type::ModAssignable<T, T>			{mutex.wait(); data %= v;				}
		Box<T>& operator<<=(T const& v) requires Type::Stream::InsAssignable<T, T>	{mutex.wait(); data <<= v;				}
		Box<T>& operator>>=(T const& v) requires Type::Stream::ExtAssignable<T, T>	{mutex.wait(); data >>= v;				}

	private:
		T		data;
		Mutex	mutex;

		template<typename V, LockType Lock> friend class BoxLock;
	};

	template<Viewable T, LockType L>
	class BoxLock: L {
	public:
		typedef T			DataType;
		typedef L			LockType;
		typedef L::DataType	MutexType;

		BoxLock(Box<T>& b): L(b.mutex), box(b) {}

	private:
		Box<T>& box;
	};

	template <Viewable T>
	using AtomicView = DataView<Atomic<T>>;
}

#endif // THREADED_DATA_MANIPULATION_H
