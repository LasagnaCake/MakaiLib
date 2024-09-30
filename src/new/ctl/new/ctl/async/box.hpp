#ifndef CTL_ASYNC_BOX_H
#define CTL_ASYNC_BOX_H

#include "../namespace.hpp"
#include "../templates.hpp"
#include "../typetraits/traits.hpp"
#include "mutex.hpp"

CTL_NAMESPACE_BEGIN

template<class T, Type::Derived<Mutex> M = Mutex>
class Box:
	public M,
	public Typed<T>,
	public SelfIdentified<Box<T, M>>,
	public Derived<M>,
	public Ordered {
public:
	using Typed				= Typed<T>;
	using SelfIdentified	= SelfIdentified<Box<T, M>>;
	using Derived			= Derived<M>;

	using
		typename Typed::DataType,
		typename Typed::ReferenceType,
		typename Typed::TemporaryType,
		typename Typed::ConstReferenceType
	;

	using
		typename SelfIdentified::SelfType
	;

	using BaseType = typename Derived::Bases::FirstType;

	using MutexType = BaseType;

	constexpr Box()	noexcept										{}
	constexpr Box(ConstReferenceType _data):	data(_data)			{}
	constexpr Box(TemporaryType _data):			data(move(_data))	{}

	inline Box& operator=(T const& val) {
		this->wait();
		data = val;
		return *this;
	}

	operator T() {return value();}

	DataType value() const {
		this->wait();
		return data;
	}

	inline bool operator==(DataType const& other) const	{return value() == other;			}
	inline bool operator==(Box const& other) const		{return value() == other.value();	}

	inline OrderType operator<=>(T const& other) const
	requires Type::Comparable::Threeway<DataType, DataType> {return value() <=> other;			}
	inline OrderType operator<=>(Box const& other) const
	requires Type::Comparable::Threeway<DataType, DataType> {return value() <=> other.value();	}

	SelfType& operator++() requires Type::PreIncrementable<DataType>									{this->wait(); ++data;					}
	DataType operator++(int) requires Type::PostIncrementable<DataType>									{this->wait(); T v = data++; return v;	}
	SelfType& operator--() requires Type::PreDecrementable<DataType>									{this->wait(); --data;					}
	DataType operator--(int) requires Type::PostDecrementable<DataType>									{this->wait(); T v = data--; return v;	}
	SelfType& operator+=(DataType const& v) requires Type::AddAssignable<DataType, DataType>			{this->wait(); data += v;				}
	SelfType& operator-=(DataType const& v) requires Type::SubAssignable<DataType, DataType>			{this->wait(); data -= v;				}
	SelfType& operator*=(DataType const& v) requires Type::MulAssignable<DataType, DataType>			{this->wait(); data *= v;				}
	SelfType& operator/=(DataType const& v) requires Type::DivAssignable<DataType, DataType>			{this->wait(); data /= v;				}
	SelfType& operator^=(DataType const& v) requires Type::Bitwise::XorAssignable<DataType, DataType>	{this->wait(); data ^= v;				}
	SelfType& operator&=(DataType const& v) requires Type::Bitwise::AndAssignable<DataType, DataType>	{this->wait(); data &= v;				}
	SelfType& operator|=(DataType const& v) requires Type::Bitwise::OrAssignable<DataType, DataType>	{this->wait(); data |= v;				}
	SelfType& operator%=(DataType const& v) requires Type::ModAssignable<DataType, DataType>			{this->wait(); data %= v;				}
	SelfType& operator<<=(DataType const& v) requires Type::Stream::InsAssignable<DataType, DataType>	{this->wait(); data <<= v;				}
	SelfType& operator>>=(DataType const& v) requires Type::Stream::ExtAssignable<DataType, DataType>	{this->wait(); data >>= v;				}

private:
	DataType data;
};

CTL_NAMESPACE_END

#endif // CTL_ASYNC_BOX_H
