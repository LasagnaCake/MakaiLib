#ifndef CTL_CONTAINER_NULLABLE_H
#define CTL_CONTAINER_NULLABLE_H

#include "../templates.hpp"
#include "error.hpp"

template<class TData> class Nullable;

template<> class Nullable<void>:
	public SelfIdentitied<Nullable<void>>,
	public Nulled {
public:
	typedef void DataType;

	constexpr Nullable() noexcept			{}
	constexpr Nullable(NullType) noexcept	{}

	constexpr bool exists()		const {return false;	}
	constexpr operator bool()	const {return exists();	}
	constexpr bool operator()()	const {return exists();	}

	constexpr Nullable& operator=(NullType)						{return *this;	}
	constexpr Nullable& operator=(Nullable<void> const& other)	{return *this;	}
	constexpr Nullable& operator=(Nullable<void>&& other)		{return *this;	}

	constexpr bool operator==(NullType) const	{return true;	}

	constexpr ~Nullable() {}
};

template<Type::Different<void> TData>
class Nullable<TData>:
	public SelfIdentitied<Nullable<TData>>,
	public Typed<TData>,
	public Nulled,
	public Defaultable<TData, TData()> {
public:
	constexpr Nullable() noexcept										{}
	constexpr Nullable(NullType) noexcept								{}
	constexpr Nullable(DataType const& value): isSet(true)				{data = value;}
	constexpr Nullable(DataType&& value): isSet(true)					{data = std::move(value);}
	constexpr Nullable(Nullable const& other): isSet(other.isSet)		{if (other.isSet) data = other.data;}
	constexpr Nullable(Nullable&& other): isSet(std::move(other.isSet))	{if (other.isSet) data = std::move(other.data);}

	constexpr ~Nullable() {}

	constexpr DataType value(DataType const& fallback) const
	requires (!Type::Constructible<DataType>) {return (isSet) ? data : fallback;}
	constexpr DataType value(DataType const& fallback = DataType()) const
	requires (Type::Constructible<DataType>) {return (isSet) ? data : fallback;}

	constexpr Nullable& then(Operation<DataType> const& op) {if (isSet) data = op(data); return *this;}

	constexpr bool exists()		const {return isSet;	}
	constexpr operator bool()	const {return exists();	}
	constexpr bool operator()()	const {return exists();	}

	constexpr Nullable& operator()(Operation<DataType> const& op) {return then();}

	constexpr Nullable& operator=(DataType const& value)	{data = value; isSet = true; return *this;				}
	constexpr Nullable& operator=(DataType&& value)			{data = std::move(value); isSet = true; return *this;	}
	constexpr Nullable& operator=(NullType)					{isSet = false; return *this;							}

	constexpr bool operator==(Nullable const& other) const	{if (isSet) return other == data; return false;}
	constexpr bool operator==(DataType const& value) const	{if (isSet) return data == value; return false;}
	constexpr bool operator==(DataType&& value) const		{if (isSet) return data == value; return false;}
	constexpr bool operator==(NullType) const				{return !isSet;}

	constexpr Nullable& operator=(Nullable<T> const& other) {
		if (other.isSet) data = other.data;
		isSet = other.isSet;
		return *this;
	}

	constexpr Nullable& operator=(Nullable<T>&& other) {
		if (other.isSet) data = std::move(other.data);
		isSet = std::move(other.isSet);
		return *this;
	}

	constexpr DataType operator *() const {
		if (isSet) return data;
		throw Error::NonexistentValue(
			"Value is not set!",
			__FILE__,
			toString(__LINE__),
			"operator T()"
		);
	}

	constexpr operator DataType() const
	requires Type::Different<DataType, bool> {
		if (isSet) return data;
		return defaultValue();
	}

	constexpr explicit operator DataType() const
	requires Type::Different<DataType, bool> {
		if (isSet) return data;
		throw Error::NonexistentValue(
			"Value is not set!",
			__FILE__,
			toString(__LINE__),
			"operator T()"
		);
	}

private:
	DataType data;
	bool isSet = false;
};

#endif // CTL_CONTAINER_NULLABLE_H
