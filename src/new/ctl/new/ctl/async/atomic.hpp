#ifndef CTL_ASYNC_ATOMIC_H
#define CTL_ASYNC_ATOMIC_H

#include "../namespace.hpp"
#include "../typetraits/traits.hpp"
#include "../templates.hpp"
#include "../container/function.hpp"
#include "../container/view.hpp"
#include "../adaptor/comparator.hpp"
#include "thread.hpp"

CTL_NAMESPACE_BEGIN

template <class T>
class Atomic:
	Base::Async::Yieldable,
	Typed<T>,
	SelfIdentified<Atomic<T>>,
	Derived<Base::Async::Yieldable>,
	Ordered {
public:
	using Typed				= Typed<T>;
	using SelfIdentified	= SelfIdentified<Atomic<T>>;
	using Derived			= Derived<Base::Async::Yieldable>;

	using
		typename Typed::DataType,
		typename Typed::TemporaryType,
		typename Typed::ConstReferenceType
	;

	using
		typename SelfIdentified::SelfType
	;

	using
		typename Derived::BaseType
	;

	using ViewType = View<Atomic<DataType>>;

	using OperationType = Function<DataType(ConstReferenceType)>;

	constexpr Atomic() noexcept											{}
	constexpr Atomic(ConstReferenceType _data): data(_data)				{}
	constexpr Atomic(TemporaryType _data):		data(CTL::move(_data))	{}

	inline bool isLocked() const {return locked;}

	inline SelfType& operator=(ConstReferenceType val) {
		capture();
		data = val;
		release();
		return *this;
	}

	inline SelfType& operator=(TemporaryType val) {
		capture();
		data = CTL::move(val);
		release();
		return *this;
	}

	inline SelfType& modify(OperationType const& op) {
		capture();
		data = op(data);
		release();
		return *this;
	}

	inline SelfType& operator()(OperationType const& op) {return modify(op);}

	inline ViewType view() {
		return ViewType(*this);
	}

	inline operator DataType() const {return value();}

	inline DataType value() const {
		capture();
		DataType v = data;
		release();
		return v;
	}

	inline bool operator==(T const& other) const
	requires Type::Comparator::Equals<DataType, DataType>	{return SimpleComparator<T>::equals(value(), other);			}
	inline bool operator==(Atomic const& other) const
	requires Type::Comparator::Equals<DataType, DataType>	{return SimpleComparator<T>::equals(value(), other.value());	}

	inline OrderType operator<=>(T const& other) const
	requires Type::Comparator::Threeway<DataType, DataType>	{return SimpleComparator<T>::compare(value(), other);			}
	inline OrderType operator<=>(Atomic const& other) const
	requires Type::Comparator::Threeway<DataType, DataType>	{return SimpleComparator<T>::compare(value(), other.value());	}

	SelfType& operator++() requires Type::PreIncrementable<DataType>									{capture(); ++data; return release();					}
	DataType operator++(int) requires Type::PostIncrementable<DataType>									{capture(); DataType v = data++; release(); return v;	}
	SelfType& operator--() requires Type::PreDecrementable<DataType>									{capture(); --data; return release();					}
	DataType operator--(int) requires Type::PostDecrementable<DataType>									{capture(); DataType v = data--; release(); return v;	}
	SelfType& operator+=(DataType const& v) requires Type::AddAssignable<DataType, DataType>			{capture(); data += v; return release();				}
	SelfType& operator-=(DataType const& v) requires Type::SubAssignable<DataType, DataType>			{capture(); data -= v; return release();				}
	SelfType& operator*=(DataType const& v) requires Type::MulAssignable<DataType, DataType>			{capture(); data *= v; return release();				}
	SelfType& operator/=(DataType const& v) requires Type::DivAssignable<DataType, DataType>			{capture(); data /= v; return release();				}
	SelfType& operator^=(DataType const& v) requires Type::Bitwise::XorAssignable<DataType, DataType>	{capture(); data ^= v; return release();				}
	SelfType& operator&=(DataType const& v) requires Type::Bitwise::AndAssignable<DataType, DataType>	{capture(); data &= v; return release();				}
	SelfType& operator|=(DataType const& v) requires Type::Bitwise::OrAssignable<DataType, DataType>	{capture(); data |= v; return release();				}
	SelfType& operator%=(DataType const& v) requires Type::ModAssignable<DataType, DataType>			{capture(); data %= v; return release();				}
	SelfType& operator<<=(DataType const& v) requires Type::Stream::InsAssignable<DataType, DataType>	{capture(); data <<= v; return release();				}
	SelfType& operator>>=(DataType const& v) requires Type::Stream::ExtAssignable<DataType, DataType>	{capture(); data >>= v; return release();				}

private:
	inline void capture() const {return acquireLock();	}
	inline void release() const {return releaseLock();	}

	inline void acquireLock() const {
		do {
			while (getLock())
				BaseType::asyncYield();
		} while (__atomic_test_and_set(&locked, __ATOMIC_ACQUIRE));
	}

	inline void releaseLock() const {
		__atomic_clear(&locked, __ATOMIC_RELEASE);
	}

	inline bool getLock() const {
		return __atomic_load_n(&locked, __ATOMIC_RELAXED);
	}

	DataType data;
	volatile bool mutable locked = false;
};

CTL_NAMESPACE_END

#endif // CTL_ASYNC_ATOMIC_H
