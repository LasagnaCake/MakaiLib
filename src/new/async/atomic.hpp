#ifndef MAKAILIB_ASYNC_ATOMIC_H
#define MAKAILIB_ASYNC_ATOMIC_H

#include <thread>
#include "../std/dataview.hpp"

namespace Makai::Async {
	template <class T>
	class Atomic: Yieldable {
	public:
		typedef T DataType;

		constexpr Atomic() noexcept							{}
		constexpr Atomic(T const& _data): data(_data)		{}
		constexpr Atomic(T&& _data): data(std::move(_data))	{}

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

	template <Viewable T>
	using AtomicView = DataView<Atomic<T>>;

	template <Viewable... Args>
	using AtomicTuple = Tuple<Atomic<Args>...>;
}

#endif // MAKAILIB_ASYNC_ATOMIC_H
