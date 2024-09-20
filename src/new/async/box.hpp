#ifndef MAKAILIB_ASYNC_BOX_H
#define MAKAILIB_ASYNC_BOX_H

#include "atomic.hpp"
#include "mutex.hpp"

namespace Makai::Async {
	template<class T, Type::Derived<Mutex> M = Mutex>
	class Box: public M {
	public:
		typedef T ValueType;
		typedef M MutexType;

		constexpr Box()											{}
		constexpr Box(T const& _data):	data(_data)				{}
		constexpr Box(T&& _data):		data(std::move(_data))	{}

		Box& operator=(T const& val) {
			this->wait();
			data = val;
			return *this;
		}

		operator T() {return value();}

		T value() {
			this->wait();
			return data;
		}

		bool operator==(T const& other) const	{return value() == other;			}
		bool operator==(Box const& other) const	{return value() == other.value();	}

		Helper::PartialOrder operator<=>(T const& other) const	{return value() <=> other;			}
		Helper::PartialOrder operator<=>(Box const& other) const	{return value() <=> other.value();	}

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
}

#endif // MAKAILIB_ASYNC_BOX_H
