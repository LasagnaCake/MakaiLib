#ifndef CTL_CONTAINER_NULLABLE_H
#define CTL_CONTAINER_NULLABLE_H

#include "../namespace.hpp"
#include "../templates.hpp"
#include "../order.hpp"
#include "../typetraits/traits.hpp"
#include "error.hpp"

CTL_NAMESPACE_BEGIN

template<class TData> class Nullable;

template<> class Nullable<void>:
	public Typed<void>,
	public SelfIdentitied<Nullable<void>>,
	public Nulled,
	public Ordered {
public:
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

	constexpr OrderType operator<=>(Nullable<void> const& other) const	{return OrderType::EQUAL;	}
	constexpr OrderType operator<=>(Nullable<void>&& other) const		{return OrderType::EQUAL;	}

	constexpr ~Nullable() {}
};

template<Type::Different<void> TData>
class Nullable<TData>:
	public SelfIdentitied<Nullable<TData>>,
	public Typed<TData>,
	public Nulled,
	public Defaultable<TData, TData()>,
	public Ordered {
public:
	using typename ::CTL::Typed<TData>::DataType;
	using typename ::CTL::SelfIdentitied<Nullable<TData>>::DataType;

	constexpr Nullable() noexcept										{												}
	constexpr Nullable(NullType) noexcept								{												}
	constexpr Nullable(DataType const& value): isSet(true)				{data = value;									}
	constexpr Nullable(DataType&& value): isSet(true)					{data = CTL::move(value);						}
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

	constexpr SelfType& then(Operation<DataType> const& op) {if (isSet) data = op(data); return *this;}

	constexpr bool exists()		const {return isSet;	}
	constexpr operator bool()	const {return exists();	}
	constexpr bool operator()()	const {return exists();	}

	constexpr SelfType& operator()(Operation<DataType> const& op) {return then();}

	constexpr SelfType& operator=(DataType const& value)	{data = value; isSet = true; return *this;			}
	constexpr SelfType& operator=(DataType&& value)			{data = move(value); isSet = true; return *this;	}
	constexpr SelfType& operator=(NullType)					{isSet = false; return *this;						}

	constexpr bool operator==(SelfType const& other) const	{if (isSet) return other == data; return false;}
	constexpr bool operator==(DataType const& value) const	{if (isSet) return data == value; return false;}
	constexpr bool operator==(DataType&& value) const		{if (isSet) return data == value; return false;}
	constexpr bool operator==(NullType) const				{return !isSet;}

	constexpr OrderType operator<=>(SelfType const& other) const
	requires Type::Comparable::Threeway<DataType, DataType>	{
		if (!other.isSet && !isSet)	return OrderType::EQUAL;
		if (other.isSet && !isSet)	return OrderType::LESS;
		if (!other.isSet && isSet)	return OrderType::GREATER;
		return data <=> other;
	}

	constexpr OrderType operator<=>(DataType const& value) const
	requires Type::Comparable::Threeway<DataType, DataType>	{
		if (!self.isSet) return OrderType::LESS;
		return data <=> self.value;
	}

	constexpr OrderType operator<=>(DataType const& value, SelfType const& self) const
	requires Type::Comparable::Threeway<DataType, DataType>	{
		if (!self.isSet) return OrderType::LESS;
		return data <=> self.value;
	}

	constexpr SelfType& operator=(SelfType const& other) {
		if (other.isSet) data = other.data;
		isSet = other.isSet;
		return *this;
	}

	constexpr SelfType& operator=(SelfType&& other) {
		if (other.isSet) data = move(other.data);
		isSet = move(other.isSet);
		return *this;
	}

	constexpr DataType operator *() const {
		return value();
	}

	constexpr operator DataType() const
	requires Type::Different<DataType, bool> {
		return value();
	}

private:
	DataType data;
	bool isSet = false;
};

CTL_NAMESPACE_END

#endif // CTL_CONTAINER_NULLABLE_H
