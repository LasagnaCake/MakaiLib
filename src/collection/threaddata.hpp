#ifndef THREADED_DATA_MANIPULATION_H
#define THREADED_DATA_MANIPULATION_H

#include "dataview.hpp"

namespace Threaded {
	template <Viewable T>
	class Atomic {
	public:
		Atomic()							{}
		Atomic(T const& _data): data(_data)	{}

		inline bool isLocked() {return locked;}

		inline Atomic& wait() {while (locked) {}; return (*this);}

		inline Atomic& operator=(T const& val) {
			capture();
			data = val;
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

	template <Viewable T>
	using AtomicView = DataView<Atomic<T>>;
}

#endif // THREADED_DATA_MANIPULATION_H
