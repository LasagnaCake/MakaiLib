#ifndef CTL_CONTAINER_NULLABLE_H
#define CTL_CONTAINER_NULLABLE_H

#include "../namespace.hpp"
#include "../templates.hpp"
#include "../order.hpp"
#include "../typetraits/traits.hpp"
#include "../algorithm/strconv.hpp"
#include "../adaptor/comparator.hpp"
#include "error.hpp"

CTL_NAMESPACE_BEGIN

template<class TData> class Nullable;

template<> class Nullable<void>:
	//public Typed<void>,
	public SelfIdentified<Nullable<void>>,
	public Nulled,
	public Ordered {
public:
	using DataType = void;

	using Nulled::NullType;
	using Ordered::OrderType;

	constexpr Nullable() noexcept			{}
	constexpr Nullable(NullType) noexcept	{}

	constexpr bool exists()		const {return false;	}
	constexpr operator bool()	const {return exists();	}
	constexpr bool operator()()	const {return exists();	}

	constexpr Nullable& operator=(NullType)						{return *this;	}
	constexpr Nullable& operator=(Nullable<void> const& other)	{return *this;	}
	constexpr Nullable& operator=(Nullable<void>&& other)		{return *this;	}

	constexpr bool operator==(NullType) const						{return true;	}
	constexpr bool operator==(Nullable<void> const& other) const	{return true;	}
	constexpr bool operator==(Nullable<void>&& other) const			{return true;	}

	constexpr OrderType operator<=>(Nullable<void> const& other) const	{return Order::EQUAL;	}
	constexpr OrderType operator<=>(Nullable<void>&& other) const		{return Order::EQUAL;	}

	constexpr ~Nullable() {}
};

template<Type::Different<void> TData>
class Nullable<TData>:
	public SelfIdentified<Nullable<TData>>,
	public Typed<TData>,
	public Nulled,
	public Defaultable<TData>,
	public Ordered {
public:
	using Typed				= ::CTL::Typed<TData>;
	using SelfIdentified	= ::CTL::SelfIdentified<Nullable<TData>>;

	using typename
		Typed::DataType,
		Typed::ConstantType,
		Typed::ReferenceType,
		Typed::ConstReferenceType,
		Typed::TemporaryType
	;

	using typename SelfIdentified::SelfType;

	using Nulled::NullType;
	using 
		Ordered::OrderType, 
		Ordered::Order
	;

	using OperationType	= Function<DataType(DataType const&)>;
	using ProcedureType	= Function<void(DataType const&)>;

	constexpr Nullable() noexcept										{												}
	constexpr Nullable(NullType) noexcept								{												}
	constexpr Nullable(ConstReferenceType value): isSet(true)			{data = value;									}
	constexpr Nullable(TemporaryType value): isSet(true)				{data = CTL::move(value);						}
	constexpr Nullable(SelfType const& other): isSet(other.isSet)		{if (other.isSet) data = other.data;			}
	constexpr Nullable(SelfType&& other): isSet(CTL::move(other.isSet))	{if (other.isSet) data = CTL::move(other.data);	}

	constexpr ~Nullable() {}

	constexpr DataType value() const {
		if (isSet) return data;
		throw Error::NonexistentValue(
			"Value is not set!",
			__FILE__,
			toString(__LINE__),
			"Nullable::value"
		);
	}

	constexpr DataType orElse(DataType const& fallback) const
	requires (!Type::Constructible<DataType>) {return (isSet) ? data : fallback;}

	constexpr DataType orElse(DataType const& fallback = DataType()) const
	requires (Type::Constructible<DataType>) {return (isSet) ? data : fallback;}

	constexpr SelfType& then(OperationType const& op) {if (isSet) data = op(data); return *this;}

	constexpr bool exists()		const {return isSet;	}
	constexpr operator bool()	const {return exists();	}
	constexpr bool operator()()	const {return exists();	}

	constexpr SelfType& operator()(OperationType const& op) {return then(op);}

	constexpr SelfType& operator=(DataType const& value)	{data = value; isSet = true; return *this;				}
	constexpr SelfType& operator=(DataType&& value)			{data = CTL::move(value); isSet = true; return *this;	}
	constexpr SelfType& operator=(NullType)					{isSet = false; return *this;							}

	constexpr bool operator==(SelfType const& other) const
	requires Type::Comparator::Equals<DataType, DataType> {
		if (!isSet && !other.isSet)
			return true;
		if (isSet && other.isSet)
			return SimpleComparator<DataType>::equals(data, other.data);
		return false;
	}
	constexpr bool operator==(DataType const& value) const
	requires Type::Comparator::Equals<DataType, DataType> {
		if (isSet)
			return SimpleComparator<DataType>::equals(data, value);
		return false;
	}
	
	constexpr bool operator==(NullType) const {return !isSet;}

	constexpr OrderType operator<=>(SelfType const& other) const
	requires Type::Comparator::Threeway<DataType, DataType>	{
		if (!other.isSet && !isSet)	return Order::EQUAL;
		if (other.isSet && !isSet)	return Order::LESS;
		if (!other.isSet && isSet)	return Order::GREATER;
		return SimpleComparator<DataType>::compare(data, other);
	}

	constexpr OrderType operator<=>(DataType const& value) const
	requires Type::Comparator::Threeway<DataType, DataType>	{
		if (!isSet)	return Order::LESS;
		return SimpleComparator<DataType>::compare(data, value);
	}

	constexpr SelfType& operator=(SelfType const& other) {
		if (other.isSet) data = other.data;
		isSet = other.isSet;
		return *this;
	}

	constexpr SelfType& operator=(SelfType&& other) {
		if (other.isSet) data = CTL::move(other.data);
		isSet = CTL::move(other.isSet);
		return *this;
	}

	constexpr DataType operator*() const {
		return value();
	}

	constexpr operator DataType() const
	requires Type::Different<DataType, bool> {
		return value();
	}

private:
	DataType data;
	bool isSet = false;

	template<class T>
	friend constexpr Nullable<T> operator<=>(typename Nullable<T>::DataType const&, Nullable<T> const&);
};

template<class TData>
constexpr Nullable<TData> operator<=>(typename Nullable<TData>::DataType const& value, Nullable<TData> const& self)
requires Type::Comparator::Threeway<
	typename Nullable<TData>::DataType,
	typename Nullable<TData>::DataType
>	{
	if (!self.isSet) return Nullable<TData>::Order::LESS;
	return SimpleComparator<TData>::compare(value, self.data);
}

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_NULLABLE_H
